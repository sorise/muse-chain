### 启动程序
```cpp
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
    try{
        //初始化日志,启动Server_Configure 就不需要此行
        //muse::InitSystemLogger(chain.get_chain_ini().log_file_path);
        //链初始化
        chain.initialize();
        //记载结点公私钥
        chain.load_node_keys(password);
        //区块 levelDB 数据库指针
        std::unique_ptr<leveldb::DB> blocks_db(muse::chain::application::get_blocks_db(chain.get_chain_ini().block_db_path));
        //链状态 levelDB 数据库指针
        std::unique_ptr<leveldb::DB> chain_db(muse::chain::application::get_chain_db(chain.get_chain_ini().chain_state_db_path));
        //MPT 分支节点 levelDB 数据库指针
        std::unique_ptr<leveldb::DB> extensions_db (muse::chain::application::get_extensions_db(chain.get_chain_ini().extensions_db_path));
        //MPT 叶子节点 levelDB 数据库指针
        std::unique_ptr<leveldb::DB> accounts_db(muse::chain::application::get_accounts_db(chain.get_chain_ini().account_db_path));
        //资产 levelDB 数据库指针
        std::unique_ptr<leveldb::DB> assets_db (muse::chain::application::get_assets_db(chain.get_chain_ini().assets_db_path));
        //判断 是否存在空节点
        if (blocks_db == nullptr ||chain_db == nullptr || extensions_db == nullptr || accounts_db == nullptr || assets_db == nullptr){
            SPDLOG_ERROR("levelDB db pointer is empty!");
            return -1;
        }
        //管理区块数据的外层接口
        connector_block cbk(blocks_db.get(),chain_db.get(),chain.get_application_state());
        //mpt树的外层接口
        connector_account cat(extensions_db.get(),accounts_db.get(),assets_db.get(), chain.get_application_state());
        //mpt树的外层接口


        auto root_hash = cat.account_mpt.get_hash();
        std::cout << "root hash:" << root_hash.get_hex() << "\n";

    }catch(chain_db_exception &db_ex) {
        std::cerr << db_ex.what() << "\n";
    }catch (exception &ex) {
        std::cerr << ex.what() << "\n";
    }catch (...) {
        std::cerr << "Non capture error"<< "\n";
    }
    return 0;
}
```