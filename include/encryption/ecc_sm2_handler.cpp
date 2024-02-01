//
// Created by 14270 on 2024-01-09.
//

#include "ecc_sm2_handler.hpp"

namespace muse::chain {
    //椭圆曲线的选择和秘钥生成
    auto ecc_sm2_handler::evp_sm2_key() -> std::shared_ptr<EVP_PKEY>{
        EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_SM2, nullptr);
        if (ctx == nullptr) {
            return nullptr;
        }
        // 初始化密钥生成过程
        EVP_PKEY *pkey = nullptr;
        if (EVP_PKEY_keygen_init(ctx) <= 0) {
            EVP_PKEY_CTX_free(ctx);
            return nullptr;
        }
        // 执行密钥生成操作
        if (EVP_PKEY_keygen(ctx, &pkey) <= 0) {
            EVP_PKEY_CTX_free(ctx);
            return nullptr;
        }
        EVP_PKEY_CTX_free(ctx);
        return {pkey, EVP_PKEY_free};
    }

    auto ecc_sm2_handler::store_private_key_pem(EVP_PKEY *pkey, const std::string &file_path) -> bool {
        std::filesystem::path save_path(file_path);
        if (!std::filesystem::exists( save_path.parent_path())){
            return false;
        }
        FILE* file = nullptr;
#ifdef _WIN32
        auto err_pri = fopen_s(&file, file_path.c_str(), "wb");
        if (err_pri != 0) {
            return false;
        }
#elifdef __linux__
        file = fopen(file_path.c_str(), "wb");
        if (file == nullptr) {
            return false;
        }
#endif
        auto result = PEM_write_PrivateKey(file,pkey,nullptr,nullptr,0,nullptr, nullptr );
        fclose(file);
        return result > 0;
    }

    auto ecc_sm2_handler::store_public_key_pem(EVP_PKEY *pkey, const std::string &file_path) -> bool {
        std::filesystem::path save_path(file_path);
        if (!std::filesystem::exists( save_path.parent_path())){
            return false;
        }
        FILE* file_pub = nullptr;
#ifdef _WIN32
        auto err_pub = fopen_s(&file_pub, file_path.c_str(), "wb");
        if (err_pub != 0) {
            return false;
        }
#elifdef __linux__
        file_pub = fopen(file_path.c_str(), "wb");
        if (file_pub == nullptr) {
            return false;
        }
#endif
        auto result = PEM_write_PUBKEY(file_pub,pkey);
        fclose(file_pub);
        return result > 0;
    };


    auto ecc_sm2_handler::signature_sm3(std::shared_ptr<EVP_PKEY> key, void *data, size_t data_size, std::string& _out_def )->bool {
        EVP_MD_CTX* mctx = EVP_MD_CTX_new();
        //初始化
        if(EVP_DigestInit(mctx, EVP_sm3()) <= 0){
            EVP_MD_CTX_free(mctx);
            auto errno_ = ERR_get_error();
            //写入日志
            const char *errorString = ERR_error_string(errno_, nullptr);
            return false;
        }
        //签名进行生成
        if (EVP_DigestUpdate(mctx, data, data_size) <= 0){
            EVP_MD_CTX_free(mctx);
            auto errno_ = ERR_get_error();
            //写入日志
            const char *errorString = ERR_error_string(errno_, nullptr);
            return false;
        }
        //获得结果
        unsigned char out[256] = { 0 };
        unsigned int out_len = 0;
        int result = EVP_SignFinal(mctx, out, &out_len, key.get());
        if (result != 1) {
            EVP_MD_CTX_free(mctx);
            auto errno_ = ERR_get_error();
            //写入日志
            const char *errorString = ERR_error_string(errno_, nullptr);
            return false;
        }
        _out_def = std::string(reinterpret_cast<char*>(out), out_len);
        EVP_MD_CTX_free(mctx);
        return true;
    }

    auto ecc_sm2_handler::verify_sm3(std::shared_ptr<EVP_PKEY> key, void *data, size_t data_size, const std::string &in_sig_data) -> bool {
        EVP_MD_CTX* mctx = EVP_MD_CTX_new();
        if (EVP_DigestInit(mctx, EVP_sm3()) <= 0){
            EVP_MD_CTX_free(mctx);
            auto errno_ = ERR_get_error();
            //写入日志
            const char *errorString = ERR_error_string(errno_, nullptr);
            return false;
        }
        if (EVP_DigestUpdate(mctx, data, data_size) <= 0){
            EVP_MD_CTX_free(mctx);
            auto errno_ = ERR_get_error();
            //写入日志
            const char *errorString = ERR_error_string(errno_, nullptr);
            return false;
        }
        auto result = EVP_VerifyFinal(mctx, reinterpret_cast<const unsigned char*>(in_sig_data.c_str()), in_sig_data.size(), key.get());
        EVP_MD_CTX_free(mctx);
        return result > 0;
    }

} // muse