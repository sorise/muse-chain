//
// Created by 14270 on 2023-12-12.
//
#define CATCH_CONFIG_MAIN

#include "catch.hpp"
#include "db/merkle_tree.hpp"
#include "db/uint256.hpp"
#include "db/transaction.hpp"

using namespace muse::chain;

TEST_CASE("get_merkle_root calculates the correct root for an empty collection", "[get_merkle_root]") {
    std::vector<uint256> collection; // 一个空的集合
    auto result = muse::chain::merkle_tree::get_merkle_root(collection);
    REQUIRE(result == uint256(0)); // 期望返回值是0
}

TEST_CASE("get_merkle_root calculates the correct root for a collection of size 1", "[get_merkle_root]") {
    std::vector<uint256> collection = { uint256("0x12345")} ; // 只有一个元素的集合
    auto result = merkle_tree::get_merkle_root(collection);
    REQUIRE(result == uint256("0x12345")); // 期望返回值与元素的哈希值相同
}

TEST_CASE("get_merkle_root calculates the correct root for a collection of size 2", "[get_merkle_root]") {
    std::vector<uint256> collection = { uint256("0x12345"), uint256("0x67890")};
    auto result = merkle_tree::get_merkle_root(collection);
    REQUIRE(result == (collection[0] + collection[1]).get_sha3_256());
}


TEST_CASE("get_merkle_root calculates the correct root for a collection of size 3", "[get_merkle_root]") {
    std::vector<uint256> collection = { uint256("0x12345"), uint256("0x67890"), uint256("0x11A67890")};
    auto result = merkle_tree::get_merkle_root(collection);
    auto left = (collection[0] + collection[1]).get_sha3_256();
    auto right = collection[2].get_sha3_256();
    auto target = (left+right).get_sha3_256();
    REQUIRE(result ==  target);
}


TEST_CASE("get_merkle_root calculates the correct root for a collection of size 6", "[get_merkle_root]") {
    transaction tx1;
    uint256 sender1 {"0x24843a48"};
    constexpr uint64_t value = 2000;
    tx1.set_sender(sender1);
    tx1.set_count(value);

    transaction tx2;
    uint256 sender2 {"0x14843a4899154"};
    constexpr uint64_t value2 = 4000;
    tx2.set_sender(sender2);
    tx2.set_count(value2);

    transaction tx3;
    uint256 sender3 {"0x14843a489915474"};
    constexpr uint64_t value3 = 7000;
    tx2.set_sender(sender3);
    tx2.set_count(value3);

    transaction tx4;
    uint256 sender4 {"0x14b843a4899154"};
    constexpr uint64_t value4 = 4000;
    tx2.set_sender(sender4);
    tx2.set_count(value4);

    transaction tx5;
    uint256 sender5 {"0x74a843a4899154"};
    constexpr uint64_t value5 = 4500;
    tx2.set_sender(sender5);
    tx2.set_count(value5);

    transaction tx6;
    uint256 sender6 {"0x174a843a4899154"};
    constexpr uint64_t value6 = 1500;
    tx2.set_sender(sender6);
    tx2.set_count(value6);

    std::vector<uint256> collection = { tx1.get_hash(), tx2.get_hash(), tx3.get_hash(), tx4.get_hash(), tx5.get_hash(), tx6.get_hash()};
    auto result = merkle_tree::get_merkle_root(collection);
    auto layer11 = (collection[0] + collection[1]).get_sha3_256();
    auto layer12 = (collection[2] + collection[3]).get_sha3_256();
    auto layer13 = (collection[4] + collection[5]).get_sha3_256();

    auto layer21 = (layer11 + layer12).get_sha3_256();
    auto layer22 = (layer13).get_sha3_256();

    auto target = (layer21 + layer22).get_sha3_256();

    std::cout << std::boolalpha << (target == result) << "\n";
    REQUIRE(result ==  target);
}


