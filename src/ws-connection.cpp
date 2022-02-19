#include "ws-connection.h"
#include<iomanip>

namespace ws {

	WSConnection::WSConnection(tcp::Socket socket) :mSocket(socket) {

	}

	WSConnection::~WSConnection() {

	}

	WSPayload WSConnection::read() {
		WSPayload payload;
		std::vector<uint8_t> raw_header(4096);

		int ret = mSocket._recv((char*)raw_header.data(), 4096);
		payload.parseHeader(raw_header);
		auto header = payload.getHeader();

		std::cout << "FIN: " << header.fin << std::endl;
		std::cout << "Opcode: "	<< int(payload.getOPCode()) << std::endl;
		std::cout << "MASK: " << header.mask << std::endl;
		std::cout << "MASK Key: " << std::hex << header.mask_key << std::dec << std::endl;
		std::cout << "Data len: " << header.len << std::endl;


		return payload;
	}



}