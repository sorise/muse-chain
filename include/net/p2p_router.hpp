#ifndef MUSE_CHAIN_P2P_ROUTER_HPP
#define MUSE_CHAIN_P2P_ROUTER_HPP
#include <map>
#include "db/uint256.hpp"
#include "entities/chain_node.hpp"
#include "application/chain_net.hpp"
#include "rpc/rpc.hpp"
#include "route_path.hpp"

namespace muse::chain{
    /* 组网 */
    class P2P_router {
    private:
        uint256 host_address_;

        node_net net_;

        std::vector<std::pair<router, bool>> routers; /* 路由器结点 */

        std::vector<std::pair<CA, bool>> CAS; /* 路由器结点 */

        std::map<uint256, chain_node> nodes;
    public:
        //启动任务
        auto set_routers_and_ca(const chain_net &net) ->bool;

        auto register_node() ->bool;

        /* 尝试和组网结点取得联系，如果全部失败，则返回false，需要使用阻塞RPC请求 */
        auto try_link_router() ->bool;

    };
}
#endif //MUSE_CHAIN_P2P_ROUTER_HPP
