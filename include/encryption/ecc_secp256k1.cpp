//
// Created by 14270 on 2024-01-11.
//

#include "ecc_secp256k1.hpp"
namespace muse::chain {
    auto ecc_secp256k1::get_key() -> std::shared_ptr<EVP_PKEY> {
        EC_GROUP* group = EC_GROUP_new_by_curve_name(NID_secp256k1);
        // NID_secp256k1 不支持支持加解密、仅仅签名、密钥交换
        if (group == nullptr) {
            auto errno_ = ERR_get_error();
            //写入日志
            const char *errorString = ERR_error_string(errno_, nullptr);
            SPDLOG_ERROR("ecc_secp256k1::get_key openssl error -- {}", errorString);
            return nullptr;
        }
        //EC 秘钥存放上下文
        EC_KEY* key = EC_KEY_new();
        //设置秘钥参数
        EC_KEY_set_group(key, group);
        auto result = EC_KEY_generate_key(key);
        if (result != 1) {
            auto errno_ = ERR_get_error();
            //写入日志
            const char *errorString = ERR_error_string(errno_, nullptr);
            SPDLOG_ERROR("ecc_secp256k1::get_key openssl error -- {}", errorString);
            EC_KEY_free(key);
            EC_GROUP_free(group);
            return nullptr;
        }
        //生成成功
        EVP_PKEY * pkey = EVP_PKEY_new();
        EVP_PKEY_set1_EC_KEY(pkey, key);
        EC_KEY_free(key);
        EC_GROUP_free(group);
        return std::shared_ptr<EVP_PKEY>{pkey, EVP_PKEY_free};
    }

    auto ecc_secp256k1::signature(EVP_PKEY* key, void *data, size_t data_size, std::string& _out_def )->bool {
        EVP_MD_CTX* mctx = EVP_MD_CTX_new();
        //初始化
        if(EVP_DigestInit(mctx, EVP_sha3_256()) <= 0){
            EVP_MD_CTX_free(mctx);
            auto errno_ = ERR_get_error();
            //写入日志
            const char *errorString = ERR_error_string(errno_, nullptr);
            SPDLOG_ERROR("ecc_secp256k1::signature openssl error -- {}", errorString);
            return false;
        }
        //签名进行生成
        if (EVP_DigestUpdate(mctx, data, data_size) <= 0){
            EVP_MD_CTX_free(mctx);
            auto errno_ = ERR_get_error();
            //写入日志
            const char *errorString = ERR_error_string(errno_, nullptr);
            SPDLOG_ERROR("ecc_secp256k1::signature openssl error -- {}", errorString);
            return false;
        }
        //获得结果
        unsigned char out[256] = { 0 };
        unsigned int out_len = 0;
        int result = EVP_SignFinal(mctx, out, &out_len, key);
        if (result != 1) {
            EVP_MD_CTX_free(mctx);
            auto errno_ = ERR_get_error();
            //写入日志
            const char *errorString = ERR_error_string(errno_, nullptr);
            SPDLOG_ERROR("ecc_secp256k1::signature openssl error -- {}", errorString);
            return false;
        }
        _out_def = std::string(reinterpret_cast<char*>(out), out_len);
        EVP_MD_CTX_free(mctx);
        return true;
    }

    auto ecc_secp256k1::verify(EVP_PKEY* key, void *data, size_t data_size, const std::string &in_sig_data) -> bool {
        EVP_MD_CTX* mctx = EVP_MD_CTX_new();
        if (EVP_DigestInit(mctx, EVP_sha3_256()) <= 0){
            EVP_MD_CTX_free(mctx);
            auto errno_ = ERR_get_error();
            //写入日志
            const char *errorString = ERR_error_string(errno_, nullptr);
            SPDLOG_ERROR("ecc_secp256k1::verify openssl error -- {}", errorString);
            return false;
        }
        if (EVP_DigestUpdate(mctx, data, data_size) <= 0){
            EVP_MD_CTX_free(mctx);
            auto errno_ = ERR_get_error();
            //写入日志
            const char *errorString = ERR_error_string(errno_, nullptr);
            SPDLOG_ERROR("ecc_secp256k1::verify openssl error -- {}", errorString);
            return false;
        }
        auto result = EVP_VerifyFinal(mctx, reinterpret_cast<const unsigned char*>(in_sig_data.c_str()), in_sig_data.size(), key);
        EVP_MD_CTX_free(mctx);
        return result > 0;
    }

