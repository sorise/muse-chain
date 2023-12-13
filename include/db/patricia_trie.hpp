//
// Created by 14270 on 2023-12-15.
//

#ifndef MAKER_PATRICIA_TRIE_HPP
#define MAKER_PATRICIA_TRIE_HPP

#include "Trie.hpp"

namespace muse::chain{

    struct patricia_node {
        //是否是叶子节点
        bool leaf {true};
        //部分key,共同前缀
        std::string sub_key;
        //节点完整 字符串 key
        std::string key;
        //子节点
        std::unordered_map<char, patricia_node*> children;
        //节点的计数
        uint32_t count {0};

        patricia_node() = default;

        patricia_node(patricia_node &&other) noexcept;

        patricia_node(const patricia_node& other) = default;

        auto operator=(const patricia_node&other) -> patricia_node &;

        auto operator=(patricia_node &&other) noexcept -> patricia_node &;

        ~patricia_node(){
            std::cout << "key: " << key << " --- die \n";
        };
    };

    /* 压缩字典树 */
    class patricia_trie {
    private:
        patricia_node *root; //根节点

        auto trie_free(patricia_node * node)-> void;

        auto delete_helper(const std::string& word, bool true_remove_false_erase) -> bool;
    public:
        patricia_trie();

        patricia_trie(const patricia_trie &other) = delete; //不允许拷贝

        patricia_trie(patricia_trie &&other) noexcept;

        auto operator=(patricia_trie &&other) noexcept -> patricia_trie &;

        ~patricia_trie();

        auto operator=(const patricia_trie &other) -> patricia_trie & = delete; //不允许复制

        /* 插入一个字符串 */
        auto insert(const std::string &word) -> void;
        /* 查找字符串 */
        [[nodiscard]] auto search(const std::string &word) const -> std::tuple<bool, uint32_t>;
        /* 删除字符串, 返回是否删除成功，不成功，说明没有找到 */
        auto erase(const std::string &word) -> bool;
        /* 删除字符串,如果计数器为1才会真真删除，否则计数器减一 返回是否删除成功，不成功，说明没有找到 */
        auto remove(const std::string &word) -> bool;
    };
}

#endif //MAKER_PATRICIA_TRIE_HPP
