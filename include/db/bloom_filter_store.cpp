//
// Created by 14270 on 2024-01-28.
//

#include "bloom_filter_store.hpp"

namespace muse::chain{

    bloom_filter_store::bloom_filter_store(bloom_filter_store &&other) noexcept
    : block_height(other.block_height),
    block_hash(std::move(other.block_hash)),
    bloom_filter_data(std::move(other.bloom_filter_data)){

    }

    bloom_filter_store::bloom_filter_store(const uint64_t &height, const uint256& hash, std::string data)
    :block_height(height), block_hash(hash), bloom_filter_data(std::move(data)){

    }

    bloom_filter_store::bloom_filter_store()
    :block_height(0), block_hash(0){

    }

    bloom_filter_store &bloom_filter_store::operator=(bloom_filter_store &&other) noexcept {
        if (&other != this){
            this->block_height = other.block_height;
            this->block_hash = std::move(other.block_hash);
            this->bloom_filter_data = std::move(other.bloom_filter_data);
        }
        return *this;
    }
}