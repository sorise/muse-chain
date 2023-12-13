//
// Created by 14270 on 2023-12-25.
//

#ifndef GLORIA_IGET_HASH_SHA3256_HPP
#define GLORIA_IGET_HASH_SHA3256_HPP

#include "uint256.hpp"

namespace muse::chain{
    class IGetHashSha3256{
        //获得hash
        virtual auto get_hash() -> uint256 = 0;
    };
}

#endif //GLORIA_IGET_HASH_SHA3256_HPP
