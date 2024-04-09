#ifndef MUSE_CHAIN_CHAIN_NODE_HPP
#define MUSE_CHAIN_CHAIN_NODE_HPP

#include "serializer/IbinarySerializable.h"
#include "node_net.hpp"
#include <secp256k1.h>

namespace muse::chain{
class chain_node: public muse::serializer::IBinarySerializable{
    private:
        uint256 host_address_;               //公钥hash
        node_net net_address;                //网络地址
        secp256k1_pubkey pubkey_xy;          //公钥
    public:
        chain_node();

        void set_host_address(const uint256& _address);
    };

}

#endif //MUSE_CHAIN_CHAIN_NODE_HPP
