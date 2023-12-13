// Created by 14270 on 2023-12-19.

#ifndef MAKER_MERKLE_PATRICIA_TRIE_HPP
#define MAKER_MERKLE_PATRICIA_TRIE_HPP

#include <stack>
#include "IGetHashSha3256.hpp"
#include "account.hpp"
#include "mpt_node.hpp"
#include "uint256.hpp"
#include "chain_db_exception.hpp"
#include "leveldb/db.h"
#include "serializer/binaryDeserializeView.hpp"
#include "spdlog/spdlog.h"

namespace muse::chain{
    /*
     * MPT（Merkle Patricia Trie）树，即默克尔前缀树，是默克尔树和前缀树的结合。
     * 节点采用节点缓存机制
     * key的长度在0~16之间
     *
     * 计算hash值，需要mpt_node 的 modified 字段配合
     * 记录扩展节点的变化，需要在insert方法里面记录变化了的中间节点。
     * 缓加载account，需要mpt_node_cache配置，还需要区分是什么类型的节点
     * 中间节点全部一下子加载进入到内存中。
     */
    template<typename Value_Type>
    class [[maybe_unused]] merkle_patricia_trie: public IGetHashSha3256{
        static_assert(std::is_base_of_v<IGetHashSha3256, Value_Type>);
    public:
        static const size_t CHILDREN_COUNT = 16; // 子节点数量、只能是16

        static const size_t EMPTY_KEY_LENGTH = 64; // 空节点key长度

        static const char EMPTY_KEY_CHAR = '0'; // 空节点key长度

        static const uint8_t MOVE_COUNT = 4; // 子节点数量、只能是16

        static const uint8_t BIT_MASK = 0x0f; // 子节点数量、只能是16
    private:
        //存储中间变换节点
        std::map<std::string, mpt_node_storage<CHILDREN_COUNT>> extension_nodes;

        mpt_node<Value_Type, CHILDREN_COUNT> *root; // 根节点

        leveldb::DB *extensions_db;

        leveldb::DB *accounts_db;
    public:
        merkle_patricia_trie(leveldb::DB *extensions_db, leveldb::DB *accounts_db);

        merkle_patricia_trie();

        merkle_patricia_trie(const merkle_patricia_trie &other) = delete; //不允许拷贝,只能移动

        merkle_patricia_trie(merkle_patricia_trie &&other) noexcept;

        auto operator=(const merkle_patricia_trie &other) -> merkle_patricia_trie& = delete;

        auto operator=(merkle_patricia_trie &&other) noexcept -> merkle_patricia_trie&;
    private:
        auto trie_free(mpt_node<Value_Type, CHILDREN_COUNT> *node) -> void;

        auto get_hash_helper_2stack() -> uint256;
    public:
        auto get_change_store() -> std::map<std::string, mpt_node_storage<CHILDREN_COUNT>>&;

        /* 插入一个新 key-value,所有的key的长度都是一样的 */
        auto insert(const uint256 &key, Value_Type value) -> void;

        /* 通过key 查找value，如果找到了就返回 value的引用  */
        auto search(const uint256 &key) -> account*;

        /* 标记查找方法 */
        auto search_and_mark(const uint256 &key) -> account*;

        /* 判断某个账户是否存在 */
        auto find(const uint256 &key) -> bool;

        /* 删除 key 对应的节点 */
        auto erase(const uint256 &key) -> bool;

        /* 获得 patricia_merkle_root,非递归深度优先遍历实现,目前采用双栈实现 */
        auto get_hash() -> uint256 override;

        virtual ~merkle_patricia_trie();

        /*需要这个节点是存在的*/
        auto load_node_from_leveldb(mpt_node_cache<mpt_node<Value_Type, CHILDREN_COUNT>>& cache) -> mpt_node<Value_Type, CHILDREN_COUNT>*;
    };

