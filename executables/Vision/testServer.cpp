
#include "Vision/Vision.hpp"

struct BlobSettings {
    double scale;
    cv::Size size;
    cv::Scalar mean;
    bool swapRB;
    bool crop;
};

BlobSettings getBlobSettings(const cv::Mat& img) {
    BlobSettings s;

    // Scale: normalize to [0,1]
    s.scale = 1.0 / 255.0;

    // Size: commonly use the input image size (for fully-conv models)
    // or you may want to resize to a square. Here we keep it unchanged:
    s.size = img.size();

    // Mean: common default (Imagenet)
    s.mean = cv::Scalar(0.485, 0.456, 0.406);

    // Swap BGR->RGB (most ONNX models expect RGB)
    s.swapRB = true;

    // Do not crop by default
    s.crop = false;

    return s;
}

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

    for (int i = 0; i < 10; i++) {
        vision.update();
    }

    auto f = vision.getFrame();
    printBlobSettings(getBlobSettings(f));

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