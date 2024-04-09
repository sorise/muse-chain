#include "node_net.hpp"

namespace muse::chain{
    bool node_net::uint32_t_to_ip_address(const uint32_t& _ip){
        in_addr address = {_ip};
        muse::chain::node_net node;
        //IP地址
        this->ip_address = std::string(inet_ntoa(address));
        return true;
    }

    node_net::node_net():IBinarySerializable(),address_hash(0),ip_address(),port(0){

    }
}