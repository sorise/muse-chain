//
// Created by remix on 23-9-14.
//

#ifndef MUSE_CHAIN_HASH_HANDLER_HPP
#define MUSE_CHAIN_HASH_HANDLER_HPP
#include <iostream>
#include <cstring>
#include "openssl/evp.h"

namespace muse::chain{

    class hash_handler{
    public:
        /* md3 国密 生成16字节 128位 的Hash 值 */
        static uint32_t md5(const char *data, uint32_t data_length,unsigned char *out);

        /* sm3 国密 生成32字节 256 位的Hash 值 */
        static uint32_t sm3(const char *data, uint32_t data_length,unsigned char *out);

        /*SHA-2-256 哈希算法 */
        static uint32_t sha2_256(const char *data, uint32_t data_length,unsigned char *out);

        /*SHA-3-256 哈希算法 */
        static uint32_t sha3_256(const char *data, uint32_t data_length,unsigned char *out);

        /*SHA-3-512 哈希算法 */
        static uint32_t sha3_512(const char *data, uint32_t data_length,unsigned char *out);

        /* 把字节数据转换为16进制字符串 * */
        static std::string convert_to_hex(const unsigned char *data, uint32_t data_length);
    };

    class hash_base{
    public:
        virtual uint32_t byte_length() = 0;
        virtual uint32_t get_hash(const char *data, uint32_t data_length,unsigned char *out) = 0;
    };

    class sha3_256: public hash_base{
        const int length = 256;
    public:
        uint32_t byte_length() override;
        uint32_t get_hash(const char *data, uint32_t data_length,unsigned char *out) override;
        virtual ~sha3_256() = default;
    };
}
#endif //MUSE_CHAIN_HASH_HANDLER_HPP
