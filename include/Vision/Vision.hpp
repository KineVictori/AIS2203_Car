
#ifndef AIS2203_JETRACER_VISION_HPP
#define AIS2203_JETRACER_VISION_HPP

#include <vector>
#include <thread>

#include <opencv2/opencv.hpp>
#include <simple_socket/TCPSocket.hpp>
#include <simple_socket/SimpleConnection.hpp>
#include <iostream>

#include <atomic>

class Vision {
public:
    Vision();
    ~Vision();

    bool isOkay;
    void update();
    cv::Mat getFrame();
    bool isFinished();

private:
    cv::VideoCapture _cap;
    cv::Mat _frame;

    simple_socket::TCPServer _server;
    std::thread _serverThread;
    std::vector<std::thread> _connectionThreads;

    void socketHandler(std::unique_ptr<simple_socket::SimpleConnection> conn);

    std::atomic<bool> _stopFlag = false;

};


#endif //AIS2203_JETRACER_VISION_HPP