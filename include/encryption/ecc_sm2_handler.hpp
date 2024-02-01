//
// Created by 14270 on 2024-01-09.
//

#ifndef GLORIA_ECC_SM2_HANDLER_HPP
#define GLORIA_ECC_SM2_HANDLER_HPP

#include <string>
#include <openssl/ec.h>
#include <openssl/pem.h>
#include <openssl/rand.h>
#include <filesystem>
#include <memory>
#include "openssl/err.h"

namespace muse::chain {
    //秘钥长度，公钥512位，私钥256，不适用！
    class ecc_sm2_handler {
    public:
        /* 生成一个ECC SM2曲线 秘钥对， 返回值由调用者 EVP_PKEY_free(EVP_PKEY*)  释放*/
        static auto evp_sm2_key() ->std::shared_ptr<EVP_PKEY>;

        /* 存储私钥到文件中 */
        static auto store_private_key_pem(EVP_PKEY *pkey, const std::string &file_path) -> bool;

        /* 存储公钥到文件中 */
        static auto store_public_key_pem(EVP_PKEY *pkey, const std::string &file_path) -> bool;

        /* 进行数字签名，返回string */
        static auto signature_sm3(std::shared_ptr<EVP_PKEY> key, void *data, size_t data_size, std::string& _out_def )->bool;

        /* 验签 */
        static auto verify_sm3(std::shared_ptr<EVP_PKEY> key, void *data, size_t data_size, const std::string& in_sig_data )->bool;
    };

} // muse

#endif //GLORIA_ECC_SM2_HANDLER_HPP
