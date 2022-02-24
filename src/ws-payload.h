#ifndef __WS_PAYLOAD_H
#define __WS_PAYLOAD_H
#include<vector>
#include<cstdint>
#include<iostream>

namespace ws {

	using FLAG = bool;

	enum class OPCODE : uint8_t {
		CONTINUE = 0x0,	//0x0
		TEXT	 = 0x1,			//0x1
		BINARY	 = 0x2,			//0x2
		CLOSE	 = 0x8,			//0x8
		PING	 = 0x9,			//0x9
		PONG     = 0xA,			//0xA
		UNKNOWN  = 0x3
	};

	struct packet_header_t {
		FLAG		fin;
		OPCODE		opcode;
		FLAG		mask;
		uint32_t	mask_key;
		uint64_t	len;
	};

	struct packet_body_t {
		std::vector<uint8_t> data;
	};

	class WSPayload {
	public:
		WSPayload();
		~WSPayload();

		size_t parseHeader(const std::vector<uint8_t>& raw_header);

		void getRawPacket(std::vector<uint8_t>& buf);

		const packet_header_t&	getHeader() const { return mHeader; }
		std::reference_wrapper<packet_header_t>
								getHeaderRef() { return std::ref(mHeader); }

		const packet_body_t&	getBody() const { return mBody; }
		std::reference_wrapper<packet_body_t>	
								getBodyRef() { return std::ref(mBody); }

		void	setHeader(const packet_header_t& header) { mHeader = header; }
		void	setBody(const packet_body_t& body) { 
			mBody = body; 
			mHeader.len = mBody.data.size();
		}
		void	setBody(const std::vector<uint8_t>& body) {
			mBody.data = body;
			mHeader.len = body.size();
		}
		
		FLAG	getIsFin() const { return mHeader.fin; }
		void	setIsFin(bool isFin) { mHeader.fin = isFin; }

		OPCODE	getOPCode() const { return mHeader.opcode;}
		void	setOPCode(OPCODE opcode) { mHeader.opcode = opcode; }
		void	setOPCode(uint8_t u_opcode) {
			switch (u_opcode) {
			case 0x00: mHeader.opcode = OPCODE::CONTINUE; break;
			case 0x01: mHeader.opcode = OPCODE::TEXT; break;
			case 0x02: mHeader.opcode = OPCODE::BINARY; break;
			case 0x08: mHeader.opcode = OPCODE::CLOSE; break;
			case 0x09: mHeader.opcode = OPCODE::PONG; break;
			case 0x0A: mHeader.opcode = OPCODE::PONG; break;
			default: mHeader.opcode = OPCODE::UNKNOWN; break;
			}
		}

		FLAG	getIsMasked() const { return mHeader.mask; }
		void	setIsMasked(bool isMasked) { mHeader.mask = isMasked; }

		uint32_t getMaskKey() const { return mHeader.mask_key; }
		void	setMaskKey(uint32_t mask_key) { mHeader.mask_key = mask_key; }

		uint64_t getLen() const { return mHeader.len; }
		void	setLen(uint64_t len) { mHeader.len = len; }


	private:
		packet_header_t mHeader;
		packet_body_t	mBody;
	};

}

#endif // __WS_PAYLOAD_H