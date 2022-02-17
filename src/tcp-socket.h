#ifndef __TCP_SOCKET_H
#define __TCP_SOCKET_H

#include "tcp-util.h"

namespace tcp {

    int SAstartup();
    int SAcleanup();


    /* Socket Descriptor OOP Wrapper */
    class Socket {
    public:
        Socket() = default;
        Socket(sock_t sock) : mSock(sock) {};
        ~Socket() = default;

        sock_t  _new();

        /*
        https://docs.microsoft.com/ko-kr/windows/win32/api/winsock/nf-winsock-getsockopt?redirectedfrom=MSDN
        */
        int _setOpt(int level, int optname, const void* optval, socklen_t optlen);
        int _getOpt(int level, int optname, void* optval, socklen_t optlen);

        int _bind(const sockaddr_t& addr);
        int _listen(int backlog);
        [[deprecated]] int _select();
        sd_pkg_t _accept();
        int _connect(const sockaddr_t& addr);

        /*Only Winsock uses flags for I/O */
        int _send(const char* buf, int len, int flags = 0);
        int _recv(char* buf, int len, int flags = 0);
        int _msg_push(const std::string& msg, int flags = 0);
        int _msg_pull(std::string& msg, int len, int flags = 0);

        int _close();
        int _shutdown(int how);

        bool isValid() const { return (mSock != INVALID_SOCKET) && (mSock > sock_t(2)); }
        sock_t get() const { return mSock; }

        bool operator<(const Socket& ref) const {
            return mSock < ref.get();
        }

    private:
        sock_t mSock = INVALID_SOCKET;
    };

}

#endif // __TCP_SOCKET_H