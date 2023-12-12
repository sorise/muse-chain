//
// Created by remix on 23-12-10.
//

#include "affair.hpp"
namespace muse::chain{
    void affair::set_nonce(const uint64_t &nonce) {
        this->nonce = nonce;
    }

    void affair::set_owner(const uint256 &owner) {
        this->owner = owner;
    }

    void affair::set_type(const affair_type &type) {
        this->_affair_type = (Affair_DataType)type;
    }

    const uint64_t& affair::get_nonce() const {
        return this->nonce;
    }

    const uint256& affair::get_owner() const {
        return this->owner;
    }

    affair_type affair::get_type() const {
        if (this->_affair_type == (Affair_DataType)affair_type::PureData){
            return affair_type::PureData;
        }else if (this->_affair_type == (Affair_DataType)affair_type::MaliciousMessages){
            return affair_type::MaliciousMessages;
        }
        return affair_type::PureData;
    }

    affair::affair(const uint64_t& nonce, const uint256& owner, affair_type _type, std::vector<Data_DataType> _data)
            :nonce(nonce),owner(owner),_affair_type((Affair_DataType)_type),data(std::move(_data)){
        if(data.empty()){
            throw std::invalid_argument("data is empty");
        }
    };

    affair::affair() :nonce(0),owner(0),_affair_type((Affair_DataType)affair_type::PureData),data(){

    }

    uint256 affair::get_hash() {
        auto base = encryption::get_hash_base();
        uint256 temp, result;
        base->get_hash(reinterpret_cast<const char*>(this->data.data()), this->data.size() * sizeof(Data_DataType), temp.get_data());
        result = temp + owner;
        base->get_hash(reinterpret_cast<const char*>(&this->nonce), sizeof(this->nonce), temp.get_data());
        result = result + temp;
        uint256 val(_affair_type);
        temp = result + val;
        base->get_hash(reinterpret_cast<const char*>(temp.get_data()), uint256::Width, result.get_data());
        return result;
    }

    std::vector<affair::Data_DataType> &affair::get_data() {
        return this->data;
    }
}