    template<typename Value_Type>
    auto merkle_patricia_trie<Value_Type>::load_node_from_leveldb(mpt_node_cache<mpt_node<Value_Type, CHILDREN_COUNT>> &cache) -> mpt_node<Value_Type, CHILDREN_COUNT> * {
        if (cache.node == nullptr){
            if (extensions_db == nullptr){
                throw std::logic_error("leveldb pointer extensions_db is nullptr, failed!");
            }
            bool is_need_load_leaf_content = (cache.key.size() >= 64);
            std::string slice;
            leveldb::Status status =  extensions_db->Get(leveldb::ReadOptions(),cache.key, &slice);
            if (status.ok()){
                muse::serializer::BinaryDeserializeView viewer;
                viewer.reset_bytestream(slice.data(), slice.size());
                mpt_node_storage<CHILDREN_COUNT> storage;
                viewer.output(storage);
                cache.node = mpt_node<Value_Type,CHILDREN_COUNT>::mns_to_mnp(storage, is_need_load_leaf_content);
            }else{
                throw std::logic_error("read extension node from leveldb failed!");
            }
            if (is_need_load_leaf_content){
                //加载账户
                if (accounts_db != nullptr){
                    uint256 account_address(cache.key,true);
                    std::string key_value;
                    leveldb::Slice key_slice(reinterpret_cast<const char*>(account_address.get_data_const()), uint256::Width);
                    status =  accounts_db->Get(leveldb::ReadOptions(),key_slice, &key_value);
                    if (status.ok()){
                        muse::serializer::BinaryDeserializeView viewer;
                        viewer.reset_bytestream(key_value.data(), key_value.size());
                        auto* _account = new_by_pool<account>();
                        viewer.output(*_account);
                        cache.node->value = _account;
                    }else{
                        throw std::logic_error("read account from leveldb failed!");
                    }
                }else{
                    throw std::logic_error("leveldb pointer accounts_db is nullptr, failed!");
                }
            }
        }
        return cache.node;
    }

    template<typename Value_Type>
    auto merkle_patricia_trie<Value_Type>::search_and_mark(const uint256 &key) -> account * {
        if (root == nullptr) {
            throw std::runtime_error("The move constructor or operator has already been called");
        }
        auto *node = root;
        std::string subscripts(uint256::Width * 2,1);
        size_t idx = 0;
        for(size_t i = 0; i < uint256::Width; i++){
            subscripts[idx++] = key[i] >> MOVE_COUNT;
            subscripts[idx++] = key[i] & BIT_MASK;
        };
        auto last = uint256::Width * 2 - 1;
        //父节点所在栈，需要统计变化点
        std::stack<mpt_node<Value_Type, CHILDREN_COUNT>*> modified_stack;
        for(int i = 0; i <= last; i++){
            //父节点入栈
            modified_stack.push(node);
            size_t subscript = subscripts[i];
            if (!node->get_children()->get_exist(subscript)){
                return nullptr;
            }
            //node_next 下一个位置
            auto* node_next = load_node_from_leveldb(node->get_children()->find(subscript));
            //node 任然是父节点
            std::string & sub_key = node_next->sub_key;
            auto subLast = node_next->sub_key.size() - 1;
            for(int j = 0; j <= subLast; j++, i++){
                if (sub_key[j] == subscripts[i]){
                    //到最后都相等
                    if (i == last){
                        // 新键比较到最后一个字符，还是一样的
                        if (j == subLast){
                            //标记修改,存在这个key了
                            while (!modified_stack.empty()){
                                modified_stack.top()->set_modified(true);
                                modified_stack.pop();
                            }
                            return node_next->value;
                        }
                        return nullptr;
                    }
                    //新增键较长，直接下一个孩子节点例如 abc，新增 abcdefg
                    if (j == subLast) {
                        break; //交给下一层来处理
                    }
                }else{
                    return nullptr;
                }
            }
            //去到下一层
            node = node_next;
        }
        return nullptr;
    }

    template<typename Value_Type>
    auto merkle_patricia_trie<Value_Type>::trie_free(mpt_node<Value_Type, CHILDREN_COUNT> *node) -> void {
        /* 放弃递归，采用循环 */
        if (node == nullptr) {
            return;
        }
        std::stack<mpt_node<Value_Type, CHILDREN_COUNT>*> node_stack;
        node_stack.push(node);
        while (!node_stack.empty()){
            auto * current = node_stack.top();
            node_stack.pop();
            //非叶子节点
            if (!current->get_leaf()){
                if (!current->get_children()->empty()){
                    for (size_t i = 0; i < CHILDREN_COUNT; i++) {
                        //非叶节点
                        if (current->get_children()->get_exist(i)){
                            auto son = current->get_children()->find(i).node;
                            if (son != nullptr){
                                node_stack.push(son);
                            }
                        }
                    }
                }
            }
            if (current->get_modified()){
                std::cerr << "modified: " << current->key << "\n";
            }
            //std::cerr << "die: " << current->key << "\n";
            delete_by_pool<mpt_node<Value_Type, CHILDREN_COUNT>>(current);
        }
    }

