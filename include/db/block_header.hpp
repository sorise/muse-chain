//
// Created by 14270 on 2024-01-28.
//

#ifndef GLORIA_BLOCK_HEADER_HPP
#define GLORIA_BLOCK_HEADER_HPP


#include "uint256.hpp"
#include "../encryption/encryption.hpp"

namespace muse::chain{
    class block_header : public muse::serializer::IBinarySerializable{
    public:
        uint256 prev_block_hash;              // 前区块hash值
        uint256 assets_blf_hash;              // 资产过滤器
        uint256 transactions_merkle_root;     // 交易默克尔树 root
        uint256 affairs_merkle_root;          // 事务默克尔树 root
        uint256 merkle_patricia_trie_root;    // 账户默克尔树 root
        std::vector<uint256> black_block_hashes; // 黑名单区块 hash
        uint64_t height;         // 区块高度
        uint64_t create_time;    // 创建时间

        block_header();

        block_header(const block_header& other);

        block_header(block_header&& other) noexcept;

        auto operator=(const block_header& other) -> block_header&;

        auto operator=(block_header&& other) noexcept -> block_header&;

        [[nodiscard]] auto get_block_hash() const -> uint256;

        MUSE_IBinarySerializable(
                prev_block_hash,
                assets_blf_hash,
                transactions_merkle_root,
                affairs_merkle_root,
                merkle_patricia_trie_root,
                black_block_hashes,
                height,
                create_time
        )

        virtual ~block_header() = default;
    };
}


#endif //GLORIA_BLOCK_HEADER_HPP
