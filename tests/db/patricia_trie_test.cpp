//
// Created by 14270 on 2023-12-16.
//
#define CATCH_CONFIG_MAIN

#include "catch.hpp"
#include "db/merkle_tree.hpp"
#include "db/uint256.hpp"
#include "db/patricia_trie.hpp"

using namespace muse::chain;

TEST_CASE("patricia_trie::search", "[patricia_trie]") {
    // Initialize the trie
    patricia_trie trie;
    // Insert some words into the trie
    trie.insert("apple");
    trie.insert("app");
    trie.insert("banana");
    trie.insert("orange");

    SECTION("Test word \"apple\"") {
        std::tuple<bool, uint32_t> result = trie.search("apple");
        REQUIRE(std::get<0>(result));  // Expect count > 0
        REQUIRE(std::get<1>(result) == 1);  // Expect count = 1
    }

    SECTION("Test word \"grape\"") {
        std::tuple<bool, uint32_t> result = trie.search("grape");
        REQUIRE_FALSE(std::get<0>(result));  // Expect count <= 0
        REQUIRE(std::get<1>(result) == 0);  // Expect count = 0
    }

    SECTION("Test word \"banana\"") {
        std::tuple<bool, uint32_t> result = trie.search("banana");
        REQUIRE(std::get<0>(result));  // Expect count > 0
        REQUIRE(std::get<1>(result) == 1);  // Expect count = 1
    }

    SECTION("Test word \"oranges\"") {
        std::tuple<bool, uint32_t> result = trie.search("oranges");
        REQUIRE_FALSE(std::get<0>(result));  // Expect count <= 0
        REQUIRE(std::get<1>(result) == 0);  // Expect count = 0
    }
}