#ifndef __WS_SECURE_H
#define __WS_SECURE_H
#include<iostream>
#include<cstdint>
#include<vector>

namespace ws {

	void sha1_hash(const uint8_t* src, size_t src_len, uint8_t* digest);

	std::vector<uint8_t> sha1_hash(std::string_view src);

	size_t base64_encode(const uint8_t* src, size_t src_len, std::string& dst);

	std::string base64_encode(std::string_view src);

	size_t base64_decode(const std::string& src, uint8_t* dst);

	std::string base64_decode(std::string_view src);

	std::string gen_ws_access_key(std::string_view ws_key);
}

#endif //__WS_SECURE_H