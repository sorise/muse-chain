//
// Created by remix on 23-9-14.
//

#include "hash_handler.hpp"

#define EVP_DIGEST_MOVE(func)                                       \
    uint32_t out_size = 0;                                          \
    EVP_Digest(data, data_length, out, &out_size, func, nullptr);   \
    return out_size;

namespace muse::chain{
    uint32_t hash_handler::sm3(const char *data, uint32_t data_length, unsigned char *out) {
        EVP_DIGEST_MOVE(EVP_sm3())
    }

    std::string hash_handler::convert_to_hex(const unsigned char *data, uint32_t data_length) {
        std::string result;
        static const char* hex {"0123456789abcdef"};
        int j = 0;
        for (int i = 0; i < data_length; ++i) {
            result.push_back(hex[(data[i]>>4) & 0xf]);   //取得二进制高位
            result.push_back(hex[ data[i] & 0xf]);       //取得二进制高位
        }
        return result;
    }

    uint32_t hash_handler::sha3_256(const char *data, uint32_t data_length, unsigned char *out) {
        EVP_DIGEST_MOVE(EVP_sha3_256())
    }

    uint32_t hash_handler::sha3_512(const char *data, uint32_t data_length, unsigned char *out) {
        EVP_DIGEST_MOVE(EVP_sha3_512())
    }

    uint32_t hash_handler::md5(const char *data, uint32_t data_length, unsigned char *out) {
        EVP_DIGEST_MOVE(EVP_md5())
    }

    uint32_t hash_handler::sha2_256(const char *data, uint32_t data_length, unsigned char *out) {
        EVP_DIGEST_MOVE(EVP_sha256())
    }

    uint32_t sha3_256::byte_length() {
        return length / 8;
    }

    uint32_t sha3_256::get_hash(const char *data, uint32_t data_length, unsigned char *out) {
        return hash_handler::sha3_256(data, data_length, out);
    }
}
