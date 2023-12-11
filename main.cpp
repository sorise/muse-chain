#include <iostream>
#include <string>
#include <cstring>
#include <encryption/hash_handler.hpp>
#include <err.h>
#include "db/uint256.hpp"
#include "db/block.hpp"
#include "db/transaction.hpp"
#include "db/affair.hpp"
#include "encryption/rsa_handler.hpp"
#include "encryption/encryption.hpp"
#include "rpc/rpc.hpp"

using namespace std;
using namespace muse::chain;
using namespace muse::rpc;

int main()
{
//    uint256 owner(11);
//
//    constexpr int nonce = 10;
//    affair_type type = affair_type::MaliciousMessages;
//    vector<affair::Data_DataType> data {1,2,3,4,5,6,7,8,9,10};
//
//    affair attack_error { nonce, owner, type, data};
//    std::cout << attack_error.get_nonce();
//    std::cout << attack_error.get_hash().GetHex();

    uint256 me {"0x0123484Aa"};
    std::cout << me.get_hex() << "\n";
}




