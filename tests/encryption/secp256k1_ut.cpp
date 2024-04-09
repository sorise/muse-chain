#define CATCH_CONFIG_MAIN

#include "catch.hpp"
#include "encryption/ecc_secp256k1.hpp"
#include "encryption/hash_handler.hpp"
#include <string>


TEST_CASE("64B", "ecc_secp256k1"){
    auto key = muse::chain::ecc_secp256k1::get_key();
    std::string message {"hello world"};
    std::string signature;
    bool success = muse::chain::ecc_secp256k1::signature(key.get(), message.c_str(), message.size(), signature);
    REQUIRE(success);

    std::string pub_64b = muse::chain::ecc_secp256k1::convert_public_key_no_compressed_64B(key.get());
    REQUIRE(pub_64b.size() == 64);

    auto pub = muse::chain::ecc_secp256k1::convert_compressed_64B_to_public_key(pub_64b);
    REQUIRE(pub != nullptr);

    std::cout << muse::chain::hash_handler::convert_to_hex(reinterpret_cast<const unsigned char*>(pub_64b.c_str()), pub_64b.size());

    bool result =  muse::chain::ecc_secp256k1::verify(pub, message.c_str(), message.size(), signature);
    REQUIRE(result);
}