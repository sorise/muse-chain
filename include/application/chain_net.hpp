#ifndef MUSE_CHAIN_CHAIN_NET_HPP
#define MUSE_CHAIN_CHAIN_NET_HPP

#include "nlohmann/json.hpp"

using namespace nlohmann;

namespace muse::chain {
    struct router{
        std::string ip;
        std::string id;
        uint16_t port{0};
        std::string pubkey_no_compressed_64B_HEX;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(router, ip, id, port, pubkey_no_compressed_64B_HEX);

    struct CA{
        std::string ip;
        std::string id;
        uint16_t port{0};
        std::string pubkey_no_compressed_64B_HEX;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(CA, ip, id, port, pubkey_no_compressed_64B_HEX)

    class chain_net {
    private:
        std::vector<router> routers;
        std::vector<CA> cas;
    public:
        chain_net() = default;

        chain_net(const chain_net& other) ;

        chain_net(chain_net&& other) noexcept;

        chain_net& operator=(const chain_net& other);

        chain_net& operator=(chain_net&& other) noexcept ;

        NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(chain_net, routers, cas)

        [[nodiscard]] auto get_routers() const -> const std::vector<router>&;

        [[nodiscard]] auto get_cas() const -> const std::vector<CA>&;

    };
}


#endif //MUSE_CHAIN_CHAIN_NET_HPP
