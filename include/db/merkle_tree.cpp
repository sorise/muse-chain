//
// Created by 14270 on 2023-12-12.
//

#include "merkle_tree.hpp"

namespace muse::chain{

    int merkle_tree::distance_power_of_2_n(uint32_t n){
        uint32_t v = n;
        if (n == 0) {
            return -1;  // 负数或零无法计算距离
        }
        if ((n & (n - 1)) == 0) {
            return 0;  // 是2的n次幂，距离为0
        }
        uint32_t distance = 1;
        while (n > 0) {
            n >>= 1;  // 右移一位，相当于除以2
            distance *= 2;  // 距离乘以2
        }
        return static_cast<int>(distance - v);
    }

    auto merkle_tree::get_merkle_root_worker(std::vector<uint256> &collection) -> uint256 {
        // 确保输入的collection不为空
        if (collection.empty()) {
            return uint256(0);
        }
        // 对于只有一个元素的情况，直接返回该元素的哈希值
        if (collection.size() == 1) {
            return collection[0];
        }
        std::vector<uint256> intermediate;
        // 使用size_t类型的索引遍历集合
        for (std::size_t i = 0; i < collection.size(); i += 2) {
            if (i + 1 < collection.size()) {
                intermediate.push_back((collection[i] + collection[i + 1]).get_sha3_256());
            }
        }
        // 递归调用，处理剩余的intermediate集合
        return get_merkle_root_worker(intermediate);
    }

    auto merkle_tree::get_merkle_root(std::vector<uint256> &collection) -> uint256 {
        // 确保输入的collection不为空
        if (collection.empty()) {
            return uint256(0);
        }
        // 对于只有一个元素的情况，直接返回该元素的哈希值
        if (collection.size() == 1) {
            return collection[0];
        }
        //使用最后一个叶子节点，填充中间节点
        auto distance = distance_power_of_2_n(collection.size());
        //填充中间节点，直到叶子节点数目为2的n次方
        if (distance > 0){
            uint256 last = collection.back();
            for (int i = 0; i < distance; ++i) {
                collection.push_back(last);
            }
        }
        assert((collection.size() & (collection.size() - 1)) == 0);
        // 递归调用，处理剩余的intermediate集合
        return get_merkle_root_worker(collection);
    }

    auto merkle_tree::get_merkle_root(std::queue<uint256> &&store_queue) -> uint256 {
        // 确保输入的collection不为空
        if (store_queue.empty()) {
            return uint256(0);
        }
        // 对于只有一个元素的情况，直接返回该元素的哈希值
        if (store_queue.size() == 1) {
            return store_queue.front();
        }
        //使用最后一个叶子节点，填充中间节点
        auto distance = distance_power_of_2_n(store_queue.size());
        //填充中间节点，直到叶子节点数目为2的n次方
        if (distance > 0){
            uint256 last = store_queue.back();
            for (int i = 0; i < distance; ++i) {
                store_queue.push(last);
            }
        }
        while (store_queue.size() > 1){
            std::queue<uint256> level;
            while (!store_queue.empty()) {
                auto left = store_queue.front();
                store_queue.pop();
                auto right = store_queue.front();
                store_queue.pop();
                level.push((left + right).get_sha3_256());
            }
            store_queue.swap(level);
        }
        return store_queue.front();
    }

    auto merkle_tree::get_merkle_proof(uint256 tx_hash, std::list<uint256> &&collection, int index) -> std::vector<uint256> {
        // 如果集合为空，返回一个空的uint256向量
        if (collection.empty()) {
            return {};
        }
        // 如果集合只有一个元素，返回一个只包含该元素的SHA3-256哈希值的uint256向量
        if (collection.size() == 1) {
            return {collection.front().get_sha3_256()};
        }

        // 如果索引超出范围，返回一个空的uint256向量
        if (index < 0 || index >= collection.size()) {
            return {};
        }
        //使用最后一个叶子节点，填充中间节点
        auto distance = distance_power_of_2_n(collection.size());
        //填充中间节点，直到叶子节点数目为2的n次方
        if (distance > 0){
            uint256 last = collection.back();
            for (int i = 0; i < distance; ++i) {
                collection.emplace_back(last);
            }
        }
        std::vector<uint256> proof;

        while (collection.size() > 1){
            std::list<uint256> level;
            while (!collection.empty()) {
                auto left = collection.front();
                collection.pop_front();
                auto right = collection.front();
                collection.pop_front();
                auto insert = (left + right).get_sha3_256();
                if (left == tx_hash){
                    proof.emplace_back(right);
                    tx_hash = insert;
                }else if(right == tx_hash){
                    proof.emplace_back(left);
                    tx_hash = insert;
                }else{
                    //无事发生
                }
                level.emplace_back(insert);

            }
            collection.swap(level);
        }
        //加入定点
        proof.emplace_back(collection.back());
        return proof;
    }

    auto merkle_tree::get_merkle_root(std::list<uint256> &&collection) -> uint256 {
        // 确保输入的collection不为空
        if (collection.empty()) {
            return uint256(0);
        }
        // 对于只有一个元素的情况，直接返回该元素的哈希值
        if (collection.size() == 1) {
            return collection.front();
        }
        //使用最后一个叶子节点，填充中间节点
        auto distance = distance_power_of_2_n(collection.size());
        //填充中间节点，直到叶子节点数目为2的n次方
        if (distance > 0){
            uint256 last = collection.back();
            for (int i = 0; i < distance; ++i) {
                collection.emplace_back(last);
            }
        }
        while (collection.size() > 1){
            std::list<uint256> level;
            while (!collection.empty()) {
                auto left = collection.front();
                collection.pop_front();
                auto right = collection.front();
                collection.pop_front();
                level.emplace_back((left + right).get_sha3_256());
            }
            collection.swap(level);
        }
        return collection.front();
    }

    auto merkle_tree::merkle_proof_verify(const std::vector<uint256> &proof, const uint256& tx_hash, const uint256 &root) -> bool {
        if(proof.empty()) return false;
        if(proof.size() == 1){
            return tx_hash == root;
        }
        if (proof.back() != root) return false;
        uint256 target = tx_hash;
        for (int i = 0; i < proof.size() - 1; ++i) {
            target = (target + proof[i]).get_sha3_256();
        }
        return target == root;
    }
}