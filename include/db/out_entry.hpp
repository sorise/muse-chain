//
// Created by remix on 23-10-7.
//
#ifndef MUSE_CHAIN_OUT_ENTRY_HPP
#define MUSE_CHAIN_OUT_ENTRY_HPP
#include <iostream>
#include "serializer/IbinarySerializable.h"
#include "uint256.hpp"

namespace muse::chain{
    struct out_entry: public muse::serializer::IBinarySerializable{
        uint256 receiver;
        uint64_t count;
        MUSE_IBinarySerializable(receiver, count);


        out_entry();
        out_entry(const uint256& other, const uint64_t& count);
        out_entry(const out_entry &other);
        ~out_entry() = default;
    };
}
#endif //MUSE_CHAIN_OUT_ENTRY_HPP
