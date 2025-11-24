
#ifndef AIS2203_JETRACER_VISION_HPP
#define AIS2203_JETRACER_VISION_HPP

#include <vector>
#include <thread>

#include <opencv2/opencv.hpp>

#include <simple_socket/TCPSocket.hpp>
#include <simple_socket/SimpleConnection.hpp>

#include <atomic>

class Vision {
public:
    Vision();
    ~Vision();

    void update();
    cv::Mat getFrame();
    bool isFinished();

    void setExportFrame(const cv::Mat &frame);

private:
    std::mutex _frameMutex;
    cv::VideoCapture _cap;
    cv::Mat _frame;
    cv::Mat exportFrame;
    std::mutex _exportMutex;

    simple_socket::TCPServer _server;
    std::thread _serverThread;
    std::thread _stoppingThread;
    std::vector<std::thread> _connectionThreads;

    void socketHandler(std::unique_ptr<simple_socket::SimpleConnection> conn);
    void listenForConnection();
    void listenForUserStop();

    std::atomic<bool> _stopFlag = false;
};


#endif //AIS2203_JETRACER_VISION_HPP