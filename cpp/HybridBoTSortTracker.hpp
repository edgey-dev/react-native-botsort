#pragma once

#include <HybridBoTSortTrackerSpec.hpp>
// #include <motcpp/trackers/botsort.hpp>
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

        void initialize(const BoTSORTConfig &trackerConfig, const std::optional<GMC_Config> &gmcConfig) override;

        std::vector<TrackedObject> updateWithFrame(
            const std::shared_ptr<margelo::nitro::camera::HybridFrameSpec> &frame,
            const std::vector<BoundingBox> &detections) override;

    private:
        // Fix: Instantiate utilizing the exact factory base or direct tracker interface type
        std::unique_ptr<BoTSORT> tracker;
    };

} // namespace margelo::nitro::botsort
