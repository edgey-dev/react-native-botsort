#pragma once

#include <HybridBoTSortTrackerSpec.hpp>
#include "BoTSORT.hpp"
#include <memory>
#include <string>
#include <vector>
#include "DataType.hpp"

namespace margelo::nitro::botsort
{

    class HybridBoTSortTracker : public HybridBoTSortTrackerSpec
    {
    public:
        HybridBoTSortTracker();
        ~HybridBoTSortTracker() override = default;

        void initialize(const std::optional<BoTSORTConfig> &trackerConfig, const std::optional<GMC_Config> &gmcConfig) override;

        std::vector<TrackedObject> track(
            const std::shared_ptr<margelo::nitro::camera::HybridFrameSpec> &frame,
            const std::vector<BoundingBox> &detections) override;

    private:
        std::unique_ptr<BoTSORT> tracker;
    };

} // namespace margelo::nitro::botsort
