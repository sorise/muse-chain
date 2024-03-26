//
// Created by 14270 on 2024-01-10.
//

#include "application.hpp"

namespace muse::chain{

    auto application::load_setting_file(const std::string& path) -> bool {
        if (!std::filesystem::exists(path)){
            std::cerr << "setting file path is not exist! \n" ;
            return false;
        }
        inicpp::IniManager loader(path);
        // Check if the key exists.
        if (!loader["chain"].isKeyExist("initial_keys"))
        {
            std::cerr << "setting file not exist chain.initial_keys  \n" ;
            return false;
        }
        //Check if the key [transaction_maximum_number_in_block] exists.
        if (!loader["chain"].isKeyExist("transaction_maximum_number_in_block"))
        {
            std::cerr << "setting file not exist chain.transaction_maximum_number_in_block  \n" ;
            return false;
        }
        //Check if the key [transaction_minimum_number_in_block] exists.
        if (!loader["chain"].isKeyExist("transaction_minimum_number_in_block"))
        {
            std::cerr << "setting file not exist chain.transaction_minimum_number_in_block  \n" ;
            return false;
        }
        //Check if the key [affair_maximum_number_in_block] exists.
        if (!loader["chain"].isKeyExist("affair_maximum_number_in_block"))
        {
            std::cerr << "setting file not exist chain.affair_maximum_number_in_block  \n" ;
            return false;
        }
        //Check if the key [affair_minimum_number_in_block] exists.
        if (!loader["chain"].isKeyExist("affair_minimum_number_in_block"))
        {
            std::cerr << "setting file not exist chain.affair_minimum_number_in_block  \n" ;
            return false;
        }
        if (!loader["chain"].isKeyExist("log_file_path"))
        {
            std::cerr << "setting file not exist chain.log_file_path  \n" ;
            return false;
        }
        if (!loader["leveldb"].isKeyExist("chain_states"))
        {
            std::cerr << "setting file not exist leveldb.chain_states  \n" ;
            return false;
        }
        if (!loader["leveldb"].isKeyExist("blocks"))
        {
            std::cerr << "setting file not exist leveldb.blocks  \n" ;
            return false;
        }
        if (!loader["leveldb"].isKeyExist("accounts"))
        {
            std::cerr << "setting file not exist leveldb.accounts  \n" ;
            return false;
        }
        if (!loader["leveldb"].isKeyExist("assets"))
        {
            std::cerr << "setting file not exist leveldb.assets  \n" ;
            return false;
        }
        if (!loader["leveldb"].isKeyExist("extensions"))
        {
            std::cerr << "setting file not exist leveldb.extensions  \n" ;
            return false;
        }
        if (!loader["node"].isKeyExist("public_key"))
        {
            std::cerr << "setting file not exist node.public_key  \n" ;
            return false;
        }
        if (!loader["node"].isKeyExist("private_key"))
        {
            std::cerr << "setting file not exist node.private_key  \n" ;
            return false;
        }
        //路径
        this->setting_ini.initial_keys_path = loader["chain"]["initial_keys"];
        remove_last_r(this->setting_ini.initial_keys_path);
        this->setting_ini.log_file_path = loader["chain"]["log_file_path"];
        remove_last_r(this->setting_ini.log_file_path);
        this->setting_ini.chain_state_db_path = loader["leveldb"]["chain_states"];
        remove_last_r(this->setting_ini.chain_state_db_path);
        this->setting_ini.block_db_path = loader["leveldb"]["blocks"];
        remove_last_r(this->setting_ini.block_db_path);
        this->setting_ini.account_db_path = loader["leveldb"]["accounts"];
        remove_last_r(this->setting_ini.account_db_path);
        this->setting_ini.extensions_db_path = loader["leveldb"]["extensions"];
        remove_last_r(this->setting_ini.extensions_db_path);
        this->setting_ini.assets_db_path = loader["leveldb"]["assets"];
        remove_last_r(this->setting_ini.assets_db_path);
        this->setting_ini.public_key_path = loader["node"]["public_key"];
        remove_last_r(this->setting_ini.public_key_path);
        this->setting_ini.private_key_path = loader["node"]["private_key"];
        remove_last_r(this->setting_ini.private_key_path);
        //参数
        size_t out_size = 0;
        uint64_t out = std::stoul(loader["chain"]["affair_maximum_number_in_block"], &out_size,10);
        if (out_size <= 0){
            std::cerr << "the value of the affair_maximum_number_in_block is illegal!  \n" ;
            return false;
        }
        this->setting_ini.affair_maximum_number_in_block = out;
        out = std::stoul(loader["chain"]["affair_minimum_number_in_block"], &out_size,10);
        if (out_size <= 0){
            std::cerr << "the value of the affair_minimum_number_in_block is illegal!  \n" ;
            return false;
        }
        this->setting_ini.affair_minimum_number_in_block = out;
        out = std::stoul(loader["chain"]["transaction_maximum_number_in_block"], &out_size,10);
        if (out_size <= 0){
            std::cerr << "the value of the transaction_maximum_number_in_block is illegal!  \n" ;
            return false;
        }
        this->setting_ini.transaction_maximum_number_in_block = out;
        out = std::stoul(loader["chain"]["transaction_minimum_number_in_block"], &out_size,10);
        if (out_size <= 0){
            std::cerr << "the value of the transaction_minimum_number_in_block is illegal!  \n" ;
            return false;
        }
        this->setting_ini.transaction_minimum_number_in_block = out;
        load = true;
        return true;
    }

