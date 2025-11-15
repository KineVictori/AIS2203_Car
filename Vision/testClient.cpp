
#include <opencv2/opencv.hpp>
#include "visionClient/visionClient.hpp"

int main() {

    visionClient client{"10.22.22.165", 45678};

    while (true) {
        auto maybeFrame = client.getFrame();
        if (maybeFrame.has_value()) {
            cv::imshow("frame", maybeFrame.value());
        }

        if (cv::waitKey(1) == 27) {
            break;
        }
    }

}