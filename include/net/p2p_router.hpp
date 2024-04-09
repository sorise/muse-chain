#ifndef MUSE_CHAIN_P2P_ROUTER_HPP
#define MUSE_CHAIN_P2P_ROUTER_HPP
#include <map>
#include "db/uint256.hpp"
#include "entities/chain_node.hpp"

namespace muse::chain{
    /* 组网 */
    class p2p_router {
    private:
        uint256 host_address_;
        node_net net_;
        std::vector<node_net> router;

        std::map<uint256, node_net> nodes;
    public:

    };
}
#endif //MUSE_CHAIN_P2P_ROUTER_HPP