    auto application::initialize() -> void {
        //检查是否加载配置文件
        if (!load){
            throw std::logic_error("should load setting file!");
        }
        //初始化key路径是否未empty
        if (this->setting_ini.initial_keys_path.empty()){
            throw std::logic_error("setting file content is not right!");
        }
        //配置文件路径不正确
        if (this->setting_ini.chain_state_db_path.empty()){
            throw std::logic_error("setting file content is not right!");
        }
        //not find key path
        if (!std::filesystem::is_directory(this->setting_ini.initial_keys_path) || !std::filesystem::exists(this->setting_ini.initial_keys_path)){
            std::cout << this->setting_ini.initial_keys_path << "\n";
            throw std::logic_error("initial_keys path is not right!");
        }
        if (!std::filesystem::exists(this->setting_ini.chain_state_db_path)){
            std::string message = "the path(" + this->setting_ini.chain_state_db_path + ") is not exist.";
            std::cout << message << "\n";
            std::cout << "create directory: " << this->setting_ini.chain_state_db_path << " \n";
            //直接创建目录
            auto result = std::filesystem::create_directories(this->setting_ini.chain_state_db_path);
            if (!result){
                std::cerr << "create directory: " << this->setting_ini.chain_state_db_path << " failed. please the create it! " << " \n";
                throw std::logic_error(message);
            }
        }
        //chain_state_db_path 路径检查
        if (!std::filesystem::is_directory(this->setting_ini.chain_state_db_path)){
            std::string message = "the path (" + this->setting_ini.chain_state_db_path + ") is not a directory.";
            throw std::logic_error(message);
        }
        if (!std::filesystem::exists(this->setting_ini.block_db_path) ){
            std::string message = "the path (" + this->setting_ini.block_db_path + ") is not exist.";
            std::cout << message << "\n";
            std::cout << "create directory: " << this->setting_ini.block_db_path << " \n";
            //直接创建目录
            auto result = std::filesystem::create_directories(this->setting_ini.block_db_path);
            if (!result){
                std::cerr << "create directory: " << this->setting_ini.block_db_path << " failed. please the create it! " << " \n";
                throw std::logic_error(message);
            }
        }
        //block_db_path 路径检查
        if (!std::filesystem::is_directory(this->setting_ini.block_db_path)){
            throw std::logic_error("block_db_path path is not right!");
        }
        if(!std::filesystem::exists(this->setting_ini.account_db_path)){
            std::string message = "the path (" + this->setting_ini.account_db_path + ") is not exist.";
            std::cout << message << "\n";
            std::cout << "create directory: " << this->setting_ini.account_db_path << " \n";
            //直接创建目录
            auto result = std::filesystem::create_directories(this->setting_ini.account_db_path);
            if (!result){
                std::cerr << "create directory: " << this->setting_ini.account_db_path << " failed. please the create it! " << " \n";
                throw std::logic_error(message);
            }
        }
        //account_db_path 路径检查
        if (!std::filesystem::is_directory(this->setting_ini.account_db_path)){
            throw std::logic_error("account_db_path path is not right!");
        }
        if (!std::filesystem::exists(this->setting_ini.extensions_db_path)){
            std::string message = "the path (" + this->setting_ini.extensions_db_path + ") is not exist.";
            std::cout << message << "\n";
            std::cout << "create directory: " << this->setting_ini.extensions_db_path << " \n";
            //直接创建目录
            auto result = std::filesystem::create_directories(this->setting_ini.extensions_db_path);
            if (!result){
                std::cerr << "create directory: " << this->setting_ini.extensions_db_path << " failed. please the create it! " << " \n";
                throw std::logic_error(message);
            }
        }
        //extensions_db_path 路径检查
        if (!std::filesystem::is_directory(this->setting_ini.extensions_db_path)){
            throw std::logic_error("extensions_db_path path is not right!");
        }
        if (!std::filesystem::exists(this->setting_ini.assets_db_path) ){
            std::string message = "the path (" + this->setting_ini.assets_db_path + ") is not exist.";
            std::cout << message << "\n";
            std::cout << "create directory: " << this->setting_ini.assets_db_path << " \n";
            //直接创建目录
            auto result = std::filesystem::create_directories(this->setting_ini.assets_db_path);
            if (!result){
                std::cerr << "create directory: " << this->setting_ini.assets_db_path << " failed. please the create it! " << " \n";
                throw std::logic_error(message);
            }
        }
        //assets_db_path 路径检查
        if (!std::filesystem::is_directory(this->setting_ini.assets_db_path)){
            throw std::logic_error("assets_db_path path is not right!");
        }
        if (!std::filesystem::exists(this->setting_ini.log_file_path) ){
            std::string message = "the path (" + this->setting_ini.log_file_path + ") is not exist.";
            std::cout << message << "\n";
            std::cout << "create directory: " << this->setting_ini.log_file_path << " \n";
            //直接创建目录
            auto result = std::filesystem::create_directories(this->setting_ini.log_file_path);
            if (!result){
                std::cerr << "create directory: " << this->setting_ini.assets_db_path << " failed. please the create it! " << " \n";
                throw std::logic_error(message);
            }
        }
        //log_file_path 路径检查
        if (!std::filesystem::is_directory(this->setting_ini.log_file_path)){
            throw std::logic_error("log_file_path path is not directory!");
        }
        //levelDB 查找 state 是否存在
        leveldb::DB* chain_db = nullptr;
        leveldb::Options chain_db_options;
        chain_db_options.create_if_missing = true;
        chain_db_options.write_buffer_size = static_cast<size_t>(1024 * 1024 * 16); //16M 写缓存
        leveldb::Status status = leveldb::DB::Open(chain_db_options, this->setting_ini.chain_state_db_path, &chain_db);
        std::unique_ptr<leveldb::DB> chain_ptr(chain_db);
        if (status.ok()){
            //存放读取的内容
            std::string state_data_out;
            //levelDB 查找 state 是否存在
            status = chain_ptr->Get(leveldb::ReadOptions(), application_state::STORE_STATE_KEY, &state_data_out);
            if (status.ok()){
                try{
                    muse::serializer::BinaryDeserializeView viewer;
                    viewer.reset_bytestream(state_data_out.c_str(), state_data_out.size());
                    viewer.output(this->state);
                    std::cout << "block chain current height: " <<this->state.current_height << std::endl;
                    std::cout << "top block hash: " << this->state.top_block_hash.get_hex() << std::endl;
                    std::cout << "genesis block hash: " << this->state.initial_block_hash.get_hex() << std::endl;
                    std::cout << "initial finish!" << std::endl;
                } catch (std::exception &ex) {
                    throw ex;
                }
            }
            else{
                //不存在，则需要创建
                leveldb::DB* block_db = nullptr;
                leveldb::Options block_db_options;
                block_db_options.create_if_missing = true;
                block_db_options.write_buffer_size = 1024 * 1024 * 16; //16M 写缓存
                leveldb::Status open_status = leveldb::DB::Open(block_db_options, this->setting_ini.block_db_path, &block_db);
                std::unique_ptr<leveldb::DB> block_ptr(block_db);
                if (!open_status.ok()){
                    throw std::logic_error(status.ToString().c_str());
                }
                std::vector<EVP_PKEY*> keys;
                std::vector<std::string> filenames;
                try {
                    // 遍历目录下的所有文件
                    for (const auto& entry : std::filesystem::directory_iterator(this->setting_ini.initial_keys_path)) {
                        if (entry.is_regular_file() && (entry.path().extension() == ".pub")) {
                            EVP_PKEY* key = ecc_secp256k1::read_public_key_from_file(entry.path().string());
                            if (key != nullptr) {
                                keys.emplace_back(key);
                                filenames.emplace_back(entry.path().string());
                            }
                        }
                    }
                } catch (...) {
                    throw std::logic_error("initial_keys_path occur openssl error!");
                }
                //检查是否合法
                if (keys.empty() || keys.size() < application::Minimum_Initialization_Nodes ){
                    throw std::logic_error("initial_keys is not right!");
                }
                //获取地址
                std::vector<std::string> addresses(keys.size());
                //构建 初始化 block
                for(int i = 0; i < keys.size(); i++){
                    //存储公钥, 该函数有可能失败，因为有可能秘钥不正确！
                    addresses[i] = ecc_secp256k1::convert_public_key_to_binary(keys[i]);
                    if (addresses[i].empty()){
                        SPDLOG_ERROR("convert_public_key_to_binary failed, public key store filename: {}", filenames[i]);
                        throw std::logic_error("initialize convert_public_key_to_binary failed!");
                    }
                }
                //释放掉
                for (auto *key : keys) {
                    EVP_PKEY_free(key);
                }
                //排个序
                std::sort(addresses.begin(), addresses.end());
                //创建创世块
                block genesis_block;
                auto create_time = muse::utils::get_microseconds_now_UTC_Zone_0();
                //初始化事务 affair
                std::vector<affair>& initial_affairs = genesis_block.body.affairs;
                //构造affair
                for (int i = 0; i < keys.size(); ++i) {
                    affair temp;
                    temp.set_data(addresses[i].c_str(), addresses[i].size());
                    temp.set_owner(uint256(0));
                    temp.set_nonce(0);
                    temp.set_type(affair_type::GenesisData);
                    temp.set_create_time(create_time);
                    temp.set_epoch(application::EPOCH_MAX) ;
                    initial_affairs.emplace_back(temp);
                };
                //构造 merkle_root
                std::vector<uint256> affairs_hash(keys.size());
                for (int i = 0; i < keys.size(); ++i) {
                    affairs_hash[i] = initial_affairs[i].get_hash();
                }
                uint256 affair_merkle_root = merkle_tree::get_merkle_root(affairs_hash);
                genesis_block.header.transactions_merkle_root = uint256(0);
                genesis_block.header.prev_block_hash = uint256(0);
                genesis_block.header.height = 1;
                //设置为0时区时间
                genesis_block.header.create_time = create_time;
                genesis_block.header.merkle_patricia_trie_root = uint256(0);
                genesis_block.header.affairs_merkle_root = affair_merkle_root;

                uint256 genesis_block_hash = genesis_block.header.get_block_hash();
                //创建状态

                this->state.initial_block_hash = genesis_block_hash;
                //创世块 初始化高度为0
                this->state.current_height = 0;
                this->state.top_block_hash = genesis_block_hash;
                //序列化
                muse::serializer::BinarySerializer serializer;
                serializer.input(state);
                //可以减少复制操作
                leveldb::Slice out(serializer.getBinaryStream(),serializer.byteCount());

                auto opt = leveldb::WriteOptions();
                opt.sync = true;
                //存储区块链状态
                leveldb::Status stt = chain_ptr->Put(opt, application_state::STORE_STATE_KEY, out);
                //存储资产过滤器
                if (stt.ok()){
                    //存入创世区块
                    serializer.clear();
                    serializer.input(genesis_block);
                    leveldb::Slice result(serializer.getBinaryStream(),serializer.byteCount());
                    uint64_t height = 0;
                    leveldb::Slice leveldb_key(reinterpret_cast<const char *>(&height), sizeof(height));
                    leveldb::Status lst = block_db->Put(leveldb::WriteOptions(), leveldb_key, result);
                    if (!lst.ok()){
                        leveldb::DestroyDB(this->setting_ini.chain_state_db_path, chain_db_options);
                        leveldb::DestroyDB(this->setting_ini.block_db_path, block_db_options);
                        SPDLOG_ERROR("levelDB Write Error, error des: {}", lst.ToString());
                        throw std::logic_error("levelDB Write Error");
                    }
                    SPDLOG_INFO("create genesis block success!");
                }else{
                    SPDLOG_ERROR("levelDB Write Error, error des: {}", stt.ToString());
                    throw std::logic_error("levelDB Write Error");
                }
            }
        }
        else{
            SPDLOG_ERROR("levelDB Open Error, error des: {}", status.ToString());
            throw std::logic_error(status.ToString().c_str());
        }
    }

