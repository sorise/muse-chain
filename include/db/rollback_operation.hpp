//
// Created by 14270 on 2024-01-05.
//

#ifndef GLORIA_ROLLBACK_OPERATION_HPP
#define GLORIA_ROLLBACK_OPERATION_HPP
#include <iostream>
#include "uint256.hpp"

namespace muse::chain{
    /* 交易 ---> mpt 操作 */
    struct rollback_operation{
        static const int INSERT  = 1;   /* type值 怎么会出现INSERT呢？有删除用户这个操作吗？ */
        static const int DELETE_ACCOUNT  = 2;
        static const int UPDATE  = 4;
        static const int UPDATE_ADD = 8; /* 追加某个账户的资产 */
        static const int UPDATE_REDUCE = 16; /* 减少某个账户的资产 */
        static const int UPDATE_DELETE_ASSET = 32;/* 删除这个账户创建的资源 */
        static const int ONLY_REDUCE_NONCE = 64; /* 执行的是affair,更新nonce */
        /*
         * insert 插入同一个账户
         * delete 删除一个账户
         * update 修改一个账户的值
         * */
        int8_t type; /* 操作类型 */

        uint32_t asset_type; /* 资产类型 */

        uint256 target_address;

        int8_t update_type ; /* update add or reduce */

        uint64_t value; /* 增加 or 减少 多少 */

        auto to_string() const-> std::string;
    };
}

#endif //GLORIA_ROLLBACK_OPERATION_HPP
