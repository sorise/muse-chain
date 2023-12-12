#include <iostream>
#include <string>
#include <cstring>
#include <encryption/hash_handler.hpp>
#include <err.h>
#include "db/uint256.hpp"
#include "db/block.hpp"
#include "db/transaction.hpp"
#include "db/affair.hpp"
#include "db/merkle_tree.hpp"
#include "encryption/rsa_handler.hpp"
#include "encryption/encryption.hpp"
#include "rpc/rpc.hpp"

using namespace std;
using namespace muse::chain;
using namespace muse::rpc;

int main()
{
    std::cout << merkle_tree::distance_power_of_2_n(15) << "\n";
    std::cout << merkle_tree::distance_power_of_2_n(1) << "\n";
    std::cout << merkle_tree::distance_power_of_2_n(0) << "\n";
    std::cout << merkle_tree::distance_power_of_2_n(16) << "\n";
    std::cout << merkle_tree::distance_power_of_2_n(17) << "\n";
}




