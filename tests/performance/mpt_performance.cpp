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

static void Account_MPT_Search(benchmark::State& state) {
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
    for (auto _ : state){
        cat.account_mpt.search(address);
    }
}
static void Account_MPT_Insert(benchmark::State& state) {
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
    for (auto _ : state){
        cat.account_mpt.insert(address, {});
    }
}

static void Account_MPT_Hash(benchmark::State& state) {
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
    for (auto _ : state){
        muse::chain::uint256 hash = cat.account_mpt.get_hash();
    }
}

static void Account_MPT_Update(benchmark::State& state) {
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
    for (auto _ : state){
        auto user = cat.account_mpt.search_and_mark(address);
        user->set_nonce(10);
    }
}

BENCHMARK(Account_MPT_Search);
BENCHMARK(Account_MPT_Insert);
BENCHMARK(Account_MPT_Hash);
BENCHMARK(Account_MPT_Update);

BENCHMARK_MAIN();