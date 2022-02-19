#ifndef __WS_CONNECTION_H
#define __WS_CONNECTION_H
#include "ws-payload.h"
#include "tcp-socket.h"
#include <memory>
#include <iomanip>

namespace ws {

	class WSConnection : std::enable_shared_from_this<WSConnection> {
	public:
		WSConnection(tcp::Socket socket);
		~WSConnection();

		/* if read data failed, return false  */
		bool read(WSPayload& payload);

		std::shared_ptr<WSConnection> share() {
			return shared_from_this();
		}

	private:
		tcp::Socket mSocket;
	};

}

#endif // __WS_CONNECTION_H