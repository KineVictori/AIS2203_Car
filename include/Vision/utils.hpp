
#ifndef AIS2203_CAR_UTILS_HPP
#define AIS2203_CAR_UTILS_HPP

#include <opencv2/opencv.hpp>

namespace visionUtils {
    enum VisionModel {
        NONE,
        POSE
    };

    struct Keypoint {
        Keypoint(float x, float y, float score);

        cv::Point2d position{};
        float conf{0.0};
    };

    struct Person {
        Person(cv::Rect2i _box, float _score, std::vector<Keypoint> &_kp);

        cv::Rect2i box{};
        float score{0.0};
        std::vector<Keypoint> kp{};
    };

    inline static float clamp(float val, float min, float max) {
        return val > min ? (val < max ? val : max) : min;
    }

    inline std::vector<Person> decodeYoloPose(std::vector<cv::Mat> outputs) {
        const cv::Size modelShape = cv::Size(640, 640);


        const int channels = outputs[0].size[2];
        const int anchors = outputs[0].size[1];
        outputs[0] = outputs[0].reshape(1, anchors);
        cv::Mat output = outputs[0].t();


        std::vector<cv::Rect> bboxList;
        std::vector<float> scoreList;
        std::vector<int> indicesList;
        std::vector<std::vector<Keypoint>> kpList;

        for (int i = 0; i < channels; i++) {
            auto row_ptr = output.row(i).ptr<float>();
            auto bbox_ptr = row_ptr;
            auto score_ptr = row_ptr + 4;
            auto kp_ptr = row_ptr + 5;

            float score = *score_ptr;
            if (score > 0.7f) {
                float x = *bbox_ptr++;
                float y = *bbox_ptr++;
                float w = *bbox_ptr++;
                float h = *bbox_ptr;

                float x0 = clamp((x - 0.5f * w) * 1.0F, 0.f, float(modelShape.width));
                float y0 = clamp((y - 0.5f * h) * 1.0F, 0.f, float(modelShape.height));
                float x1 = clamp((x + 0.5f * w) * 1.0F, 0.f, float(modelShape.width));
                float y1 = clamp((y + 0.5f * h) * 1.0F, 0.f, float(modelShape.height));

                cv::Rect_<float> bbox;
                bbox.x = x0;
                bbox.y = y0;
                bbox.width = x1 - x0;
                bbox.height = y1 - y0;

                std::vector<Keypoint> kps;
                for (int k = 0; k < 17; k++) {
                    float kps_x = (*(kp_ptr + 3 * k));
                    float kps_y = (*(kp_ptr + 3 * k + 1));
                    float kps_s = *(kp_ptr + 3 * k + 2);
                    kps_x = clamp(kps_x, 0.f, float(modelShape.width));
                    kps_y = clamp(kps_y, 0.f, float(modelShape.height));

                    kps.emplace_back(kps_x, kps_y, kps_s);
                }

                bboxList.push_back(bbox);
                scoreList.push_back(score);
                kpList.push_back(kps);
            }
        }

        cv::dnn::NMSBoxes(
                bboxList,
                scoreList,
                0.7f,
                0.5f,
                indicesList
        );

        std::vector<Person> result{};
        for (auto &i: indicesList) {
            result.emplace_back(bboxList[i], scoreList[i], kpList[i]);
        }

        return result;
    }
}

#endif //AIS2203_CAR_UTILS_HPP