//
// Created by 14270 on 2024-01-02.
//

#include "environment_db.hpp"

namespace muse::chain{

    auto environment_db::get_now_tick() -> std::chrono::milliseconds {
        std::chrono::time_point<std::chrono::system_clock> tp =
                std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
        return std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch());
    }

}