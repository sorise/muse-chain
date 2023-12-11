#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include "db/uint256.hpp"


TEST_CASE("base_binary constructor with valid vch", "[base_binary]") {
    std::string vch = "0x0A1B";
    muse::chain::base_binary bb = muse::chain::uint256 (vch);
    // Assert the correctness of bb
}

TEST_CASE("base_binary constructor with invalid vch length", "[base_binary]") {
    std::string vch = "0x";
    REQUIRE_THROWS(muse::chain::uint256(vch));
    std::string vch1 = "0x0";
    REQUIRE_NOTHROW(muse::chain::uint256(vch1));
}

TEST_CASE("base_binary constructor with invalid vch format", "[base_binary]") {
    std::string vch = "0xG0";
    REQUIRE_THROWS(muse::chain::uint256(vch));
}

TEST_CASE("base_binary constructor with invalid vch values", "[base_binary]") {
    std::string vch = "0x0GG0";
    REQUIRE_THROWS(muse::chain::uint256(vch));
}

TEST_CASE("base_binary constructor with vch exceeds width", "[base_binary]") {
    std::string vch = "0x000000000aaa000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";
    REQUIRE_THROWS(muse::chain::uint256(vch));
}
