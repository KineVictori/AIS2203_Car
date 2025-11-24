
#include "Vision/Vision.hpp"
#include "ModelEstimation/ModelEstimation.hpp"
#include "ModelEstimation/inference.hpp"


int main() {

    Vision vision;

    std::string path = std::string(DATA_PATH) + "/ONNXModels/yolov8s.onnx";
    Inference inference{path, {640, 480}};

    while(!vision.isFinished()) {
        vision.update();
        auto frame = vision.getFrame();

        try {
            if (frame.channels() == 4) { cv::cvtColor(frame, frame, cv::COLOR_BGRA2BGR); }

            std::vector<Detection> output = inference.runInference(frame);
            int detections = output.size();
            std::cout << "Number of detections:" << detections << std::endl;

            for (int i = 0; i < detections; ++i)
            {
                Detection detection = output[i];
                cv::Rect box = detection.box;
                std::string classString = detection.className + ' ' + std::to_string(detection.confidence).substr(0, 4);
                std::cout << classString << " | " << box.x << ", " << box.y << std::endl;
            }
        } catch (...) {}


        // If connected to a display, you may uncomment these lines.
        // auto frame = vision.getFrame();
        if(frame.empty()) break;
        cv::imshow("CSI Camera", frame);
        if(cv::waitKey(1) == 27) break; // ESC to exit
    }

    return 0;
}