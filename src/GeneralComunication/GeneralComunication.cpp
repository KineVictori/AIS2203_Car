
#include "GeneralComunication/GeneralComunication.hpp"

GeneralComunication::GeneralComunication(const int port)
	: _loggingThread(&GeneralComunication::loggData, this), _server(port),
	_serverThread(&GeneralComunication::acceptConnections, this) {

}

GeneralComunication::~GeneralComunication() {
	std::cout << "Stopping" << std::endl;
	_stopFlag = true;
	_server.close();

	std::cout << "_server.close()" << std::endl;
	if (_serverThread.joinable()) {
		std::cout << "server thread joinable" << std::endl;
		_serverThread.join();
		std::cout << "server thread joined" << std::endl;
	}

	if (_loggingThread.joinable()) {
		std::cout << "logging thread joinable" << std::endl;
		_loggingThread.join();
		std::cout << "logging thread joined" << std::endl;
	}

	std::cout << "checking conn threads" << std::endl;
	for (auto &t : _connectionThreads) {
		std::cout << "found conn thread" << std::endl;
		if (t.joinable()) {
			std::cout << "is joinable" << std::endl;
			t.join();
			std::cout << "is joined" << std::endl;
		}
	}

	std::cout << "finished destructor" << std::endl;
}

Data GeneralComunication::getData() {
	std::lock_guard lock(_dataMutex); // Probably dont need a lock here, since its read?
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
				std::cout << "Setting new master" << std::endl;
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

			if (_stopFlag) {
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
		std::cout << "Master retires" << std::endl;
	}
}

void GeneralComunication::loggData() {
	while (!_stopFlag) {
		{
			std::lock_guard lock(_coutMutex);
			std::cout << "Data: " << _data.toJson() << std::endl;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}

