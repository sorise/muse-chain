//
// Created by 14270 on 2024-01-04.
//

#ifndef GLORIA_NODE_TABLE_HPP
#define GLORIA_NODE_TABLE_HPP

#include <exception>
#include <array>
#include <stdexcept>

#include "mpt_node_cache.hpp"

namespace muse::chain{
    template<typename Element_Type, size_t LENGTH>
    struct node_table{
    private:
        size_t count {0};
        //必须返回引用
        std::array<mpt_node_cache<Element_Type>, LENGTH> table;
    public:
        auto operator[](const size_t & idx) -> const mpt_node_cache<Element_Type>& {
            if (idx >= LENGTH) {
                throw std::out_of_range("index out of range!");
            }
            return table[idx];
        };

        auto find(const size_t & idx) -> mpt_node_cache<Element_Type>&{
            if (idx >= LENGTH) {
                throw std::out_of_range("index out of range!");
            }
            return table[idx];
        }

        auto insert(const size_t & idx, Element_Type *node) -> bool{
            if (idx >= LENGTH) {
                throw std::out_of_range("index out of range!");
            }
            if (table[idx].exist){
                return false;
            }
            table[idx].exist = true;
            table[idx].node = node;
            count++;
            return true;
        }
        /* 删除节点，同时也需要释放内存 */
        auto erase(const size_t & idx) -> bool {
            if (idx >= LENGTH) {
                throw std::out_of_range("index out of range!");
            }
            if (table[idx].exist){
                delete_by_pool<Element_Type>(table[idx].node) ;
                //释放内存
                table[idx].exist = false;
                table[idx].node = nullptr;
                count--;
                return true;
            }
            return false;
        };

        /* 修改指针信息,返回原来的指针，如果原来不存在，返回nullptr*/
        auto update(const size_t & idx, Element_Type *node) -> Element_Type *{
            if (idx >= LENGTH) {
                throw std::out_of_range("index out of range!");
            }
            if (table[idx].exist && node != nullptr ){
                Element_Type* prev = table[idx].node;
                table[idx].node = node;
                return prev;
            }
            return nullptr;
        };

        auto get_exist(const size_t & idx) -> bool{
            if (idx >= LENGTH) {
                throw std::out_of_range("index out of range!");
            }
            return table[idx].exist;
        }

        auto first() -> int{
            for (int i = 0; i < LENGTH; ++i) {
                if (table[i].exist){
                    return i;
                }
            }
            return -1;
        };

        auto last() -> int {
            for (int i = LENGTH - 1; i >= 0; --i) {
                if (table[i].exist){
                    return i;
                }
            }
            return -1;
        };

        auto size() ->size_t{
            return count;
        };

        auto empty() -> bool{
            return count <= 0;
        };

        node_table() = default;

        node_table(const node_table& other)
        :table(other.table), count(other.count){

        }

        node_table& operator=(const node_table& other){
            if (this!= &other){
                table = other.table;
                count = other.count;
            }
            return *this;
        }

        ~node_table() = default;
    };

}

#endif //GLORIA_NODE_TABLE_HPP
