#ifndef __WS_SERVER_H
#define __WS_SERVER_H

#include<iostream>
#include "ws-listener.h"

namespace ws {

	class WebSocketServer {
	public:
		WebSocketServer();
		~WebSocketServer();

		int		init();
		int		run(std::string_view ip = "0.0.0.0", int port = 7070);
		int		stop();
		bool	isRunning() const { return mIsRunning; };

	private:
		bool	mIsRunning = false;
		std::unique_ptr<WebSocketListener> mListener;
	};

}

#endif // __WS_SERVER_H