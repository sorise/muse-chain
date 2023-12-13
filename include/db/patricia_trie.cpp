//
// Created by 14270 on 2023-12-15.
//

#include "patricia_trie.hpp"

namespace muse::chain{

    auto patricia_node::operator=(const patricia_node &other) -> patricia_node &{
        if (this!= &other) {
            leaf = other.leaf;
            sub_key = other.sub_key;
            key = other.key;
            children = other.children;
            count = other.count;
        }
        return *this;
    }

    patricia_node::patricia_node(patricia_node &&other) noexcept
    :leaf(other.leaf), sub_key(std::move(other.sub_key)), key(std::move(other.key)), children(std::move(other.children)), count(other.count) {

    }

    auto patricia_node::operator=(patricia_node &&other) noexcept -> patricia_node & {
        if (this!= &other) {
            leaf = other.leaf;
            sub_key = std::move(other.sub_key);
            key = std::move(other.key);
            children = std::move(other.children);
            count = other.count;
        }
        return *this;
    }

    auto patricia_trie::trie_free(patricia_node *node) -> void {
        if (node != nullptr){
            for(auto &child : node->children){
                trie_free(child.second);
            }
            delete node;
        }
    }

    patricia_trie::patricia_trie():root(new patricia_node()){

    }

    patricia_trie::patricia_trie(patricia_trie &&other) noexcept:root(other.root)
    {
        other.root = nullptr;
    }

    auto patricia_trie::operator=(patricia_trie &&other) noexcept -> patricia_trie & {
        if (this!= &other) {
            root = other.root;
            other.root = nullptr;
        }
        return *this;
    }

    patricia_trie::~patricia_trie(){
        //释放树中的各个节点
        trie_free(root);
    }

    void patricia_trie::insert(const std::string &word) {
        if (root == nullptr) {
            throw std::runtime_error("The move constructor or operator has already been called");
        }
        patricia_node *node = root;
        auto last = word.size() - 1;
        for(size_t i = 0; i < word.size(); i++){
            //没有找到下一层对应节点，则直接建立这个节点
            auto node_iterator = node->children.find(word[i]);
            //如果改变map，迭代器会失效哦
            if( node_iterator == node->children.end()){
                //由于需要向下延伸，则其父节点不再是叶子节点
                node->leaf = false;
                patricia_node* new_node = new patricia_node();
                new_node->sub_key = word.substr(i);
                new_node->key = word;
                //计数点 +1
                new_node->count++;
                node->children[word[i]] = new_node;
                return; //可以直接撤了
            }
            auto* node_next = node_iterator->second;
            //此时node为父节点
            char child_key = word[i];
            auto subLast = node_next->sub_key.size() - 1;
            // 与子节点进行字符串比较
            // i 指向新key
            // j 指向 node的 sub_key
            for(int j = 0; j <= subLast; j++, i++){
                //具有相同的前缀，则继续向下比较
                if (node_next->sub_key[j] == word[i]){
                    //到最后都相等
                    if (i == last){
                        // 新键比较到最后一个字符，还是一样的
                        if (j == subLast){
                            //内容一致、长度一直，计算器 + 1 即可
                            node_next->count++;
                            /*  插入 abc
                             *        原来                 现在
                             *       (head)             (head)
                             *   abc                  abc
                             * */
                        }else{
                            /*  插入 ab
                             *        原来                 现在
                             *       (head)             (head)
                             *   abc                  ab
                             *                     c
                             * */
                            // 新增键短，例如 abc插入 ab 变成：ab,c
                            patricia_node* child = new patricia_node();
                            //child 为 c 节点
                            child->leaf = true;
                            child->count = node->children[child_key]->count;
                            child->sub_key = node->children[child_key]->sub_key.substr(j + 1);
                            child->key = node_next->key;

                            patricia_node* ab_node = node->children[child_key];
                            //abc node 转变为 ab 节点
                            ab_node->children[child->sub_key[0]] = child;
                            ab_node->key = word;
                            ab_node->sub_key = ab_node->sub_key.substr(0,j + 1);
                            ab_node->count = 1;
                            ab_node->leaf = false;
                        }
                        return;
                    }
                    //新增键较长，直接下一个孩子节点例如 abc，新增 abcdefg
                    if (j == subLast) {
                        /*  插入 abcd
                         *        原来                 现在
                         *       (head)             (head)
                         *   abc                  abc
                         *                     d
                         * */
                        break;
                        //交给下一层来处理
                    }
                }
                else{
                    //不具有相同的前缀，则需要分裂、建立新的子节点
                    /* 插入 aqqff
                     *         原来                  现在
                     *       (head)                (head)
                     *   abc                     a
                     *                     bc      qqff
                     * */
                    // 新增相同前缀分支 a
                    patricia_node* child = new patricia_node();
                    child->leaf = false;
                    child->count = 0;
                    child->sub_key = node_next->sub_key.substr(0, j);

                    // 新增相同前缀分支 qqff
                    patricia_node* child_child = new patricia_node();
                    child_child->leaf = true;
                    child_child->count = 1;
                    child_child->sub_key = word.substr(i);
                    child_child->key = word;

                    //child 成为父节点
                    node->children[child_key] = child;
                    //旧 abc
                    patricia_node *old = node_next;
                    old->sub_key = old->sub_key.substr(j);

                    child->children[old->sub_key[0]] = old;
                    child->children[child_child->sub_key[0]] = child_child;
                    return;
                }
            }
            //去到下一层
            node = node->children[child_key];
        }
    }

