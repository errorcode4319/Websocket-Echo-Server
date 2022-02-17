#include"ws-listener.h"
#include<exception>
#include"http-message.h"

namespace ws {


	WebSocketListener::WebSocketListener() {

	}

	WebSocketListener::~WebSocketListener() {
        release();
	}

	int WebSocketListener::run(std::string_view ip, int port) {
        if (mIsRunning) return 1;
        int ret = socketInit(ip, port);
        if (ret != 0) return -1;
        mIsRunning = true;
        mListenerThread = std::thread{ &WebSocketListener::listener_process, this };
        return 0;
	}

	int WebSocketListener::socketInit(std::string_view ip, int port) {
        mListenerAddr = tcp::SocketAddr(ip, port);
        if (mListenerSocket._new() == INVALID_SOCKET) {
            return 1;
        }
        if (int value = 1; mListenerSocket._setOpt(SOL_SOCKET, SO_REUSEADDR, (void*)&value, sizeof(value)) < 0) {
            return 2;
        }
        if (mListenerSocket._bind(mListenerAddr.getCRef()) != 0) {
            return 3;
        }
        if (mListenerSocket._listen(LISTENER_BACK_LOG) != 0) {
            return 4;
        }
        FD_ZERO(&mFdSet);
        return 0;
	}

	void WebSocketListener::listener_process() {

        tcp::sock_t listener_sd = mListenerSocket.get();


        while (mIsRunning) {
            bool acceptable = wait_new_client(1000);
            
            if (!acceptable) 
                continue;

            auto [sock, addr] = mListenerSocket._accept();
            tcp::Socket clntSock(sock);
            tcp::SocketAddr clntAddr(addr);
            std::string rdBuf;
            clntSock._msg_pull(rdBuf, 4096);

            //std::cout << rdBuf << std::endl;
            http::HttpMessage req(rdBuf);
            std::cout << "---Received New Request------------" << std::endl;
            std::cout << "Target     => " << req.getTarget() << std::endl;
            std::cout << "Method     => " << req.getMethod() << std::endl;
            std::cout << "Host       => " << req.getHeader("Host") << std::endl;
            std::cout << "Connection => " << req.getHeader("Connection") << std::endl;
            std::cout << "Upgrade    => " << req.getHeader("Upgrade") << std::endl;
            std::cout << std::endl;

            clntSock._close();
        }

	}

    bool WebSocketListener::wait_new_client(int timeout_ms) {
        struct timeval to;
        to.tv_sec = timeout_ms / 1000;
        to.tv_usec = (timeout_ms % 1000) * 1000;
        tcp::sock_t sd = mListenerSocket.get();
        FD_SET(sd, &mFdSet);
        int ret_select = select(sd + 1, &mFdSet, nullptr, nullptr, &to);
        if (ret_select < 0) throw std::runtime_error("Listener Select Failed");
        if (ret_select == 0 || !FD_ISSET(sd, &mFdSet)) 
            return false;
        return true;
    }

    void WebSocketListener::release() {
        mIsRunning = false;
        if (mListenerThread.joinable())
            mListenerThread.join();
    }


}