    template<typename Value_Type>
    auto merkle_patricia_trie<Value_Type>::get_hash_helper_2stack() -> uint256 {
        if (root == nullptr) {
            throw std::runtime_error("The move constructor or operator has already been called");
        }
        auto *node = root;
        if (node->get_modified()){
            //使用栈，避免递归调用
            std::stack<mpt_node<Value_Type, CHILDREN_COUNT>*> auxiliary_stack;
            std::stack<mpt_node<Value_Type, CHILDREN_COUNT>*> cache_stack;

            auxiliary_stack.push(node);
            cache_stack.push(node);

            while (!auxiliary_stack.empty()){
                auto * current = auxiliary_stack.top();
                auxiliary_stack.pop();
                //当前节点是分支节点
                if (!current->get_leaf()){
                    if (!current->get_children()->empty()){
                        for (size_t i = 0; i < CHILDREN_COUNT; i++){
                            //它的孩子节点存在，并且该孩子节点的内容已经修改了
                            if(current->get_children()->get_exist(i))
                            {
                               auto * _mpt_node = load_node_from_leveldb(current->get_children()->find(i));
                                if (_mpt_node->get_modified() && !_mpt_node->get_leaf()){
                                    auxiliary_stack.push(_mpt_node);
                                    cache_stack.push(_mpt_node);
                                }
                            }
                        }
                    }
                }
            }
            //计算 hash 工具
            auto hash_tool = encryption::get_hash_base();
            while (!cache_stack.empty()){
                auto top = cache_stack.top();
                cache_stack.pop();
                uint256 result;
                //std::cout << "test: " << top->key << "\n";
                for (size_t i = 0; i < CHILDREN_COUNT; i++){
                    if (top->get_children()->get_exist(i)){
                        auto * _mpt_node = load_node_from_leveldb(top->get_children()->find(i));
                        if (_mpt_node->get_leaf()){
                            Value_Type* left_node = _mpt_node->value;
                            _mpt_node->value_hash = left_node->get_hash();
                            //std::map<std::string, mpt_node_storage<CHILDREN_COUNT>>::iterator
                            auto itr = extension_nodes.find(_mpt_node->key);
                            if (itr != extension_nodes.end()){
                                itr->second.value_hash = _mpt_node->value_hash;
                            }
                        }
                        result = result + _mpt_node->value_hash;
                        _mpt_node->set_modified(false);
                    }
                }
                top->set_modified(false);
//                std::cerr << "before: " << top->value_hash.get_hex() << "\n";
                top->value_hash = result.get_sha3_256();
//                std::cerr << "after: " << top->value_hash.get_hex() << "\n";
                auto top_itr = extension_nodes.find(top->key);
                //std::map<std::string, mpt_node_storage<CHILDREN_COUNT>>::iterator
                if (top_itr != extension_nodes.end()){
                    top_itr->second.value_hash = top->value_hash;
                }else{
                    extension_nodes[top->key] = top->to_mpt_node_storage(mpt_node_storage<CHILDREN_COUNT>::UPDATE_OP_MUSE);
                }
            }
        };
        return root->value_hash;
    }

    template<typename Value_Type>
    auto merkle_patricia_trie<Value_Type>::get_hash() -> uint256 {
        return get_hash_helper_2stack();
    }

    template<typename Value_Type>
    auto merkle_patricia_trie<Value_Type>::get_change_store() ->
    std::map<std::string, mpt_node_storage<CHILDREN_COUNT>> &
    {
        get_hash();//更新hash内容
        return extension_nodes;
    }

