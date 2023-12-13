#define OPENSSL_API_COMPAT 0x10100000L
#define LIBC_NAMESPACE _llvm_libc_muse_

#include <iostream>
#include <vector>

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

    //测试地址
    uint256 key1 {"0x1c417a04f8be4f13d9fb18647e39030ff4a7efbc3bb6340c38582d0de45cc25d"};
    uint256 key2 {"0x1c417a04f8be4f13d9fb18647e39030ff4a7efbc3bb6340c38582d0de45cc111"};
    uint256 key3 {"0x1c417a04f8be4f13d9fb18647e39030ff4a7efbc3bb6340c38582d0de4154111"};

    muse::chain::application chain;
    bool success = chain.load_setting_file(path);
    if (success){
        try{
            muse::InitSystemLogger(chain.get_chain_ini().log_file_path);
            chain.initialize();
            chain.load_node_keys(password);

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

            account* address = cat.find_account(key1);
            std::cout << address->get_nonce() << std::endl;
            std::cout << address->get_balance(10) << std::endl;


        }catch(chain_db_exception &db_ex) {
            std::cerr << db_ex.what() << "\n";
        }catch (exception &ex) {
            std::cerr << ex.what() << "\n";
        }catch (...) {
            std::cerr << "Non capture error"<< "\n";
        }
    }
}



