#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include <vector>
#include "db/affair.hpp"

using namespace muse::chain;

TEST_CASE("TestConstructor") {
    // Prepare test data
    uint64_t nonce = 123;
    uint256 owner {12};
    affair_type type = affair_type::MaliciousMessages;
    std::vector<affair::Data_DataType> data = {1, 2, 3};

    // Create an instance of the class
    affair testAffair(nonce, owner, type, data);

    // Verify the instance values
    REQUIRE(testAffair.get_nonce() == nonce);
    REQUIRE(testAffair.get_owner() == owner);
    REQUIRE(testAffair.get_type() == type);
    REQUIRE(testAffair.get_data() == data);
}

TEST_CASE("TestSetters") {
    // Create an instance of the class
    affair testAffair;

    // Set the values
    uint64_t nonce = 123;
    uint256 owner {12};
    affair_type type = affair_type::MaliciousMessages;
    std::vector<affair::Data_DataType> data = {1, 2, 3};
    testAffair.set_nonce(nonce);
    testAffair.set_owner(owner);
    testAffair.set_type(type);
    testAffair.get_data() = data;

    // Verify the instance values
    REQUIRE(testAffair.get_nonce() == nonce);
    REQUIRE(testAffair.get_owner() == owner);
    REQUIRE(testAffair.get_type() == type);
    REQUIRE(testAffair.get_data() == data);
}

TEST_CASE("TestGetters") {
    // Create an instance of the class with test data
    uint64_t nonce = 123;
    uint256 owner {12};
    affair_type type = affair_type::MaliciousMessages;
    std::vector<affair::Data_DataType> data = {1, 2, 3};
    affair testAffair(nonce, owner, type, data);

    // Use getters to get values and verify them
    REQUIRE(testAffair.get_nonce() == nonce);
    REQUIRE(testAffair.get_owner() == owner);
    REQUIRE(testAffair.get_type() == type);
    REQUIRE(testAffair.get_data() == data);
}

TEST_CASE("TestGetHash") {
    // Create an instance of the class with test data
    uint256 owner(11);
    constexpr int nonce = 10;
    affair_type type = affair_type::MaliciousMessages;
    std::vector<affair::Data_DataType> data {1,2,3,4,5,6,7,8,9,10};
    affair testAffair(nonce, owner, type, data);

    // Call get_hash() and verify the result
    // TODO: Implement the verification part
    std::string result {"101adb6d19f624668d3536cb6ebfffbad4a012d9a45900839e3683af1383ba63a8"};
    REQUIRE_NOTHROW(testAffair.get_hash().get_hex() == result);
}

TEST_CASE("TestGetData") {
    // Create an instance of the class with test data
    uint64_t nonce = 123;
    uint256 owner {12};
    affair_type type = affair_type::MaliciousMessages;
    std::vector<affair::Data_DataType> data = {1, 2, 3};
    affair testAffair(nonce, owner, type, data);

    // Call get_data() and verify the result
    // TODO: Implement the verification part
    REQUIRE(testAffair.get_data().front() == data.front());
}
