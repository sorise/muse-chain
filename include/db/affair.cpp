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
        encryption::get_hash_base()->get_hash(&create_time, sizeof(create_time), temp.get_data());
        result = temp + uint256(epoch);
        base->get_hash(this->data.data(), this->data.size() * sizeof(Data_DataType), temp.get_data());
        result = temp + owner;
        base->get_hash(&this->nonce, sizeof(this->nonce), temp.get_data());
        result = result + temp;
        uint256 val(_affair_type);
        temp = result + val;
        base->get_hash(temp.get_data(), uint256::Width, result.get_data());
        return result;
    }

    const std::vector<affair::Data_DataType> &affair::get_data() const{
        return this->data;
    }

    void affair::set_data(const void *start, size_t len) {
        data = std::vector<Data_DataType>();
        data.reserve(len);
        data.resize(len);
        std::memcpy(data.data(), start, len);

    }

    void affair::set_signature(const std::string &_signature) {
        this->signature = _signature;
    }

    auto affair::get_signature() const -> const std::string & {
        return this->signature;
    }

    void affair::set_signature(std::string &&_signature) {
        this->signature = std::move(_signature);
    }

    affair::affair(affair &&other) noexcept
    :nonce(other.nonce),
    owner(std::move(other.owner)),
    _affair_type(other._affair_type),
    data(std::move(other.data)),
    signature(std::move(other.signature)),
    epoch(other.epoch),
    create_time(other.create_time){

    }


    auto affair::operator=(const affair &other) -> affair & {
        if (this != &other) {
            this->nonce = other.nonce;
            this->owner = other.owner;
            this->_affair_type = other._affair_type;
            this->data = other.data;
            this->signature = other.signature;
            this->create_time = other.create_time;
            this->epoch = other.epoch;
        }
        return *this;
    }


    auto affair::operator=(affair &&other) noexcept -> affair & {
        if (this != &other) {
            this->nonce = other.nonce;
            this->owner = std::move(other.owner);
            this->_affair_type = other._affair_type;
            this->data = std::move(other.data);
            this->signature = std::move(other.signature);
            this->create_time = other.create_time;
            this->epoch = other.epoch;
        }
        return *this;
    }

    auto affair::is_db_level_affair_type() const -> bool {
        switch (this->_affair_type) {
            case (Affair_DataType)affair_type::OwnerAddressHash:
                return false;
            case (Affair_DataType)affair_type::PureData:
                return false;
            default:
                return true;
        }
    }

    void affair::set_create_time(const uint64_t& create_time){
        this->create_time = create_time;
    }

    void affair::set_epoch(const uint8_t& epoch){
        this->epoch = epoch;
    }


    affair::affair(const uint64_t &nonce, const uint256 &owner, affair_type _type, const uint64_t &create_time,
                   const uint8_t &epoch, std::vector<Data_DataType> _data)
    :nonce(nonce),
    owner(owner),
    _affair_type((Affair_DataType)_type),
    data(std::move(_data)),
    create_time(create_time),
    epoch(epoch){

    }

    auto affair::get_create_time() const -> const uint64_t & {
        return this->create_time;
    }

    auto affair::get_epoch() const -> const uint8_t & {
        return this->epoch;
    }
}