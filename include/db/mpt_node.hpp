//
// Created by 14270 on 2023-12-23.
//

#ifndef GLORIA_MPT_NODE_HPP
#define GLORIA_MPT_NODE_HPP

#include <iostream>
#include <list>
#include <utility>
#include <vector>
#include <string>
#include "serializer/IbinarySerializable.h"
#include "uint256.hpp"
#include "node_table.hpp"
#include "mpt_node_storage.hpp"

namespace muse::chain{
    // V 是值类型
    template<typename Value_Type, size_t LENGTH>
    struct mpt_node{
    private:
        //孩子节点
        node_table<mpt_node<Value_Type,LENGTH>, LENGTH> *children {nullptr};
        //是否是叶子节点
        bool leaf;
        //内容是否已经修改,新创建的内容肯定是修改了
        bool modified {true};
    public:
        // hash 值
        uint256 value_hash{0};
        // key
        std::string key;
        // 部分key,共同前缀
        std::string sub_key;
        // 真正的值存储在这个位置
        Value_Type* value {nullptr};
        // 值是否加载到内存中
        bool load {true};

        auto get_modified() -> bool {
            return modified;
        };

        auto set_modified(bool _modified) -> void {
            modified = _modified;
        };

        auto get_leaf() -> bool {
            return leaf;
        };

        auto get_children() ->node_table<mpt_node<Value_Type,LENGTH>, LENGTH>*{
            return children;
        }

        auto to_mpt_node_storage(const int& op_type) -> mpt_node_storage<LENGTH>{
            mpt_node_storage<LENGTH> storage;
            storage.key = this->key;
            storage.operator_type = op_type;
            storage.sub_key = this->sub_key;
            storage.value_hash = this->value_hash;
            if (!leaf){
                for (int i = 0; i < LENGTH; ++i) {
                    if (children->get_exist(i)){
                        mpt_node<Value_Type,LENGTH> *node =  children->find(i).node;
                        if ( node == nullptr){
                            storage.values.emplace_back(children->find(i).key);
                        }else{
                            storage.values.emplace_back(node->key);
                        }

                    }
                }
            };
            return storage;
        };

        explicit mpt_node(bool _leaf): leaf(_leaf){

        }

        ~mpt_node() {
            if (value != nullptr){
                delete_by_pool<Value_Type>(value);
            }
        }

        static auto get_index_by_16(const char& index) -> unsigned int{
            unsigned int idx = 0;
            switch (index) {
                case '0':
                    idx = 0;
                    break;
                case '1':
                    idx = 1;
                    break;
                case '2':
                    idx = 2;
                    break;
                case '3':
                    idx = 3;
                    break;
                case '4':
                    idx = 4;
                    break;
                case '5':
                    idx = 5;
                    break;
                case '6':
                    idx = 6;
                    break;
                case '7':
                    idx = 7;
                    break;
                case '8':
                    idx = 8;
                    break;
                case '9':
                    idx = 9;
                    break;
                case 'A':
                case 'a':
                    idx = 10;
                    break;
                case 'B':
                case 'b':
                    idx = 11;
                    break;
                case 'C':
                case 'c':
                    idx = 12;
                    break;
                case 'D':
                case 'd':
                    idx = 13;
                    break;
                case 'E':
                case 'e':
                    idx = 14;
                    break;
                case 'F':
                case 'f':
                    idx = 15;
                    break;
                default:
                    break;
            }
            return idx;
        }

        //从 mpt_node_storage 创建 mpt_node
        [[nodiscard]] static auto mns_to_mnp(const mpt_node_storage<LENGTH>& storage, bool leaf = false) -> mpt_node<Value_Type, LENGTH>*{
            mpt_node<Value_Type, LENGTH>* node = nullptr;
            if (!leaf){
                node = mpt_node<Value_Type, LENGTH>::create_extension_node(false);
            }else{
                node = mpt_node<Value_Type, LENGTH>::create_leaf_node(false);
            }
            node->key = storage.key;
            node->value_hash = storage.value_hash;
            node->sub_key = storage.sub_key;
            if (!leaf){
                auto idx = node->key.size() % (uint256::Width * 2); //这个操作很关键，取余64位，因为key最长是64
                for(auto& son_key: storage.values){
                    auto son_idx = get_index_by_16(son_key.at(idx));
                    //需要转换为16进制
                    node->get_children()->insert(son_idx, nullptr);
                    node->get_children()->find(son_idx).key = son_key;
                }
            }
            return node;
        }

        [[nodiscard]] static auto create_extension_node(const bool& modified = true) -> mpt_node<Value_Type, LENGTH>*{
            auto* node = new_by_pool<mpt_node<Value_Type, LENGTH>>(false);
            node->children = new_by_pool<node_table<mpt_node<Value_Type,LENGTH>, LENGTH>>();
            node->set_modified(modified);
            return node;
        };

        [[nodiscard]] static auto create_leaf_node(const bool& modified  = true) -> mpt_node<Value_Type, LENGTH>*{
            auto* node = new_by_pool<mpt_node<Value_Type, LENGTH>>(true);;
            node->children = nullptr;
            node->set_modified(modified);
            return node;
        };
    };
}

#endif //GLORIA_MPT_NODE_HPP
