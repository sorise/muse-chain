//
// Created by remix on 23-11-21.
//
#ifndef MUSE_CHAIN_ENCRYPTION_HPP
#define MUSE_CHAIN_ENCRYPTION_HPP
#include <memory>
#include "hash_handler.hpp"

namespace muse::chain{
    class encryption {
    public:
        static std::shared_ptr<hash_base> get_hash_base();
    };
}
#endif //MUSE_CHAIN_ENCRYPTION_HPP
