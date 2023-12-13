//
// Created by remix on 23-9-23.
//

#ifndef MUSE_CHAIN_BLOCK_HPP
#define MUSE_CHAIN_BLOCK_HPP

#include <vector>

#include "uint256.hpp"
#include "transaction.hpp"
#include "affair.hpp"
#include "serializer/IbinarySerializable.h"

#include "block_header.hpp"
#include "block_body.hpp"

namespace muse::chain{
    /*
     * 区块链 - 区块数据
     * 内部含有：交易事务、内部日志
     * 时间使用 UTC-0 时区的时间
     * */
    struct block: public muse::serializer::IBinarySerializable{
        block_header header;
        block_body body;
        std::string signature; //签名

        MUSE_IBinarySerializable(header,body, signature)

        [[nodiscard]] auto get_hash() const ->uint256;

        block() = default;

        block(const block& other);

        block(block&& other) noexcept;

        auto operator=(const block& other) -> block&;

        auto operator=(block&& other) noexcept -> block&;

        virtual ~block() = default;
    };

}

#endif //MUSE_CHAIN_BLOCK_HPP
