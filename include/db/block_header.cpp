//
// Created by 14270 on 2024-01-28.
//

#include "block_header.hpp"

namespace muse::chain{
    [[maybe_unused]] auto block_header::get_block_hash() const -> uint256 {
        uint256 result = prev_block_hash + transactions_merkle_root + affairs_merkle_root;
        uint256 temp(0);
        encryption::get_hash_base()->get_hash(&height, sizeof(height), temp.get_data());
        result = result + temp;
        encryption::get_hash_base()->get_hash(&create_time, sizeof(create_time), temp.get_data());
        result = result + temp;
        if (!black_block_hashes.empty()){
            for (const auto& hash: black_block_hashes) {
                temp = temp + hash;
            }
            result = result + temp;
        }
        result = result.get_sha3_256();
        return result;
    }

    block_header::block_header()
    :prev_block_hash(0),
     transactions_merkle_root(0),
     affairs_merkle_root(0),
     merkle_patricia_trie_root(0),
     height(0),
     create_time(0) {

    }

    block_header::block_header(const block_header &other)
    :prev_block_hash(other.prev_block_hash)
    ,assets_blf_hash(other.assets_blf_hash)
    ,transactions_merkle_root(other.transactions_merkle_root)
    ,affairs_merkle_root(other.affairs_merkle_root)
    ,merkle_patricia_trie_root(other.merkle_patricia_trie_root)
    ,black_block_hashes(other.black_block_hashes)
    ,height(other.height)
    ,create_time(other.create_time){

    }

    block_header::block_header(block_header &&other) noexcept
    :prev_block_hash(std::move(other.prev_block_hash))
    ,assets_blf_hash(std::move(other.assets_blf_hash))
    ,transactions_merkle_root(std::move(other.transactions_merkle_root))
    ,affairs_merkle_root(std::move(other.affairs_merkle_root))
    ,merkle_patricia_trie_root(std::move(other.merkle_patricia_trie_root))
    ,black_block_hashes(std::move(other.black_block_hashes))
    ,height(other.height)
    ,create_time(other.create_time){

    }

    auto block_header::operator=(const block_header &other) -> block_header & {
        if (this != &other){
            this->height = other.height;
            this->create_time = other.create_time;
            this->prev_block_hash = other.prev_block_hash;
            this->assets_blf_hash = other.assets_blf_hash;
            this->transactions_merkle_root = other.transactions_merkle_root;
            this->affairs_merkle_root = other.affairs_merkle_root;
            this->merkle_patricia_trie_root = other.merkle_patricia_trie_root;
            this->black_block_hashes = other.black_block_hashes;
        }
        return *this;
    }

    auto block_header::operator=(block_header &&other) noexcept -> block_header & {
        if (this != &other){
            this->height = other.height;
            this->create_time = other.create_time;
            this->prev_block_hash = std::move(other.prev_block_hash);
            this->assets_blf_hash = std::move(other.assets_blf_hash);
            this->transactions_merkle_root = std::move(other.transactions_merkle_root);
            this->affairs_merkle_root = std::move(other.affairs_merkle_root);
            this->merkle_patricia_trie_root = std::move(other.merkle_patricia_trie_root);
            this->black_block_hashes = std::move(other.black_block_hashes);
        }
        return *this;
    }
}