    auto application::get_chain_ini() const -> const chain_ini & {
        return this->setting_ini;
    }

    auto application::get_extensions_db(const std::string &path) -> leveldb::DB * {
        leveldb::DB* ex_db = nullptr;
        leveldb::Options options;
        options.create_if_missing = true;
        options.write_buffer_size = 1024 * 1024 * 32; //32M 写缓存
        options.max_file_size = 4 * 1024 * 1024; //4M SSTable
        leveldb::Status status = leveldb::DB::Open(options, path, &ex_db);
        if (status.ok()) {
            return ex_db;
        }
        SPDLOG_ERROR("application::get_extensions_db leveldb::DB::Open  Error, error des: {}", status.ToString());
        return nullptr;
    }

    auto application::get_blocks_db(const std::string &path) -> leveldb::DB * {
        //levelDB 查找 state 是否存在
        leveldb::DB* blk_db = nullptr;
        leveldb::Options options;
        options.create_if_missing = true;
        options.write_buffer_size = 1024 * 1024 * 32; //32M 写缓存
        options.max_file_size = 32 * 1024 * 1024; //32M SSTable
        leveldb::Status status = leveldb::DB::Open(options, path, &blk_db);
        if (status.ok()) {
            return blk_db;
        }
        SPDLOG_ERROR("application::get_blocks_db leveldb::DB::Open  Error, error des: {}", status.ToString());
        return nullptr;
    }