    template<typename Value_Type>
    auto merkle_patricia_trie<Value_Type>::find(const uint256 &key) -> bool {
        if (root == nullptr) {
            throw std::runtime_error("The move constructor or operator has already been called");
        }
        auto *node = root;
        std::string subscripts(uint256::Width * 2,1);
        size_t idx = 0;
        for(size_t i = 0; i < uint256::Width; i++){
            subscripts[idx++] = key[i] >> MOVE_COUNT;
            subscripts[idx++] = key[i] & BIT_MASK;
        };
        auto last = uint256::Width * 2 - 1;
        for(int i = 0; i <= last; i++){
            size_t subscript = subscripts[i];
            if (!node->get_children()->get_exist(subscript)){
                return false;
            }
            //node_next 下一个位置
            auto* node_next = load_node_from_leveldb(node->get_children()->find(subscript));
            //node 任然是父节点
            std::string & sub_key = node_next->sub_key;
            auto subLast = node_next->sub_key.size() - 1;
            for(int j = 0; j <= subLast; j++, i++){
                if (sub_key[j] == subscripts[i]){
                    //到最后都相等
                    if (i == last){
                        // 新键比较到最后一个字符，还是一样的
                        if (j == subLast){
                            //存在这个key了
                            return true;
                        }
                        return false;
                    }
                    //新增键较长，直接下一个孩子节点例如 abc，新增 abcdefg
                    if (j == subLast) {
                        break; //交给下一层来处理
                    }
                }else{
                    return false;
                }
            }
            //去到下一层
            node = node_next;
        }
        return false;
    }

    template<typename Value_Type>
    auto merkle_patricia_trie<Value_Type>::erase(const uint256 &key) -> bool {
        if (root == nullptr) {
            throw std::runtime_error("The move constructor or operator has already been called");
        }
        auto *node = root;
        std::string subscripts(uint256::Width * 2,1);
        size_t idx = 0;
        for(size_t i = 0; i < uint256::Width; i++){
            subscripts[idx++] = key[i] >> MOVE_COUNT;
            subscripts[idx++] = key[i] & BIT_MASK;
        };
        //父节点所在栈
        std::stack<mpt_node<Value_Type, CHILDREN_COUNT>*> stk;
        auto last = uint256::Width * 2 - 1;
        for(size_t i = 0; i <= last; i++){
            auto subscript = subscripts[i];
            if (!node->get_children()->get_exist(subscript)){
                return false;
            }
            //node_next 下一个位置
            auto* node_next = load_node_from_leveldb(node->get_children()->find(subscript));
            //node 任然是父节点
            std::string & sub_key = node_next->sub_key;
            auto sub_last = node_next->sub_key.size() - 1;

            for(size_t j = 0; j <= sub_last; j++, i++){
                if (sub_key[j] == subscripts[i]){
                    if (i == last){
                        if (j == sub_last){
                            //当前节点就是需要删除的节点
                            break;
                        }
                        //如果 j 没有结束，没有找到，说明不是要删除的节点
                        return false;
                    }
                    if (j == sub_last){
                        break; // 到下一层去
                    }
                }else{
                    //没找到，直接结束
                    return false;
                }
            }
            stk.push(node);
            node = node_next;
        }
        mpt_node<Value_Type, CHILDREN_COUNT>* parent_node = stk.top();
        stk.pop();
        parent_node->set_modified(true);
        //被删除的元素，肯定是叶子节点
        extension_nodes[node->key] = node->to_mpt_node_storage(mpt_node_storage<CHILDREN_COUNT>::DELETE_OP_MUSE);
        if (parent_node == root){
            //erase 方法会自动释放内存
            root->get_children()->erase(node->sub_key[0]);
            extension_nodes[root->key] = root->to_mpt_node_storage(mpt_node_storage<CHILDREN_COUNT>::UPDATE_OP_MUSE);
            root->set_modified(true);
            return true;
        }
        //父节点一定是前缀节点，先删除这个叶子节点
        parent_node->get_children()->erase(node->sub_key[0]);
        size_t parent_child_size = parent_node->get_children()->size();
        if (parent_child_size == 1){
            /* 删除 **ecc
             *             (head)                      (head)(grand_parent_node)
             *       e（parent）        -->      eab
             *   cc     ab（brother）
             * */
            //只剩下一个兄弟节点
            int brother_idx = parent_node->get_children()->first();
            //需要删除父节点,爷爷节点指向兄弟
            if(brother_idx >= 0){
                extension_nodes[parent_node->key] = parent_node->to_mpt_node_storage(mpt_node_storage<CHILDREN_COUNT>::DELETE_OP_MUSE);

                auto brother = load_node_from_leveldb(parent_node->get_children()->find(brother_idx));
                //合并操作
                brother->sub_key = parent_node->sub_key + brother->sub_key; //合并
                //爷节点
                mpt_node<Value_Type, CHILDREN_COUNT>* grand_parent_node = stk.top();
                //删除父节点 e
                grand_parent_node->get_children()->erase(parent_node->sub_key[0]);
                //兄弟 变 父亲 ab ---> eab
                grand_parent_node->get_children()->insert(brother->sub_key[0], brother);

                brother->set_modified(true);
                grand_parent_node->set_modified(true);

                extension_nodes[brother->key] = brother->to_mpt_node_storage(mpt_node_storage<CHILDREN_COUNT>::UPDATE_OP_MUSE);
                extension_nodes[grand_parent_node->key] = grand_parent_node->to_mpt_node_storage(mpt_node_storage<CHILDREN_COUNT>::UPDATE_OP_MUSE);
            }
        }else if (parent_child_size >= 2){
            //父节点是前缀节点、有两个以上的兄弟节点(不管兄弟节点类型),只需要删除即可
        }
        while(!stk.empty()){
            auto front = stk.top();
            front->set_modified(true);
            stk.pop();
        }
        return true;
    }

