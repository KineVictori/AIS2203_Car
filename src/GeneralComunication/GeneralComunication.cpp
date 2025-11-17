
#include <iostream>

#include "GeneralComunication/GeneralComunication.hpp"

GeneralComunication::GeneralComunication(const int port)
	: _loggingThread(&GeneralComunication::loggData, this), _server(port),
	_serverThread(&GeneralComunication::acceptConnections, this) {
}

GeneralComunication::~GeneralComunication() {
	_stopFlag = true;
	_server.close();

	if (_serverThread.joinable()) {
		_serverThread.join();
	}

	if (_loggingThread.joinable()) {
		_loggingThread.join();
	}

	for (auto &t : _connectionThreads) {
		if (t.joinable()) {
			t.join();
		}
	}
}

Data GeneralComunication::getData() {
	return _data;
}

void GeneralComunication::setData(const Data &data) {
	std::lock_guard lock(_dataMutex);
	_data = data;
}

void GeneralComunication::acceptConnections() {
	try {
		int threadIx = 1;
		while (!_stopFlag) {
			auto conn = _server.accept();
			_connectionThreads.emplace_back(&GeneralComunication::commHandler, this, std::move(conn), threadIx);

			// If no thread is master, the new thread will be master.
			if (_masterIx == 0) {
				_masterIx = threadIx;
			}

			threadIx++;
		}
	} catch (const std::exception &e) {}
}

void GeneralComunication::commHandler(std::unique_ptr<simple_socket::SimpleConnection> conn, int ix) {
	try {
		while (!_stopFlag) {
			std::vector<unsigned char> buffer(2048);
			int bytesRead = 0;
			while (bytesRead == 0 && !_stopFlag) {
				bytesRead = conn->read(buffer);
			}

			if (_stopFlag || (bytesRead < 1)) {
				break;
			}

			std::string msg(buffer.begin(), buffer.begin() + bytesRead);
			if (msg == "STOP") {
				break;
			}

			if (ix == _masterIx) {
				std::lock_guard lock(_dataMutex);
				_data.fromJson(msg);
			}

			std::string out;
			{
				std::lock_guard lock(_dataMutex);
				out = _data.toJson();
			}
			conn->write(out);
		}
	} catch (const std::exception &e) {
		std::cout << e.what() << std::endl;
	}; // Probably the connection being closed by the client..?

	if (ix == _masterIx) {
		_masterIx = 0;
	}
}

void GeneralComunication::loggData() {
	while (!_stopFlag) {
		{
			std::lock_guard lock(_coutMutex);
			std::cout << "Data: " << _data.toJson() << "         \r";
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}

