
#include "GeneralComunication/GeneralComunication.hpp"

GeneralComunication::GeneralComunication(int port)
	: _server(port), _serverThread(&GeneralComunication::acceptConnections, this){

}

GeneralComunication::~GeneralComunication() {
	std::cout << "Stopping" << std::endl;
	_stopFlag = true;
	_server.close(); // Probably not good since this happens at the same time as _server.accept()

	std::cout << "_server.close()" << std::endl;
	if (_serverThread.joinable()) {
		std::cout << "server thread joinable" << std::endl;
		_serverThread.join();
		std::cout << "server thread joined" << std::endl;
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
				_masterIx = threadIx;
			}

			threadIx++;
		}
	} catch (const std::exception &e) {}
}

void GeneralComunication::commHandler(std::unique_ptr<simple_socket::SimpleConnection> conn, int ix) {
	std::vector<unsigned char> buffer(2048);
	try {
		while (!_stopFlag) {
			int bytesRead = 0;
			while (bytesRead == 0 && !_stopFlag) {
				bytesRead += conn->read(buffer);
			}

			if (bytesRead == -1 || _stopFlag) {
				break;
			}

			std::string msg(buffer.begin(), buffer.begin() + bytesRead);

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
			buffer.clear();
		}
	} catch (const std::exception &e) {}; // Probably the connection being closed by the client..?

	if (ix == _masterIx) {
		_masterIx = 0;
	}
}
