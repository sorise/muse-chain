//
// Created by 14270 on 2024-01-28.
//

#ifndef GLORIA_LEVELDB_ACCOUNT_COMMIT_HPP
#define GLORIA_LEVELDB_ACCOUNT_COMMIT_HPP

#include "uint256.hpp"
#include  <map>

namespace muse::chain{
    /*
     * @Des: 存储每次区块提交后数据改变了的账户信息
     * */
    struct leveldb_account_commit{
        //prev 区块 hash
        uint256 block_hash;

        uint64_t height;     //区块高度

        uint256 prev_mpt_root; //上一个区块的MPT根

        std::map<uint256 ,muse::serializer::BinarySerializer> commits;

        leveldb_account_commit();

        leveldb_account_commit(const leveldb_account_commit &other) = delete;

        leveldb_account_commit(leveldb_account_commit &&other) noexcept;

        auto operator=(const leveldb_account_commit &other) -> leveldb_account_commit & = delete;

        auto operator=(leveldb_account_commit &&other) noexcept -> leveldb_account_commit &;

        ~leveldb_account_commit() = default;
    };
}
#endif //GLORIA_LEVELDB_ACCOUNT_COMMIT_HPP
