
#ifndef AIS2203_CAR_UTILS_HPP
#define AIS2203_CAR_UTILS_HPP

namespace visionUtils {
    enum VisionModel {
        NONE,
        POSE
    };

    struct Keypoint {
        float x;
        float y;
        float score;
    };

    struct Person {
        cv::Rect box;
        std::vector<Keypoint> keypoints; // size = 17
        float score;
    };

    inline std::vector<Person> decodeYoloPose(
        const cv::Mat& output,
        const cv::Size& origSize,   // original frame size
        float confThreshold = 0.7f,
        float nmsThreshold = 0.5f)
    {
        int numPred = output.size[2];  // 8400
        int numVals = output.size[1];  // 56

        const float* data = (float*)output.data;

        std::vector<cv::Rect> boxes;
        std::vector<float> scores;
        std::vector<std::vector<Keypoint>> kps;

        float scaleX = origSize.width / 640.0f;
        float scaleY = origSize.height / 640.0f;

        for (int i = 0; i < numPred; i++) {
            const float* p = data + i * numVals;

            float score = p[0];
            if (score < confThreshold)
                continue;

            // box
            float cx = p[1] * scaleX;
            float cy = p[2] * scaleY;
            float w  = p[3] * scaleX;
            float h  = p[4] * scaleY;

            int x = cx - w / 2;
            int y = cy - h / 2;

            boxes.emplace_back(x, y, w, h);
            scores.push_back(score);

            // keypoints
            std::vector<Keypoint> personKP;
            personKP.reserve(17);

            for (int k = 0; k < 17; k++) {
                float kx = p[5 + k * 3 + 0] * scaleX;
                float ky = p[5 + k * 3 + 1] * scaleY;
                float ks = p[5 + k * 3 + 2];

                personKP.push_back({kx, ky, ks});
            }

            kps.push_back(personKP);
        }

        // Apply NMS
        std::vector<int> nmsIdx;
        cv::dnn::NMSBoxes(boxes, scores, confThreshold, nmsThreshold, nmsIdx);

        std::vector<Person> people;
        for (int idx : nmsIdx) {
            people.push_back(Person{
                boxes[idx],
                kps[idx],
                scores[idx]
            });
        }

        return people;
    }
}

#endif //AIS2203_CAR_UTILS_HPP