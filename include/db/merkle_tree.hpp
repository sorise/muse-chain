//
// Created by 14270 on 2023-12-11.
//

#ifndef MAKER_MERKLE_HPP
#define MAKER_MERKLE_HPP

#include "uint256.hpp"
#include <stack>
#include <queue>

namespace muse::chain{

    /* 参考文档：https://developer.aliyun.com/article/842854 */
    class merkle_tree{
    private:
        static auto get_merkle_root_worker(std::vector<uint256>& collection) -> uint256 ;
    public:
        /* 使用递归来实现 */
        static auto get_merkle_root(std::vector<uint256>& collection) -> uint256;
        /* 实现迭代了实现 */
        static auto get_merkle_root_queue(std::queue<uint256> &&store_queue) -> uint256;
        /*
         * 描述：生成某个交易的 默克尔证明
         * @tx_hash 交易的hash值
         * @merkle_root 默克尔树的根节点
         * @index 交易在列表中的索引
         *
            交易哈希值：对交易数据进行哈希运算得到的固定长度的字符串。
            叶子节点哈希值：将每个交易哈希值作为叶子节点，对它们进行哈希运算得到的固定长度的字符串。
            中间节点哈希值：将相邻的两个叶子节点哈希值组合在一起，对它们进行哈希运算得到的固定长度的字符串。
            根节点哈希值：将所有中间节点哈希值组合在一起，对它们进行哈希运算得到的固定长度的字符串，即默克尔树的根节点哈希值
         * */
        static auto get_merkle_proof(std::queue<uint256> &&store_queue, const uint256& merkle_root, int index) -> std::vector<uint256>;

        /* gap */
        static int distance_power_of_2_n(uint32_t n);
    };
}
#endif //MAKER_MERKLE_HPP