//启动的时候需要配置文件路径
auto _main() -> int {
    uint256 key1 {"0x1c417a04f8be4f13d9fb18647e39030ff4a7efbc3bb6340c38582d0de45cc25d"};
    uint256 key2 {"0x1c417a04f8be4f13d9fb18647e39030ff4a7efbc3bb6340c38582d0de45cc111"};
    uint256 key3 {"0x1c417a04f8be4f13d9fb18647e39030ff4a7efbc3bb6340c38582d0de4154111"};
    uint256 key4 {"0x2c417a04f8be4f13d9fb18647e39030ff4a7efbc3bb6340c38582d0de4154111"};
    uint256 key5 {"0x3c417a04f8be4f13d9fb18647e39030ff4a7efbc3bb6340c38582d0de4154111"};
    uint256 key6 {"0x3c417a05f81e4f13d9fb18647e39030ff4a7efbc3bb6340c11582d0de4154111"};
    uint256 key7 {"0x4c417a05f81e4f13d9fb18647e39030ff4a7efbc3bb6340c11582d0de4154111"};
    uint256 key8 {"0x5c417a05f81e4f13d9fb18647e39030ff4a7efbc3bb6340c11582d0de4154111"};
    uint256 key9 {"0x5c417a05f81e4f13d9fb18647e39030ff4a7efbc3bb6340c115123456789a4bc"};
    uint256 ke10 {"0x5c417a05f81e4f13d9fb18647e39030ff4a7efbc3bb6340c11582d0de4154992"};
    uint256 ke11 {"0x51417a05f81e4f13d9fb18647e39030ff4a7efbc3bb6340c11582d0de4154992"};
    uint256 ke12 {"0x52417a05f81e4f13d9fb18647e39030ff4a7efbc3bb6340c11582d0de4154992"};
    uint256 ke13 {"0x52417a05f81e4f13d9fb18647e39030ff4a7efbc3bb6340c11582d0df4112233"};

    account value1(key1, 0);
    value1.add_balance(1, 1000);
    account value2(key2, 0);
    value2.add_balance(1, 10000);
    account value3(key3, 0);
    value3.add_balance(1, 104000);
    account value4(key4, 0);
    value4.add_balance(1, 105000);
    account value5(key5, 0);
    value5.add_balance(1, 105000);
    account value6(key6, 0);
    value6.add_balance(1, 105000000);
    account value7(key7, 0);
    value7.add_balance(1, 17500);
    account value8(key8, 0);
    value8.add_balance(1, 10500);
    account value9(key9, 0);
    value9.add_balance(1, 1755);
    account value10(ke10, 0);
    value10.add_balance(1, 115000);
    account value11(ke11, 0);
    value11.add_balance(1, 175000);
    account value12(ke12, 0);
    value12.add_balance(1, 115880);
    account value13(ke13, 0);
    value13.add_balance(1, 55880);

    std::list<uint256> addresses = {key1,key2,key3,key4,key5,key6,key7,key8,key9,ke10,ke11,ke12,ke13};

    muse::chain::application chain;
    //加载配置文件
    if(chain.load_setting_file("F:\\block_setting\\chain.ini")){
        //进行初始化 or 初始化检测
        chain.initialize();

        leveldb::DB *extension_db = muse::chain::application::get_extensions_db(chain.get_chain_ini().extensions_db_path);
        leveldb::DB *account_db   = muse::chain::application::get_accounts_db(chain.get_chain_ini().account_db_path);
        leveldb::DB *assets_db    = muse::chain::application::get_assets_db(chain.get_chain_ini().assets_db_path);

        if (extension_db == nullptr || account_db == nullptr || assets_db == nullptr){
            return -1;
        }

        connector_account connector(extension_db,account_db, assets_db, chain.get_application_state());

        connector.account_mpt.insert(key1, value1);
        connector.account_mpt.insert(key2, value2);
        connector.account_mpt.insert(key3, value3);
        connector.account_mpt.insert(key4, value4);

        std::cout << "root hash: "  << connector.account_mpt.get_hash().get_hex() << std::endl;

//        leveldb_account_commit accountCommit;
//        accountCommit.commits = std::list{value1,value2,value3,value4,value5,value6,value7,value8,value9,value10,value11,value12,value13};
//        connector.commit_account_to_leveldb(accountCommit);
//
//        leveldb::WriteBatch batch;
//        for(auto& commit: accountCommit.commits){
//            muse::serializer::BinarySerializer serializer;
//            serializer.input(commit);
//            leveldb::Slice value_slice(serializer.getBinaryStream(), serializer.byteCount());
//            leveldb::Slice key_slice(reinterpret_cast<const char*>(commit.get_address().get_data_const()), commit.get_address().Width);
//            batch.Put(key_slice, value_slice);
//        }
//        leveldb::Status status = account_db->Write(leveldb::WriteOptions(), &batch);
//        return status.ok();

//
//        leveldb::Iterator* it = account_db->NewIterator(leveldb::ReadOptions());
//        for (it->SeekToFirst(); it->Valid(); it->Next()) {
//            uint256 t_address(it->key().ToString().c_str(),32);
//            std::cout << t_address.get_hex() << std::endl;
//        }
//        assert(it->status().ok());  // Check for any errors found during the scan
//        delete it;

//        std::cout << "root hash: "  << connector.account_mpt.get_hash().get_hex() << std::endl;
        //0x1c417a04f8be4f13d9fb18647e39030ff4a7efbc3bb6340c38582d0de45cc25d
        //connector.account_mpt.erase(key1);
        //ef06e8b7c0b7a4de7b91a93a71f6242c47fa375f4045e97515fde610d3a72f44

//        for (auto& addr: addresses) {
//            account* act = connector.account_mpt.search_and_mark(addr);
//        }
//        std::cout << "root hash: "  << connector.account_mpt.get_hash().get_hex() << std::endl;

//
//        transaction append_trans(ke12,1,600,1,muse::utils::get_microseconds_now_UTC_Zone_0());
//        append_trans.push_output(key1,500);
//        append_trans.push_output(key2,100);
//        transaction append_trans_2(ke12,2,600,1,muse::utils::get_microseconds_now_UTC_Zone_0());
//        append_trans_2.push_output(key3,500);
//        append_trans_2.push_output(key4,100);
//        transaction append_trans_3(ke12,3,10,muse::utils::get_microseconds_now_UTC_Zone_0(),255, transaction_operator::Establish);
//
//        block blk;
//        blk.body.transactions.push_back(append_trans);
//        blk.body.transactions.push_back(append_trans_2);
//        blk.body.transactions.push_back(append_trans_3);
//        auto result = connector.insert_block(blk );
//        std::cout << std::boolalpha << "insert result:" <<  result << std::endl;
//
//        std::cout << "root hash: "  << connector.account_mpt.get_hash().get_hex() << std::endl;
//        std::cout << "blance:"  << connector.account_mpt.search(ke12)->get_balance(1) << std::endl;
//        std::cout << "blance:"  << connector.account_mpt.search(ke12)->get_balance(10) << std::endl;
//        std::cout << "blance:"  << connector.account_mpt.search(key1)->get_balance(1) << std::endl;
//        std::cout << "blance:"  << connector.account_mpt.search(key2)->get_balance(1) << std::endl;
//        std::cout << "blance:"  << connector.account_mpt.search(key3)->get_balance(1) << std::endl;
//        std::cout << "blance:"  << connector.account_mpt.search(key4)->get_balance(1) << std::endl;
//        for (auto& addr: addresses) {
//            std::cout << addr.get_hex() << " find: " << std::boolalpha << connector.account_mpt.find(addr) << std::endl;
//        }

//        for (auto& addr: addresses) {
//            account* _account = connector.account_mpt.search(addr);
//            if (_account != nullptr){
//                std::cout << addr.get_hex() << " " << " nonce: " << _account->get_nonce() << " \n";
//                std::cout << "asset 1 count: " << _account->get_balance(1) << " \n";
//            }
//        }
//
//        connector.rollback(1);
//        std::cout << "root hash: "  << connector.account_mpt.get_hash().get_hex() << std::endl;
        //115880
//        std::cout << "blance:"  << connector.account_mpt.search(ke12)->get_balance(1) << std::endl;
//        std::cout << "blance:"  << connector.account_mpt.search(ke12)->get_balance(10) << std::endl;
//        std::cout << "blance:"  << connector.account_mpt.search(key1)->get_balance(1) << std::endl;
//        std::cout << "blance:"  << connector.account_mpt.search(key2)->get_balance(1) << std::endl;
//        std::cout << "blance:"  << connector.account_mpt.search(key3)->get_balance(1) << std::endl;
//        std::cout << "blance:"  << connector.account_mpt.search(key4)->get_balance(1) << std::endl;
        //connector.commit_extensions_node_to_leveldb(connector.account_mpt.get_change_store());
//        if (connector.asset_bloom_filter.test(10)) {
//            std::cout << "[" << to_string(10) << "]" << std::endl;
//        }
//
//        auto store = connector.asset_bloom_filter.to_store();
//
//        bloom_filter filter(store);
//
//        auto is_equal = (filter == connector.asset_bloom_filter);
//
//        std::cout << "is equal: " << is_equal << std::endl;
//
//        for (auto& addr: addresses) {
//            account* act = connector.account_mpt.search(addr);
//            std::cout <<"account: " <<addr.get_hex() << "\n" << "hash:" << act->get_hash().get_hex()  << std::endl;
//        }


        delete extension_db;
        delete account_db;
        delete assets_db;
    }else{
        return -1;
    }

    return 0;
}
