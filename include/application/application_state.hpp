//
// Created by 14270 on 2024-01-11.
//

#ifndef GLORIA_APPLICATION_STATE_HPP
#define GLORIA_APPLICATION_STATE_HPP

#include "serializer/IbinarySerializable.h"
#include "db/uint256.hpp"

namespace muse::chain{
    struct application_state: public muse::serializer::IBinarySerializable{
        //存储在LeveBD中key的值！
        static std::string STORE_STATE_KEY;
        //当前区块高度
        uint64_t current_height;
        //当前最新区块的hash值
        uint256  top_block_hash;
        //创世块hash值
        uint256 initial_block_hash;

        MUSE_IBinarySerializable(current_height, top_block_hash, initial_block_hash);

        application_state() = default;

        application_state(const application_state& other);

        application_state(application_state&& other) noexcept;

        auto operator=(const application_state& other) -> application_state&;

        auto operator=(application_state&& other) noexcept -> application_state&;

        auto operator==(const application_state& other) const -> bool;

        application_state(const uint64_t& _current_height,const uint256& _initial_block_hash, const uint256& _top_block_hash );


        virtual ~application_state() = default;
    };

}

#endif //GLORIA_APPLICATION_STATE_HPP
