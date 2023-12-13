//
// Created by 14270 on 2024-01-13.
//
#include "application_state.hpp"

namespace muse::chain{
    std::string application_state::STORE_STATE_KEY = "____CHAIN___APPLICATION___STATE___KEY____";

    application_state::application_state(const uint64_t &_current_height, const uint256 &_initial_block_hash,const uint256 &_top_block_hash)
     :current_height(_current_height), initial_block_hash(_initial_block_hash), top_block_hash(_top_block_hash){

    }

    application_state::application_state(const application_state &other)
    :current_height(other.current_height),
    initial_block_hash(other.initial_block_hash),
    top_block_hash(other.top_block_hash){

    }

    application_state::application_state(application_state &&other) noexcept
    :current_height(other.current_height),
    initial_block_hash(std::move(other.initial_block_hash)),
    top_block_hash(std::move(other.top_block_hash))
    {

    }

    application_state &application_state::operator=(const application_state &other) {
        if (this != &other){
            this->current_height = other.current_height;
            this->initial_block_hash = other.initial_block_hash;
            this->top_block_hash = other.top_block_hash;
        }
        return *this;
    }

    auto application_state::operator==(const application_state &other) const -> bool {
        return this->current_height == other.current_height &&
               this->initial_block_hash == other.initial_block_hash &&
               this->top_block_hash == other.top_block_hash;
    }

    auto application_state::operator=(application_state &&other) noexcept -> application_state & {
        if (this != &other){
            this->current_height = other.current_height;
            this->initial_block_hash = std::move(other.initial_block_hash);
            this->top_block_hash = std::move(other.top_block_hash);
        }
        return *this;
    }
}