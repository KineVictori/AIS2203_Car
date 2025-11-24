
#ifndef AIS2203_JETRACER_VISION_HPP
#define AIS2203_JETRACER_VISION_HPP

#include <vector>
#include <thread>

#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>

#include <simple_socket/TCPSocket.hpp>
#include <simple_socket/SimpleConnection.hpp>

#include <atomic>

struct BlobSettings {
    double scale;
    cv::Size size;
    cv::Scalar mean;
    bool swapRB;
    bool crop;
};

enum VisionModel {
    NONE,
    POSE
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

class Vision {
public:
    Vision();
    ~Vision();

    bool isOkay;
    void update();
    cv::Mat getFrame();
    bool isFinished();

private:
    std::mutex _frameMutex;
    cv::VideoCapture _cap;
    cv::Mat _frame;

    simple_socket::TCPServer _server;
    std::thread _serverThread;
    std::thread _stoppingThread;
    std::vector<std::thread> _connectionThreads;

    void socketHandler(std::unique_ptr<simple_socket::SimpleConnection> conn);
    void listenForConnection();
    void listenForUserStop();

    std::atomic<bool> _stopFlag = false;


    cv::dnn::Net _net;
};


#endif //AIS2203_JETRACER_VISION_HPP