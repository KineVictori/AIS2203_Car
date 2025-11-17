
#ifndef AIS2203_CAR_GENERALCOMUNICATION_HPP
#define AIS2203_CAR_GENERALCOMUNICATION_HPP

#include <vector>

#include <mutex>
#include <atomic>
#include <thread>

#include "simple_socket/TCPSocket.hpp"
#include <simple_socket/SimpleConnection.hpp>

#include "Data.hpp"

class GeneralComunication {
public:
	GeneralComunication(int port);
	~GeneralComunication();

	Data getData();
	void setData(const Data &data);

private:
	void commHandler(std::unique_ptr<simple_socket::SimpleConnection> conn, int ix);
	void acceptConnections();
	void loggData();

	std::thread _loggingThread;
	std::mutex _coutMutex;

	std::mutex _dataMutex;
	Data _data;

	std::atomic_bool _stopFlag = false;

	simple_socket::TCPServer _server;
	std::thread _serverThread;

	std::atomic_int _masterIx = 0;
	std::vector<std::thread> _connectionThreads;
};


#endif //AIS2203_CAR_GENERALCOMUNICATION_HPP