//
// Created by remix on 23-9-23.
//

#include "block.hpp"

namespace muse::chain{
    [[maybe_unused]] uint256 block::get_block_hash() {
        uint256 result = prev_block_hash + data_block_hash + transactions_merkle_root + affairs_merkle_root;
        uint256 temp(0);
        encryption::get_hash_base()->get_hash((const char *)&height, sizeof(typeof(height)), temp.get_data());
        result = result + temp;
        encryption::get_hash_base()->get_hash((const char *)&create_time, sizeof(typeof(create_time)), temp.get_data());
        result = result + temp;
        return result;
    }

    block::block():prev_block_hash(0),data_block_hash(0),transactions_merkle_root(0), affairs_merkle_root(0),height(0),create_time(0) {

    }
};