    auto application::get_accounts_db(const std::string &path) -> leveldb::DB * {
        leveldb::DB* act_db = nullptr;
        leveldb::Options options;
        options.create_if_missing = true;
        options.write_buffer_size = 1024 * 1024 * 32; //32M 写缓存
        options.max_file_size = 4 * 1024 * 1024; //4M SSTable
        leveldb::Status status = leveldb::DB::Open(options, path, &act_db);
        if (status.ok()) {
            return act_db;
        }
        SPDLOG_ERROR("application::get_accounts_db leveldb::DB::Open  Error, error des: {}", status.ToString());
        return nullptr;
    }

    auto application::get_application_state() const -> const application_state & {
        return state;
    }

    auto application::get_assets_db(const std::string &path) -> leveldb::DB * {
        leveldb::DB* assets_db = nullptr;
        leveldb::Options options;
        options.create_if_missing = true;
        options.write_buffer_size = 1024 * 1024 * 32; //32M 写缓存
        options.max_file_size = 4 * 1024 * 1024; //4M SSTable
        leveldb::Status status = leveldb::DB::Open(options, path, &assets_db);
        if (status.ok()) {
            return assets_db;
        }
        SPDLOG_ERROR("application::get_assets_db leveldb::DB::Open  Error, error des: {}", status.ToString());
        return nullptr;
    }

