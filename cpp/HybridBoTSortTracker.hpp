#pragma once

#include <HybridBoTSortTrackerSpec.hpp>
#include <motcpp/trackers/botsort.hpp>
#include <memory>
#include <string>
#include <vector>

namespace margelo::nitro::botsort
{

    class HybridBoTSortTracker : public HybridBoTSortTrackerSpec
    {
    public:
        HybridBoTSortTracker();
        ~HybridBoTSortTracker() override = default;

        void initialize(const std::string &reidModelPath, bool useGpu) override;

        std::vector<TrackedObject> updateWithFrame(
            const std::shared_ptr<margelo::nitro::camera::HybridFrameSpec> &frame,
            const std::vector<BoundingBox> &detections) override;

    private:
        // Fix: Instantiate utilizing the exact factory base or direct tracker interface type
        std::unique_ptr<motcpp::trackers::BotSort> tracker;
    };

} // namespace margelo::nitro::botsort