    template<typename Value_Type>
    auto merkle_patricia_trie<Value_Type>::search(const uint256 &key) -> account* {
        if (root == nullptr) {
            throw std::runtime_error("The move constructor or operator has already been called");
        }
        auto *node = root;
        std::string subscripts(uint256::Width * 2,1);
        size_t idx = 0;
        for(size_t i = 0; i < uint256::Width; i++){
            subscripts[idx++] = key[i] >> MOVE_COUNT;
            subscripts[idx++] = key[i] & BIT_MASK;
        };
        auto last = uint256::Width * 2 - 1;
        for(int i = 0; i <= last; i++){
            size_t subscript = subscripts[i];
            if (!node->get_children()->get_exist(subscript)){
                return nullptr;
            }
            //node_next 下一个位置
            auto* node_next = load_node_from_leveldb(node->get_children()->find(subscript));
            //node 任然是父节点
            std::string & sub_key = node_next->sub_key;
            auto subLast = node_next->sub_key.size() - 1;
            for(int j = 0; j <= subLast; j++, i++){
                if (sub_key[j] == subscripts[i]){
                    //到最后都相等
                    if (i == last){
                        // 新键比较到最后一个字符，还是一样的
                        if (j == subLast){
                            //存在这个key了
                            return node_next->value;
                        }
                        return nullptr;
                    }
                    //新增键较长，直接下一个孩子节点例如 abc，新增 abcdefg
                    if (j == subLast) {
                        break; //交给下一层来处理
                    }
                }else{
                    return nullptr;
                }
            }
            //去到下一层
            node = node_next;
        }
       return nullptr;
    }

    template<typename Value_Type>
    merkle_patricia_trie<Value_Type>::~merkle_patricia_trie() {
        //释放旧的内存
        trie_free(this->root);
        this->root = nullptr;
    }

    template<typename Value_Type>
    auto merkle_patricia_trie<Value_Type>::operator=(merkle_patricia_trie &&other) noexcept -> merkle_patricia_trie & {
        //释放旧的内存
        if (this != &other){
            trie_free(this->root);
            this->root = other.root;
            other.root = nullptr;
        }
        return *this;
    }

