#include "ws-server.h"

namespace ws {

	WSServer::WSServer() {

	}

	WSServer::~WSServer() {
		stop();
	}

	int WSServer::init() {
		mListener = std::make_unique<WSListener>();

		return 0;
	}

	int WSServer::run(std::string_view ip, int port) {
		mListener->run(ip, port);
		return 0;
	}

	int WSServer::stop() {
		mListener.reset();
		return 0;
	}

}