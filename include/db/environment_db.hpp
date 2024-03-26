//
// Created by 14270 on 2024-01-02.
//

#ifndef GLORIA_ENVIRONMENT_DB_HPP
#define GLORIA_ENVIRONMENT_DB_HPP

#include <chrono>


#include "util/singleton.hpp"

namespace muse::chain{
    /* 存储层总体设置 */
    class environment_db {
    public:
        static constexpr const uint32_t EPOCH_TIME_UINT = 300000; //300000 毫秒 等于五分钟！

        static constexpr const uint64_t ASSET_MAX_COUNT = 68719476736; //2^36 687亿单位

        static auto get_now_tick() -> std::chrono::milliseconds;
    };
}


#endif //GLORIA_ENVIRONMENT_DB_HPP
