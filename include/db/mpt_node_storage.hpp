//
// Created by 14270 on 2024-01-04.
//

#ifndef GLORIA_MPT_NODE_STORAGE_HPP
#define GLORIA_MPT_NODE_STORAGE_HPP

#include <string>
#include "serializer/IbinarySerializable.h"
#include "uint256.hpp"

namespace muse::chain{

    /* 中间节点缓存 */
    template<size_t LENGTH>
    struct mpt_node_storage: public muse::serializer::IBinarySerializable{
        static constexpr int UPDATE_OP_MUSE = 0;
        static constexpr int DELETE_OP_MUSE = -1;
        static constexpr int INSERT_OP_MUSE = 1;

        std::string key {}; /* 键 */

        /*
         * 值等于 0 表示修改内容
         * 值  > 0  表示新插入节点
         * 值  < 0 表示需要删除节点
         * */
        int operator_type {0};

        std::string sub_key {}; /* 第一位就是所处的父节点所在数组的下标 */

        uint256 value_hash; /* hash 值 */

        std::vector<std::string> values {}; /* 子节点 */

        MUSE_IBinarySerializable(key, sub_key, value_hash, values);

        mpt_node_storage() = default;

        mpt_node_storage(const mpt_node_storage& other)
        :key(other.key),operator_type(other.operator_type),sub_key(other.sub_key),value_hash(other.value_hash),values(other.values){

        }

        //移动构造函数
        mpt_node_storage(mpt_node_storage&& other) noexcept
        :key(std::move(other.key)),
        operator_type(other.operator_type),
        sub_key(std::move(other.sub_key)),
        value_hash(std::move(other.value_hash)),
        values(std::move(other.values)){

        }

        //移动赋值操作符
        auto operator=(mpt_node_storage&& other) noexcept -> mpt_node_storage& {
            key = std::move(other.key);
            operator_type = other.operator_type;
            sub_key = std::move(other.sub_key);
            value_hash = std::move(other.value_hash);
            values = std::move(other.values);
            return *this;
        }

        //拷贝赋值操作符
        auto operator=(const mpt_node_storage& other) -> mpt_node_storage& {
            if (other != *this){
                key = other.key;
                operator_type = other.operator_type;
                sub_key = other.sub_key;
                value_hash = other.value_hash;
                values = other.values;
            }
            return *this;
        }

        void flush(){
            if (operator_type ==0){
                std::cout << "update ";
            }else if (operator_type < 0){
                std::cout << "delete ";
            }else{
                std::cout << "insert ";
            }
            std::cout << "key: " << key;
            std::cout <<  "------ mpt_node_storage" <<"\n";
        }

        virtual ~mpt_node_storage(){
//            std::cout << "---------------------------------------------------\n";
//            if (operator_type ==0){
//                std::cout << "update\n";
//            }else if (operator_type < 0){
//                std::cout << "delete\n";
//            }else{
//                std::cout << "insert\n";
//            }
//            std::cout << "key: " << key << "\n";
//            std::cout << "sub_key: " ;
//            for (auto& v: sub_key) {
//                std::cout << (int)v;
//            }
//            std::cout <<  "------ mpt_node_storage" <<"\n";
        }
    };
}
#endif //GLORIA_MPT_NODE_STORAGE_HPP
