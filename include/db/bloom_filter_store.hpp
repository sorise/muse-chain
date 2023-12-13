//
// Created by 14270 on 2024-01-28.
//
#ifndef GLORIA_BLOOM_FILTER_STORE_HPP
#define GLORIA_BLOOM_FILTER_STORE_HPP

#include "serializer/IbinarySerializable.h"
#include "uint256.hpp"

namespace muse::chain{
    /* 存储在那个地方是个问题啊 */
    struct bloom_filter_store: public muse::serializer::IBinarySerializable{
        uint64_t block_height;              //区块高度

        uint256 block_hash;                 //区块hash

        std::string bloom_filter_data;      //数据

        bloom_filter_store(const uint64_t & height, const uint256 & hash, std::string  data);

        bloom_filter_store();

        bloom_filter_store(const bloom_filter_store& other) = delete;

        bloom_filter_store(bloom_filter_store&& other) noexcept;

        bloom_filter_store& operator=(const bloom_filter_store& other) = delete;

        bloom_filter_store& operator=(bloom_filter_store&& other) noexcept;

        virtual ~bloom_filter_store() = default;

        MUSE_IBinarySerializable(block_hash, block_height, bloom_filter_data);
    };
}


#endif //GLORIA_BLOOM_FILTER_STORE_HPP
