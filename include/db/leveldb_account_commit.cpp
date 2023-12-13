//
// Created by 14270 on 2024-01-28.
//
#include "leveldb_account_commit.hpp"

namespace muse::chain{
    leveldb_account_commit::leveldb_account_commit()
    : block_hash(0), height(0){

    }

    leveldb_account_commit::leveldb_account_commit(leveldb_account_commit &&other) noexcept
    : block_hash(std::move(other.block_hash)),
    height(other.height),
    commits(std::move(other.commits)),
    prev_mpt_root(std::move(other.prev_mpt_root)){

    }

    auto leveldb_account_commit::operator=(leveldb_account_commit &&other) noexcept -> leveldb_account_commit & {
        if (this != &other){
            this->block_hash = std::move(other.block_hash);
            this->height = other.height;
            this->commits = std::move(other.commits);
            this->prev_mpt_root = std::move(other.prev_mpt_root);
        }
        return *this;
    }
}
