//
// Created by 14270 on 2024-01-04.
//

#ifndef GLORIA_MPT_NODE_CACHE_HPP
#define GLORIA_MPT_NODE_CACHE_HPP

#include <string>
#include "util/singleton.hpp"

namespace muse::chain{
    template<typename Cache_Element_Type>
    struct mpt_node_cache{
        /* 是否存在这个节点 */
        bool exist {false};
        /* 内存地址 */
        Cache_Element_Type *node {nullptr};
        /* 存储在 leveldb 中节点的key */
        std::string key;
    };
}
#endif //GLORIA_MPT_NODE_CACHE_HPP
