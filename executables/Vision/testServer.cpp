
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
                cv::Scalar color = detection.color;

                // Detection box
                cv::rectangle(frame, box, color, 2);

                // Detection box text
                std::string classString = detection.className + ' ' + std::to_string(detection.confidence).substr(0, 4);
                cv::Size textSize = cv::getTextSize(classString, cv::FONT_HERSHEY_DUPLEX, 1, 2, 0);
                cv::Rect textBox(box.x, box.y - 40, textSize.width + 10, textSize.height + 20);

                std::cout << classString << std::endl;
                cv::rectangle(frame, textBox, color, cv::FILLED);
                cv::putText(frame, classString, cv::Point(box.x + 5, box.y - 10), cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(0, 0, 0), 2, 0);
            }
            vision.setExportFrame(frame);
        } catch (...) {}


        // If connected to a display, you may uncomment these lines.
        /*auto frame = vision.getFrame();
        if(frame.empty()) break;
        cv::imshow("CSI Camera", frame);
        if(cv::waitKey(1) == 27) break; // ESC to exit*/
    }

    return 0;
}