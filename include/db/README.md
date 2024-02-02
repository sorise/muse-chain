### [区块链存储核心](#)
**介绍**：区块链存储核心是基于区块链技术的存储系统，旨在为区块链应用提供可靠、安全、高效的存储服务。

#### 实现目标
1. 区块链账户秘钥的存取，从PEM格式秘钥文件中读取公钥和私钥，并提供账户的创建、导入、删除、修改、查询等功能。
2. 实现账户默克尔树MPT(Merkle Patricia Trie)并使用levelDB作为底层存储引擎，进行存储。
3. 利用levelDB存储区块，提供区块的查询、插入、删除、查询等功能。
4. 实现事务affair默克尔树并使用levelDB作为底层存储引擎，进行存储。
5. 实现交易transaction默克尔树并使用levelDB作为底层存储引擎，进行存储。

#### 代码文件解释

* **uint256.hpp**：一个用于表示256位无符号整数的类，用于区块链存储核心的账户ID和交易ID、hash值等。
* **account.hpp**：一个用于表示账户的类，包含账户的公钥和私钥，并提供账户的创建、导入、删除、修改、查询等功能。
* **block.hpp**：一个用于表示区块的类，包含区块的区块头和交易列表，并提供区块的查询、插入、删除、查询等功能。
* **merkle_tree.hpp**： 提供与操作默克尔树相关的API，包括生成默克尔树根，生成一笔交易的默克尔证明，对证明进行验证。
* **receipt.hpp**: 一个用来表示关于一个区块中一个账户的所有交易回执的类，包含交易的执行结果、交易的执行nonce顺序等，通过检测交易回执来验证交易的执行结果是否合法。


#### 节点生成区块的流程
```cpp
//管理区块数据的外层接口
connector_block cbk(blocks_db.get(),chain_db.get(),chain.get_application_state());

//等待打包的测试交易
transaction append_trans(key1,1,10,muse::utils::get_microseconds_now_UTC_Zone_0(),255, transaction_operator::Establish);
transaction append_trans1(key1,2,600,10,muse::utils::get_microseconds_now_UTC_Zone_0());

append_trans1.push_output(key2,500);
append_trans1.push_output(key3,100);

//加入到交易池中
cbk.transactions_queue.emplace_back(append_trans1);
cbk.transactions_queue.emplace_back(append_trans);

//mpt树的外层接口
connector_account cat(extensions_db.get(),accounts_db.get(),assets_db.get(), chain.get_application_state());
auto root_hash = cat.account_mpt.get_hash();
std::cout << "root hash:" << root_hash.get_hex() << "\n";

bool inject_result = false;
//在区块体注入共识层证明
block blk = chain.package_inject_special_affairs(inject_result, {});
//打包交易、事务到区块体
cat.package_next_block_body(blk, cbk,chain.get_chain_ini());
//打包生成 merkle_root
success = cbk.package_next_block_header(blk, chain.get_chain_ini(), {uint256{"0x157234"}});
if (success){
    try {
        //插入到 mpt树中 获得 merkle_patricia_tire_root hash
        success = cat.package_next_block_MPTR(blk);
        //节点对自己生成的区块进行签名
        chain.signature_block(blk);
        if (success){
            std::cout << "package block success!" << "\n";
            cbk.input_block_cache(blk);
            cbk.insert_block_save_to_disk();
        }
    } catch (...) {
        SPDLOG_INFO("package_next_block_MPTR failed");
    }
}else{
    std::cerr << "package_next_block_header failed!" << "\n";
}
```

遍历区块
```cpp
leveldb::Iterator* it = blocks_db->NewIterator(leveldb::ReadOptions());
for (it->SeekToFirst(); it->Valid(); it->Next()) {
    uint64_t height = *reinterpret_cast<const uint64_t*>(it->key().data());
    muse::serializer::BinaryDeserializeView viewer;
    viewer.reset_bytestream(it->value().data(), it->value().size());
    block blk;
    viewer.output(blk);
    std::cout << "height: " << height <<  "\n";
    std::cout << "block hash: " << blk.header.get_block_hash().get_hex() << "\n";
}
assert(it->status().ok());  // Check for any errors found during the scan
delete it;


leveldb::Iterator* it = assets_db->NewIterator(leveldb::ReadOptions());
for (it->SeekToFirst(); it->Valid(); it->Next()) {
    std::cout << it->key().ToString() <<  "\n";
}
assert(it->status().ok());  // Check for any errors found during the scan
delete it;
```

```cpp
if (blocks_db == nullptr ||chain_db == nullptr || extensions_db == nullptr || accounts_db == nullptr || assets_db == nullptr){
    SPDLOG_ERROR("levelDB db pointer is empty!");
    return -1;
}
//c60f3be16c376e3966775203c7c9111d44c10ef93c8cdcfef5e675eff491aedb
connector_block cbk(blocks_db.get(),chain_db.get(),chain.get_application_state());

connector_account cat(extensions_db.get(),accounts_db.get(),assets_db.get(), chain.get_application_state());
std::cout << cat.get_block_height() << "\n";
std::cout << cat.get_top_block_hash().get_hex() << "\n";
```

框架
```cpp
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
```