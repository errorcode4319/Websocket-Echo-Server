#include"ws-listener.h"
#include"ws-secure-util.h"
#include"ws-connection.h"
#include<exception>
#include<sstream>

namespace ws {

    constexpr const char* HANDSHAKE_RESPONSE_INTERFACE =
        "HTTP/1.1 101 Switching Protocols\r\n"
        "Upgrade: websocket\r\n"
        "Connection: Upgrade\r\n"
        "Sec-WebSocket-Accept: ";

	WSListener::WSListener() {

	}

	WSListener::~WSListener() {
        release();
	}

	int WSListener::run(std::string_view ip, int port) {
        if (mIsRunning) return 1;
        int ret = socketInit(ip, port);
        if (ret != 0) return -1;
        mIsRunning = true;
        mListenerThread = std::thread{ &WSListener::listener_process, this };
        return 0;
	}

	int WSListener::socketInit(std::string_view ip, int port) {
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

	void WSListener::listener_process() {

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
                    /*
                        Send Client Socket to Main Service 
                    */
                    auto conn = std::make_shared<WSConnection>(clntSock);
                    conn->read();
                }
                else {
                    std::cout << "Handshake Failed" << std::endl;
                    clntSock._close();
                }
            }
            else {
                std::cout << "Not Websocket Connection Request" << std::endl;
            }
            std::cout << std::endl;
        }

	}

    bool WSListener::ws_handshake(tcp::Socket& clnt_sock, http::HttpMessage& req) {

        auto ws_key = req.getHeader("Sec-WebSocket-Key");
        auto ws_version = req.getHeader("Sec-WebSocket-Version");
        /*
        std::cout << "Sec-WebSocket-Key => " << ws_key << std::endl;
        std::cout << "Sec-WebSocket-Version => " << ws_version << std::endl;
        */ 
        auto ws_access = ws::gen_ws_access_key(ws_key);

        std::stringstream res;
        res << HANDSHAKE_RESPONSE_INTERFACE << ws_access << "\r\n\r\n" << std::endl;

        std::cout << "Handshake Response => " << std::endl;
        std::string res_msg = res.str();
        std::cout << res_msg << std::endl;

        int serv_bytes = clnt_sock._msg_push(res_msg);
        if (serv_bytes < int(res_msg.size())) {
            return false;
        }
        return true;
    }

    bool WSListener::wait_new_client(int timeout_ms) {
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

    void WSListener::release() {
        mIsRunning = false;
        if (mListenerThread.joinable())
            mListenerThread.join();
    }


}