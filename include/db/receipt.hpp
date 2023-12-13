//
// Created by 14270 on 2024-01-05.
//

#ifndef GLORIA_RECEIPT_HPP
#define GLORIA_RECEIPT_HPP

#include <map>
#include <iostream>
#include <vector>
#include <algorithm>

#include "transaction.hpp"

namespace muse::chain{
    struct receipt {
        //不重数，起始值
        uint64_t nonce_start;
        //std::pair<剩余余额, 需要减少的余额>
        std::map<uint32_t, std::pair<uint64_t, uint64_t>> balances;
        //不重数
        std::vector<uint64_t> nonce;
        /* 检测nonce是否合法 */
        auto check_nonce() -> bool;
        /* 检查交易结果是否合法 */
        auto check() -> bool;
        //默认构造函数
        receipt();
        //复制构造函数

        auto input_and_check(transaction& trans) -> bool;

        explicit receipt(const uint64_t& start);

        receipt(receipt&& other) noexcept ;

        auto operator=(const receipt& rcp) -> receipt&;

        auto operator=(receipt&& other) noexcept -> receipt&;

        ~receipt() = default;
    };
}


#endif //GLORIA_RECEIPT_HPP
