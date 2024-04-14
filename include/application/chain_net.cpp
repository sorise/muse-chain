//
// Created by remix on 24-4-13.
//

#include "chain_net.hpp"

namespace muse::chain{
    chain_net::chain_net(const chain_net &other)
    :cas(other.cas), routers(other.routers),port(0){

    }

    chain_net::chain_net(chain_net &&other) noexcept
    :cas(std::move(other.cas)), routers(std::move(other.routers)),port(0){

    }

    chain_net &chain_net::operator=(chain_net &&other) noexcept {
        if (this != &other){
            cas = std::move(other.cas);
            routers = std::move(other.routers);
            port = other.port;
        }
        return *this;
    }

    chain_net &chain_net::operator=(const chain_net &other) {
        if (this != &other){
            cas = other.cas;
            routers = other.routers;
            port = other.port;
        }
        return *this;
    }

    auto chain_net::get_routers() const -> const std::vector<router> & {
        return this->routers;
    }

    auto chain_net::get_cas() const -> const std::vector<CA> & {
        return this->cas;
    }

    chain_net::chain_net():cas(), routers(),port() {

    };
}