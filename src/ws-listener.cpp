#include"ws-listener.h"
#include<exception>

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
            auto upgrade = req.getHeader("Upgrade");
            auto connection = req.getHeader("Connection");
            
            if (upgrade == "websocket" && connection == "Upgrade") {
                std::cout << "WebSocket Connection Request" << std::endl;
                if (ws_handshake(clntSock, req)) {
                    std::cout << "Handshake Success" << std::endl;
                }
                else {
                    std::cout << "Handshake Failed" << std::endl;
                }
            }
            else {
                std::cout << "Not Websocket Connection Request" << std::endl;
            }
            std::cout << std::endl;

            clntSock._close();
        }

	}

    bool WebSocketListener::ws_handshake(tcp::Socket& clnt_sock, http::HttpMessage& req) {


        auto ws_key = req.getHeader("Sec-WebSocket-Key");
        auto ws_version = req.getHeader("Sec-WebSocket-Version");

        std::cout << "Sec-WebSocket-Key => " << ws_key << std::endl;
        std::cout << "Sec-WebSocket-Version => " << ws_version << std::endl;

        ws_key += WS_KEY_POSTFIX;
        // ws_key -> SHA1 -> Base64 Enc -> ws_access!!  



        return true;
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