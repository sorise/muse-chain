#ifndef MUSE_CHAIN_STACK_CALCULATOR_HPP
#define MUSE_CHAIN_STACK_CALCULATOR_HPP
#include <iostream>
#include <cstdint>
#include <cmath>

namespace muse::chain{
    class stack_calculator {
    public:
        static auto epoch(uint64_t chain_height, uint64_t block_height, uint32_t n) -> int64_t;

        static auto decay(int64_t _epoch)->double;
    };
}

#endif //MUSE_CHAIN_STACK_CALCULATOR_HPP
