//
// Created by 14270 on 2024-01-28.
//

#ifndef GLORIA_BLOCK_BODY_HPP
#define GLORIA_BLOCK_BODY_HPP

#include "transaction.hpp"
#include "affair.hpp"
#include "serializer/IbinarySerializable.h"
#include <vector>

namespace muse::chain{
    /* 区块体 */
    struct block_body : public muse::serializer::IBinarySerializable{

        /* 所有的交易 */
        std::vector<transaction> transactions;
        /* 事务 */
        std::vector<affair> affairs;

        MUSE_IBinarySerializable(transactions,affairs)

        block_body() = default;

        block_body(const block_body& other);

        block_body(block_body&& other) noexcept;

        auto operator=(const block_body& other) -> block_body&;

        auto operator=(block_body&& other) noexcept -> block_body&;

        virtual ~block_body() = default;
    };
}

#endif //GLORIA_BLOCK_BODY_HPP
