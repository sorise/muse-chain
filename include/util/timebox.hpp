//
// Created by 14270 on 2024-01-13.
//

#ifndef GLORIA_TIMEBOX_HPP
#define GLORIA_TIMEBOX_HPP
#include <string>
#include <ctime>
#include <cstring>

namespace muse::utils{
    //获得时区,东 八 区 +8, 西 八 区 -8
    auto get_zone() -> int;

    //得到当前时间
    auto get_microseconds_now_UTC_default() -> uint64_t;

    auto get_microseconds_now_UTC_Zone_0() -> uint64_t;

    auto get_now_UTC_Zone_0_string() -> std::string;

    auto get_now_UTC_default_string() -> std::string;
}


#endif //GLORIA_TIMEBOX_HPP
