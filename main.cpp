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
    muse::rpc::Disposition::Server_Configure(2,4,4096,5000ms,"/home/remix/log", true);
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