    auto ecc_secp256k1::store_private_key_pem(EVP_PKEY *pkey, const std::string &file_path) -> bool {
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

    auto ecc_secp256k1::store_private_key_pem(EVP_PKEY *pkey, const std::string &file_path,const std::string &password) -> bool {
        std::filesystem::path save_path(file_path);
        if (password.empty()){
            return false;
        }
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
        const EVP_CIPHER *cipher = EVP_aes_256_cbc(); // 使用 AES-256-CBC 加密
        auto result = PEM_write_PrivateKey(file,pkey,cipher, reinterpret_cast<const unsigned char*>(password.c_str()),password.size(),nullptr, nullptr );
        fclose(file);
        return result > 0;
    }

    auto ecc_secp256k1::store_public_key_pem(EVP_PKEY *pkey, const std::string &file_path) -> bool {
        std::filesystem::path save_path(file_path);
        if (!std::filesystem::exists( save_path.parent_path())){
            SPDLOG_ERROR("ecc_secp256k1::store_public_key_pem the path \"{}\" not exist!", file_path);
            return false;
        }
        FILE* file_pub = nullptr;
#ifdef _WIN32
        auto err_pub = fopen_s(&file_pub, file_path.c_str(), "wb");
        if (err_pub != 0) {
            SPDLOG_ERROR("ecc_secp256k1::store_public_key_pem fopen_s failed");
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
    }

    EVP_PKEY * ecc_secp256k1::read_public_key_from_file(const std::string &_pub_key_path) {
        std::filesystem::path save_path(_pub_key_path);
        if (!std::filesystem::exists( save_path)){
            SPDLOG_ERROR("ecc_secp256k1::read_public_key_from_file the path \"{}\" not exist!", _pub_key_path);
            return nullptr;
        }
        FILE* file_pub = nullptr;
#ifdef _WIN32
        auto err_pub = fopen_s(&file_pub, _pub_key_path.c_str(), "r");
        if (err_pub != 0) {
            SPDLOG_ERROR("ecc_secp256k1::read_public_key_from_file fopen_s failed");
            return nullptr;
        }
#elifdef __linux__
        file_pub = fopen(_pub_key_path.c_str(), "wb");
        if (file_pub == nullptr) {
            return nullptr;
        }
#endif
        EVP_PKEY * pkey = nullptr;
        pkey = PEM_read_PUBKEY(file_pub, nullptr, nullptr, nullptr);
        fclose(file_pub);
        return pkey;
    }

    EVP_PKEY * ecc_secp256k1::read_private_key_from_file(const std::string &_pri_key_path) {
        std::filesystem::path save_path(_pri_key_path);
        if (!std::filesystem::exists( save_path)){
            SPDLOG_ERROR("ecc_secp256k1::read_private_key_from_file the path \"{}\" not exist!", _pri_key_path);
            return nullptr;
        }
        FILE* file_pri = nullptr;
#ifdef _WIN32
        auto err_pub = fopen_s(&file_pri, _pri_key_path.c_str(), "rb");
        if (err_pub != 0) {
            SPDLOG_ERROR("ecc_secp256k1::read_private_key_from_file fopen_s failed");
            return nullptr;
        }
#elifdef __linux__
        file_pri = fopen(_pri_key_path.c_str(), "wb");
        if (file_pri == nullptr) {
            return nullptr;
        }
#endif

        EVP_PKEY * pkey = nullptr;
        pkey = PEM_read_PrivateKey(file_pri, &pkey, nullptr, nullptr);
        fclose(file_pri);
        return pkey;
    }

    auto ecc_secp256k1::convert_public_key_to_binary(EVP_PKEY *pub) -> std::string {
        if (pub != nullptr){
            unsigned char *pub_key_data = nullptr;
            int pub_key_length = i2d_PUBKEY(pub, &pub_key_data);
            if (pub_key_length > 0){
                void* ptr = pub_key_data;
                std::string result((char *)ptr, pub_key_length);
                OPENSSL_free(pub_key_data);
                return result;
            }else{
                auto errno_ = ERR_get_error();
                const char *errorString = ERR_error_string(errno_, nullptr);
                SPDLOG_ERROR("ecc_secp256k1::get_key openssl error -- {}", errorString);
            }
        }
        return {};
    }

    auto ecc_secp256k1::public_key_binary_to_EVP_PKEY(const void *ptr, size_t len) -> EVP_PKEY*{
        EVP_PKEY* new_pkey = nullptr;
        if (ptr!= nullptr){
            const auto* pub_key_data_ptr = reinterpret_cast<const unsigned char*>(ptr);
            new_pkey = d2i_PUBKEY(nullptr, &pub_key_data_ptr, len);
        }
        return new_pkey;
    }

    auto ecc_secp256k1::convert_private_key_to_binary(EVP_PKEY *key) -> std::string {
        if (key != nullptr){
            unsigned char *pub_key_data = nullptr;
            int pub_key_length = i2d_PrivateKey(key, &pub_key_data);
            if (pub_key_length > 0){
                void* ptr = pub_key_data;
                std::string result((char *)ptr, pub_key_length);
                OPENSSL_free(pub_key_data);
                return result;
            }
        }
        return {};
    }

    auto ecc_secp256k1::private_key_binary_to_EVP_PKEY(void *ptr, size_t len) -> EVP_PKEY * {
        EVP_PKEY* new_pkey = nullptr;
        if (ptr!= nullptr){
            const auto* pub_key_data_ptr = reinterpret_cast<const unsigned char*>(ptr);
            new_pkey = d2i_PrivateKey(EVP_PKEY_EC,nullptr, &pub_key_data_ptr, len);
        }
        return new_pkey;
    }

    auto ecc_secp256k1::convert_public_key_to_uint256(EVP_PKEY* evpPublicKey, uint256_prefix& pub_key, bool isCompressed) -> bool
    {
        // 获取 EVP_PKEY 对象中的 EC_KEY
        EC_KEY* ecKey = EVP_PKEY_get1_EC_KEY(evpPublicKey);
        if (ecKey == nullptr) {
            std::cerr << "Failed to get EC_KEY from EVP_PKEY." << std::endl;
            return false;
        }

        // 获取 EC_KEY 中的 EC_POINT
        const EC_POINT* ecPoint = EC_KEY_get0_public_key(ecKey);
        if (ecPoint == nullptr) {
            std::cerr << "Failed to get EC_POINT from EC_KEY." << std::endl;
            EC_KEY_free(ecKey);
            return false;
        }

        // 将 EC_POINT 转换为字节数组
        size_t keyLength = EC_POINT_point2oct(EC_KEY_get0_group(ecKey), ecPoint, POINT_CONVERSION_COMPRESSED, nullptr, 0, nullptr);
        if (keyLength == 0) {
            std::cerr << "Failed to get key length." << std::endl;
            EC_KEY_free(ecKey);
            return false;
        }

        unsigned char* keyBuffer = new unsigned char[keyLength];
        if (EC_POINT_point2oct(EC_KEY_get0_group(ecKey), ecPoint, POINT_CONVERSION_COMPRESSED, keyBuffer, keyLength, nullptr) != keyLength) {
            std::cerr << "Failed to convert EC_POINT to byte array." << std::endl;
            EC_KEY_free(ecKey);
            delete[] keyBuffer;
            return false;
        }

        // 将字节数组转换为 uint256
        pub_key.data = uint256(keyBuffer + 1 , keyLength - 1);
        pub_key.set_prefix(keyBuffer[0]);
        // 打印 uint256 公钥
        std::cout << "Public Key (uint256): " << pub_key.data.get_hex() << std::endl;

        // 释放资源
        EC_KEY_free(ecKey);
        delete[] keyBuffer;
        return true;
    }
    // 将 uint256 转换为 EVP_PKEY 的公钥
    auto ecc_secp256k1::convert_uint256_to_public_key(const uint256_prefix& pub_key, bool isCompressed) -> EVP_PKEY*
    {
        // 创建 EC_KEY
        EC_KEY* ecKey = EC_KEY_new();
        if (ecKey == nullptr) {
            std::cerr << "Failed to create EC_KEY." << std::endl;
            return nullptr;
        }

        // 设置椭圆曲线参数
        EC_GROUP* group = EC_GROUP_new_by_curve_name(NID_secp256k1);
        if (group == nullptr) {
            std::cerr << "Failed to create EC_GROUP." << std::endl;
            EC_KEY_free(ecKey);
            return nullptr;
        }
        if (EC_KEY_set_group(ecKey, group) != 1) {
            std::cerr << "Failed to set EC_GROUP for EC_KEY." << std::endl;
            EC_GROUP_free(group);
            EC_KEY_free(ecKey);
            return nullptr;
        }

        // 创建 EC_POINT
        EC_POINT* ecPoint = EC_POINT_new(group);
        if (ecPoint == nullptr) {
            std::cerr << "Failed to create EC_POINT." <<__LINE__<< std::endl;
            EC_GROUP_free(group);
            EC_KEY_free(ecKey);
            return nullptr;
        }
        // 判断公钥类型，并添加前缀字节
        BIGNUM* bn = BN_new();
        if (pub_key.get_prefix() == 0x02 || pub_key.get_prefix() == 0x03) {
            unsigned char prefix[2] = { 0x00, static_cast<unsigned char>(pub_key.get_prefix()) };
            BN_bin2bn(prefix, 2, bn);
            BN_bin2bn(pub_key.data.get_data_const(), uint256::Width, bn);
        }
        if (bn == nullptr) {
            std::cerr << "Failed to convert uint256 to BIGNUM." << std::endl;
            EC_POINT_free(ecPoint);
            EC_GROUP_free(group);
            EC_KEY_free(ecKey);
            return nullptr;
        }

        // 将 BIGNUM 转换为 EC_POINT
        if (EC_POINT_bn2point(group, bn, ecPoint, nullptr) != nullptr) {
            std::cerr << "Failed to convert BIGNUM to EC_POINT." << std::endl;
            BN_free(bn);
            EC_POINT_free(ecPoint);
            EC_GROUP_free(group);
            EC_KEY_free(ecKey);
            auto errno_ = ERR_get_error();
            //写入日志
            const char *errorString = ERR_error_string(errno_, nullptr);
            std::cerr << errorString<< std::endl;
            return nullptr;
        }
        BN_free(bn);

        // 设置 EC_KEY 的公钥
        if (EC_KEY_set_public_key(ecKey, ecPoint) != 1) {
            SPDLOG_ERROR("Failed to set public key for EC_KEY");
            EC_POINT_free(ecPoint);
            EC_GROUP_free(group);
            EC_KEY_free(ecKey);
            return nullptr;
        }
        EC_POINT_free(ecPoint);

        // 创建 EVP_PKEY
        EVP_PKEY* evpPublicKey = EVP_PKEY_new();
        if (evpPublicKey == nullptr) {
            std::cerr << "Failed to create EVP_PKEY." << std::endl;
            EC_GROUP_free(group);
            EC_KEY_free(ecKey);
            return nullptr;
        }

        // 将 EC_KEY 分配给 EVP_PKEY
        if (EVP_PKEY_assign_EC_KEY(evpPublicKey, ecKey) != 1) {
            std::cerr << "Failed to assign EC_KEY to EVP_PKEY." << std::endl;
            EVP_PKEY_free(evpPublicKey);
            EC_GROUP_free(group);
            EC_KEY_free(ecKey);
            return nullptr;
        }

        EC_GROUP_free(group);
        return evpPublicKey;
    }

    auto ecc_secp256k1::get_public_key_hash_no_compressed(EVP_PKEY *evpPublicKey, uint256& out) -> bool {
        // 获取 EVP_PKEY 对象中的 EC_KEY
        EC_KEY* ecKey = EVP_PKEY_get1_EC_KEY(evpPublicKey);
        if (ecKey == nullptr) {
            auto errno_ = ERR_get_error();
            //写入日志
            const char *errorString = ERR_error_string(errno_, nullptr);
            SPDLOG_ERROR("ecc_secp256k1::get_public_key_hash_no_compressed Failed to get EC_KEY from EVP_PKEY, openssl error: {}", errorString);
            return false;
        }

        // 获取 EC_KEY 中的 EC_POINT
        const EC_POINT* ecPoint = EC_KEY_get0_public_key(ecKey);
        if (ecPoint == nullptr) {
            auto errno_ = ERR_get_error();
            //写入日志
            const char *errorString = ERR_error_string(errno_, nullptr);
            SPDLOG_ERROR("ecc_secp256k1::get_public_key_hash_no_compressed Failed to get EC_POINT from EC_KEY, openssl error: {}", errorString);
            EC_KEY_free(ecKey);
            return false;
        }

        // 将 EC_POINT 转换为字节数组
        size_t keyLength = EC_POINT_point2oct(EC_KEY_get0_group(ecKey), ecPoint, POINT_CONVERSION_UNCOMPRESSED, nullptr, 0, nullptr);
        if (keyLength == 0) {
            auto errno_ = ERR_get_error();
            //写入日志
            const char *errorString = ERR_error_string(errno_, nullptr);
            SPDLOG_ERROR("ecc_secp256k1::get_public_key_hash_no_compressed Failed to get key length., openssl error: {}", errorString);
            EC_KEY_free(ecKey);
            return false;
        }

        auto* keyBuffer = new unsigned char[keyLength];
        if (EC_POINT_point2oct(EC_KEY_get0_group(ecKey), ecPoint, POINT_CONVERSION_UNCOMPRESSED, keyBuffer, keyLength, nullptr) != keyLength) {
            auto errno_ = ERR_get_error();
            //写入日志
            const char *errorString = ERR_error_string(errno_, nullptr);
            SPDLOG_ERROR("ecc_secp256k1::get_public_key_hash_no_compressed Failed to convert EC_POINT to byte array, openssl error: {}", errorString);
            EC_KEY_free(ecKey);
            delete[] keyBuffer;
            return false;
        }

        hash_handler::sha3_256(keyBuffer, keyLength, out.get_data());
        // 释放资源
        EC_KEY_free(ecKey);
        delete[] keyBuffer;
        return true;
    }

    auto ecc_secp256k1::read_private_key_from_file(const std::string &_pri_key_path, const std::string &password) -> EVP_PKEY * {
        if (password.empty()){
            return nullptr;
        }
        std::filesystem::path save_path(_pri_key_path);
        if (!std::filesystem::exists( save_path)){
            SPDLOG_ERROR("ecc_secp256k1::read_private_key_from_file the path \"{}\" not exist!", _pri_key_path);
            return nullptr;
        }
        FILE* file_pri = nullptr;
#ifdef _WIN32
        auto err_pub = fopen_s(&file_pri, _pri_key_path.c_str(), "rb");
        if (err_pub != 0) {
            SPDLOG_ERROR("ecc_secp256k1::read_private_key_from_file fopen_s failed");
            return nullptr;
        }
#elifdef __linux__
        file_pri = fopen(_pri_key_path.c_str(), "wb");
        if (file_pri == nullptr) {
            return nullptr;
        }
#endif
        EVP_PKEY * pkey = nullptr;
        pkey = PEM_read_PrivateKey(file_pri, &pkey, nullptr, (void *)password.c_str());
        fclose(file_pri);
        return pkey;
    }


}