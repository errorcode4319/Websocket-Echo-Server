#include "tcp-util.h"



namespace tcp {

    void        SocketAddr::setup(std::string_view ip, uint16_t port, int af) {
        reset();
        setAF(af);
        setIP(ip);
        setPort(port);
    }
    void        SocketAddr::reset() {
        mAddr = sockaddr_t{};
    }
    void        SocketAddr::setIP(std::string_view ip) {
        inet_pton(mAddr.sin_family, ip.data(), &mAddr.sin_addr.s_addr);
    }
    std::string SocketAddr::getIP() {
        std::string buf(16, 0);
        return std::string(inet_ntop(mAddr.sin_family, &mAddr.sin_addr, &buf[0], 16));
    }
    void        SocketAddr::setPort(uint16_t port) {
        mAddr.sin_port = htons(port);
    }
    uint16_t    SocketAddr::getPort() {
        return uint16_t(ntohs(mAddr.sin_port));
    }
    void        SocketAddr::setAF(int af) {
        mAddr.sin_family = af;
    }
    int         SocketAddr::getAF() {
        return int(mAddr.sin_family);
    }



}