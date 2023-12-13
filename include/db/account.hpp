//
// Created by remix on 23-12-10.
//
#ifndef MUSE_CHAIN_ACCOUNT_HPP
#define MUSE_CHAIN_ACCOUNT_HPP

#include <map>
#include "serializer/IbinarySerializable.h"
#include "IGetHashSha3256.hpp"
#include "../encryption/encryption.hpp"
#include "uint256.hpp"

namespace muse::chain{
    class account: public muse::serializer::IBinarySerializable, public muse::chain::IGetHashSha3256{
    private:
        uint256  address;     //账户地址
        uint64_t nonce;      //不重数
        std::map<uint32_t, uint64_t> balances; //资产余额必须大于0
    public:
        account();

        account(const uint256 &_address, const uint64_t &_nonce);

        account(const account &other);

        virtual ~account() = default;

        MUSE_IBinarySerializable(address, nonce, balances);

        [[nodiscard]] auto get_address()  const noexcept -> const uint256 &;

        [[nodiscard]] auto get_nonce() const noexcept -> uint64_t;

        [[nodiscard]] auto get_balance(uint32_t _asset_type) const noexcept -> uint64_t;

        auto set_nonce(const uint64_t &_nonce) -> void;

        auto delete_asset(const uint32_t& _asset_type) -> void;

        auto add_balance(const uint32_t& _asset_type, const uint64_t &count) -> void;

        auto set_balance(const uint32_t& _asset_type, const uint64_t &count) -> void;

        auto reduce_balance(const uint32_t& _asset_type, const uint64_t &count) -> void;

        auto set_address(const uint256 &_address) -> void ;

        [[nodiscard]] auto get_hash() -> uint256 override;
    };
}


#endif //MUSE_CHAIN_ACCOUNT_HPP
