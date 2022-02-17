#include "ws-server.h"

namespace ws {

	WebSocketServer::WebSocketServer() {

	}

	WebSocketServer::~WebSocketServer() {
		stop();
	}

	int WebSocketServer::init() {
		mListener = std::make_unique<WebSocketListener>();

		return 0;
	}

	int WebSocketServer::run(std::string_view ip, int port) {
		mListener->run(ip, port);
		return 0;
	}

	int WebSocketServer::stop() {
		mListener.reset();
		return 0;
	}

}