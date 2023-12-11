//
// Created by remix on 23-12-10.
// 区块链中 用于存储纯粹的数据、或者用于检举恶意的攻击者。
//
#ifndef MUSE_CHAIN_AFFAIR_HPP
#define MUSE_CHAIN_AFFAIR_HPP
#include <utility>

#include "serializer/IbinarySerializable.h"
#include "uint256.hpp"
#include "../encryption/encryption.hpp"

namespace muse::chain{
    using Affair_DataType = uint8_t;

    /* 标识事务的类型，可以是纯粹的数据流，也可以是恶意攻击的证据 */
    enum class affair_type: Affair_DataType{
        PureData = 0,
        MaliciousMessages = 1,
    };

    class affair: public muse::serializer::IBinarySerializable{
    public:
        using Data_DataType = char; //
    private:
        //不重数
        uint64_t nonce;
        //数据拥有者
        uint256  owner;
        //事务类型
        Affair_DataType _affair_type;
        //数据
        std::vector<Data_DataType> data;
    public:
        affair();
        affair(const affair& other) = default;

        /* nonce: 不重数
         * owner: 数据拥有者
         * _type：事务类型
         * _data：数据
         * */
        affair(const uint64_t& nonce, const uint256& owner, affair_type _type, std::vector<Data_DataType> _data);

        MUSE_IBinarySerializable(nonce, owner,_affair_type, data);

        void set_nonce(const uint64_t& nonce);
        void set_owner(const uint256& owner);
        void set_type(const affair_type& type);

        [[nodiscard]] const uint64_t& get_nonce() const;
        [[nodiscard]] const uint256& get_owner() const;
        [[nodiscard]] affair_type get_type() const;

        uint256 get_hash();

        std::vector<Data_DataType>& get_data();

        //析构函数，没啥特点，默认即可
        ~affair() = default;
    };
}


#endif //MUSE_CHAIN_AFFAIR_HPP
