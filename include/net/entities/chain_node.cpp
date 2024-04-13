#include "chain_node.hpp"

namespace muse::chain{
    chain_node::chain_node()
    :host_address_(0), net_address(),pubkey_xy(64, '\0'){

    }

    void chain_node::set_host_address(const uint256 &_address) {
        this->host_address_ = _address;
        this->net_address.address_hash = _address;
    }

    void chain_node::set_pubkey_xy(const secp256k1_pubkey &pub) {
        for (int i = 0; i < sizeof(pub.data); ++i) {
            this->pubkey_xy[i] = pub.data[i];
        }
    }

    void chain_node::set_net_address(const node_net &_net) {
        this->net_address = _net;
    }

    auto chain_node::get_pubkey_xy() const -> secp256k1_pubkey {
        secp256k1_pubkey pub;
        for (int i = 0; i < sizeof(pub.data); ++i) {
            pub.data[i] = this->pubkey_xy[i];
        }
        return pub;
    }

    auto chain_node::get_host_address() const -> uint256 {
        return this->host_address_;
    }

    auto chain_node::get_net_address() const -> const node_net & {
        return this->net_address;
    }

    chain_node::chain_node(const chain_node &node)
    :host_address_(node.host_address_),
    net_address(node.net_address),
    pubkey_xy(node.pubkey_xy){

    }

    chain_node::chain_node(chain_node &&node) noexcept
    :host_address_(std::move(node.host_address_)),
    net_address(std::move(node.net_address)),
    pubkey_xy(std::move(node.pubkey_xy)){

    }

    chain_node &chain_node::operator=(const chain_node &node) {
        if (this != &node){
            this->host_address_ = node.host_address_;
            this->net_address = node.net_address;
            this->pubkey_xy = node.pubkey_xy;
        }
        return *this;
    }

    chain_node &chain_node::operator=(chain_node &&node) noexcept {
        if (this != &node){
            this->host_address_ =  std::move(node.host_address_);
            this->net_address = std::move(node.net_address);
            this->pubkey_xy =  std::move(node.pubkey_xy);
        }
        return *this;
    }
}