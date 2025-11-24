
#include "Vision/Vision.hpp"
#include "ModelEstimation/ModelEstimation.hpp"

void printDetections(const std::vector<cv::Rect>& boxes) {
    if (boxes.empty()) {
        std::cout << "No people detected.\n";
        return;
    }

    std::cout << "Detected " << boxes.size() << " people:\n";
    for (size_t i = 0; i < boxes.size(); ++i) {
        const auto& box = boxes[i];
        std::cout << "Person " << i+1 << ": "
                  << "x=" << box.x << ", y=" << box.y
                  << ", width=" << box.width << ", height=" << box.height
                  << "\n";
    }
}

int main() {

    Vision vision;

    auto model_path = std::to_string(DATA_PATH) + "/ONNXModels/yolo11n.onnx";
    ModelEstimation modelEstimation{model_path};

    while(!vision.isFinished()) {
        vision.update();
        auto frame = vision.getFrame();
        auto results = modelEstimation.detect(frame);
        printDetections(results);

        // If connected to a display, you may uncomment these lines.
        /*auto frame = vision.getFrame();
        if(frame.empty()) break;
        cv::imshow("CSI Camera", frame);
        if(cv::waitKey(1) == 27) break; // ESC to exit*/
    }

    return 0;
}