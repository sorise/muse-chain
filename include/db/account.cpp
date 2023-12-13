//
// Created by remix on 23-12-10.
//

#include "account.hpp"

namespace muse::chain{

    account::account():address(0), nonce(0){

    }

    auto account::get_hash() -> uint256 {
        auto base = encryption::get_hash_base();
        uint256 temp;
        uint256 result;
        uint256 temp_type;
        uint256 temp_value;
        for (auto key_value: balances) {
            base->get_hash(&key_value.second,sizeof(key_value.second), temp_value.get_data());
            base->get_hash(&key_value.second,sizeof(key_value.first), temp_type.get_data());
            temp = temp + temp_type + temp_value;
        }
        result = address + temp;
        base->get_hash(&this->nonce,sizeof(this->nonce), temp.get_data());
        result = result + temp;
        return result.get_sha3_256();
    }

    void account::set_address(const uint256 &_address){
        address = _address;
    }

    void account::reduce_balance(const uint32_t &_asset_type, const uint64_t &count)  {
        if (balances.find(_asset_type) != balances.end()){
            //判断减少是否合法
            uint64_t prev = balances[_asset_type];
            assert(prev - count >= 0);
            if (prev < count){
                throw std::runtime_error("reduce balance error");
            }
            if (prev == count){
                balances.erase(_asset_type); //如果余额为0，删除资产
            } else{
                balances[_asset_type] -= count;
            }
        }else{
            throw std::runtime_error("reduce balance error");
        }
    }

    void account::add_balance(const uint32_t &_asset_type, const uint64_t &count) {
        if (balances.find(_asset_type) != balances.end()){
            balances[_asset_type] += count;
        }else{
            balances.insert(std::make_pair(_asset_type,count));
        }
    }

    void account::set_nonce(const uint64_t &_nonce) {
        nonce = _nonce;
    }

    auto account::get_balance(uint32_t _asset_type) const noexcept -> uint64_t {
        if (balances.find(_asset_type) != balances.end()){
            return balances.at(_asset_type);
        }
        return 0;
    }

    auto account::get_nonce() const noexcept -> uint64_t  {
        return nonce;
    }

    auto account::get_address() const noexcept -> const uint256 & {
        return address;
    }

    account::account(const uint256 &address_, const uint64_t &_nonce)
    :address(address_), nonce(_nonce){

    }

    account::account(const account &other):address(other.address), nonce(other.nonce) {
        balances.insert(other.balances.begin(), other.balances.end());
    }

    auto account::delete_asset(const uint32_t &_asset_type) -> void {
        balances.erase(_asset_type);
    }

    auto account::set_balance(const uint32_t &_asset_type, const uint64_t &count) -> void {
        if(balances.find(_asset_type) != balances.end()){
            if (count > 0){
                balances[_asset_type] = count;
            }else{
                balances.erase(_asset_type);
            }
        }else{
            if (count > 0){
                balances.insert(std::make_pair(_asset_type,count));
            }else{
                balances.erase(_asset_type);
            }
        }
    };
}