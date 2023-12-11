//
// Created by remix on 23-12-10.
//
#ifndef MUSE_CHAIN_ACCOUNT_HPP
#define MUSE_CHAIN_ACCOUNT_HPP

#include "uint256.hpp"
#include "serializer/IbinarySerializable.h"

namespace muse::chain{
    class account: muse::serializer::IBinarySerializable {
    private:
        uint256 address;     //账户地址
        uint64_t nonce;      //不重数
        uint64_t balance;    //余额
    public:
        account();
        account(const uint256 &_address, const uint64_t &_nonce, const uint64_t &_balance)
        :address(_address), nonce(_nonce), balance(_balance) {

        };

        MUSE_IBinarySerializable(address, nonce, balance);

        [[nodiscard]] const uint256 &get_address() const {
            return address;
        }

        [[nodiscard]] uint64_t get_nonce() const {
            return nonce;
        }

        [[nodiscard]] uint64_t get_balance() const {
            return balance;
        }

        void set_nonce(const uint64_t &_nonce) {
            nonce = _nonce;
        }
        void set_balance(const uint64_t &_balance) {
            balance = _balance;
        }
        void set_address(const uint256 &_address) {
            address = _address;
        }

    };
}


#endif //MUSE_CHAIN_ACCOUNT_HPP
