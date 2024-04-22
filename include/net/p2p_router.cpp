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

    auto P2P_router::register_node() -> bool {
        //没有router
        if (!this->routers.empty()){
            return false;
        }
        //判断是否所有的 router 都还可以使用
        for (auto &[router, is_available]: this->routers) {
            if (!is_available){
                return false;
            }
        }
        return true;
    }

    auto P2P_router::try_link_router() -> bool{
        for (auto & router : this->routers) {
            //传入 服务器地址和服务端端口号、一个C++ 17 标准内存池
            try {
                Client remix(router.first.ip.c_str(), router.first.port, MemoryPoolSingleton());
                Outcome<int> resp = remix.call<int>(route_path::RouterHeartBeatTest);
                //调用 无参无返回值方法
                if (resp.isOK()){
                    router.second = true;
                    SPDLOG_INFO("RPC OK Route: {}, [IP/Port {}:{}]", route_path::RouterHeartBeatTest, router.first.ip, router.first.port);
                }else{
                    router.second = false;
                    //网络协议没有错
                    if (resp.protocolReason == FailureReason::OK){
                        //错误原因是RPC错误 RpcFailureReason
                        SPDLOG_ERROR("RPC ERR Route {}, RpcFailureReason: {}, {}:{}", route_path::RouterHeartBeatTest,resp.response.getReason(), router.first.ip, router.first.port);
                    }else{
                        //错误原因是网络通信过程中的错误
                        SPDLOG_ERROR("RPC ERR Route {}, FailureReason: {}", route_path::RouterHeartBeatTest,(short)resp.protocolReason, router.first.ip, router.first.port);
                    }
                }
            }catch (ClientException &ce) {
                //ip错误，不可达
                router.second = false;
            }
        }
        bool available = false;
        //判断是否所有的 router 都还可以使用
        for (auto &[router, is_available]: this->routers) {
            available = available | is_available;
        }
        return available;
    }


}