#include"ws-secure-util.h"

#include<openssl/sha.h>

#include "base64-table.h"
// encBase64[]
// decBase64[]
//https://home.uncg.edu/cmp/faculty/srtate/580.f11/sha1examples.php

namespace ws {

    std::string sha1_hash(std::string_view src) {

        size_t len = src.length();

        SHA_CTX shactx;
        std::string digest;
        digest.resize(SHA_DIGEST_LENGTH);

        SHA1_Init(&shactx);
        SHA1_Update(&shactx, src.data(), len);
        SHA1_Final(reinterpret_cast<uint8_t*>(digest.data()), &shactx);

        return digest;
    }

    size_t base64_encode(const uint8_t* src, size_t src_len, std::string& dst) {

        uint8_t tmp_in[3] = {};
        uint8_t tmp_out[4] = {};

        size_t dst_len = (4 * (src_len / 3)) + (src_len % 3 ? 4 : 0) + 1;
        dst.clear();
        dst.reserve(dst_len);

        for (int i = 0; i < src_len; i++) {
            int enc_idx = i % 3;
            tmp_in[enc_idx] = src[i];
            if (enc_idx == 2 || i + 1 == src_len) {
                tmp_out[0] = ((tmp_in[0] & 0xFC) >> 2);
                tmp_out[1] = ((tmp_in[0] & 0x3) << 4) | ((tmp_in[1] & 0xF0) >> 4);
                tmp_out[2] = ((tmp_in[1] & 0xF) << 2) | ((tmp_in[2] & 0xC0) >> 6);
                tmp_out[3] = (tmp_in[2] & 0x3F);
                dst.push_back(encBase64[tmp_out[0]]);
                dst.push_back(encBase64[tmp_out[1]]);
                dst.push_back(enc_idx == 0 ? '=' : encBase64[tmp_out[2]]);
                dst.push_back(enc_idx < 2 ? '=' : encBase64[tmp_out[3]]);
                tmp_in[0] = tmp_in[1] = tmp_in[2] = 0;
            }
        }

        return dst_len;
    }

    std::string base64_encode(std::string_view src) {
        std::string dst;
        const uint8_t* src_ptr = reinterpret_cast<const uint8_t*>(src.data());
        auto _ = base64_encode(src_ptr, src.length(), dst);
        return dst;
    }

    size_t base64_decode(const std::string& src, uint8_t* dst) {
        size_t dst_idx = 0;
        int phase = 0;
        int dec = 0;
        int prev_dec = 0;

        for (const auto& c : src) {
            dec = decBase64[size_t(c)];
            if (dec != -1) {
                switch (phase) {
                case 0: 
                    phase++; break;
                case 1:
                    dst[dst_idx++] = uint8_t(((prev_dec << 2) | ((dec & 0x30) >> 4)));
                    phase++; break;
                case 2:
                    dst[dst_idx++] = uint8_t((((prev_dec & 0xf) << 4) | ((dec & 0x3c) >> 2)));
                    phase++; break;
                case 3:
                    dst[dst_idx++] = uint8_t((((prev_dec & 0x03) << 6) | dec));
                    phase = 0;
                    break;
                }
                prev_dec = dec;
            }
        }
        return dst_idx;
    }

	std::string base64_decode(std::string_view src) {
        size_t src_len = src.length();
        uint8_t* dst_buf = new uint8_t[src_len];

        size_t dst_len = base64_decode(src.data(), dst_buf);
        std::string dst = std::string(reinterpret_cast<char*>(dst_buf));

        delete[] dst_buf;

        return dst;
	}

    std::string create_ws_access_key(std::string_view ws_key) {



    }

}