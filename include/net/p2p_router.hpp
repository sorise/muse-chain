#ifndef MUSE_CHAIN_P2P_ROUTER_HPP
#define MUSE_CHAIN_P2P_ROUTER_HPP
#include <map>
#include "db/uint256.hpp"
#include "entities/chain_node.hpp"

namespace muse::chain{
    /* 组网 */
    class P2P_router {
    private:
        uint256 host_address_;

        node_net net_;

        std::vector<std::pair<node_net, bool>> router; /* 路由器结点 */

        std::map<uint256, node_net> nodes;
    public:
        //启动任务

    };
}
#endif //MUSE_CHAIN_P2P_ROUTER_HPP