    template<typename Value_Type>
    auto merkle_patricia_trie<Value_Type>::insert(const uint256 &key, Value_Type value) -> void {
        if (root == nullptr) {
            throw std::runtime_error("The move constructor or operator has already been called");
        }
        auto *node = root;
        std::string subscripts(uint256::Width * 2,1);
        size_t idx = 0;
        for(size_t i = 0; i < uint256::Width; i++){
            subscripts[idx++] = key[i] >> MOVE_COUNT;
            subscripts[idx++] = key[i] & BIT_MASK;
        }
        //父节点所在栈，需要统计变化点
        std::stack<mpt_node<Value_Type, CHILDREN_COUNT>*> modified_stack;
        auto last = uint256::Width * 2 - 1;
        for(int i = 0; i < uint256::Width * 2; i++){
            //父节点入栈
            modified_stack.push(node);
            uint8_t subscript = subscripts[i];
            //查找子节点,判断这个节点是否存在
            if (!node->get_children()->get_exist(subscript)){
                //新创建一个节点，存储这个值
                mpt_node<Value_Type, CHILDREN_COUNT>* new_node = mpt_node<Value_Type, CHILDREN_COUNT>::create_leaf_node(true);
                new_node->value = new_by_pool<Value_Type>(value);
                //设置前缀
                new_node->key = key.get_hex();
                //设置共同前缀
                new_node->sub_key = subscripts.substr(i);
                new_node->load = true;
                node->get_children()->insert(subscript, new_node);

                /* 新节点插入，记录变化点 */
                //父节点已经变化
                extension_nodes[node->key] = node->to_mpt_node_storage( mpt_node_storage<CHILDREN_COUNT>::UPDATE_OP_MUSE);
                //新节点
                extension_nodes[new_node->key] = new_node->to_mpt_node_storage( mpt_node_storage<CHILDREN_COUNT>::INSERT_OP_MUSE);
                //父节点内容已经变更了
                node->set_modified(true);
                while (!modified_stack.empty()){
                    modified_stack.top()->set_modified(true);
                    modified_stack.pop();
                }
                return;
            }
            //node_next 下一个位置
            auto* node_next = load_node_from_leveldb(node->get_children()->find(subscript));
            //node 任然是父节点
            std::string & sub_key = node_next->sub_key;
            auto subLast = node_next->sub_key.size() - 1;
            // 与子节点进行下标比较
            // i 指向新key
            // j 指向 node的 sub_key
            for(int j = 0; j <= subLast; j++, i++){
                //具有相同的前缀，则继续向下比较
                if (sub_key[j] == subscripts[i]){
                    //到最后都相等
                    if (i == last){
                        // 新键比较到最后一个字符，还是一样的
                        if (j == subLast){
                            //已经存在这个key了
                            return;
                        }
                        return;
                        //新增键较短，直接下一个孩子节点例如 abc，新增 ab,这种情况不应该出现，因为所有的key长度都是相同的
                    }
                    //新增键较长，直接下一个孩子节点例如 abc，新增 abcdefg,由于key的长度是相同的，所以不会出现
                    if (j == subLast) {
                        break; //交给下一层来处理
                    }
                }else{
                    /* (情况1)插入 [abqff] 不具有相同的前缀，则需要分裂、建立新的子节点
                     *         原来          --->             现在
                     *      (head/node)                   (head/node)
                     *   abcee                        ab
                     *                           cee       qff
                     *                     (prev_node)   (child_QQF)     */

                    /* (情况2) 插入 [aceee] 不具有相同的前缀，则需要分裂、建立新的子节点
                     *              原来          --->             现在
                     *          (head/node)                   (head/node)
                     *       ab                           a
                     *    cee   qff                 eee      b
                     *                                   cee   qff         */
                    // 新增相同前缀分支 ab/a
                    auto *child = mpt_node<Value_Type, CHILDREN_COUNT>::create_extension_node(true);
                    child->key = key.get_hex().substr(0,i);
                    child->sub_key = node_next->sub_key.substr(0, j);
                    child->load = true;

                    // 新建叶子节点 qqf/eee
                    auto *child_QQF = mpt_node<Value_Type, CHILDREN_COUNT>::create_leaf_node(true);
                    child_QQF->key = key.get_hex();
                    child_QQF->sub_key = std::string(subscripts.begin() + i, subscripts.end());
                    child_QQF->load = true;

                    //存储其值
                    child_QQF->value = new_by_pool<Value_Type>(value);
                    node_next->sub_key.erase(sub_key.begin(), sub_key.begin() + j);

                    //child 成为父节点，prev_node 就是 node_next
                    auto *prev_node = node->get_children()->update(subscript, child);
                    assert(node_next == prev_node);
                    child->get_children()->insert(child_QQF->sub_key.front(), child_QQF);
                    child->get_children()->insert(sub_key.front(), prev_node);

                    /* 记录变化点 */
                    extension_nodes[node_next->key] = node_next->to_mpt_node_storage(mpt_node_storage<CHILDREN_COUNT>::UPDATE_OP_MUSE);
                    extension_nodes[child->key] = child->to_mpt_node_storage(mpt_node_storage<CHILDREN_COUNT>::INSERT_OP_MUSE);
                    extension_nodes[node->key] = node->to_mpt_node_storage(mpt_node_storage<CHILDREN_COUNT>::UPDATE_OP_MUSE);
                    extension_nodes[child_QQF->key] = child_QQF->to_mpt_node_storage(mpt_node_storage<CHILDREN_COUNT>::INSERT_OP_MUSE);

                    //标记修改
                    while (!modified_stack.empty()){
                        modified_stack.top()->set_modified(true);
                        modified_stack.pop();
                    }
                    return;
                }
            }
            //去到下一层
            node = node_next;
        }
        //获得key的实际小标
    };

