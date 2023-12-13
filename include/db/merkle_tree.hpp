//
// Created by 14270 on 2023-12-11.
//

#ifndef MAKER_MERKLE_HPP
#define MAKER_MERKLE_HPP

#include "uint256.hpp"
#include <stack>
#include <queue>
#include <list>

namespace muse::chain{
    /* 提供与操作默克尔树相关的API */
    class merkle_tree{
    private:
        static auto get_merkle_root_worker(std::vector<uint256>& collection) -> uint256 ;
    public:
        /* 使用递归来实现，获得默克尔树根 */
        static auto get_merkle_root(std::vector<uint256>& collection) -> uint256;
        /* 基于List来实现，获得默克尔树根 */
        static auto get_merkle_root(std::list<uint256>&& collection) -> uint256;
        /* 实现迭代了实现，获得默克尔树根 */
        static auto get_merkle_root(std::queue<uint256> &&store_queue) -> uint256;
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
        static auto get_merkle_proof(uint256 tx_hash, std::list<uint256> &&collection, int index) -> std::vector<uint256>;

        /* 证明某一笔交易和默克尔proof是否有效 */
        static auto merkle_proof_verify(const std::vector<uint256> &proof,const uint256& tx_hash, const uint256 &root) -> bool;

        /* 计算填充数量
         * 在比特币中，为了确保能够构造出标准的默克尔树结构，采用了叶子节点复制的方法。具体来说，如果交易数量不是2的n次幂，
         * 会通过复制最后一个交易来填充缺失的交易，并将复制的交易的哈希值添加到默克尔树中。这样，就可以保证默克尔树的叶子
         * 节点个数是2的n次幂，从而构造出完整的默克尔树结构。
         * */
        static int distance_power_of_2_n(uint32_t n);
    };
}
#endif //MAKER_MERKLE_HPP
