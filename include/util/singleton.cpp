//
// Created by 14270 on 2024-01-07.
//
#include "singleton.hpp"

namespace muse::chain{
    template <> std::shared_ptr<std::pmr::synchronized_pool_resource> Singleton() {
        static std::pmr::pool_options options {4096,1024*1024};
        static auto* resource = new std::pmr::synchronized_pool_resource(options);
        static std::shared_ptr<std::pmr::synchronized_pool_resource> instance( resource,
           [&](std::pmr::synchronized_pool_resource *ptr)->void {
               delete ptr;
               resource = nullptr;
           });
        return instance;
    }
}