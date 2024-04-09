#include "chain_node.hpp"

namespace muse::chain{
    chain_node::chain_node()
    :host_address_(0), net_address(){

    }

    void chain_node::set_host_address(const uint256 &_address) {
        this->host_address_ = _address;
        this->net_address.address_hash = _address;
    }
}