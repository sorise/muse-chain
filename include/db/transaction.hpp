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

/* 表示一笔资产交易 */
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
     * */
    class transaction: public muse::serializer::IBinarySerializable{
    public:
        static const uint16_t Bytes = 64;
    private:
        std::array<uint8_t, Bytes> data;

        std::string signature;
    public:


        //直接暴露接口
        std::vector<out_entry> outs; //交易输出

        /* 默认构造函数 */
        transaction();

        /* 参与依次：发送者公钥, 不重数，交易数量，资产类型，交易生产时间（UTC-0时间），过期epoch */
        transaction(
                uint256& _sender,
                const uint64_t& nonce,
                const uint64_t& _count,
                const uint32_t& _type,
                const uint64_t& _create_time,
                const uint8_t &_epoch = 255,
                transaction_operator t_op = transaction_operator::Append,
                const transaction_version& _tv = transaction_version::V0001
        );

        /* 参与依次：发送者公钥, 不重数，创建资产类型，交易生产时间（UTC-0时间），过期epoch */
        transaction(
                uint256& _sender,
                const uint64_t& nonce,
                const uint32_t& _type,
                const uint64_t& _create_time,
                const uint8_t &_epoch = 255,
                transaction_operator t_op = transaction_operator::Establish,
                const transaction_version& _tv = transaction_version::V0001
        );

        transaction(const transaction&other);

        transaction(transaction&&other) noexcept;

        auto operator=(const transaction&other) -> transaction&;

        auto operator=(transaction&&other) noexcept -> transaction&;

        void push_output(uint256& _reciver, const uint64_t& _count);

        //设置交易类型
        void set_transaction_version(transaction_version _type);
        //获得交易类型
        transaction_version get_transaction_version() ;
        //设置资产类型 asset_type >= 1
        void set_asset_type(const uint32_t& _type);
        //获得交易类型
        uint32_t get_asset_type() ;
        //设置操作
        void set_transaction_operator(const transaction_operator& _op);
        //获得操作类型
        transaction_operator get_transaction_operator() ;
        //设置数量
        bool set_count(const uint64_t& _count);
        //获得数量
        uint64_t get_count() ;
        //设置交易产生时间,毫秒单位
        bool set_create_time(const uint64_t& _create_time);
        //获得交易产生时间
        uint64_t get_create_time() ;
        //设置过期时间 五分钟一个单位
        bool set_expired_epoch(const uint8_t& _epoch);
        //获得过期时间
        uint8_t get_expired_epoch() ;
        //设置源账户 公钥
        void set_sender(uint256& _sender);
        //获得源账户
        uint256 get_sender() ;
        //设置不重数
        void set_nonce(const uint64_t& nonce);
        //获取不重数
        uint64_t get_nonce()  ;
        //接口
        MUSE_IBinarySerializable(data, outs, signature);
        //可以设置64个字节的备注信息
        //bool set_64_bytes_comment(const uint8_t * _store, const uint16_t& count);
        //获得备注信息
        //const uint8_t* get_64_bytes_comment();
        void set_signature(const std::string& _signature);

        auto get_signature() const -> const std::string &;
    public:
        uint256 get_hash();
    };
}
#endif //MUSE_CHAIN_TRANSACTION_HPP
