//
// Created by 14270 on 2023-12-14.
//

#ifndef MAKER_TRIE_HPP
#define MAKER_TRIE_HPP
#include <iostream>
#include <string>
#include <array>
#include <stack>
#include <queue>
#include <functional>
#include <exception>
#include <cstdint>
#include <unordered_map>

namespace muse::chain{
    struct TrieNode{
        std::unordered_map<unsigned char, TrieNode*> children;
        bool is_leaf{true};
        std::string value;
        uint32_t count{0}; //字符串插入了多少次数

        TrieNode() = default;
        TrieNode(const TrieNode& other) = default;
        TrieNode(TrieNode &&other) noexcept
        :children(std::move(other.children)), is_leaf(other.is_leaf), value(std::move(other.value)), count(other.count)
        {

        }

        auto operator=(const TrieNode &other) -> TrieNode & {
            if (this!= &other) {
                children = other.children;
                is_leaf = other.is_leaf;
                value = other.value;
                count = other.count;
            }
            return *this;
        };

        auto operator=(TrieNode &&other)  noexcept -> TrieNode & {
            if (this!= &other) {
                children = std::move(other.children);
                is_leaf = other.is_leaf;
                value = std::move(other.value);
                count = other.count;
            }
            return *this;
        };

        ~TrieNode() = default;
    };

    /* 字典树 */
    class Trie{
    private:
        TrieNode *root; //根节点

        auto trie_free(TrieNode * node)-> void {
            if (node != nullptr){
                for(auto &child : node->children){
                    trie_free(child.second);
                }
                delete node;
            }
        }

        auto delete_helper(const std::string& word, bool true_remove_false_erase) -> bool{
            if (root == nullptr) {
                throw std::runtime_error("The move constructor or operator has already been called");
            }
            TrieNode *head = root;
            std::stack<TrieNode*> stk;
            for(auto key : word){
                if(head->children.find(key) == head->children.end()){
                    return false;
                }
                //压栈，用于返回的时候找到父节点，避免递归调用
                stk.push(head);
                head = head->children[key];
            }
            // 没有找到
            if (head->count == 0){
                return false;
            }
            head->count--;
            if (true_remove_false_erase){
                //没必要删除
                if (head->count > 0) {
                    return true;
                };
            }
            if (!head->is_leaf) {
                // 情况：字典树中存储有：abc、ab
                // 此时删除 ab, 由于 abc 的存在不可以删除ab路径
                return true;
            };
            auto last = word.rbegin();
            //已经是叶子节点了，需要删除节点
            while (!stk.empty()){
                TrieNode* parent = stk.top();
                stk.pop();
                parent->children.erase(*last);
                last++;

                //判断是否需要继续向上删除
                if (!parent->children.empty() || parent->count > 0){
                    parent->is_leaf = true;
                    break;
                    //不能再删除了
                }
            }
            return true;
        }

        auto size_helper(bool true_all_false_no_repeat) -> uint32_t{
            if (root == nullptr) {
                throw std::runtime_error("The move constructor or operator has already been called");
            }
            uint32_t key_count = 0;
            //层次遍历
            std::queue<TrieNode*> layer;
            layer.push(root);

            while (!layer.empty()){
                auto len = layer.size();
                for (int i = 0; i < len; i++) {
                    TrieNode *node = layer.front();
                    layer.pop();
                    if(node->count > 0) {
                        if (true_all_false_no_repeat){
                            key_count += node->count;
                        }else{
                            key_count++;
                        }
                    }
                    for (auto &child : node->children) {
                        layer.push(child.second);
                    }
                }
            }
            return key_count;
        }
    public:

        Trie():root(new TrieNode()){

        };
        Trie(Trie &&other) noexcept
        :root(other.root)
        {
            other.root = nullptr;
        }

        auto insert(const std::string &word)->void {
            if (root == nullptr) {
                throw std::runtime_error("The move constructor or operator has already been called");
            }
            TrieNode *ptr = root;
            for(auto key : word){
                //没有找到下一层对应节点，则直接建立这个节点
                if(ptr->children.find(key) == ptr->children.end()){
                    //有了下一层节点就不是叶节点了
                    ptr->is_leaf = false;
                    ptr->children[key] = new TrieNode {};
                    ptr->children[key]->is_leaf = false;
                }
                //去到下一层
                ptr = ptr->children[key];
            }
            if (ptr->value.empty()) {
                ptr->value = word;
            }
            //有子节点，说明非叶子节点
            if (ptr->children.empty()){
                ptr->is_leaf = true;
            }
            ptr->count++;
        };

        auto search(const std::string &word) -> bool{
            if (root == nullptr) {
                throw std::runtime_error("The move constructor or operator has already been called");
            }
            TrieNode *ptr = root;
            for(auto key : word){
                if(ptr->children.find(key) == ptr->children.end()){
                    return false;
                }
                ptr = ptr->children[key];
            }
            return ptr->count > 0;
        }

        //删除某个键,如果某个键插入了多次，只减少插入次数
        auto remove(const std::string& word) -> bool{
            return delete_helper(word, true);
        }

        //删除某个键，无论出现了多少次，统统删除
        auto erase(const std::string& word) -> bool{
            return delete_helper(word, false);
        }

        //返回当前树中键的数量（不包括重复key）
        auto size() -> uint32_t{
            return size_helper(false);
        }
        //返回当前树中键的数量（不包括重复key）
        auto all_size() -> uint32_t{
            return size_helper(true);
        }

        auto clear() -> void  {
            trie_free(root);
            root = new TrieNode();
        }

        auto get_string_count(const std::string &word) -> uint32_t{
            if (root == nullptr) {
                root = new TrieNode();
            }
            TrieNode *ptr = root;
            for(auto key : word){
                if(ptr->children.find(key) == ptr->children.end()){
                    return 0;
                }
                ptr = ptr->children[key];
            }
            return ptr->count;
        }

        ~Trie(){
            //释放树中的各个节点
            trie_free(root);
            root = nullptr;
        }
    };
}
#endif //MAKER_TRIE_HPP
