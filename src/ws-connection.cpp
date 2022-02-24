#include "ws-connection.h"

namespace ws {

	WSConnection::WSConnection(tcp::Socket socket) :mSocket(socket) {

	}

	WSConnection::~WSConnection() {
		mSocket._close();
	}
	
	bool WSConnection::read(WSPayload& payload) {
		std::vector<uint8_t> raw_header(14);
		int offset = 0;
		int recv_len = mSocket._recv((char*)raw_header.data(), 2);
		if (recv_len != 2) return false;
		offset += recv_len;
		bool	b_fin = (raw_header[0] & 0b10000000) > 0;
		uint8_t u_opcode = uint8_t(raw_header[0] & 0b00001111);
		bool	b_mask = (raw_header[1] & 0b10000000) > 0;
		if (b_mask == false) return false;
		uint64_t data_len = uint64_t(raw_header[1] & 0b01111111);
		if (data_len == 126) {
			recv_len = mSocket._recv((char*)raw_header.data() + offset, 2);
			if (recv_len != 2) return false;
			data_len = uint64_t(*reinterpret_cast<uint16_t*>(&raw_header[offset]));
			offset += recv_len;
		}
		else if (data_len == 127) {
			recv_len = mSocket._recv((char*)raw_header.data() + offset, 8);
			if (recv_len != 8) return false;
			data_len = uint64_t(*reinterpret_cast<uint64_t*>(&raw_header[offset]));
			offset += recv_len;
		}
		uint32_t mask_key = 0;
		if (b_mask) {
			recv_len = mSocket._recv((char*)raw_header.data() + offset, 4);
			if (recv_len != 4) return false;
			mask_key = *reinterpret_cast<uint32_t*>(&raw_header[offset]);
			offset += recv_len;
		}
		std::vector<uint8_t> raw_body(data_len);
		recv_len = mSocket._recv((char*)raw_body.data(), data_len);
		if (recv_len < data_len) {
			std::cerr << "Read All Body Data Failed" << std::endl;
		}
		uint8_t* p_u8mask_key = reinterpret_cast<uint8_t*>(&mask_key);
		for (int i = 0; i < data_len; i++) {
			raw_body[i] ^= p_u8mask_key[i % 4];
		}
		payload.setIsFin(b_fin);
		payload.setOPCode(u_opcode);
		payload.setIsMasked(b_mask);
		payload.setLen(data_len);
		payload.setMaskKey(mask_key);
		payload.setBody(raw_body);
		return true;
	}

	bool WSConnection::write(WSPayload& payload) {
		std::vector<uint8_t> raw_packet;
		payload.getRawPacket(raw_packet);
		if (raw_packet.size() < 2) return false;

		int serv_len = mSocket._send((char*)raw_packet.data(), raw_packet.size());
		if (serv_len == -1) return false;
		return true;
	}

	bool WSConnection::write_msg(std::string_view msg) {
		char packet_buf[4096];
		// FIN = 1 OpCode = 1 (text)
		packet_buf[0] = 0b10000001;

		size_t offset;
		size_t msg_len = msg.length();

		if (msg_len < 126) {
			offset = 2;
			packet_buf[1] = (char)msg_len;
		}
		else if (msg_len < 65536) {
			offset = 4;
			packet_buf[1] = 126;
			*((uint16_t*)packet_buf + 2) = uint16_t(msg_len);
		}
		else {
			offset = 10;
			packet_buf[1] = 127;
			*((uint64_t*)packet_buf + 2) = uint64_t(msg_len);
		}

		memcpy(packet_buf + offset, msg.data(), msg_len);

		size_t msg_size = offset + msg_len;

		mSocket._send(packet_buf, msg_len);

		return 0;


	}

}