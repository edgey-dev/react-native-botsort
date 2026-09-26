#pragma once

#include <map>
#include <numeric>
#include <string>

// .clang-format off
#include "DataType.hpp"
// #include "GmcParams.hpp"
// .clang-format on

#include <opencv2/core/eigen.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/videostab.hpp>
#include <opencv2/videostab/global_motion.hpp>

using namespace margelo::nitro::botsort;

class GMC_Algorithm
{
public:
    virtual ~GMC_Algorithm() = default;
    virtual HomographyMatrix
    apply(const cv::Mat &frame_raw,
          const std::vector<Detection> &detections) = 0;
};

class ORB_GMC : public GMC_Algorithm
{
public:
    explicit ORB_GMC(const OrbGMCConfig &orb_config);
    HomographyMatrix apply(const cv::Mat &frame_raw,
                           const std::vector<Detection> &detections) override;

private:
    void _load_params_from_config(const OrbGMCConfig &orb_config);

private:
    std::string _algo_name = "orb";
    float _downscale;
    cv::Ptr<cv::FeatureDetector> _detector;
    cv::Ptr<cv::DescriptorExtractor> _extractor;
    cv::Ptr<cv::DescriptorMatcher> _matcher;

    bool _first_frame_initialized = false;
    cv::Mat _prev_frame;
    std::vector<cv::KeyPoint> _prev_keypoints;
    cv::Mat _prev_descriptors;
    float _inlier_ratio, _ransac_conf;
    int _ransac_max_iters;
};

class ECC_GMC : public GMC_Algorithm
{
public:
    explicit ECC_GMC(const EccGMCConfig &config);
    HomographyMatrix apply(const cv::Mat &frame_raw,
                           const std::vector<Detection> &detections) override;

private:
    void _load_params_from_config(const EccGMCConfig &config);

private:
    std::string _algo_name = "ecc";
    float _downscale;
    int _max_iterations, _termination_eps;

    bool _first_frame_initialized = false;
    cv::Mat _prev_frame;
    cv::Size _gaussian_blur_kernel_size = cv::Size(3, 3);
    cv::TermCriteria _termination_criteria;
};

class SparseOptFlow_GMC : public GMC_Algorithm
{
public:
    explicit SparseOptFlow_GMC(const SofGMCConfig &config);
    HomographyMatrix apply(const cv::Mat &frame_raw,
                           const std::vector<Detection> &detections) override;

private:
    void _load_params_from_config(const SofGMCConfig &config);

private:
    std::string _algo_name = "sparseOptFlow";
    float _downscale;

    bool _first_frame_initialized = false;
    cv::Mat _prev_frame;
    std::vector<cv::Point2f> _prev_keypoints;

    // Parameters
    int _maxCorners, _blockSize, _ransac_max_iters;
    double _qualityLevel, _k, _minDistance;
    bool _useHarrisDetector;
    float _inlier_ratio, _ransac_conf;
};

// class OpenCV_VideoStab_GMC : public GMC_Algorithm
// {
// public:
//     explicit OpenCV_VideoStab_GMC(const VideoStabGMCConfig &config);
//     HomographyMatrix apply(const cv::Mat &frame_raw,
//                            const std::vector<Detection> &detections) override;

// private:
//     void _load_params_from_config(const VideoStabGMCConfig &config);

// private:
//     std::string _algo_name = "OpenCV_VideoStab";
//     float _downscale;
//     int _num_features;
//     bool _detections_masking;

//     cv::Mat _prev_frame;
//     cv::Mat _prev_homography;

//     cv::Ptr<cv::videostab::MotionEstimatorRansacL2> _motion_estimator;
//     cv::Ptr<cv::videostab::KeypointBasedMotionEstimator>
//         _keypoint_motion_estimator;
// };

class GlobalMotionCompensation
{
public:
    /**
     * @brief Construct a new Global Motion Compensation object
     * @param gmc_method Method for GMC algorithm
     * @param gmc_params Paramerters for GMC algorithm
     */
    explicit GlobalMotionCompensation(const GMC_Config &gmc_params);
    ~GlobalMotionCompensation() = default;

    /**
     * @brief Apply GMC algorithm to find homography matrix given frame and detections
     *
     * @param frame_raw Input frame
     * @param detections Detections in the frame
     * @return HomographyMatrix Predicted homography matrix
     */
    HomographyMatrix apply(const cv::Mat &frame_raw,
                           const std::vector<Detection> &detections);

// public:
//     static std::map<std::string, GMC_Method> GMC_method_map;

private:
    std::unique_ptr<GMC_Algorithm> _gmc_algorithm;
};