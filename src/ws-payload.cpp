#include "ws-payload.h"
#include <string>

namespace ws {

	WSPayload::WSPayload() : mHeader(packet_header_t{false, OPCODE::UNKNOWN, false, 0, 0}) {

	}

	WSPayload::~WSPayload() {

	}

	bool WSPayload::parseHeader(const std::vector<uint8_t>& raw_header) {
		if (raw_header.size() < 14) return false;	
		bool b_fin	= (raw_header[0] & 0b10000000) > 0;
		OPCODE opcode = [](uint8_t opcode) -> OPCODE {
			switch (opcode) {
				case 0x00: 
					return OPCODE::CONTINUE;
				case 0x01:
					return OPCODE::TEXT;
				case 0x02: 
					return OPCODE::BINARY;
				case 0x08: 
					return OPCODE::CLOSE;
				case 0x09: 
					return OPCODE::PING;
				case 0x0A: 
					return OPCODE::PONG;
				default:	
					return OPCODE::UNKNOWN;
			}
		}(uint8_t(raw_header[0] & 0b00001111));
		bool b_mask = (raw_header[1] & 0b10000000) > 0;

		uint64_t len = raw_header[1] & 0b01111111;
		if (len == 126) {
			len = *(reinterpret_cast<const uint16_t*>(&raw_header[2]));
		}
		else if (len == 127) {
			len = *(reinterpret_cast<const uint64_t*>(&raw_header[2]));
		}
		uint32_t mask_key = *(reinterpret_cast<const uint32_t*>(&raw_header[10]));

		mHeader.fin			= b_fin;
		mHeader.opcode		= opcode;
		mHeader.mask		= b_mask;
		mHeader.len			= len;
		mHeader.mask_key	= mask_key;
		return true;
	}

	void WSPayload::getRawPacket(std::vector<uint8_t>& buf) {
		size_t body_len = mBody.data.size();
		buf.resize(14 + body_len);

		buf[0] |= uint8_t((mHeader.fin ? 1 : 0) << 7);
		uint8_t u_opcode = [](OPCODE opcode) -> uint8_t {
			switch (opcode) {
				case OPCODE::CONTINUE:	return 0x00;
				case OPCODE::TEXT:		return 0x01;
				case OPCODE::BINARY:	return 0x02;
				case OPCODE::CLOSE:		return 0x08;
				case OPCODE::PING:		return 0x09;
				case OPCODE::PONG:		return 0x0A;
				case OPCODE::UNKNOWN:	return 0x03;
			}
		}(mHeader.opcode);
		buf[0] |= u_opcode;
		buf[1] |= uint8_t((mHeader.mask ? 1 : 0) << 7);
		
		uint8_t		u_payload_len = 0;
		uint64_t	u_payload_len_ext = 0;

		if (mHeader.len < 126) {
			u_payload_len = uint8_t(mHeader.len);
		}
		else if (mHeader.len < 0x10000) {
			u_payload_len = 126;
			u_payload_len_ext = uint16_t(mHeader.len);
		}
		else {
			u_payload_len = 127;
			u_payload_len_ext = mHeader.len;
		}

		buf[1] |= (u_payload_len & 0b01111111);
		*(reinterpret_cast<uint64_t*>(&buf[2])) = u_payload_len_ext;
		*(reinterpret_cast<uint32_t*>(&buf[10])) = mHeader.mask_key;

		memcpy(&buf[14], mBody.data.data(), body_len);
	}



}