    auto application::get_chain_db(const std::string &path) -> leveldb::DB * {
        leveldb::DB* chain_db = nullptr;
        leveldb::Options chain_db_options;
        chain_db_options.create_if_missing = true;
        chain_db_options.write_buffer_size = 1024 * 1024 * 16; //16M 写缓存
        chain_db_options.max_file_size = 3 * 1024 * 1024; //4M SSTable
        leveldb::Status status = leveldb::DB::Open(chain_db_options, path, &chain_db);
        if (status.ok()) {
            return chain_db;
        }
        SPDLOG_ERROR("application::get_chain_db leveldb::DB::Open  Error, error des: {}", status.ToString());
        return nullptr;
    }

    application::~application() {
        if (private_key != nullptr){
            EVP_PKEY_free(private_key);
        }
        if (public_key != nullptr){
            EVP_PKEY_free(public_key);
        }
    }

    application::application(application &&other) noexcept
    : setting_ini(std::move(other.setting_ini)),
    load(other.load), state(std::move(other.state)),
    private_key(other.private_key),
    public_key_hash(std::move(other.public_key_hash)),
    public_key(other.public_key){
        if (this->private_key != nullptr){
            EVP_PKEY_free(this->private_key);
        }
        if (this->public_key != nullptr){
            EVP_PKEY_free(this->public_key);
        }
        other.private_key = nullptr;
        other.public_key = nullptr;
    }