TEST_CASE("get_merkle_root_queue calculates the correct root for an empty collection", "[get_merkle_root_queue]") {
    std::queue<uint256> level; // 一个空的集合
    auto result = muse::chain::merkle_tree::get_merkle_root(std::move(level));
    REQUIRE(result == uint256(0)); // 期望返回值是0
}

TEST_CASE("get_merkle_root_queue calculates the correct root for a collection of size 1", "[get_merkle_root_queue]") {
    std::queue<uint256> collection { } ; // 只有一个元素的集合
    collection.emplace("0x12345");
    auto result = merkle_tree::get_merkle_root(std::move(collection));
    REQUIRE(result == uint256("0x12345")); // 期望返回值与元素的哈希值相同
}

TEST_CASE("get_merkle_root_queue calculates the correct root for a collection of size 2", "[get_merkle_root_queue]") {
    std::queue<uint256> collection { } ; // 只有一个元素的集合
    collection.emplace("0x12345");
    collection.emplace("0x67890");
    auto result = merkle_tree::get_merkle_root(std::move(collection));
    REQUIRE(result == (uint256("0x12345") +uint256("0x67890")).get_sha3_256());
}


TEST_CASE("get_merkle_root_queue calculates the correct root for a collection of size 3", "[get_merkle_root_queue]") {
    std::queue<uint256> collection;
    collection.emplace("0x12345");
    collection.emplace("0x67890");
    collection.emplace("0x11A67890");
    auto result = merkle_tree::get_merkle_root(std::move(collection));

    auto left = (uint256("0x12345") + uint256("0x67890")).get_sha3_256();
    auto right = uint256("0x11A67890").get_sha3_256();
    auto target = (left + right).get_sha3_256();

    REQUIRE(result ==  target);
}


TEST_CASE("get_merkle_root_queue calculates the correct root for a collection of size 6", "[get_merkle_root_queue]") {
    transaction tx1;
    uint256 sender1 {"0x24843a48"};
    constexpr uint64_t value = 2000;
    tx1.set_sender(sender1);
    tx1.set_count(value);

    transaction tx2;
    uint256 sender2 {"0x14843a4899154"};
    constexpr uint64_t value2 = 4000;
    tx2.set_sender(sender2);
    tx2.set_count(value2);

    transaction tx3;
    uint256 sender3 {"0x14843a489915474"};
    constexpr uint64_t value3 = 7000;
    tx2.set_sender(sender3);
    tx2.set_count(value3);

    transaction tx4;
    uint256 sender4 {"0x14b843a4899154"};
    constexpr uint64_t value4 = 4000;
    tx2.set_sender(sender4);
    tx2.set_count(value4);

    transaction tx5;
    uint256 sender5 {"0x74a843a4899154"};
    constexpr uint64_t value5 = 4500;
    tx2.set_sender(sender5);
    tx2.set_count(value5);

    transaction tx6;
    uint256 sender6 {"0x174a843a4899154"};
    constexpr uint64_t value6 = 1500;
    tx2.set_sender(sender6);
    tx2.set_count(value6);

    std::vector<uint256> collection = { tx1.get_hash(), tx2.get_hash(), tx3.get_hash(), tx4.get_hash(), tx5.get_hash(), tx6.get_hash()};

    std::queue<uint256> level(std::deque<uint256>(collection.begin(), collection.end()));

    auto result = merkle_tree::get_merkle_root(std::move(level));
    auto layer11 = (collection[0] + collection[1]).get_sha3_256();
    auto layer12 = (collection[2] + collection[3]).get_sha3_256();
    auto layer13 = (collection[4] + collection[5]).get_sha3_256();

    auto layer21 = (layer11 + layer12).get_sha3_256();
    auto layer22 = (layer13).get_sha3_256();

    auto target = (layer21 + layer22).get_sha3_256();

    std::cout << std::boolalpha << (target == result) << "\n";
    REQUIRE(result ==  target);
}