#include <benchmark/benchmark.h>
#include <array>
#include "serializer/binarySerializer.h"
#include "db/uint256.hpp"
#include "db/transaction.hpp"
#include "db/affair.hpp"
#include "db/block.hpp"
#include "db/connector_account.hpp"
#include "db/connector_block.hpp"
#include "application/application.hpp"

static void PackAge(benchmark::State& state) {
    muse::chain::application chain;
    bool success = chain.load_setting_file("/home/remix/chain/chain.ini");
    if (!success) return;
    chain.initialize();
    chain.load_node_keys("123456");

    //levelDB 指针
    std::unique_ptr<leveldb::DB> blocks_db(muse::chain::application::get_blocks_db(chain.get_chain_ini().block_db_path));
    std::unique_ptr<leveldb::DB> chain_db(muse::chain::application::get_chain_db(chain.get_chain_ini().chain_state_db_path));
    std::unique_ptr<leveldb::DB> extensions_db (muse::chain::application::get_extensions_db(chain.get_chain_ini().extensions_db_path));
    std::unique_ptr<leveldb::DB> accounts_db(muse::chain::application::get_accounts_db(chain.get_chain_ini().account_db_path));
    std::unique_ptr<leveldb::DB> assets_db (muse::chain::application::get_assets_db(chain.get_chain_ini().assets_db_path));

    if (blocks_db == nullptr ||chain_db == nullptr || extensions_db == nullptr || accounts_db == nullptr || assets_db == nullptr){
        SPDLOG_ERROR("levelDB db pointer is empty!");
        return;
    }
    //管理区块数据的外层接口
    muse::chain::connector_block cbk(blocks_db.get(),chain_db.get(),chain.get_application_state());
    //mpt树的外层接口
    muse::chain::connector_account cat(extensions_db.get(),accounts_db.get(),assets_db.get(), chain.get_application_state());
    muse::chain::uint256 address{"0x09e306220f94e62bfb2b3886af6ae0a6eb1c0598f07a23635a7be61be0c5081c"};
    //等待打包的测试交易
    muse::chain::uint256 sender_("0xe6aa0c897f1a2e752ed7e4d075e8d536fa76e71d92d141ae9b35bf47568faeba");
    for (int i = 0; i < 2000; ++i) {
        muse::chain::transaction tras(sender_,i+2+1000,20,100,muse::utils::get_microseconds_now_UTC_Zone_0());
        auto key = muse::chain::ecc_secp256k1::get_key();
        muse::chain::uint256 address;
        muse::chain::ecc_secp256k1::get_public_key_hash_no_compressed(key.get(),address);
        tras.outs.emplace_back(address,20);
        //加入到交易池中
        cbk.transactions_queue.emplace_back(tras);
    }
    for (auto _ : state){
        bool inject_result = false;
        //在区块体注入共识层证明
        muse::chain::block blk = chain.package_inject_special_affairs(inject_result, {});
        //打包交易、事务到区块体
        cat.package_next_block_body(blk, cbk,chain.get_chain_ini());
        //打包生成 merkle_root
        cbk.package_next_block_header(blk, chain.get_chain_ini(), {});
    }
}

static void CheckBlock(benchmark::State& state) {
    muse::chain::application chain;
    bool success = chain.load_setting_file("/home/remix/chain/chain.ini");
    if (!success) return;
    chain.initialize();
    chain.load_node_keys("123456");

    //levelDB 指针
    std::unique_ptr<leveldb::DB> blocks_db(muse::chain::application::get_blocks_db(chain.get_chain_ini().block_db_path));
    std::unique_ptr<leveldb::DB> chain_db(muse::chain::application::get_chain_db(chain.get_chain_ini().chain_state_db_path));
    std::unique_ptr<leveldb::DB> extensions_db (muse::chain::application::get_extensions_db(chain.get_chain_ini().extensions_db_path));
    std::unique_ptr<leveldb::DB> accounts_db(muse::chain::application::get_accounts_db(chain.get_chain_ini().account_db_path));
    std::unique_ptr<leveldb::DB> assets_db (muse::chain::application::get_assets_db(chain.get_chain_ini().assets_db_path));

    if (blocks_db == nullptr ||chain_db == nullptr || extensions_db == nullptr || accounts_db == nullptr || assets_db == nullptr){
        SPDLOG_ERROR("levelDB db pointer is empty!");
        return;
    }
    //管理区块数据的外层接口
    muse::chain::connector_block cbk(blocks_db.get(),chain_db.get(),chain.get_application_state());
    //mpt树的外层接口
    muse::chain::connector_account cat(extensions_db.get(),accounts_db.get(),assets_db.get(), chain.get_application_state());
    muse::chain::uint256 address{"0x09e306220f94e62bfb2b3886af6ae0a6eb1c0598f07a23635a7be61be0c5081c"};
    //等待打包的测试交易
    muse::chain::uint256 sender_("0xe6aa0c897f1a2e752ed7e4d075e8d536fa76e71d92d141ae9b35bf47568faeba");
    for (int i = 0; i < 2000; ++i) {
        muse::chain::transaction tras(sender_,i+2+1000,20,100,muse::utils::get_microseconds_now_UTC_Zone_0());
        auto key = muse::chain::ecc_secp256k1::get_key();
        muse::chain::uint256 address;
        muse::chain::ecc_secp256k1::get_public_key_hash_no_compressed(key.get(),address);
        tras.outs.emplace_back(address,20);
        //加入到交易池中
        cbk.transactions_queue.emplace_back(tras);
    }
    bool inject_result = false;
    //在区块体注入共识层证明
    muse::chain::block blk = chain.package_inject_special_affairs(inject_result, {});
    //打包交易、事务到区块体
    cat.package_next_block_body(blk, cbk,chain.get_chain_ini());
    //打包生成 merkle_root
    cbk.package_next_block_header(blk, chain.get_chain_ini(), {});
    for (auto _ : state){
        cat.check_block_body(blk);
    }
}


BENCHMARK(PackAge);
BENCHMARK(CheckBlock);


BENCHMARK_MAIN();