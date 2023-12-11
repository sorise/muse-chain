//
// Created by remix on 23-10-6.
//
#include "rsa_handler.hpp"

namespace muse::chain{
    EVP_PKEY *rsa_handler::create_rsa_key(int bits) {
        // 1. 创建 RSA 公钥加密上下文
        EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, nullptr);
        if (!ctx) {
            ERR_print_errors_fp(stderr);
            return nullptr;
        }
        // 2. 初始化密钥对生成上下文
        if (EVP_PKEY_keygen_init(ctx) <= 0) {
            ERR_print_errors_fp(stderr);
            EVP_PKEY_CTX_free(ctx);
            return nullptr;
        }

        // 3. 设置参数 RSA 秘钥位数
        if (EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, bits) <= 0) {
            ERR_print_errors_fp(stderr);
            EVP_PKEY_CTX_free(ctx);
            return nullptr;
        }
        // 4. 秘钥生成
        // 内部会需要申请空间
        EVP_PKEY* pkey = nullptr;
        if (EVP_PKEY_keygen(ctx, &pkey) <= 0) {
            ERR_print_errors_fp(stderr);
            EVP_PKEY_CTX_free(ctx);
            return nullptr;
        }

        // 5. 释放上下文
        EVP_PKEY_CTX_free(ctx);
        return pkey;
    }

    bool rsa_handler::save_key_to_file(EVP_PKEY *kp, const std::string& _pub_key_path, const std::string& _pri_key_path ) {
        if (kp == nullptr) return false;

        //输出公钥 pem 文件
        FILE* pubf = fopen(_pub_key_path.c_str(), "w+");
        if (!pubf) {
            return false;
        }
        PEM_write_PUBKEY(pubf, kp);

        //输出私钥 pem 文件
        FILE* prif = fopen(_pri_key_path.c_str(), "w+");
        if (!prif) {
            fclose(pubf);
            return false;
        }

        PEM_write_PrivateKey(prif,kp, nullptr, nullptr,0, nullptr, nullptr);

        fclose(pubf);
        fclose(prif);
        return true;
    }

    void rsa_handler::print_big_number(const BIGNUM *n) {
        unsigned char out[256] = { 0 };
        BN_bn2bin(n, out);
        int byte_size = BN_num_bytes(n);
        for (size_t i = 0; i < byte_size; i++)
        {
            printf("%02x", out[i]);
        }
        printf("\n");
    }

    EVP_PKEY * rsa_handler::read_pub_key_from_file(const std::string &_pub_key_path) {
        FILE* public_file = fopen(_pub_key_path.c_str(), "r");
        return PEM_read_PUBKEY(public_file, nullptr, nullptr, nullptr);
    }

    EVP_PKEY * rsa_handler::read_private_key_from_file(const std::string &_pri_key_path) {
        FILE* private_file = fopen(_pri_key_path.c_str(), "r");
        return PEM_read_PrivateKey(private_file, nullptr, nullptr, nullptr);
    }
}



