#define OPENSSL_API_COMPAT 0x10100000L
#define LIBC_NAMESPACE _llvm_libc_muse_

#include <iostream>
#include <vector>

#include "rpc/rpc.hpp"
#include "package.hpp"
#include "application/application.hpp"
#include "util/timebox.hpp"
#include "util/logger.hpp"
#include "util/argparse.hpp"

using namespace muse::chain;
using namespace std;

int add_calculate(int a, int b){
    return a + b;
}

int main(int argc, char const *argv[]){
    //参数解析
    auto args = muse::utils::argparser("muse-blockchain program.");

    args.set_program_name("muse")
            .add_help_option()
            .add_option<std::string>("-p", "--password", "Please enter the password that resolves the node's private key.", "")
            .add_option<std::string>("-s", "--setting", "Please enter the path to the configuration file.", "")
            .parse(argc, argv);

    //密码和配置文件地址
    auto password = args.get_option<std::string>("-p");
    auto path = args.get_option<std::string>("-s");

    //注册解压缩中间件
    MiddlewareChannel::configure<ZlibService>();
    //注册方法存储路由，解析用户请求方法和方法参数 再注册表中寻找方法并调用
    MiddlewareChannel::configure<RouteService>(muse::rpc::Singleton<Registry>(), muse::rpc::Singleton<SynchronousRegistry>());

    //配置线程池
    ThreadPoolSetting::MinThreadCount = 2;  //设置 核心线程数
    ThreadPoolSetting::MaxThreadCount = 4;  //设置 核心线程数
    ThreadPoolSetting::TaskQueueLength = 4096;    //设置 任务缓存队列长度
    ThreadPoolSetting::DynamicThreadVacantMillisecond = 3000ms; //动态线程空闲时间
    //启动线程池
    GetThreadPoolSingleton();
    Reactor reactor(15000, 2, 1500, ReactorRuntimeThread::Asynchronous);

    muse_bind_async("add_calculate", add_calculate);

    try {
        //开始运行
        reactor.start();
    }catch (const ReactorException &ex){
        SPDLOG_ERROR("Main-Reactor start failed!");
    }
    std::cin.get();
    //程序结束
    spdlog::default_logger()->flush(); //刷新日志
    return 0;
}