    application::application()
    :private_key(nullptr),
    public_key(nullptr)
    {

    }

    auto application::load_node_keys(const std::string &_password) -> bool {
        if (!std::filesystem::exists(this->setting_ini.private_key_path)){
            throw std::logic_error("private_key path is not right!");
        }
        if (!std::filesystem::exists(this->setting_ini.public_key_path)){
            throw std::logic_error("public_key path is not right!");
        }
        EVP_PKEY* private_key_ = ecc_secp256k1::read_private_key_from_file(this->setting_ini.private_key_path, _password);
        EVP_PKEY* public_key_ = ecc_secp256k1::read_public_key_from_file(this->setting_ini.public_key_path);
        if (private_key_ != nullptr && public_key_ != nullptr){
            if (this->private_key != nullptr){
                EVP_PKEY_free(this->private_key);
            }
            if (this->public_key != nullptr){
                EVP_PKEY_free(this->public_key);
            }
            this->private_key = private_key_;
            this->public_key = public_key_;
            ecc_secp256k1::get_public_key_hash_no_compressed(this->public_key, this->public_key_hash);
            return true;
        }else{
            return false;
        }
    }

    auto application::package_inject_special_affairs(bool &success, std::list<affair> participants) -> block {
        block blk;
        if(this->private_key != nullptr && this->public_key != nullptr){
            muse::chain::affair air;
            //设置类型
            air.set_type(muse::chain::affair_type::OwnerAddressHash);
            //创建者的公钥hash
            air.set_owner(this->public_key_hash);
            //nonce 默认为 0
            air.set_nonce(0);
            //设置创建时间和过期间隔
            air.set_create_time(muse::utils::get_microseconds_now_UTC_Zone_0());
            air.set_epoch(application::EPOCH_MAX);
            //affair 的 hash
            uint256 hash = air.get_hash();
            std::string _out_sig;
            //签个名
            bool status = ecc_secp256k1::signature(this->private_key, hash.get_data(),uint256::Width, _out_sig);
            if (!status){
                success = false;
                return blk;
            }
            air.set_signature(std::move(_out_sig));

            blk.body.affairs.push_back(std::move(air));
            for (auto &item : participants) {
                blk.body.affairs.push_back(std::move(item));
            }
            success = true;
        }else{
            success = false;
        }
        return blk;
    }

    auto application::signature_block(block &blk) -> bool {
        if (this->private_key != nullptr){
            uint256 hash = blk.get_hash();
            //签个名
            std::string _out_sig;
            bool status = ecc_secp256k1::signature(this->private_key, hash.get_data(),uint256::Width, _out_sig);
            if (!status){
                return false;
            }
            blk.signature = std::move(_out_sig);
            return true;
        }else{
            return false;
        }
    }

    auto application::remove_last_r(std::string &path) -> void {
        if (!path.empty() && path.back() == '\r' ){
            path.pop_back();
        }
    }

}