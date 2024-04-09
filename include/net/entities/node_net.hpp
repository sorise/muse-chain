#ifndef MUSE_CHAIN_NODE_NET_HPP
#define MUSE_CHAIN_NODE_NET_HPP

#include "serializer/IbinarySerializable.h"
#include "db/uint256.hpp"

#include <string>
#include <netinet/in.h>
#include <arpa/inet.h>

namespace muse::chain{
    struct node_net: muse::serializer::IBinarySerializable{
        // 结点 Hash 值
        muse::chain::uint256 address_hash;
        // IP 地址
        std::string ip_address;
        // 端口
        uint16_t port;

        MUSE_IBinarySerializable(address_hash, ip_address, port);

        node_net();

        bool uint32_t_to_ip_address(const uint32_t& _ip);

        virtual ~node_net() = default;
    };
};
#endif //MUSE_CHAIN_NODE_NET_HPP
