//
// Created by remix on 23-10-6.
//

#ifndef MUSE_CHAIN_RSA_HANDLER_HPP
#define MUSE_CHAIN_RSA_HANDLER_HPP
#include <openssl/err.h>
#include <string>
#include "openssl/evp.h"
#include "openssl/ssl.h"
#include "openssl/rsa.h"

namespace muse::chain{

    class asymmetric_handler{

    };

    class rsa_handler{
    public:
        // 返回值由调用者 EVP_PKEY_free(EVP_PKEY*)  释放
        static EVP_PKEY* create_rsa_key(int bits);
        /*
         *  _pub_key_path : public_key.pem 公钥存储位置 需要英文路径
         *  _pri_key_path: private_key.pem 私钥存储位置 需要英文路径
         */
        static bool save_key_to_file(EVP_PKEY *kp, const std::string& _pub_key_path, const std::string& _pri_key_path);
        /*
         *  _pub_key_path : public_key.pem 公钥存储位置 需要英文路径
         *  _pri_key_path: private_key.pem 私钥存储位置 需要英文路径
         * */
        static EVP_PKEY* read_pub_key_from_file(const std::string& _pub_key_path);

        static EVP_PKEY* read_private_key_from_file(const std::string& _pri_key_path);

        static void print_big_number(const BIGNUM* n);
    };
}
#endif //MUSE_CHAIN_RSA_HANDLER_HPP
