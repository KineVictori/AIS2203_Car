
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
    {
        std::lock_guard lock(_frameMutex);
        _cap >> _frame;
    }

    if (_visionModel == visionUtils::VisionModel::POSE) {

        cv::Mat frame3;
        if (_frame.channels() == 4) {
            cv::cvtColor(_frame, frame3, cv::COLOR_BGRA2BGR);
        } else {
            frame3 = _frame;
        }

        cv::Mat blob = cv::dnn::blobFromImage(
            frame3,
            1.0 / 255.0,        // scale factor
            cv::Size(640, 640), // input size
            cv::Scalar(0.0, 0.0, 0.0), // mean (optional)
            true,               // swapRB (BGR->RGB)
            false               // crop
        );

        _net.setInput(blob);
        static std::vector<cv::Mat> outputs;
        _net.forward(outputs, _net.getUnconnectedOutLayersNames());

        auto people = visionUtils::decodeYoloPose(outputs);
        std::cout << "Num people: " << people.size() << std::endl;

        std::lock_guard lock(_frameMutex);
        for (const auto &person : people) {
            // Draw bounding box
            cv::rectangle(_frame, person.box, cv::Scalar(0, 255, 0), 2); // green box, thickness=2

            // Optional: draw confidence score
            std::string text = cv::format("%.2f", person.score);
            int baseLine = 0;
            cv::Size labelSize = cv::getTextSize(text, cv::FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);
            cv::Point labelOrigin(person.box.x, person.box.y - 5);
            cv::putText(_frame, text, labelOrigin, cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0,255,0), 1);

            // Optional: draw keypoints
            for (const auto &kp : person.kp) {
                cv::circle(_frame, kp.position, 3, cv::Scalar(0, 0, 255), -1); // red keypoints
            }
        }
    }
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

