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
#include "db/interface_db.hpp"

using namespace muse::chain;
using namespace std;


int main(int argc, char const *argv[]){
    //参数解析
    auto args = muse::utils::argparser("muse-blockchain program.");
    args.set_program_name("muse")
    .add_help_option()
    .add_option<std::string>("-p", "--password", "Please enter the password that resolves the node's private key.", "")
    .add_option<std::string>("-s", "--setting", "Please enter the path to the configuration file.", "")
    .parse(argc, argv);
    //私钥密码 和 配置文件地址
    auto password = args.get_option<std::string>("-p");
    auto path = args.get_option<std::string>("-s");

    muse::chain::application chain;
    //加载配置文件
    bool success = chain.load_setting_file(path);
    if (!success){
        fmt::print("load setting file failed!");
        return 1;
    }
    //注册解压缩中间件
    muse::rpc::Disposition::Server_Configure(2,4,4096,5000ms,chain.get_chain_ini().log_file_path, true);
    //链初始化
    chain.initialize();
    //加载结点公私钥
    chain.load_node_keys(password);
    //插入数据块
    singleton_lazy_heap<interface_db>::get_ptr()->inject_levelDB(chain);

    fmt::print("{}\n", singleton_lazy_heap<interface_db>::get_reference().account_mpt_hash().get_hex());
    return 0;
}


