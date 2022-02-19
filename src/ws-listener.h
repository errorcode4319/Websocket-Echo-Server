#ifndef __WS_LISTENER_H
#define __WS_LISTENER_H

#include"tcp-socket.h"
#include"http-message.h"
#include<iostream>
#include<thread>
#include<mutex>

namespace ws {

	constexpr int LISTENER_BACK_LOG = 512;

	class WebSocketListener {

	public:
		WebSocketListener();
		~WebSocketListener();

		int		run(std::string_view ip, int port);
		bool	isRunning() const { return mIsRunning; }

	private:
		int		socketInit(std::string_view ip, int port);
		bool	ws_handshake(tcp::Socket& clnt_sock, http::HttpMessage& req);
		void	listener_process();
		bool	wait_new_client(int timeout_ms = 1000);
		void	release();

		bool	mIsRunning = false;
		tcp::Socket		mListenerSocket;
		tcp::SocketAddr mListenerAddr;
		std::thread		mListenerThread;

		fd_set			mFdSet;	//select  
	};

}

#endif // __WS_LISTENER_H