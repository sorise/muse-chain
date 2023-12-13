//
// Created by 14270 on 2024-01-28.
//

#include "block_body.hpp"

namespace muse::chain{
    block_body::block_body(const block_body &other)
    :transactions(other.transactions),
    affairs(other.affairs){

    }

    block_body::block_body(block_body &&other) noexcept
            :transactions(std::move(other.transactions)), affairs(std::move(other.affairs)){

    }

    auto block_body::operator=(const block_body &other) -> block_body & {
        if (this != &other){
            this->transactions = other.transactions;
            this->affairs = other.affairs;
        }
        return *this;
    }

    auto block_body::operator=(block_body &&other) noexcept -> block_body & {
        if (this != &other){
            this->transactions = std::move(other.transactions);
            this->affairs = std::move(other.affairs);
        }
        return *this;
    }
}