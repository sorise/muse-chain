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
        GenesisData = 0,//创世数据
        PureData = 1, //存数据
        MaliciousMessages = 2,
        MaliciousTransactions = 3,
        MaliciousAffair = 4,
        MaliciousBlock = 5,
        OwnerAddressHash = 6, //创始人地址哈希
        ParticipatorAddressHash = 7, //参与者地址哈希

    };

    class affair: public muse::serializer::IBinarySerializable{
    public:
        using Data_DataType = char;
    private:
        //不重数
        uint64_t nonce;
        //数据拥有者
        uint256  owner;
        //事务类型
        Affair_DataType _affair_type;
        //数据
        std::vector<Data_DataType> data;
        //签名
        std::string signature;
        //创建时间
        uint64_t create_time;
        //过期时间
        uint8_t epoch;
    public:
        affair();

        affair(const affair& other) = default;
        /* nonce: 不重数
         * owner: 数据拥有者
         * _type：事务类型
         * _data：数据
         * */
        affair(const uint64_t& nonce, const uint256& owner, affair_type _type, std::vector<Data_DataType> _data);

        affair( const uint64_t& nonce,
                const uint256& owner,
                affair_type _type,
                const uint64_t& create_time,
                const uint8_t& epoch,
                std::vector<Data_DataType> _data);

        affair(affair &&other) noexcept ;

        auto operator=(const affair& other) -> affair&;

        auto operator=(affair&& other) noexcept -> affair&;

        MUSE_IBinarySerializable(nonce, owner,_affair_type,signature,create_time,epoch,data);

        void set_nonce(const uint64_t& nonce);

        void set_data(const void *start, size_t len);

        void set_owner(const uint256& owner);

        void set_type(const affair_type& type);

        void set_create_time(const uint64_t& create_time);

        void set_epoch(const uint8_t& epoch);

        [[nodiscard]] auto get_create_time() const -> const uint64_t&;

         [[nodiscard]] auto get_epoch() const -> const uint8_t&;

        [[nodiscard]] auto get_nonce() const -> const uint64_t&;

        [[nodiscard]] auto get_owner() const -> const uint256&;

        [[nodiscard]] affair_type get_type() const;

        uint256 get_hash();

        auto get_data() const -> const std::vector<Data_DataType>&;

        //析构函数，没啥特点，默认即可
        virtual ~affair() = default;

        void set_signature(const std::string& _signature);

        void set_signature(std::string &&_signature);

        auto get_signature() const -> const std::string &;

        auto is_db_level_affair_type() const -> bool;
    };
}


#endif //MUSE_CHAIN_AFFAIR_HPP
