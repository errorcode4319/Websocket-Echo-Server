#include "ws-connection.h"

namespace ws {

	WSConnection::WSConnection(tcp::Socket socket) :mSocket(socket) {

	}

	WSConnection::~WSConnection() {
		mSocket._close();
	}

	WSPayload WSConnection::read() {
		WSPayload payload;
		std::vector<uint8_t> raw_header(14);

		int total_recv_bytes = 0;

		int recv_len = mSocket._recv((char*)raw_header.data(), 14);
		if (recv_len == -1) {
			std::cerr << "Cannot Read Header" << std::endl;
			return payload;
		}
		total_recv_bytes += recv_len;
		
		size_t header_len = payload.parseHeader(raw_header);
		auto header = payload.getHeader();
		size_t data_len = header.len;

		int read_body_len = (recv_len - header_len);
		int readable_bytes = data_len - (recv_len - header_len);
		std::vector<uint8_t>	raw_body(data_len);
		memcpy(raw_body.data(), raw_header.data() + header_len, read_body_len);
		if (readable_bytes > 0) {
			recv_len = mSocket._recv((char*)raw_body.data() + read_body_len, readable_bytes);
			total_recv_bytes += recv_len;
		}
		if (header.mask) {
			uint32_t mask_key = header.mask_key;
			uint8_t* p_u8mask = reinterpret_cast<uint8_t*>(&mask_key);
			for (int i = 0; i < data_len; i++) {
				raw_body[i] ^= p_u8mask[i % 4];
			}
		}

		payload.getBodyRef().get().data = raw_body;

		return payload;
	}



}