// cpp/HybridBoTSortTracker.cpp
#include "HybridBoTSortTracker.hpp"
#include <opencv2/imgproc.hpp>
#include <Eigen/Dense>
#include <algorithm> // For std::max
#include <stdexcept>

#if __APPLE__
#include <CoreVideo/CVPixelBuffer.h>
#else
#include <android/hardware_buffer.h>
#endif

namespace margelo::nitro::botsort
{

    HybridBoTSortTracker::HybridBoTSortTracker() : HybridBoTSortTrackerSpec() {}

    void HybridBoTSortTracker::initialize(const std::string &reidModelPath, bool useGpu)
    {
        tracker = std::make_unique<motcpp::trackers::BotSort>(
            reidModelPath,
            true,
            useGpu,
            0.3f,
            30,
            50,
            3,
            0.3f,
            false,
            80,
            "iou",
            false,
            0.5f,
            0.1f,
            0.6f,
            30,
            0.8f,
            0.5f,
            0.25f,
            "sof",
            30,
            false,
            false);
    }

    std::vector<TrackedObject> HybridBoTSortTracker::updateWithFrame(
        const std::shared_ptr<margelo::nitro::camera::HybridFrameSpec> &frame,
        const std::vector<BoundingBox> &detections)
    {
        if (!tracker)
            return {};

        cv::Mat matFrame;
        int originalWidth = 0;
        int originalHeight = 0;

// 1. Hardware Direct Video Frame Extraction & Channel Mapping
#if __ANDROID__
        auto nativeBuffer = frame->getNativeBuffer();
        AHardwareBuffer *buffer = reinterpret_cast<AHardwareBuffer *>(nativeBuffer.pointer);
        if (!buffer)
            return {};

        AHardwareBuffer_Desc desc;
        AHardwareBuffer_describe(buffer, &desc);
        originalWidth = desc.width;
        originalHeight = desc.height;

        void *baseAddress = nullptr;
        if (AHardwareBuffer_lock(buffer, AHARDWAREBUFFER_USAGE_CPU_READ_OFTEN, -1, nullptr, &baseAddress) != 0 || !baseAddress)
        {
            nativeBuffer.release();
            return {};
        }

        if (desc.format == AHARDWAREBUFFER_FORMAT_R8G8B8A8_UNORM)
        {
            size_t bytesPerRow = desc.stride * 4;
            cv::Mat rgbaFrame = cv::Mat(desc.height, desc.width, CV_8UC4, baseAddress, bytesPerRow);
            cv::cvtColor(rgbaFrame, matFrame, cv::COLOR_RGBA2GRAY);
        }
        else if (desc.format == AHARDWAREBUFFER_FORMAT_Y8Cb8Cr8_420)
        {
            matFrame = cv::Mat(desc.height, desc.width, CV_8UC1, baseAddress, desc.stride);
        }
        else
        {
            AHardwareBuffer_unlock(buffer, nullptr);
            nativeBuffer.release();
            return {};
        }

#elif __APPLE__
        CVPixelBufferRef pixelBuffer = frame->getPixelBuffer();
        if (!pixelBuffer)
            return {};

        if (CVPixelBufferLockBaseAddress(pixelBuffer, kCVPixelBufferLock_ReadOnly) != kCVReturnSuccess)
        {
            return {};
        }

        originalWidth = (int)CVPixelBufferGetWidth(pixelBuffer);
        originalHeight = (int)CVPixelBufferGetHeight(pixelBuffer);

        if (CVPixelBufferIsPlanar(pixelBuffer))
        {
            void *baseAddress = CVPixelBufferGetBaseAddressOfPlane(pixelBuffer, 0);
            size_t bytesPerRow = CVPixelBufferGetBytesPerRowOfPlane(pixelBuffer, 0);
            matFrame = cv::Mat(originalHeight, originalWidth, CV_8UC1, baseAddress, bytesPerRow);
        }
        else
        {
            void *baseAddress = CVPixelBufferGetBaseAddress(pixelBuffer);
            size_t bytesPerRow = CVPixelBufferGetBytesPerRow(pixelBuffer);
            cv::Mat rgbaFrame = cv::Mat(originalHeight, originalWidth, CV_8UC4, baseAddress, bytesPerRow);
            cv::cvtColor(rgbaFrame, matFrame, cv::COLOR_BGRA2GRAY);
        }
#endif

        // Mathematical Division-by-Zero / Frame Empty Safety Gate
        if (originalWidth <= 0 || originalHeight <= 0 || matFrame.empty())
        {
#if __ANDROID__
            AHardwareBuffer_unlock(buffer, nullptr);
            nativeBuffer.release();
#elif __APPLE__
            CVPixelBufferUnlockBaseAddress(pixelBuffer, kCVPixelBufferLock_ReadOnly);
#endif
            return {};
        }

        // 2. Proportional Scaling & Rescale Factor Extractions with Edge Guards
        int targetWidth = 320;
        // Fix 1: Guard against targetHeight truncating to zero to prevent divide-by-zero crashes
        int targetHeight = std::max((int)((float)originalHeight * ((float)targetWidth / (float)originalWidth)), 1);

        cv::Mat lowResFrame;
        cv::resize(matFrame, lowResFrame, cv::Size(targetWidth, targetHeight));

        // Calculate scale tracking dimensions
        float scaleFactorX = (float)originalWidth / (float)targetWidth;
        float scaleFactorY = (float)originalHeight / (float)targetHeight;

        // 3. Downscale Bounding Boxes to 320px Tracker Workspace Coordinates
        Eigen::MatrixXf scaledDets(detections.size(), 6);
        for (size_t i = 0; i < detections.size(); ++i)
        {
            const auto &d = detections[i];
            scaledDets(i, 0) = static_cast<float>(d.x) / scaleFactorX;
            scaledDets(i, 1) = static_cast<float>(d.y) / scaleFactorY;
            scaledDets(i, 2) = static_cast<float>(d.x + d.width) / scaleFactorX;
            scaledDets(i, 3) = static_cast<float>(d.y + d.height) / scaleFactorY;
            scaledDets(i, 4) = static_cast<float>(d.confidence);
            scaledDets(i, 5) = static_cast<float>(d.classId);
        }

        // 4. Update the tracker tracking matrices
        Eigen::MatrixXf tracks = tracker->update(scaledDets, lowResFrame);

// 5. Clean up Native Memory Locks Immediately
#if __ANDROID__
        AHardwareBuffer_unlock(buffer, nullptr);
        nativeBuffer.release();
#elif __APPLE__
        CVPixelBufferUnlockBaseAddress(pixelBuffer, kCVPixelBufferLock_ReadOnly);
#endif

        // 6. Rescale Tracks Back to Original Video Dimensions
        std::vector<TrackedObject> nativeResults;
        for (int i = 0; i < tracks.rows(); ++i)
        {
            float trackedX1 = tracks(i, 0) * scaleFactorX;
            float trackedY1 = tracks(i, 1) * scaleFactorY;
            float trackedX2 = tracks(i, 2) * scaleFactorX;
            float trackedY3 = tracks(i, 3) * scaleFactorY;

            nativeResults.emplace_back(
                static_cast<double>(tracks(i, 4)),
                static_cast<double>(trackedX1),
                static_cast<double>(trackedY1),
                static_cast<double>(trackedX2 - trackedX1),
                static_cast<double>(trackedY3 - trackedY1),
                static_cast<double>(tracks(i, 6)));
        }

        return nativeResults;
    }

} // namespace margelo::nitro::botsort