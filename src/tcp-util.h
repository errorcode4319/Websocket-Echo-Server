#ifndef __HTTP_MJPEG_TCP_TCP_UTIL_HPP
#define __HTTP_MJPEG_TCP_TCP_UTIL_HPP


#include<iostream>
#include<string>
#include<cstring>
#include<cstdint>
#include<functional>

#if defined(_MSC_VER)    /* Win socket */
#define _WINSOCKAPI_
#include<Windows.h>
#include<WinSock2.h>
#include<WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#elif defined(__linux__) /* Unix Socket */
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <signal.h>
#endif 

namespace tcp {

#if defined(_MSC_VER)
    using sock_t = SOCKET;
    using sockaddr_t = SOCKADDR_IN;
#elif defined(__linux__)
    using sock_t = int;
    using sockaddr_t = struct sockaddr_in;
    using SOCKADDR = struct sockaddr;
#define INVALID_SOCKET -1 
#endif 

    struct sd_pkg_t {
        sock_t      sd;
        sockaddr_t  addr;
    };

    class SocketAddr {
    public:
        SocketAddr(const sockaddr_t& addr = sockaddr_t{}) : mAddr(addr) {}
        SocketAddr(std::string_view ip, int port, int af = AF_INET) {
            setup(ip, uint16_t(port), af);
        }
        void setup(std::string_view ip, uint16_t port, int af);
        void reset();

    public: // Getter&Setter
        void setIP(std::string_view ip);
        std::string getIP();
        void setPort(uint16_t port);
        uint16_t getPort();
        void setAF(int af);
        int  getAF();

    public: // Raw Data Handling
        void        setRawData(sockaddr_t addr) { mAddr = addr; }
        sockaddr_t  getRawData() { return mAddr; }
        sockaddr_t& getRef() { return std::ref(mAddr); }
        const sockaddr_t& getCRef() { return std::cref(mAddr); }
    private:
        sockaddr_t mAddr{};
    };

}


#endif // __TCP_UTIL_H 