    auto patricia_trie::search(const std::string &word) const -> std::tuple<bool, uint32_t> {
        if (root == nullptr) {
            throw std::runtime_error("The move constructor or operator has already been called");
        }
        patricia_node *node = root;
        auto last = word.size() - 1;
        for(size_t i = 0; i < word.size(); i++){
            //没有找到下一层对应节点，则直接建立这个节点
            auto child_iterator = node->children.find(word[i]);
            //如果改变map，迭代器会失效哦
            if( child_iterator == node->children.end()){
                return {false, 0}; //可以直接撤了
            }
            patricia_node* child_node = child_iterator->second;
            std::string & sub_key = child_node->sub_key;
            auto last_sub_key = sub_key.size() - 1;
            //如果找到了，需要进行前缀匹配
            for(size_t j = 0; j <= last_sub_key; j++, i++){
                if (sub_key[j] == word[i]){
                    if (i == last){
                        if (j == last_sub_key){
                            //内容一致、长度一直
                            return {child_node->count > 0, child_node->count};
                        }
                        return {false, 0};
                    }
                    //新增键较长，直接下一个孩子节点例如 abc，新增 abcdefg
                    if (j == last_sub_key) {
                        break;
                    }
                }else{
                    return {false, 0}; //没找到
                }
            }
            node = child_node;
        }
        return {node->count > 0, node->count};
    }

    auto patricia_trie::delete_helper(const std::string &word, bool true_remove_false_erase) -> bool {
        if (root == nullptr) {
            throw std::runtime_error("The move constructor or operator has already been called");
        }
        patricia_node *node = root;
        auto last = word.size() - 1;
        //stk 使用栈来代替递归操作
        std::stack<patricia_node*> stk;
        for(size_t i = 0; i <= last; i++){
            auto child_node_iterator = node->children.find(word[i]);
            if (child_node_iterator == node->children.end()){
                //没找到，直接结束
                return false;
            }
            patricia_node* child_node = child_node_iterator->second;
            //如果找到了，需要进行前缀匹配
            std::string& sub_key = child_node->sub_key;
            auto sub_key_last = sub_key.size() - 1;

            for(size_t j = 0; j <= sub_key_last; j++, i++){
                if (sub_key[j] == word[i]){
                    if (i == last){
                        if (j == sub_key_last){
                            //当前节点就是需要删除的节点
                            break;
                        }
                        //如果 j 没有结束，没有找到，说明不是要删除的节点
                        return false;
                    }
                    if (j == sub_key_last){
                        break; // 到下一层去
                    }
                }else{
                    //没找到，直接结束
                    return false;
                }
            }
            stk.push(node);
            node = child_node;
        }
        //此时 node 是需要删除的节点
        node->count--;
        if (true_remove_false_erase && node->count > 0){
            //没必要删除
            return true;
        }
        patricia_node* parent_node = stk.top();
        stk.pop();
        size_t child_size = node->children.size();
        if (child_size == 0){
            //如果直属于根节点，那么删除即可
            if (parent_node == root){
                root->children.erase(node->sub_key[0]);
                delete node;
                return true;
            }
            //如果父节点是实际节点，那么直接删除即可。
            if (parent_node->count > 0){
                //说明是实际节点
                parent_node->children.erase(node->sub_key[0]);
                parent_node->leaf = parent_node->children.empty();
                delete node;
                return true;
            }
            //现在 parent_node->count == 0 说明是前缀节点
            parent_node->children.erase(node->sub_key[0]);
            delete node; //释放当前节点
            size_t parent_child_size = parent_node->children.size();
            //父节点是前缀节点、只有一个兄弟节点、兄弟节点为实节点 or 前缀节点
            if (parent_child_size == 1){
                //获得兄弟节点
                patricia_node *brother = parent_node->children.begin()->second;
                //合并操作
                //爷爷节点指向兄弟  兄弟 变 父亲
                patricia_node* grand_parent_node = stk.top(); //爷节点
                brother->sub_key = parent_node->sub_key + brother->sub_key; //合并

                grand_parent_node->children[brother->sub_key[0]] = brother;
                delete parent_node;
            }else if (parent_child_size >= 2){
                //父节点是前缀节点、有两个以上的兄弟节点(不管兄弟节点类型),只需要删除即可
            }
        }
        else if (child_size == 1){
            // 单亲节点
            // child_node 是当前节点的子节点
            patricia_node* child_node = node->children.begin()->second;
            child_node->sub_key = node->sub_key + child_node->sub_key;

            parent_node->children[child_node->sub_key[0]] = child_node;
            delete node; //释放当前节点
        }else{ //child_size >= 2
            node->count = 0;
            node->key.clear();
        }
        return true;
    }

    auto patricia_trie::erase(const std::string &word) -> bool {
        return delete_helper(word, false);
    }

    auto patricia_trie::remove(const std::string &word) -> bool {
        return delete_helper(word, true);
    }
};
