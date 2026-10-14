// cpp/HybridBoTSortTracker.cpp
#include "HybridBoTSortTracker.hpp"
#include "CameraFrameView.hpp"
#include <opencv2/imgproc.hpp>
#include <Eigen/Dense>
#include <algorithm> // For std::max
#include <cstdint>
#include <stdexcept>


namespace margelo::nitro::botsort
{

    HybridBoTSortTracker::HybridBoTSortTracker() : HybridObject(TAG), HybridBoTSortTrackerSpec() {}

    void HybridBoTSortTracker::initialize(const std::optional<BoTSORTConfig> &trackerConfig, const std::optional<GMC_Config> &gmcConfig)
    {
        tracker = std::make_unique<BoTSORT>(trackerConfig, gmcConfig);
    }

    std::vector<TrackedObject> HybridBoTSortTracker::track(
        const VisionCameraFrame &frame,
        const std::vector<BoundingBox> &detections, std::optional<bool> skip_gmc)
    {
        if (!tracker)
            return {};

        CameraFrameView<margelo::nitro::camera::HybridFrameSpec> frameView(frame);
        if (!frameView)
            return {};

        const cv::Mat &matFrame = frameView.image();

        std::vector<Detection> tracker_detections;
        tracker_detections.reserve(detections.size());
        for (auto &detection : detections)
        {
            Detection tracker_detection;
            tracker_detection.bbox_tlwh =
                cv::Rect_<float>(detection.tx, detection.ty, detection.width, detection.height);
            tracker_detection.class_id = static_cast<int>(detection.classId);
            tracker_detection.confidence = detection.confidence;
            tracker_detections.push_back(tracker_detection);
        }

        auto tracks = tracker->track(tracker_detections, matFrame, skip_gmc);
        frameView.release();

        std::vector<TrackedObject> nativeResults;
        nativeResults.reserve(tracks.size());
        for (auto &track : tracks)
        {
            auto bbox = track->get_tlwh();
            nativeResults.emplace_back(
                static_cast<double>(track->track_id),
                static_cast<double>(bbox[0]),
                static_cast<double>(bbox[1]),
                static_cast<double>(bbox[2]),
                static_cast<double>(bbox[3]),
                static_cast<double>(track->get_class_id()));
        }

        return nativeResults;
    }

    std::vector<TrackedObject> HybridBoTSortTracker::predict_only(const std::optional<VisionCameraFrame> &frame)
    {
        if (!tracker)
            return {};
        std::vector<std::shared_ptr<Track>> tracks;

        if (!frame)
        {
            tracks = tracker->predict_only(std::nullopt);
        }
        else
        {
            CameraFrameView<margelo::nitro::camera::HybridFrameSpec> frameView(frame.value());
            if (!frameView)
            {
                tracks = tracker->predict_only(std::nullopt);
            }
            else
            {
                cv::Mat matFrame = frameView.image();
                tracks = tracker->predict_only(matFrame);
                frameView.release();
            }
        }

        std::vector<TrackedObject> nativeResults;
        nativeResults.reserve(tracks.size());
        for (auto &track : tracks)
        {
            auto bbox = track->get_tlwh();
            nativeResults.emplace_back(
                static_cast<double>(track->track_id),
                static_cast<double>(bbox[0]),
                static_cast<double>(bbox[1]),
                static_cast<double>(bbox[2]),
                static_cast<double>(bbox[3]),
                static_cast<double>(track->get_class_id()));
        }

        return nativeResults;
    }

} // namespace margelo::nitro::botsort