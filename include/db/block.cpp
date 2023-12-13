//
// Created by remix on 23-9-23.
//
#include "block.hpp"

namespace muse::chain{
    auto block::get_hash() const ->uint256{
        return this->header.get_block_hash();
    }

    block::block(const block &other)
    :header(other.header), body(other.body){

    }

    block::block(block &&other) noexcept
    :header(std::move(other.header)), body(std::move(other.body)){

    }

    auto block::operator=(const block &other) -> block & {
        if (this != &other){
            this->header = other.header;
            this->body = other.body;
        }
        return *this;
    }

    auto block::operator=(block &&other) noexcept -> block & {
        if (this != &other){
            this->header = other.header;
            this->body = other.body;
        }
        return *this;
    }
};
