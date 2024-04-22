//
// Created by remix on 24-4-14.
//

#include "stack_calculator.hpp"

namespace muse::chain{
    auto stack_calculator::epoch(uint64_t chain_height, uint64_t block_height, uint32_t n) -> int64_t {
        uint64_t up = chain_height + (chain_height%n - block_height);
        int64_t epoch = static_cast<int64_t>(floor( static_cast<double>(up) / n * 1.0))  - 1;
        return epoch;
    }

    auto stack_calculator::decay(int64_t _epoch) -> double {
        if (_epoch > 20){
            return 0;
        }
        constexpr const double limit = 0.01;
        constexpr const uint32_t scale = 4096;
        int64_t emi = _epoch * -2 + 3;
        return scale * pow(exp(1), static_cast<double>(emi)) + limit;
    }
}