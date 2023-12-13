//
// Created by 14270 on 2023-12-27.
//

#ifndef GLORIA_CONNECTOR_ACCOUNT_HPP
#define GLORIA_CONNECTOR_ACCOUNT_HPP

#include <list>
#include <queue>
#include <memory>
#include <memory_resource>
#include <set>

#include "spdlog/spdlog.h"
#include "serializer/IbinarySerializable.h"
#include "merkle_patricia_trie.hpp"
#include "account.hpp"

#include "transaction.hpp"
#include "block.hpp"
#include "util/singleton.hpp"
#include "environment_db.hpp"
#include "chain_db_exception.hpp"
#include "rollback_operation.hpp"
#include "receipt.hpp"
#include "bloom_filter.hpp"
#include "leveldb/db.h"
#include "leveldb/options.h"
#include "leveldb/write_batch.h"
#include "leveldb/slice.h"
#include "connector_block.hpp"
#include "util/timebox.hpp"
#include "application/application.hpp"
#include "leveldb_account_commit.hpp"

namespace muse::chain{

    /*
     * levelDB 硬盘 <-> merkle_patricia_trie 内存
     * levelDB 一个库用来存真实的账户       account_db
     * levelDB 一个库用来存中间节点信息     account_mpt_middle_db
     */
    class connector_account {
    private:
        using RollBack_Storage_TP = std::list<rollback_operation>;
        using Extension_Element_TP = std::map<std::string, mpt_node_storage<merkle_patricia_trie<account>::CHILDREN_COUNT>>;
    public:
        // 空节点key长度
        static const size_t EMPTY_KEY_LENGTH = 64;
        // 空节点key长度
        static const char EMPTY_KEY_CHAR = 0;
        // 账户树
        merkle_patricia_trie<account>  account_mpt;
    private:
        //资产过滤器
        bloom_filter  asset_bloom_filter;
        //当前账户树的根hash值
        uint256     merkle_patricia_trie_root_hash;
        // 当前区块链高度
        uint64_t    block_height;
        // 最新的区块hash值
        uint256     top_block_hash;
        // 账户的bloom filter
        std::list<bloom_filter>  account_bloom_filters;
        // 账户提交队列，缓存区
        std::list<leveldb_account_commit>  commits;
        // 保存回退点
        std::list<RollBack_Storage_TP>  rollbacks;
        /* mpt 要和 资产bloom filter 一致！ */
        std::list<Extension_Element_TP> extensions;

        leveldb::DB *extensions_db;

        leveldb::DB *accounts_db;

        leveldb::DB *assets_db;
    public:
        auto get_block_height() const -> uint64_t;

        auto get_top_block_hash() const -> uint256;

        //默认构造函数，智能创建内存型，用于测试使用
        connector_account();

        connector_account(leveldb::DB *_extensions_db, leveldb::DB *_accounts_db, leveldb::DB *_assets_db,  const application_state& _state);

        connector_account(const connector_account &other) = delete;

        connector_account(connector_account &&other) noexcept ;

        auto operator=(const connector_account &other) = delete;

        auto operator=(connector_account &&other) noexcept -> connector_account &;

        ~connector_account();

        /* 查看某个账户是否存在 */
        auto exists(const uint256 &key) -> bool;

        /* 检查某个交易是否合法, 单个检查*/
        auto check_transaction(transaction &trans) -> bool;

        /* 检查某个区块内容是否合法, 检测交易内容，不检查区块头  */
        auto check_block_body(block &blk) -> bool;

        /* 插入一个 block */
        auto insert_block(block &blk, bool check = true) -> bool;

        /* 回滚操作 */
        auto rollback(const uint32_t& step) -> bool;

        /* 执行回滚操作 */
        auto execute_rollback(const RollBack_Storage_TP& rtp) -> bool;

        /* 将中间节点数据写入leveldb */
        auto commit_extensions_node_to_leveldb(const Extension_Element_TP &extensions) -> bool;

        auto commit_account_to_leveldb(const leveldb_account_commit &lac) -> bool;

        auto commit_asset_bloom_filter_to_leveldb(const bloom_filter &blf) -> bool;

        //打包生成一个区块,需要一个高效的算法，对于随机出现各种交易
        auto package_next_block_body(block& blk,connector_block &c_blk, const chain_ini& _ini) -> void ;

        // @desc: 填充区块头部的 merkle_patricia_trie_root
        auto package_next_block_MPTR(block &blk) noexcept -> bool;

        //将已经确认的区块相关数据写入leveldb中, 进行落盘
        [[maybe_unused]] auto total_commit_to_leveldb() -> bool;

        [[maybe_unused]] auto total_rollback() -> bool;

        auto find_account(const uint256& address)-> account*;
        /* 输入必须是合法的交易 */
        auto append_created_rollback(RollBack_Storage_TP& rtp, transaction &trans) -> void ;

        auto append_created_rollback(RollBack_Storage_TP& rtp, const affair &afi) -> void;
    };
}
#endif //GLORIA_CONNECTOR_ACCOUNT_HPP
