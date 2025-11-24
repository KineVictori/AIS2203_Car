
#include "Vision/Vision.hpp"

void printBlobSettings(const BlobSettings& s) {
    std::cout << "Blob Settings:\n";
    std::cout << "  scale   : " << s.scale << "\n";
    std::cout << "  size    : " << s.size.width << " x " << s.size.height << "\n";
    std::cout << "  mean    : ["
              << s.mean[0] << ", "
              << s.mean[1] << ", "
              << s.mean[2] << "]\n";
    std::cout << "  swapRB  : " << (s.swapRB ? "true" : "false") << "\n";
    std::cout << "  crop    : " << (s.crop ? "true" : "false") << "\n";
}


int main() {

    Vision vision;

    for (int i = 0; i < 100; i++) {
        if (!vision.isFinished()) {
            vision.update();
        }
    }

    auto frame = vision.getFrame();
    auto v = getBlobSettings(frame);
    printBlobSettings(v);

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