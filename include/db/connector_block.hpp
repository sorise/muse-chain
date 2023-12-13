//
// Created by 14270 on 2024-01-23.
//

#ifndef GLORIA_CONNECTOR_BLOCK_HPP
#define GLORIA_CONNECTOR_BLOCK_HPP
#include <list>
#include <queue>
#include <memory>
#include <memory_resource>

#include "serializer/IbinarySerializable.h"
#include "merkle_patricia_trie.hpp"
#include "account.hpp"
#include "uint256.hpp"
#include "transaction.hpp"
#include "block.hpp"
#include "util/singleton.hpp"
#include "environment_db.hpp"
#include "chain_db_exception.hpp"
#include "rollback_operation.hpp"
#include "receipt.hpp"
#include "bloom_filter.hpp"

#include "application/chain_ini.hpp"
#include "leveldb/db.h"
#include "leveldb/options.h"
#include "leveldb/write_batch.h"
#include "leveldb/slice.h"
#include "merkle_tree.hpp"
#include "util/logger.hpp"
#include "application/application_state.hpp"

namespace muse::chain{
    class connector_block {
    private:
        leveldb::DB * leveldb_blocks {nullptr};
        leveldb::DB * leveldb_chain {nullptr};
        //当前区块高度
        uint64_t current_height {0};
        //当前最新区块的hash值
        uint256  top_block_hash;
        //创世块hash值
        uint256 initial_block_hash;
        //缓存最新的几个区块
        std::list<block> block_queue;
    public:
        //交易队列
        std::list<transaction> transactions_queue;
        //事务队列
        std::list<affair> affairs_queue;

        connector_block() = default;

        explicit connector_block(leveldb::DB *_leveldb_blocks,leveldb::DB * leveldb_chain, const application_state& _state);

        /* 将区块缓存到缓存队列中，等待落盘 */
        auto input_block_cache(const block& blk) -> void;
        /* 将区块缓存到缓存队列中，等待落盘 */
        auto input_block_cache(block&& blk) -> void;

        //讲一个区块加入链上
        auto update_connector(uint64_t _current_height, uint256  _top_block_hash) ->void;

        [[nodiscard]] auto get_current_height() const -> uint64_t;

        [[nodiscard]] auto get_top_block_hash() const -> uint256;

        auto package_next_block_header(block &blk, const chain_ini& _ini, const std::vector<uint256> &blacks = {}) -> bool;

        /* 需要存储机制存储每个高度index对应的区块hash值 */
        auto get_block_by_index(const uint64_t& height, bool &success) const -> block;

        [[nodiscard]] auto store_index_hash(const uint64_t& height, uint256 hash) const -> bool;

        auto load_hash_by_index(const uint64_t& height, bool &success) const -> uint256;

        //把区块插入到链上
        auto insert_block_save_to_disk() -> bool;

    private:
        //把区块插入到leveldb数据库中
        auto insert_block_to_levelDB(block &blk) -> bool;
    };
}
#endif //GLORIA_CONNECTOR_BLOCK_HPP
