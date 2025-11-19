
#include "Vision/Vision.hpp"

int main() {

    std::cout << "Running" << std::endl;
    Vision vision;
    std::cout << "Created" << std::endl;

    while(!vision.isFinished()) {
        vision.update();

        // If connected to a display, you may uncomment these lines.
        /*auto frame = vision.getFrame();
        if(frame.empty()) break;
        cv::imshow("CSI Camera", frame);
        if(cv::waitKey(1) == 27) break; // ESC to exit*/
    }

    std::cout << "Finished" << std::endl;
    std::cout << "B: " << vision.isFinished() << std::endl;

    return 0;
}