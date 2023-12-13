#define CATCH_CONFIG_MAIN

#include "catch.hpp"
#include "db/account.hpp"
#include "db/merkle_tree.hpp"
#include "db/uint256.hpp"
#include "db/merkle_patricia_trie.hpp"

using namespace muse::chain;


TEST_CASE("search", "[merkle_patricia_trie]") {
    uint256 key1 {"0x1c417a04f8be4f13d9fb18647e39030ff4a7efbc3bb6340c38582d0de45cc25d"};
    uint256 key2 {"0x1c417a04f8be4f13d9fb18647e39030ff4a7efbc3bb6340c38582d0de45cc111"};
    uint256 key3 {"0x1c417a04f8be4f13d9fb18647e39030ff4a7efbc3bb6340c38582d0de4154111"};
    uint256 key4 {"0x2c417a04f8be4f13d9fb18647e39030ff4a7efbc3bb6340c38582d0de4154111"};

    merkle_patricia_trie<account> mpt;

    uint256 address1{"0x1c417a04f8be4f13d9fb18647e39030ff4a7efbc3bb6340c38582d0de45cc25d"};
    uint256 address2{"0x1c417a04f8be4f13d9fb18647e39030ff4a7efbc3bb6340c38582d0de45cc111"};
    uint256 address3{"0x1c417a04f8be4f13d9fb18647e39030ff4a7efbc3bb6340c38582d0de4154111"};
    uint256 address4{"0x2c417a04f8be4f13d9fb18647e39030ff4a7efbc3bb6340c38582d0de4154111"};

    account value1(address1, 0);
    value1.add_balance(1, 1000);
    account value2(address2, 0);
    value2.add_balance(1, 10000);
    account value3(address3, 0);
    value3.add_balance(1, 104000);
    account value4(address4, 0);
    value4.add_balance(1, 105000);


    mpt.insert(key1, value1);
    mpt.insert(key2, value2);
    mpt.insert(key3, value3);
    mpt.insert(key4, value4);

    account* add1 = mpt.search(key1);
    account* add2 = mpt.search(key2);
    account* add3 = mpt.search(key3);
    account* add4 = mpt.search(key4);

    REQUIRE(add1->get_balance(1) == value1.get_balance(1));
    REQUIRE(add2->get_balance(1) == value2.get_balance(1));
    REQUIRE(add3->get_balance(1) == value3.get_balance(1));
    REQUIRE(add4->get_balance(1) == value4.get_balance(1));
}

TEST_CASE("get_change_store", "[merkle_patricia_trie]"){
    uint256 key1 {"0x1c417a04f8be4f13d9fb18647e39030ff4a7efbc3bb6340c38582d0de45cc25d"};
    uint256 key2 {"0x1c417a04f8be4f13d9fb18647e39030ff4a7efbc3bb6340c38582d0de45cc111"};
    uint256 key3 {"0x1c417a04f8be4f13d9fb18647e39030ff4a7efbc3bb6340c38582d0de4154111"};
    uint256 key4 {"0x2c417a04f8be4f13d9fb18647e39030ff4a7efbc3bb6340c38582d0de4154111"};
    uint256 key5 {"0x3c417a04f8be4f13d9fb18647e39030ff4a7efbc3bb6340c38582d0de4154111"};


    account value1(key1, 0);
    value1.add_balance(1, 1000);
    account value2(key2, 0);
    value2.add_balance(1, 10000);
    account value3(key3, 0);
    value3.add_balance(1, 104000);
    account value4(key4, 0);
    value4.add_balance(1, 105000);
    account value5(key5, 0);
    value5.add_balance(1, 105000);

    merkle_patricia_trie<account> mpt;
    mpt.insert(key1, value1);
    mpt.insert(key2, value2);
    mpt.insert(key3, value3);
    mpt.insert(key4, value4);
    mpt.insert(key5, value5);


    auto& mmp = mpt.get_change_store();

    REQUIRE(mmp.size() == 3);
}

TEST_CASE("get_hash", "[merkle_patricia_trie]"){
    merkle_patricia_trie<account> mpt;

    uint256 key1 {"0x1c417a04f8be4f13d9fb18647e39030ff4a7efbc3bb6340c38582d0de45cc25d"};
    uint256 key2 {"0x1c417a04f8be4f13d9fb18647e39030ff4a7efbc3bb6340c38582d0de45cc111"};
    uint256 key3 {"0x1c417a04f8be4f13d9fb18647e39030ff4a7efbc3bb6340c38582d0de4154111"};

    account value1(key1, 0);
    value1.add_balance(1, 1000);
    account value2(key2, 0);
    value2.add_balance(1, 10000);
    account value3(key3, 0);
    value3.add_balance(1, 104000);

    mpt.insert(key1, value1);
    REQUIRE( mpt.get_hash().get_hex() == value1.get_hash().get_sha3_256().get_hex());
    mpt.insert(key2, value2);
    auto tdd = mpt.get_hash().get_hex();
    uint256 result = (value1.get_hash() + value2.get_hash()).get_sha3_256().get_sha3_256();
    std::cout << "2root hash: " << mpt.get_hash().get_hex() << "\n";
    std::cout << "2root expect hash: " << result.get_hex() << "\n";
    REQUIRE(mpt.get_hash() == result);
    mpt.insert(key3, value3);
    std::cout << "3root hash: " << tdd << "\n";
    uint256 r5cc = (value1.get_hash() + value2.get_hash()).get_sha3_256();
    r5cc = (value3.get_hash() + r5cc).get_sha3_256().get_sha3_256();
    REQUIRE(mpt.get_hash() == r5cc);
}