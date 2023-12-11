//
// Created by remix on 23-9-23.
//

#ifndef MUSE_CHAIN_TRANSACTION_HPP
#define MUSE_CHAIN_TRANSACTION_HPP
#include <iostream>
#include "serializer/IbinarySerializable.h"
#include "uint256.hpp"
#include "out_entry.hpp"
#include "encryption/encryption.hpp"

namespace muse::chain{

    enum class transaction_version: uint8_t {
        Ban = 0,        //非指定类型、已禁用
        V0001 = 1,      //第一个版本
    };

    enum class transaction_operator: uint8_t {
        NullOP = 0, //非法操作
        Append = 1, // 向他人追加资产、自己减少资产
        Establish = 2, // 创建一类资产
    };

    /* 一个用户级交易, 一个count 表示实际金额 0.001
     * 第 0 字节          标识版本
     * 第 1-4 字节        标识资产类型
     * 第 5 字节标识       标识操作类型
     * 第 6-13 字节       标识交易数量
     * 第 14-21 字节      标识交易生产时间, 8 字节
     * 第 22 字节         标识过期的 4 epoch 数量，如果值是200.则等待800个epoll后无效
     * 第 23-54 字节      标识发送者公钥
     * 第 55 ~ 63 字节    8 字节标识 不重数
     * 第 64 - 127 字节   64个字节存储额外信息 也可以不存储， 暂时取消 --- 2023.12.10
     * */
    class transaction: public muse::serializer::IBinarySerializable{
    public:
        static const uint16_t Bytes = 64;
    private:
        std::array<uint8_t, Bytes> data;
    public:
        //直接暴露接口
        std::vector<out_entry> outs; //交易输出
        /* 构造函数 */
        transaction();
        //设置交易类型
        void set_transaction_version(transaction_version _type);
        //获得交易类型
        transaction_version get_transaction_version();
        //设置资产类型 asset_type >= 1
        void set_asset_type(const uint32_t& _type);
        //获得交易类型
        uint32_t get_asset_type();
        //设置操作
        void set_transaction_operator(transaction_operator _op);
        //获得操作类型
        transaction_operator get_transaction_operator();
        //设置数量
        bool set_count(const uint64_t& _count);
        //获得数量
        uint64_t get_count();
        //设置交易产生时间
        bool set_create_time(const uint64_t& _create_time);
        //获得交易产生时间
        uint64_t get_create_time();
        //设置过期时间 当epoch 大于 多少时过期 、24576 秒=6.8266667 时
        bool set_expired_epoch(const uint8_t& _epoch);
        //获得过期时间
        uint8_t get_expired_epoch();
        //设置源账户 公钥
        void set_sender(uint256& _sender);
        //获得源账户
        uint256 get_sender();
        //设置不重数
        void set_nonce(uint64_t nonce);
        //获取不重数
        uint64_t get_nonce();
        //接口
        MUSE_IBinarySerializable(data,outs);
        //可以设置64个字节的备注信息
        //bool set_64_bytes_comment(const uint8_t * _store, const uint16_t& count);
        //获得备注信息
        //const uint8_t* get_64_bytes_comment();
    public:
        uint256 get_hash();
    };
}
#endif //MUSE_CHAIN_TRANSACTION_HPP
