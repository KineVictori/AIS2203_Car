
#ifndef AIS2203_JETRACER_VISION_HPP
#define AIS2203_JETRACER_VISION_HPP

#include <vector>
#include <thread>

#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>

#include <simple_socket/TCPSocket.hpp>
#include <simple_socket/SimpleConnection.hpp>

#include <atomic>

#include "utils.hpp"

class Vision {
public:
    Vision();
    ~Vision();

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

    VisionModel _visionModel = POSE;
    bool _hasInitializedNet = false;
    cv::dnn::Net _net;
};


#endif //AIS2203_JETRACER_VISION_HPP