
#include "Vision/Vision.hpp"
#include "Vision/utils.hpp"


Vision::Vision(): _server(simple_socket::TCPServer(45678)),
    _stoppingThread(&Vision::listenForUserStop, this),
    _serverThread(&Vision::listenForConnection, this) {

    std::string pipeline = "nvarguscamerasrc ! video/x-raw(memory:NVMM), width=1280, height=720, framerate=30/1 ! "
                           "nvvidconv ! videoconvert ! appsink";
    _cap = cv::VideoCapture(pipeline, cv::CAP_GSTREAMER);
    if(!_cap.isOpened()) {
        std::cerr << "Failed to open CSI camera\n";
        isOkay = false;
    }

    _net = cv::dnn::readNetFromONNX(static_cast<std::string>(DATA_PATH) + "/ONNXModels/yolo11n-pose.onnx");
    _net.setPreferableBackend(cv::dnn::DNN_BACKEND_CUDA);
    _net.setPreferableTarget(cv::dnn::DNN_TARGET_CUDA);
}

Vision::~Vision() {
    _stopFlag = true;
    _cap.release();

    for (auto &t : _connectionThreads) {
        if (t.joinable()) {
            t.join();
        }
    }

    if (_serverThread.joinable()) {
        _serverThread.join();
    }

    if (_stoppingThread.joinable()) {
        _stoppingThread.join();
    }

}

void Vision::update() {
    std::lock_guard lock(_frameMutex);
    _cap >> _frame;

    auto v = getBlobSettings(_frame);
    printBlobSettings(v);
}

cv::Mat Vision::getFrame() {
    return _frame;
}

void Vision::socketHandler(std::unique_ptr<simple_socket::SimpleConnection> conn) {
    std::vector<unsigned char> buffer(1024);
    const auto bytesRead = conn->read(buffer);
    std::string msg(buffer.begin(), buffer.begin() + bytesRead);

    int mode = 0;
    if (msg == "AUTO") {
        mode = 1;
    } // TODO: implement mode switching (example AUTO: frames as fast as possible, one: just get one frame)

    try {
        while (!_stopFlag) {
            auto frame = getFrame();
            std::vector<uchar> buf;
            cv::imencode(".jpg", frame, buf);

            int numBytes = buf.size();

            auto b = conn->write(reinterpret_cast<char*>(&numBytes), sizeof(numBytes)); // send size as int
            if (!b) {break;}
            b = conn->write(reinterpret_cast<char*>(buf.data()), buf.size());      // send raw bytes
            if (!b) {break;}
        }
    } catch (std::exception &e) {
        // Probably conn has closed from client side, should probably do this in a better way.
        // Could probably get client to send a msg "END" or whatever, then close.
        // TODO: Handle better connection shutdown.
    }

}

bool Vision::isFinished() {
    return _stopFlag;
}

void Vision::listenForUserStop() {
    return; // Running code on boot (by systemd), std::cin will immideatly return.

    std::cout << "Press ENTER to stop server.\n";
    std::cin.get();

    _stopFlag = true;
    _server.close();
}

void Vision::listenForConnection() {
    try {
        while (!_stopFlag) {
            auto conn = _server.accept();
            _connectionThreads.emplace_back(&Vision::socketHandler, this, std::move(conn));
        }
    } catch (const std::exception &e) {
    }
}

