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

    node_net::node_net(const node_net &other)
    :address_hash(other.address_hash),
    ip_address(other.ip_address),
    port(other.port) {

    }

    //移动构造函数
    node_net::node_net(node_net &&other) noexcept
    :address_hash(std::move(other.address_hash)),
    ip_address(std::move(other.ip_address)),
    port(other.port) {

    }

    node_net& node_net::operator=(const node_net &other) {
        /* 复制运算符 */
        if (this != &other){
            this->address_hash = other.address_hash;
            this->ip_address = other.ip_address;
            this->port = other.port;
        }
        return *this;
    }

    node_net &node_net::operator=(node_net &&other) noexcept {
        /* 复制运算符 */
        if (this != &other){
            this->address_hash = std::move(other.address_hash);
            this->ip_address = std::move(other.ip_address);
            this->port = other.port;
        }
        return *this;
    }
}