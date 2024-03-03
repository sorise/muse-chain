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

    //密码和配置文件地址
    auto password = args.get_option<std::string>("-p");
    auto path = args.get_option<std::string>("-s");

    //注册解压缩中间件
    //muse::rpc::Disposition::Server_Configure(2,4,4096,5000ms,"/home/remix/log", true);

    muse::chain::application chain;
    bool success = chain.load_setting_file(path);
    if (success){
        try{
            muse::InitSystemLogger(chain.get_chain_ini().log_file_path);
            chain.initialize();
            chain.load_node_keys(password);
            //levelDB 指针
            std::unique_ptr<leveldb::DB> blocks_db(muse::chain::application::get_blocks_db(chain.get_chain_ini().block_db_path));
            std::unique_ptr<leveldb::DB> chain_db(muse::chain::application::get_chain_db(chain.get_chain_ini().chain_state_db_path));
            std::unique_ptr<leveldb::DB> extensions_db (muse::chain::application::get_extensions_db(chain.get_chain_ini().extensions_db_path));
            std::unique_ptr<leveldb::DB> accounts_db(muse::chain::application::get_accounts_db(chain.get_chain_ini().account_db_path));
            std::unique_ptr<leveldb::DB> assets_db (muse::chain::application::get_assets_db(chain.get_chain_ini().assets_db_path));

            if (blocks_db == nullptr ||chain_db == nullptr || extensions_db == nullptr || accounts_db == nullptr || assets_db == nullptr){
                SPDLOG_ERROR("levelDB db pointer is empty!");
                return -1;
            }
            //管理区块数据的外层接口
            connector_block cbk(blocks_db.get(),chain_db.get(),chain.get_application_state());
            //mpt树的外层接口
            connector_account cat(extensions_db.get(),accounts_db.get(),assets_db.get(), chain.get_application_state());

            //等待打包的测试交易
            uint256 sender_("0xe6aa0c897f1a2e752ed7e4d075e8d536fa76e71d92d141ae9b35bf47568faeba");
            for (int i = 0; i < 600; ++i) {
                transaction tras(sender_,i+2+500,20,100,muse::utils::get_microseconds_now_UTC_Zone_0());
                auto key =  ecc_secp256k1::get_key();
                muse::chain::uint256 address;
                ecc_secp256k1::get_public_key_hash_no_compressed(key.get(),address);
                //std::cout << "0x" << address.get_hex() << "\n";
                tras.outs.emplace_back(address,20);
                //加入到交易池中
                cbk.transactions_queue.emplace_back(tras);
            }

            //mpt树的外层接口
            auto root_hash = cat.account_mpt.get_hash();
            std::cout << "root hash:" << root_hash.get_hex() << "\n";

            bool inject_result = false;
            //在区块体注入共识层证明
            block blk = chain.package_inject_special_affairs(inject_result, {});
            //打包交易、事务到区块体
            cat.package_next_block_body(blk, cbk,chain.get_chain_ini());
            //打包生成 merkle_root
            success = cbk.package_next_block_header(blk, chain.get_chain_ini(), {});

        }catch(chain_db_exception &db_ex) {
            std::cerr << db_ex.what() << "\n";
        }catch (exception &ex) {
            std::cerr << ex.what() << "\n";
        }catch (...) {
            std::cerr << "Non capture error"<< "\n";
        }
    }

    return 0;
}


