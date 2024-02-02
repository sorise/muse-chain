//
// Created by 14270 on 2024-01-13.
//

#include "timebox.hpp"

namespace muse::utils{
    auto get_zone() -> int{
        // 获取系统时间
        time_t system_time = time(nullptr);
        // 系统时间转换为GMT时间
        tm gtm_time{};
        //本地时间转换为GMT时间
#ifdef _WIN32
        gmtime_s(&gtm_time,&system_time);
#elifdef __linux__
        gmtime_r(&system_time,&gtm_time);
#endif
        // 再将GMT时间重新转换为系统时间
        time_t _gt = mktime(&gtm_time);
        tm _gtm2{};
#ifdef  _WIN32
        localtime_s(&_gtm2,&_gt);
#elif defined(__linux__)
        localtime_r(&_gt, &_gtm2);
#endif
        // 这时的_gt已经与实际的系统时间_rt有时区偏移了,计算两个值的之差就是时区偏的秒数,除60就是分钟
        long long offset = (((system_time - _gt ) + (_gtm2.tm_isdst != 0 ? 3600 : 0)) / 60)/60;
        return static_cast<int>(offset);
    }

    auto get_microseconds_now_UTC_default() -> uint64_t{
        timespec utc_time{};
        timespec_get(&utc_time, TIME_UTC);
        uint64_t tick = utc_time.tv_sec * 1000000 + utc_time.tv_nsec / 1000;
        return tick;
    }

    auto get_microseconds_now_UTC_Zone_0() -> uint64_t{
        timespec utc_time{};
        timespec_get(&utc_time, TIME_UTC);
        uint64_t tick = utc_time.tv_sec * 1000000 + utc_time.tv_nsec / 1000;
        int zone = get_zone();
        tick = tick - (zone * 60 * 60 * 1000);
        return tick;
    }

    auto get_now_UTC_Zone_0_string() -> std::string{
        // 获取系统时间
        time_t system_time = time(nullptr);
        // 系统时间转换为GMT时间
        tm gtm_time{};
        //本地时间转换为GMT时间
#ifdef  _WIN32
        gmtime_s(&gtm_time,&system_time);
#elif defined(__linux__)
        gmtime_r(&system_time,&gtm_time);
#endif
        // 系统时间转换为本地时间
        tm local_time{};
#ifdef _WIN32
        localtime_s(&local_time,&system_time);
#elifdef __linux__
        localtime_r(&system_time,&local_time);
#endif
        char buffer[128]{'\0'};
        std::strftime(buffer, sizeof(buffer), "%Y/%m/%d %H:%M:%S", &gtm_time);
        return {buffer, strlen(buffer)};
    }

    auto get_now_UTC_Zone_default_string() -> std::string{
        // 获取系统时间
        time_t system_time = time(nullptr);
        //本地时间转换为GMT时间
        tm local_time{};
#ifdef _WIN32
        localtime_s(&local_time,&system_time);
#elifdef __linux__
        localtime_r(&system_time,&local_time);
#endif
        char buffer[128]{'\0'};
        std::strftime(buffer, sizeof(buffer), "%Y/%m/%d %H:%M:%S", &local_time);
        return {buffer, strlen(buffer)};
    }
}