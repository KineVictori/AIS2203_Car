
#include <opencv2/opencv.hpp>
#include "Vision/Vision.hpp"

int main() {

    Vision vision;

    while(!vision.isFinished()) {
        vision.update();

        // If connected to a display, you may uncomment these lines.
        /*auto frame = vision.getFrame();
        if(frame.empty()) break;
        cv::imshow("CSI Camera", frame);
        if(cv::waitKey(1) == 27) break; // ESC to exit*/
    }
    return 0;
}