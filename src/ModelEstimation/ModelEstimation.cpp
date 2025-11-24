
#include "ModelEstimation/ModelEstimation.hpp"

ModelEstimation::PersonDetector(const std::string &modelPath, float confThreshold, float nmsThreshold)
    : confThreshold(confThreshold), nmsThreshold(nmsThreshold) {

    auto path = std::to_string(DATA_PATH) + modelPath;
    net = cv::dnn::readNetFromONNX(path);
    net.setPreferableBackend(cv::dnn::DNN_BACKEND_CUDA);
    net.setPreferableTarget(cv::dnn::DNN_TARGET_CUDA);
}

std::vector<cv::Rect> ModelEstimation::detect(const cv::Mat &frame) {
    cv::Mat blob;
    cv::dnn::blobFromImage(frame, blob, 1/255.0, cv::Size(640, 640), cv::Scalar(), true, false);
    net.setInput(blob);

    std::vector<cv::Mat> outputs;
    net.forward(outputs, net.getUnconnectedOutLayersNames());

    std::vector<cv::Rect> boxes;
    std::vector<float> confidences;

    // Typical YOLO output format: [x, y, w, h, conf, class_scores...]
    for (int i = 0; i < outputs[0].rows; i++) {
        float* data = (float*)outputs[0].data + i * outputs[0].cols;
        float confidence = data[4];
        if (confidence >= confThreshold) {
            cv::Mat scores(1, outputs[0].cols - 5, CV_32FC1, data + 5);
            cv::Point classIdPoint;
            double maxClassScore;
            cv::minMaxLoc(scores, 0, &maxClassScore, 0, &classIdPoint);
            if (classIdPoint.x == 0) { // class 0 = person in COCO
                int cx = (int)(data[0] * frame.cols);
                int cy = (int)(data[1] * frame.rows);
                int w  = (int)(data[2] * frame.cols);
                int h  = (int)(data[3] * frame.rows);
                int x  = cx - w/2;
                int y  = cy - h/2;
                boxes.push_back(cv::Rect(x, y, w, h));
                confidences.push_back(confidence);
            }
        }
    }

    std::vector<int> indices;
    cv::dnn::NMSBoxes(boxes, confidences, confThreshold, nmsThreshold, indices);

    std::vector<cv::Rect> result;
    for (int idx : indices) {
        result.push_back(boxes[idx]);
    }

    return result;
}