    template<typename Value_Type>
    merkle_patricia_trie<Value_Type>::merkle_patricia_trie(merkle_patricia_trie &&other) noexcept {
        this->root = other.root;
        other.root = nullptr;
    }

    /* 创建一个内存型trie */
    template<typename Value_Type>
    merkle_patricia_trie<Value_Type>::merkle_patricia_trie()
    :root(mpt_node<Value_Type,CHILDREN_COUNT>::create_extension_node(true)),accounts_db(nullptr),extensions_db(nullptr) {
        this->root->key = std::string(EMPTY_KEY_LENGTH, EMPTY_KEY_CHAR);
        this->get_hash();
    }

    template<typename Value_Type>
    merkle_patricia_trie<Value_Type>::merkle_patricia_trie(leveldb::DB *_extensions_db, leveldb::DB *_accounts_db)
    :extensions_db(_extensions_db), accounts_db(_accounts_db){
        try{
            std::string root_key(EMPTY_KEY_LENGTH, EMPTY_KEY_CHAR);
            std::string root_data;
            leveldb::Status status = _extensions_db->Get(leveldb::ReadOptions(), root_key, &root_data);
            if (status.ok()){
                muse::serializer::BinaryDeserializeView viewer;
                viewer.reset_bytestream(root_data.c_str(), root_data.size());
                mpt_node_storage<CHILDREN_COUNT> storage;
                viewer.output(storage);
                this->root = mpt_node<Value_Type,CHILDREN_COUNT>::mns_to_mnp(storage,false);
            }else{
                this->root = mpt_node<Value_Type,CHILDREN_COUNT>::create_extension_node(true);
                this->root->key = std::string(EMPTY_KEY_LENGTH,EMPTY_KEY_CHAR);
                //计算一下hash
                this->root->value_hash = this->get_hash();
                muse::serializer::BinarySerializer serializer;
                auto storage = this->root->to_mpt_node_storage(mpt_node_storage<CHILDREN_COUNT>::INSERT_OP_MUSE);
                serializer.input(storage);
                leveldb::Slice slice(serializer.getBinaryStream(), serializer.byteCount());
                auto opt = leveldb::WriteOptions();
                opt.sync = true;
                status = extensions_db->Put(opt,this->root->key, slice);
                if (!status.ok()){
                    SPDLOG_ERROR("LevelDB Put Error, Des: {}", status.ToString());
                    throw std::runtime_error("error in  constructor -- merkle_patricia_trie(leveldb::DB *_extensions_db, leveldb::DB *_accounts_db)");
                }
            }
        } catch (std::exception& ex){
            SPDLOG_ERROR("error in  constructor -- merkle_patricia_trie(leveldb::DB *_extensions_db, leveldb::DB *_accounts_db) {}", ex.what());
            throw ex;
        } catch (...){
            SPDLOG_ERROR("error in  constructor -- merkle_patricia_trie(leveldb::DB *_extensions_db, leveldb::DB *_accounts_db)");
            throw std::runtime_error("error in  constructor -- merkle_patricia_trie(leveldb::DB *_extensions_db, leveldb::DB *_accounts_db)");
        }
    }
}
#endif //MAKER_MERKLE_PATRICIA_TRIE_HPP
