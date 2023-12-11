//
// Created by remix on 23-9-23.
//
#include "transaction.hpp"
namespace muse::chain{
    transaction::transaction(): data() {
        data.fill(0);
    }

    uint256 transaction::get_hash() {
        auto hash = encryption::get_hash_base();
        uint256 result;
        //基础数据求hash
        hash->get_hash((const char *)data.data(), Bytes, result.get_data());
        //加上发送者
        result = result + get_sender();
        //out 数值hash
        uint256 temp;
        for(auto & out : outs) {
            result = result + out.receiver;
            hash->get_hash((const char *)&out.count, sizeof(typeof(out.count)), temp.get_data());
            result = temp + result;
        }
        return result;
    }

    void transaction::set_transaction_version(transaction_version _type) {
        data[0] = (uint8_t)_type;
    }

    transaction_version transaction::get_transaction_version() {
        switch (data[0]) {
            case (uint8_t)transaction_version::V0001:
                return transaction_version::V0001;
            default:
                return transaction_version::Ban;
        }
    }

    void transaction::set_asset_type(const uint32_t &_type) {
        auto _asset_type = reinterpret_cast<uint32_t*>(&data[1]);
        *_asset_type = _type;
    }

    uint32_t transaction::get_asset_type() {
        return *(reinterpret_cast<uint32_t*>(&data[1]));
    }

    void transaction::set_transaction_operator(transaction_operator _op) {
        data[5] = (uint8_t)_op;
    }

    transaction_operator transaction::get_transaction_operator() {
        switch (data[5]) {
            case (uint8_t)transaction_operator::Append :
                return transaction_operator::Append;
            case (uint8_t)transaction_operator::Establish :
                return transaction_operator::Establish;
            default:
                return transaction_operator::NullOP;
        }
    }

    bool transaction::set_count(const uint64_t &_count) {
        auto _count_ptr = reinterpret_cast<uint64_t*>(&data[6]);
        *_count_ptr = _count;
        return true;
    }

    uint64_t transaction::get_count() {
        return *(reinterpret_cast<uint64_t*>(&data[6]));
    }

    bool transaction::set_create_time(const uint64_t &_create_time) {
        auto _create_time_ptr = reinterpret_cast<uint64_t*>(&data[14]);
        *_create_time_ptr = _create_time;
        return true;
    }

    uint64_t transaction::get_create_time() {
        return *reinterpret_cast<uint64_t*>(&data[14]);
    }

    bool transaction::set_expired_epoch(const uint8_t &_epoch) {
        *reinterpret_cast<uint8_t*>(&data[22]) = _epoch;
        return true;
    }

    uint8_t transaction::get_expired_epoch() {
        return *reinterpret_cast<uint8_t*>(&data[22]);
    }

    void transaction::set_sender(uint256 &_sender) {
        std::memcpy(&data[23], _sender.get_data(), uint256::Width);
    }

    uint256 transaction::get_sender() {
        uint256 sender;
        std::memcpy(sender.get_data(), &data[23], uint256::Width);
        return sender;
    }

//    bool transaction::set_64_bytes_comment(const uint8_t *_store,  const uint16_t & count) {
//        if (count <= 64){
//            std::memcpy(&data[64], _store, count);
//            return true;
//        }
//        return false;
//    }
//
//    const uint8_t *transaction::get_64_bytes_comment() {
//        return &data[64];
//    }

    void transaction::set_nonce(uint64_t nonce) {
        auto _nonce_ptr = reinterpret_cast<uint64_t*>(&data[55]);
        *_nonce_ptr = nonce;
    }

    uint64_t transaction::get_nonce() {
        return *(reinterpret_cast<uint64_t*>(&data[55]));
    }
}