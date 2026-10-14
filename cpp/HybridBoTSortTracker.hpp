#pragma once

#include <HybridBoTSortTrackerSpec.hpp>
#include "BoTSORT.hpp"
#include <memory>
#include <string>
#include <vector>
#include "DataType.hpp"

using VisionCameraFrame = std::shared_ptr<margelo::nitro::camera::HybridFrameSpec>;

namespace margelo::nitro::botsort
{

    class HybridBoTSortTracker : public HybridBoTSortTrackerSpec
    {
    public:
        HybridBoTSortTracker();
        ~HybridBoTSortTracker() override = default;

        void initialize(const std::optional<BoTSORTConfig> &trackerConfig, const std::optional<GMC_Config> &gmcConfig) override;

        std::vector<TrackedObject> track(
            const VisionCameraFrame &frame,
            const std::vector<BoundingBox> &detections, std::optional<bool> skip_gmc) override;

        std::vector<TrackedObject> predict_only(const std::optional<VisionCameraFrame> &frame) override;

    private:
        std::unique_ptr<BoTSORT> tracker;
    };

} // namespace margelo::nitro::botsort
