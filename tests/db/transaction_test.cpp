#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "db/transaction.hpp"

using namespace muse::chain;

TEST_CASE("data", "[transaction]"){
    transaction tx;
    tx.set_asset_type(32);
    REQUIRE(tx.get_asset_type() == 32);

    tx.set_transaction_operator(transaction_operator::Append);
    bool equal = (tx.get_transaction_operator() == transaction_operator::Append);
    REQUIRE(equal);

    tx.set_count(600000);
    REQUIRE(tx.get_count() == 600000);

    tx.set_transaction_version(transaction_version::V0001);
    bool t_op = tx.get_transaction_version() == transaction_version::V0001;
    REQUIRE(t_op);

    tx.set_expired_epoch(12);
    REQUIRE(tx.get_expired_epoch() == 12);

    tx.set_create_time(1200);
    REQUIRE(tx.get_create_time() == 1200);

    tx.set_nonce(450);
    REQUIRE(tx.get_nonce() == 450);

    uint256 han;
    out_entry t(han, 10);
    tx.outs.push_back(t);
    REQUIRE(tx.outs.front().count == 10);
    REQUIRE(tx.outs.size() == 1);

    uint256 real_sender(11);
    tx.set_sender(real_sender);
    uint256 sender = tx.get_sender();

    REQUIRE(real_sender == sender);
    real_sender.get_data()[0] = 215;
    REQUIRE_FALSE(real_sender == sender);
}

TEST_CASE("hash", "[transaction]"){
    uint256 target_1(110);
    uint256 target_2(119);
    uint256 sender(11);

    transaction tx;
    tx.outs.emplace_back(target_1, 10000);
    tx.outs.emplace_back(target_2, 10000);

    constexpr int count = 20000;

    tx.set_sender(sender);
    tx.set_count(count);
    tx.set_expired_epoch(255);

    tx.set_nonce(1);

    constexpr uint64_t time_tick = 4855550ll;
    tx.set_create_time(time_tick);

    REQUIRE(tx.get_count() == count);
    REQUIRE(tx.get_create_time() == time_tick);
    std::string v1 = tx.get_hash().get_hex();
    std::string v2 {"1bd5195e8289902d65f7467eb0a20d7f409667b9b712f8121027cf53567bd22d"};
    REQUIRE(v1 == v2);
}
