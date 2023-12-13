//
// Created by 14270 on 2024-01-10.
//
#ifndef GLORIA_APPLICATION_HPP
#define GLORIA_APPLICATION_HPP

#include <leveldb/db.h>
#include <leveldb/cache.h>
#include <leveldb/slice.h>
#include <filesystem>
#include <exception>

#include "chain_ini.hpp"
#include "serializer/binaryDeserializeView.hpp"
#include "inicpp.hpp"
#include "encryption/ecc_secp256k1.hpp"
#include "application_state.hpp"
#include "db/affair.hpp"
#include "db/block.hpp"
#include "db/merkle_tree.hpp"
#include "util/timebox.hpp"
#include "util/logger.hpp"

namespace muse::chain{
    class application {
    public:
        static constexpr const uint32_t Minimum_Initialization_Nodes = 10;

        static constexpr const uint8_t EPOCH_MAX = UINT8_MAX;

        static constexpr const uint32_t Epoch_Time_Uint = 300000000;
    private:
        chain_ini setting_ini {};

        bool load {false};

        application_state state;
        //节点私钥
        EVP_PKEY *private_key;
        //节点公钥
        EVP_PKEY *public_key;

        uint256 public_key_hash;
    public:
        application();

        application(const application& other) = default;

        application(application&& other) noexcept;

        [[nodiscard]] auto get_application_state() const -> const application_state&;

        //启动区块链应用初始化程序
        auto load_setting_file(const std::string& path) -> bool;
        /* 初始化检测，如果未初始化则直接初始化,如果已经初始化了，则跳过，否则创建两个leveldb数据库 , 并且创建创世块  */
        auto initialize() -> void;
        /* 加载节点公私钥,有点不安全，公私钥没有加密 */
        auto load_node_keys(const std::string &_password) -> bool;

        auto package_inject_special_affairs(bool &success, std::list<affair> participants) -> block;

        [[nodiscard]] auto get_chain_ini() const -> const chain_ini&;

        static auto get_accounts_db(const std::string& path) -> leveldb::DB*;

        static auto get_blocks_db(const std::string& path) -> leveldb::DB*;

        static auto get_extensions_db(const std::string& path) -> leveldb::DB*;

        static auto get_assets_db(const std::string& path) -> leveldb::DB*;

        static auto get_chain_db(const std::string& path) -> leveldb::DB*;

        //对区块进行数字签名
        auto signature_block(block &blk) -> bool;

        ~application() ;
    };
}


#endif //GLORIA_APPLICATION_HPP
