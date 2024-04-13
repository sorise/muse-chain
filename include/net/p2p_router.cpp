#include "p2p_router.hpp"

namespace muse::chain{

    auto P2P_router::set_routers_and_ca(const chain_net &net) -> bool {
        if (net.get_routers().empty()){
            return false;
        }
        for(auto &router:net.get_routers()){
            this->routers.emplace_back(router, true);
        }
        for (auto &ca: net.get_cas()) {
            this->CAS.emplace_back(ca, true);
        }
        return true;
    }


}