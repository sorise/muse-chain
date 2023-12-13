#define CATCH_CONFIG_MAIN

#include "catch.hpp"
#include "db/account.hpp"
#include "db/merkle_tree.hpp"
#include "db/uint256.hpp"
#include "db/merkle_patricia_trie.hpp"
#include "db/connector_account.hpp"

using namespace muse::chain;

TEST_CASE("basic use", "connector_account"){
    uint256 key1 {"0x1c417a04f8be4f13d9fb18647e39030ff4a7efbc3bb6340c38582d0de45cc25d"};
    uint256 key2 {"0x1c417a04f8be4f13d9fb18647e39030ff4a7efbc3bb6340c38582d0de45cc111"};
    uint256 key3 {"0x1c417a04f8be4f13d9fb18647e39030ff4a7efbc3bb6340c38582d0de4154111"};
    uint256 key4 {"0x2c417a04f8be4f13d9fb18647e39030ff4a7efbc3bb6340c38582d0de4154111"};
    uint256 key_no {"0x3c417a04f8be4f13d9fb18647e39030ff4a7efbc3bb6340c38582d0de4154111"};

    connector_account connector;

    account value1(key1, 0);
    value1.add_balance(1, 1000);
    account value2(key2, 0);
    value2.add_balance(1, 10000);
    account value3(key3, 0);
    value3.add_balance(1, 104000);
    account value4(key4, 0);
    value4.add_balance(1, 105000);

    connector.account_mpt.insert(key1, value1);
    connector.account_mpt.insert(key2, value2);
    connector.account_mpt.insert(key3, value3);
    connector.account_mpt.insert(key4, value4);

    transaction tx;
    tx.set_transaction_operator(transaction_operator::Append);
    tx.set_sender(key4);
    tx.set_nonce(1);
    tx.set_count(5000);
    tx.outs.push_back(out_entry(key1, 1000));
    tx.outs.push_back(out_entry(key2, 1000));
    tx.outs.push_back(out_entry(key3, 1000));
    tx.outs.push_back(out_entry(key_no, 1000));

    std::list<rollback_operation> rollbacks;
    connector.append_created_rollback(rollbacks,tx);

    for (auto& back: rollbacks) {
        std::cout << back.to_string() ;
    }

    std::cout << "GAP ----------------------------------------------------" << "\n";
    transaction tx_out;
    tx_out.set_transaction_operator(transaction_operator::Establish);
    uint256 addr("0x1321");
    tx_out.set_sender(addr);
    tx_out.set_nonce(2);
    tx_out.set_count(0);
    tx_out.set_asset_type(115);

    std::list<rollback_operation> rollbacks2;
    connector.append_created_rollback(rollbacks2,tx);

    std::cout << "GAP ----------------------------------------------------" << "\n";
    transaction tx_out_1;
    tx_out_1.set_transaction_operator(transaction_operator::Establish);
    tx_out_1.set_sender(key1);
    tx_out_1.set_nonce(2);
    tx_out_1.set_count(0);
    tx_out_1.set_asset_type(1015);

    std::list<rollback_operation> rollbacks3;
    connector.append_created_rollback(rollbacks3, tx_out_1);

    REQUIRE(rollbacks3.front().update_type == rollback_operation::UPDATE_DELETE_ASSET);
}


TEST_CASE("check block body", "connector_account"){

    SECTION("transaction_operator::Establish") {
        block blk{};
        transaction build;
        uint256 sender("0x15824542145");

        build.set_nonce(1);
        build.set_transaction_operator(transaction_operator::Establish);
        build.set_sender(sender);

        blk.body.transactions.push_back(build);
        connector_account connector;
        auto result = connector.check_block_body(blk);
        REQUIRE(result);
    }

    SECTION("transaction_operator::Append") {
        block blk{};
        transaction trans;
        uint256 sender("0x15824542145");
        uint256 receiver("0x75824542145");

        trans.set_nonce(1);
        trans.set_transaction_operator(transaction_operator::Append);
        trans.set_sender(sender);
        trans.set_count(2000);
        trans.set_asset_type(748);

        trans.outs.emplace_back( out_entry{receiver, 2000});

        blk.body.transactions.push_back(trans);
        connector_account connector;
        auto result = connector.check_block_body(blk);
        REQUIRE(!result);
    }

    SECTION("transaction_operator::Append Error") {
        block blk{};
        transaction trans;
        uint256 sender("0x15824542145");
        uint256 receiver("0x75824542145");

        trans.set_nonce(1);
        trans.set_transaction_operator(transaction_operator::Append);
        trans.set_sender(sender);
        trans.set_count(2000);
        trans.set_asset_type(748);

        trans.outs.emplace_back( out_entry{receiver, 1000});

        blk.body.transactions.push_back(trans);
        connector_account connector;
        auto result = connector.check_block_body(blk);
        REQUIRE(!result);
    }


    SECTION("transaction_operator::Append Error Nonce") {
        block blk{};
        transaction trans;
        uint256 sender("0x15824542145");
        uint256 receiver("0x75824542145");

        trans.set_nonce(2);
        trans.set_transaction_operator(transaction_operator::Append);
        trans.set_sender(sender);
        trans.set_count(2000);
        trans.set_asset_type(748);

        trans.outs.emplace_back( out_entry{receiver, 2000});

        blk.body.transactions.push_back(trans);
        connector_account connector;
        auto result = connector.check_block_body(blk);
        REQUIRE(!result);
    }

    SECTION("transaction_operator::Append Right ") {
        block blk{};
        transaction trans;
        uint256 sender("0x15824542145");
        uint256 receiver("0x75824542145");

        connector_account connector;

        account me(sender, 0);
        me.add_balance(748,8000);

        connector.account_mpt.insert(sender, me);

        trans.set_nonce(1);
        trans.set_transaction_operator(transaction_operator::Append);
        trans.set_sender(sender);
        trans.set_count(2000);
        trans.set_asset_type(748);

        trans.outs.emplace_back( out_entry{receiver, 2000});

        blk.body.transactions.push_back(trans);

        auto result = connector.check_block_body(blk);
        REQUIRE(result);
    }

}
