#include "tcp-socket.h"


namespace tcp {

    class NET_init {
    public:
        NET_init() {
            if (SAstartup() != 0)
                std::cerr << "NET INIT FAILED" << std::endl;
        }
        ~NET_init() { SAcleanup(); }
    };
    static NET_init ni;

    int SAstartup() {
        static bool is_done = false;
        if (is_done) return 0;
        is_done = true;
#if defined(_MSC_VER) //Winsock 
        static WSADATA wsaData;
        return WSAStartup(MAKEWORD(2, 2), &wsaData);
#elif defined(__linux__)
        signal(SIGPIPE, SIG_IGN);
        /*
    Only Linux(and UNIX)
    https://www.doof.me.uk/2020/09/23/sigpipe-and-how-to-ignore-it/
        */
#endif 
        return 0;
    }
    int SAcleanup() {
        static bool is_done = false;
        if (is_done) return 0;
        is_done = true;
#if defined(_MSC_VER)
        return WSACleanup();
#elif defined(__linux__)

#endif 
        return 0;
    }

    sock_t Socket::_new() {
        return mSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    }
    int Socket::_setOpt(int level, int optname, const void* optval, socklen_t optlen) {
#if defined(_MSC_VER)
        return setsockopt(mSock, level, optname, reinterpret_cast<const char*>(optval), optlen);
#elif defined(__linux__) 
        return setsockopt(mSock, level, optname, optval, optlen);
#endif 
    }
    int Socket::_getOpt(int level, int optname, void* optval, socklen_t optlen) {
#if defined(_MSC_VER)
        return getsockopt(mSock, level, optname, reinterpret_cast<char*>(optval), (int*)&optlen);
#elif defined(__linux__)
        return getsockopt(mSock, level, optname, optval, &optlen);
#endif 
    }
    int Socket::_bind(const sockaddr_t& addr) {
        return bind(mSock, (SOCKADDR*)&addr, sizeof(addr));
    }
    int Socket::_listen(int backlog) {
        return listen(mSock, backlog);
    }
    int Socket::_select() {
        return 0;
    }
    sd_pkg_t Socket::_accept() {
        sockaddr_t addr;
#if defined(_MSC_VER)
        int addrlen = sizeof(addr);
#elif defined(__linux__)
        socklen_t addrlen = sizeof(addr);
#endif 
        sock_t sock = accept(mSock, (SOCKADDR*)&addr, &addrlen);
        return sd_pkg_t{ sock, addr };
    }
    int Socket::_connect(const sockaddr_t& addr) {
        return connect(mSock, (SOCKADDR*)&addr, sizeof(addr));
    }
    int Socket::_send(const char* buf, int len, int flags) {
#if defined(_MSC_VER)
        return send(mSock, buf, len, flags);
#elif defined(__linux__)
        return write(mSock, buf, len);
#endif 
    }
    int Socket::_recv(char* buf, int len, int flags) {
#if defined(_MSC_VER)
        return recv(mSock, buf, len, flags);
#elif defined(__linux__)
        return read(mSock, buf, len);
#endif 
    }
    int Socket::_msg_push(const std::string& msg, int flags) {
        return _send(&msg[0], msg.length(), flags);
    }
    int Socket::_msg_pull(std::string& msg, int len, int flags) {
        msg.resize(len);
        return _recv(&msg[0], len, flags);
    }
    int Socket::_close() {
#if defined(_MSC_VER)
        return closesocket(mSock);
#elif defined(__linux__)
        return close(mSock);
#endif 
    }
    int Socket::_shutdown(int how) {
        return shutdown(mSock, how);
    }
}