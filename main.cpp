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
    std::list<uint256> collection {uint256("0x158159AABB"),uint256("0x158159AA11"),uint256("0x158159AA12"),uint256("0x158159AA13")};
    std::vector<uint256> vec {uint256("0x158159AABB"),uint256("0x158159AA11"),uint256("0x158159AA12"),uint256("0x158159AA13")};

    auto root = merkle_tree::get_merkle_root(vec);

    auto proof = merkle_tree::get_merkle_proof(uint256("0x158159AABB"),std::move(collection),0);

    auto result = merkle_tree::merkle_proof_verify(proof,uint256("0x158159AABB"),root);

    std::cout << std::boolalpha << result<< "\n";

}




