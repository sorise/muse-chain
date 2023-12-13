//
// Created by 14270 on 2024-01-05.
//
#define CATCH_CONFIG_MAIN

#include "catch.hpp"
#include "db/receipt.hpp"


using namespace muse::chain;

TEST_CASE("Receipt Check", "[receipt]") {
    SECTION("CheckNonce") {
        receipt r;
        r.nonce_start = 1;
        r.nonce = {2, 3, 4};
        r.balances = {{1, {100,100}}, {2, {200,200}}, {3, {400,300}}};
        REQUIRE(r.check_nonce() == true);
        REQUIRE(r.check() == true);
    }

    SECTION("CheckInvalidNonce") {
        receipt r;
        r.nonce_start = 1;
        r.nonce = {3, 2, 1};
        r.balances = {{1, {100,100}}, {2, {200,200}}, {3, {400,300}}};
        REQUIRE(r.check_nonce() == false);
        REQUIRE(r.check() == false);
    }

    SECTION("CheckEmptyNonce") {
        receipt r;
        r.nonce_start = 0;
        r.nonce = {};
        r.balances = {{1, {100,100}}, {2, {200,200}}, {3, {400,300}}};;
        REQUIRE(r.check_nonce() == true);
        REQUIRE(r.check() == true);
    }

    SECTION("CheckSingleNonce") {
        receipt r;
        r.nonce_start = 0;
        r.nonce = {1};
        r.balances = r.balances = {{1, {100,100}}, {2, {200,200}}, {3, {400,300}}};
        REQUIRE(r.check_nonce() == true);
        REQUIRE(r.check() == true);
    }

    SECTION("CheckNegativeBalance") {
        receipt r;
        r.nonce_start = 0;
        r.nonce = {1, 2, 3};
        r.balances = {{1, {100,100}}, {2, {200,200}}, {3, {400,700}}};
        REQUIRE(r.check_nonce() == true);
        REQUIRE(r.check() == false);
    }
}