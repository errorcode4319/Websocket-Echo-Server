#include "ws-payload.h"
#include <string>

namespace ws {

	WSPayload::WSPayload() : mHeader(packet_header_t{false, OPCODE::UNKNOWN, false, 0, 0}) {

	}

	WSPayload::~WSPayload() {

	}

	size_t WSPayload::parseHeader(const std::vector<uint8_t>& raw_header) {
		//if (raw_header.size() < 14) return false;	
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

		size_t offset = 2;
		if (len == 126) {
			len = *(reinterpret_cast<const uint16_t*>(&raw_header[offset]));
			offset += 2;
		}
		else if (len == 127) {
			len = *(reinterpret_cast<const uint64_t*>(&raw_header[offset]));
			offset += 8;
		}
		uint32_t mask_key = 0;
		if (b_mask) {
			mask_key = *(reinterpret_cast<const uint32_t*>(&raw_header[offset]));
			offset += 4;
		}

		mHeader.fin			= b_fin;
		mHeader.opcode		= opcode;
		mHeader.mask		= b_mask;
		mHeader.len			= len;
		mHeader.mask_key	= mask_key;
		return offset;
	}

	void WSPayload::getRawPacket(std::vector<uint8_t>& buf) {
		
		size_t header_len = 2;
		size_t body_len = mHeader.len;
		if (126 <= body_len && body_len < 0x10000) {
			header_len += 2;
		}
		else if (body_len >= 0x10000) {
			header_len += 8;
		}
		if (mHeader.mask) {
			header_len += 4;
		}
		size_t payload_len = header_len + body_len;
		buf.resize(payload_len);
		buf[0] |= uint8_t((mHeader.fin ? 1 : 0) << 7);
		buf[0] |= [](OPCODE opcode) -> uint8_t {
			switch (opcode) {
			case OPCODE::CONTINUE:	return 0x00;
			case OPCODE::TEXT:		return 0x01;
			case OPCODE::BINARY:	return 0x02;
			case OPCODE::CLOSE:		return 0x08;
			case OPCODE::PING:		return 0x09;
			case OPCODE::PONG:		return 0x0A;
			case OPCODE::UNKNOWN:	
			default:				return 0x03;
			}
		}(mHeader.opcode);
		buf[1] |= uint8_t((mHeader.mask ? 1 : 0) << 7);
		if (body_len < 126) {
			buf[1] |= body_len;
		}
		else if (body_len < 0x10000) {
			buf[1] |= 126;
			*(reinterpret_cast<uint16_t*>(&buf[2])) = uint16_t(body_len);
		} 
		else {
			buf[1] |= 127;
			*(reinterpret_cast<uint64_t*>(&buf[2])) = uint64_t(body_len);
		}
		if (mHeader.mask) {
			size_t mask_key_offset = header_len - 4;
			*(reinterpret_cast<uint32_t*>(&buf[mask_key_offset])) = uint32_t(mHeader.mask_key);
		}
		memcpy(buf.data() + header_len, mBody.data.data(), body_len);

	}



}