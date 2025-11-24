
#ifndef AIS2203_CAR_MODELESTIMATION_HPP
#define AIS2203_CAR_MODELESTIMATION_HPP

#include <string>
#include <vector>

#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>

class ModelEstimation {
public:
    ModelEstimation(const std::string& modelPath, float confThreshold = 0.5, float nmsThreshold = 0.4);
    std::vector<cv::Rect> detect(const cv::Mat& frame);

private:
    cv::dnn::Net net;
    float confThreshold;
    float nmsThreshold;
};


#endif //AIS2203_CAR_MODELESTIMATION_HPP