//
// Created by 14270 on 2024-01-23.
//

#ifndef GLORIA_CHAIN_INI_HPP
#define GLORIA_CHAIN_INI_HPP
#include <iostream>
#include <string>

namespace muse::chain{

    /* 区块链的配置信息 */
    struct chain_ini{
        //区块链状态 leveldb 数据库 地址
        std::string chain_state_db_path;
        //区块链区块数据 leveldb 数据库 地址
        std::string block_db_path;
        //区块链账户数据 leveldb 数据库 地址
        std::string account_db_path;
        //区块链资产数据 leveldb 数据库 地址
        std::string assets_db_path;
        //初始化公钥地址路径
        std::string initial_keys_path;
        //扩展节点地址路径
        std::string extensions_db_path;
        //日志路径
        std::string log_file_path;
        //最多打包多少个交易
        uint32_t transaction_maximum_number_in_block {500};
        //一个区块最多打包多少个事务
        uint32_t affair_maximum_number_in_block {500};
        //一个区块最少打包多少个交易
        uint32_t transaction_minimum_number_in_block {1};
        //一个区块最少打包多少个事务
        uint32_t affair_minimum_number_in_block {1};
        //节点的私钥存放路径
        std::string private_key_path;
        //节点的公钥存放路径
        std::string public_key_path;
    };
}
#endif //GLORIA_CHAIN_INI_HPP
