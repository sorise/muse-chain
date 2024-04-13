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
        std::string pubkey_xy;               //公钥
    public:

        MUSE_IBinarySerializable(host_address_, net_address, pubkey_xy)

        chain_node();

        chain_node(const chain_node& node);

        chain_node(chain_node&& node) noexcept;

        chain_node& operator=(const chain_node& node);

        chain_node& operator=(chain_node&& node) noexcept;

        void set_host_address(const uint256& _address);

        void set_net_address(const node_net& _net);

        void set_pubkey_xy(const secp256k1_pubkey& pub);

        [[nodiscard]] auto get_pubkey_xy() const -> secp256k1_pubkey;

        [[nodiscard]] auto get_host_address() const -> uint256;

        [[nodiscard]] auto get_net_address() const -> const node_net&;
    };
}

#endif //MUSE_CHAIN_CHAIN_NODE_HPP
