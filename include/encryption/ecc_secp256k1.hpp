//
// Created by 14270 on 2024-01-11.
//
#define OPENSSL_API_COMPAT 0x10100000L
#ifndef GLORIA_ECC_SECP256K1_HPP
#define GLORIA_ECC_SECP256K1_HPP

#include <string>
#include <openssl/ec.h>
#include <openssl/pem.h>
#include <openssl/rand.h>
#include <filesystem>
#include <memory>
#include "hash_handler.hpp"
#include "openssl/err.h"
#include "util/logger.hpp"
#include "db/uint256.hpp"

namespace muse::chain {
    //秘钥长度，公钥512位，私钥256，不适用！
    class ecc_secp256k1 {
    public:
        /* get key */
        static auto get_key() -> std::shared_ptr<EVP_PKEY>;
        /* 进行数字签名，返回string */
        static auto signature(EVP_PKEY* key, void *data, size_t data_size, std::string& _out_def )->bool;
        /* 验签 */
        static auto verify(EVP_PKEY* key, void *data, size_t data_size, const std::string &in_sig_data) -> bool;
        /* 存储私钥到文件中 */
        static auto store_private_key_pem(EVP_PKEY *pkey, const std::string &file_path) -> bool;
        /* 存储私钥到文件中,需要加密 */
        static auto store_private_key_pem(EVP_PKEY *pkey, const std::string &file_path, const std::string &password) -> bool;
        /* 存储公钥到文件中 */
        static auto store_public_key_pem(EVP_PKEY *pkey, const std::string &file_path) -> bool;
        /* 从文件中读取公钥 */
        static auto read_public_key_from_file(const std::string &_pub_key_path) ->  EVP_PKEY *;
        /* 从文件中读取私钥，需要提供加密密码 */
        static auto read_private_key_from_file(const std::string &file_path, const std::string &password) -> EVP_PKEY *;
        /* 从文件中读取私钥 */
        static auto read_private_key_from_file(const std::string &_pri_key_path) ->  EVP_PKEY *;
        /* 要从 EVP_PKEY 对象中获取公钥的二进制数据*/
        static auto convert_public_key_to_binary(EVP_PKEY * key) -> std::string;
        /* 从二进制数据恢复 公钥EVP_PKEY */
        static auto public_key_binary_to_EVP_PKEY(const void *ptr, size_t len) -> EVP_PKEY*;
        /* 要从 EVP_PKEY 对象中获取私钥的二进制数据 */
        static auto convert_private_key_to_binary(EVP_PKEY * key) -> std::string;
        /* 从二进制数据恢复 公钥EVP_PKEY */
        static auto private_key_binary_to_EVP_PKEY(void *ptr, size_t len) -> EVP_PKEY*;
        /* 将 EVP_PKEY 的公钥转换为 uint256 */
        static auto convert_public_key_to_uint256(EVP_PKEY* evpPublicKey, uint256_prefix& pub_key, bool isCompressed = true) -> bool;
        /* 将 uint256 转换为 EVP_PKEY 的公钥 */
        static auto convert_uint256_to_public_key(const uint256_prefix& pub_key, bool isCompressed = true) -> EVP_PKEY*;
        /* 获得公钥的哈希值 */
        static auto get_public_key_hash_no_compressed(EVP_PKEY* evpPublicKey, uint256& out) -> bool;
        /* 获得私钥 32位 */
        static auto convert_private_key_32B(EVP_PKEY *key) -> std::string;
        /* 获得公钥 1+64位 */
        static auto convert_public_key_no_compressed_64B(EVP_PKEY *key) -> std::string;
    };
}

#endif //GLORIA_ECC_SECP256K1_HPP
