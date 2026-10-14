#pragma once

#include <opencv2/core.hpp>

#include <cstdint>
#include <memory>
#include <optional>
#include <utility>

#if __APPLE__
#include <CoreVideo/CVPixelBuffer.h>
#elif __ANDROID__
#include <android/hardware_buffer.h>
#endif

namespace margelo::nitro::botsort
{

    template <typename Frame>
    class CameraFrameView
    {
        using NativeBuffer = decltype(std::declval<Frame &>().getNativeBuffer());

    public:
        explicit CameraFrameView(const std::shared_ptr<Frame> &frame)
        {
            if (!frame)
                return;

#if __APPLE__
            if (!frame->getHasNativeBuffer())
                return;
#endif

            nativeBuffer_.emplace(frame->getNativeBuffer());
            if (!nativeBuffer_->pointer)
                return;

#if __ANDROID__
            auto *buffer = reinterpret_cast<AHardwareBuffer *>(nativeBuffer_->pointer);
            AHardwareBuffer_Desc desc{};
            AHardwareBuffer_describe(buffer, &desc);

            void *baseAddress = nullptr;
            if (AHardwareBuffer_lock(buffer, AHARDWAREBUFFER_USAGE_CPU_READ_OFTEN, -1, nullptr, &baseAddress) != 0)
                return;
            locked_ = true;
            if (!baseAddress)
                return;

            if (desc.format == AHARDWAREBUFFER_FORMAT_R8G8B8A8_UNORM)
            {
                image_ = cv::Mat(static_cast<int>(desc.height), static_cast<int>(desc.width), CV_8UC4,
                                 baseAddress, static_cast<size_t>(desc.stride) * 4);
            }
            else if (desc.format == AHARDWAREBUFFER_FORMAT_Y8Cb8Cr8_420)
            {
                image_ = cv::Mat(static_cast<int>(desc.height), static_cast<int>(desc.width), CV_8UC1,
                                 baseAddress, desc.stride);
            }
#elif __APPLE__
            auto pixelBuffer = reinterpret_cast<CVPixelBufferRef>(
                static_cast<uintptr_t>(nativeBuffer_->pointer));
            if (CVPixelBufferLockBaseAddress(pixelBuffer, kCVPixelBufferLock_ReadOnly) != kCVReturnSuccess)
                return;
            locked_ = true;

            int width = static_cast<int>(CVPixelBufferGetWidth(pixelBuffer));
            int height = static_cast<int>(CVPixelBufferGetHeight(pixelBuffer));
            if (CVPixelBufferIsPlanar(pixelBuffer))
            {
                void *baseAddress = CVPixelBufferGetBaseAddressOfPlane(pixelBuffer, 0);
                size_t bytesPerRow = CVPixelBufferGetBytesPerRowOfPlane(pixelBuffer, 0);
                image_ = cv::Mat(height, width, CV_8UC1, baseAddress, bytesPerRow);
            }
            else
            {
                void *baseAddress = CVPixelBufferGetBaseAddress(pixelBuffer);
                size_t bytesPerRow = CVPixelBufferGetBytesPerRow(pixelBuffer);
                image_ = cv::Mat(height, width, CV_8UC4, baseAddress, bytesPerRow);
            }
#endif
        }

        ~CameraFrameView() { release(); }

        CameraFrameView(const CameraFrameView &) = delete;
        CameraFrameView &operator=(const CameraFrameView &) = delete;

        const cv::Mat &image() const { return image_; }
        explicit operator bool() const { return !image_.empty(); }

        void release()
        {
            if (locked_)
            {
#if __ANDROID__
                AHardwareBuffer_unlock(reinterpret_cast<AHardwareBuffer *>(nativeBuffer_->pointer), nullptr);
#elif __APPLE__
                auto pixelBuffer = reinterpret_cast<CVPixelBufferRef>(
                    static_cast<uintptr_t>(nativeBuffer_->pointer));
                CVPixelBufferUnlockBaseAddress(pixelBuffer, kCVPixelBufferLock_ReadOnly);
#endif
                locked_ = false;
            }

            if (nativeBuffer_)
            {
                nativeBuffer_->release();
                nativeBuffer_.reset();
            }
        }

    private:
        std::optional<NativeBuffer> nativeBuffer_;
        cv::Mat image_;
        bool locked_ = false;
    };

} // namespace margelo::nitro::botsort