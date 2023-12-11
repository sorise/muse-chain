//
// Created by remix on 23-9-23.
//

#ifndef MUSE_CHAIN_BLOCK_HPP
#define MUSE_CHAIN_BLOCK_HPP
#include "uint256.hpp"
#include "../encryption/encryption.hpp"

namespace muse::chain{
    /*
     * 区块链 - 区块数据
     * 内部含有：交易事务、内部日志
     * 时间使用 UTC-0 时区的时间
     * */
    class block {
    private:
        uint256 prev_block_hash;              // 前区块hash值
        uint256 data_block_hash;              // 数据区块hash值
        uint256 transactions_merkle_root;     // 交易默克尔树 root
        uint256 affairs_merkle_root;          // 事务默克尔树 root
        uint256 merkle_patricia_trie_root;    // 账户默克尔树 root
        uint64_t height;         // 区块高度
        uint64_t create_time;    // 创建时间
    public:
        block();
        [[maybe_unused]] uint256 get_block_hash();
    };
}

#endif //MUSE_CHAIN_BLOCK_HPP
