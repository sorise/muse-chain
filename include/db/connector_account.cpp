//
// Created by 14270 on 2023-12-27.
//
#include "connector_account.hpp"

namespace muse::chain{
    connector_account::connector_account()
    :extensions_db(nullptr),
    accounts_db(nullptr),
    assets_db(nullptr),
    block_height(0){

    }

    auto connector_account::exists(const uint256 &key) -> bool {
        return account_mpt.find(key);
    }

    /* 会抛出异常，检查某一笔交易是否合法 */
    auto connector_account::check_transaction(transaction &tsc) -> bool {
        bool result = true;
        switch (tsc.get_transaction_operator()) {
            case transaction_operator::Establish :{
                //判断这种资产是否已经存在了
                bool exist = this->asset_bloom_filter.test(tsc.get_asset_type());
                if (!exist){
                    result = false;
                    break;
                }
                if (!tsc.outs.empty()){
                    result = false;
                    break;
                }
                break;
            }
            case transaction_operator::Append :{
                uint64_t out_count = 0;
                /* 检测输入输出是否正确 */
                for (auto &out: tsc.outs) {
                    out_count += out.count;
                }
                if (out_count!= tsc.get_count()){
                    result = false;
                    break;
                }
                /* 查询账户余额 */
                account* sender = account_mpt.search(tsc.get_sender());
                if (sender == nullptr){
                    result = false; //根本没有这个账户
                    break;
                }
                if (sender->get_balance(tsc.get_asset_type()) < tsc.get_count()){
                    result = false; //余额不足
                    break;
                }
                /* 查看nonce 正确否 */
                if (sender->get_nonce() <= tsc.get_nonce()){
                    result = false; //nonce 不正确
                    break;
                }
                //检查是否是过期
                uint64_t expire =  tsc.get_create_time() + (static_cast<uint64_t>(tsc.get_expired_epoch()) * environment_db::EPOCH_TIME_UINT);
                if (expire < environment_db::get_now_tick().count()){
                    result = false; //交易已经过期了
                    break;
                }
                break;
            }
            default:
                result = false;
                break;
        }
        return result;
    }

    auto connector_account::package_next_block_body(block& blk, connector_block &c_blk, const chain_ini& _ini) -> void {
        std::map<uint256, receipt> receipts; //执行回执单
        bloom_filter asset_filter(this->asset_bloom_filter);//资产过滤器
        uint64_t current = muse::utils::get_microseconds_now_UTC_Zone_0();
        blk.header.create_time = current;
        int add_count = 1;
        while (add_count > 0){
            add_count = 0;
            auto start = c_blk.transactions_queue.begin();
            while (start != c_blk.transactions_queue.end()) {
                transaction &trans = *start;
                uint64_t expire_dt = trans.get_create_time() + trans.get_expired_epoch() * application::Epoch_Time_Uint;
                if (expire_dt < current){
                    //过期了，直接删除
                    start = c_blk.transactions_queue.erase(start);
                    continue;
                }
                else
                {
                    uint256 sender = trans.get_sender();
                    account* _account = account_mpt.search(sender);
                    uint32_t asset_type = trans.get_asset_type();
                    switch(trans.get_transaction_operator()){
                        case transaction_operator::Append:{
                            if (_account == nullptr){
                                //查找
                                if(receipts.find(sender) == receipts.end()){
                                    //账户还未建立，且没有建立账户的交易出现
                                    start++;
                                    continue;
                                }else{
                                    //查看资产类型
                                    auto _iterator=  receipts[sender].balances.find(asset_type);
                                    if(_iterator == receipts[sender].balances.end()){
                                        //不存在这个资产类型,连续交易内容不全
                                        start++;
                                        continue;
                                    }else{
                                        //已经存了，附加执行结果，植入nonce
                                        receipts[sender].nonce.push_back(trans.get_nonce());
                                        //增加输出数量
                                        _iterator->second.second += trans.get_count();
                                        if (!receipts[sender].check()){
                                            receipts[sender].nonce.pop_back();
                                            _iterator->second.second -= trans.get_count();
                                            start++;
                                        }else{
                                            //交易加入
                                            blk.body.transactions.emplace_back(trans);
                                            //从队列中删除
                                            start = c_blk.transactions_queue.erase(start);
                                            //计数器 加 1
                                            add_count++;
                                        }
                                    }
                                }
                            }
                            else{
                                //判断是否已经存在回执单
                                if(receipts.find(sender) == receipts.end()){
                                    //创建新的回执单
                                    receipt rcp;
                                    rcp.nonce_start = _account->get_nonce();
                                    rcp.balances.insert({asset_type,{_account->get_balance(asset_type), trans.get_count()}});
                                    rcp.nonce.push_back(trans.get_nonce()); //植入nonce
                                    if (rcp.check()){
                                        receipts[sender] = std::move(rcp);
                                        //交易加入
                                        blk.body.transactions.emplace_back(trans);
                                        //从队列中删除
                                        start = c_blk.transactions_queue.erase(start);
                                        //计数器 加 1
                                        add_count++;
                                    }else{
                                        start++;
                                    }
                                }
                                else
                                {
                                    //已经存了，附加执行结果
                                    receipts[sender].nonce.push_back(trans.get_nonce()); //植入nonce
                                    //查看资产类型
                                    auto _iterator=  receipts[sender].balances.find(asset_type);
                                    if(_iterator == receipts[sender].balances.end()){
                                        receipts[sender].balances.insert({asset_type,{_account->get_balance(asset_type), trans.get_count()}});
                                        if (!receipts[sender].check()){
                                            receipts[sender].nonce.pop_back();
                                            receipts[sender].balances.erase(asset_type);
                                        }else{
                                            //交易加入
                                            blk.body.transactions.emplace_back(trans);
                                            //从队列中删除
                                            start = c_blk.transactions_queue.erase(start);
                                            //计数器 加 1
                                            add_count++;
                                        }
                                    }else{
                                        _iterator->second.second += trans.get_count();
                                        if (!receipts[sender].check()){
                                            _iterator->second.second -= trans.get_count();
                                            start++;
                                        }else{
                                            //交易加入
                                            blk.body.transactions.emplace_back(trans);
                                            //从队列中删除
                                            start = c_blk.transactions_queue.erase(start);
                                            //计数器 加 1
                                            add_count++;
                                        }
                                    }
                                }
                            }
                            break;
                        }
                        case transaction_operator::Establish:{
                            //操作账户可以不存在
                            if (asset_filter.test(asset_type)){
                                //资产已经具有了，不可再有
                                start++;
                            }
                            else{
                                //插入资产
                                asset_filter.insert(asset_type);
                                //判断是否已经存在回执单
                                if(receipts.find(sender) == receipts.end()){
                                    //创建新的回执单
                                    receipt rcp;
                                    rcp.nonce_start = (_account == nullptr? 0:_account->get_nonce());
                                    rcp.balances.insert({asset_type,{environment_db::ASSET_MAX_COUNT, 0}});
                                    rcp.nonce.push_back(trans.get_nonce()); //植入nonce
                                    if (rcp.check()){
                                        receipts[sender] = std::move(rcp);
                                        //交易加入
                                        blk.body.transactions.emplace_back(trans);
                                        //从队列中删除
                                        start = c_blk.transactions_queue.erase(start);
                                        //计数器 加 1
                                        add_count++;
                                    }else{
                                        start++;
                                    }
                                }
                                else{
                                    receipts[sender].nonce.push_back(trans.get_nonce()); //植入nonce
                                    if (receipts[sender].check_nonce()){
                                        receipts[sender].balances[asset_type].first += environment_db::ASSET_MAX_COUNT;
                                        //交易加入
                                        blk.body.transactions.emplace_back(trans);
                                        //从队列中删除
                                        start = c_blk.transactions_queue.erase(start);
                                        //计数器 加 1
                                        add_count++;
                                    } else{
                                        //恢复原样
                                        receipts[sender].nonce.pop_back();
                                        start++;
                                    }
                                }
                            }
                            break;
                        }
                        case transaction_operator::NullOP:{
                            //丢弃交易
                            start = c_blk.transactions_queue.erase(start);
                            break;
                        }
                        default:{
                            //丢弃交易
                            start = c_blk.transactions_queue.erase(start);
                        }
                    }
                }
                if (blk.body.transactions.size() >= _ini.transaction_maximum_number_in_block){
                    break;
                }
            }
        };
        add_count = 1;
        while (add_count > 0){
            add_count = 0;
            auto start = c_blk.affairs_queue.begin();
            //再检测affair交易的nonce合法性
            while (start != c_blk.affairs_queue.end()){
                uint64_t expire_dt = start->get_create_time() + start->get_epoch() * application::Epoch_Time_Uint;
                if (expire_dt < current){
                    //过期了，直接删除
                    start = c_blk.affairs_queue.erase(start);
                    continue;
                }
                affair &air = *start;
                uint256 owner = air.get_owner();
                account* _account = account_mpt.search(owner);
                //操作账户可以不存在
                if(receipts.find(owner) == receipts.end()){
                    //创建新的回执单
                    receipt rcp;
                    rcp.nonce_start = _account == nullptr? 0: _account->get_nonce();
                    rcp.nonce.push_back(air.get_nonce()); //植入nonce
                    if (rcp.check()){
                        receipts[owner] = std::move(rcp);
                        //交易加入
                        blk.body.affairs.emplace_back(air);
                        //从队列中删除
                        start = c_blk.affairs_queue.erase(start);
                        //计数器 加 1
                        add_count++;
                    }
                }else{
                    receipts[owner].nonce.push_back(air.get_nonce()); //植入nonce
                    if (receipts[owner].check_nonce()){
                        //交易加入
                        blk.body.affairs.emplace_back(air);
                        //从队列中删除
                        start = c_blk.affairs_queue.erase(start);
                        //计数器 加 1
                        add_count++;
                    } else{
                        //恢复原样
                        auto last = std::remove(receipts[owner].nonce.begin(), receipts[owner].nonce.end(), air.get_nonce());
                        receipts[owner].nonce.erase(last, receipts[owner].nonce.end());
                    }
                }
            }
        }
        blk.header.assets_blf_hash = asset_filter.get_hash();
    }

    auto connector_account::check_block_body(block &blk) -> bool {
        std::map<uint256, receipt> receipts; //执行回执单
        bloom_filter asset_filter(this->asset_bloom_filter);//资产过滤器
        //先检查资产交易
        for(auto &tsc: blk.body.transactions){
            uint256 sender = tsc.get_sender();
            account* _account = account_mpt.search(sender);
            uint32_t asset_type = tsc.get_asset_type();
            //这个操作要求发出账户必须存在
            if(tsc.get_transaction_operator() == transaction_operator::Append){
                //目标账户是否已经存在
                if (_account == nullptr){
                    if(receipts.find(sender) == receipts.end()){
                        return false; //账户不存在，直接返回 false
                    }else{
                        //查看资产类型
                        auto _iterator=  receipts[sender].balances.find(asset_type);
                        if(_iterator == receipts[sender].balances.end()){
                            return false;
                        }else{
                            //已经存了，附加执行结果，植入nonce
                            receipts[sender].nonce.push_back(tsc.get_nonce());
                            //增加输出数量
                            _iterator->second.second += tsc.get_count();
                        }
                    }
                    continue;
                }
                uint64_t out_count = 0;
                /* 检测输入输出是否正确 */
                for (auto &out: tsc.outs) {
                    out_count += out.count;
                }
                if (out_count != tsc.get_count()){
                    return false;
                }
                //判断是否已经存在回执单
                if(receipts.find(sender) == receipts.end()){
                    //创建新的回执单
                    receipt rcp;
                    rcp.nonce_start = _account->get_nonce();
                    rcp.balances.insert({asset_type,{_account->get_balance(asset_type), tsc.get_count()}});
                    rcp.nonce.push_back(tsc.get_nonce()); //植入nonce
                    receipts[sender] = std::move(rcp);
                }else{
                    //已经存再了，附加执行结果
                    receipts[sender].nonce.push_back(tsc.get_nonce()); //植入nonce
                    //查看资产类型
                    auto _iterator=  receipts[sender].balances.find(asset_type);
                    if(_iterator == receipts[sender].balances.end()){
                        receipts[sender].balances.insert({asset_type,{_account->get_balance(asset_type), tsc.get_count()}});
                    }else{
                        _iterator->second.second += tsc.get_count();
                    }
                }
            }
            else if (tsc.get_transaction_operator() == transaction_operator::Establish){
                //操作账户可以不存在
                if (asset_filter.test(asset_type)){
                    return false;
                }
                //插入资产
                asset_filter.insert(asset_type);
                //判断是否已经存在回执单
                if(receipts.find(sender) == receipts.end()){
                    //创建新的回执单
                    receipt rcp;
                    rcp.nonce_start = (_account == nullptr? 0:_account->get_nonce());
                    rcp.balances.insert({asset_type,{environment_db::ASSET_MAX_COUNT, 0}});
                    rcp.nonce.push_back(tsc.get_nonce()); //植入nonce
                    receipts[sender] = std::move(rcp);
                }else{
                    receipts[sender].nonce.push_back(tsc.get_nonce()); //植入nonce
                    receipts[sender].balances[asset_type].first += environment_db::ASSET_MAX_COUNT;
                }
            }
            else{
                return false;
            }
        }
        //再检测affair交易的nonce合法性
        for(auto &air: blk.body.affairs){
            if (!air.is_db_level_affair_type()){
                continue;
            }
            uint256 owner = air.get_owner();
            account* _account = account_mpt.search(owner);
            //操作账户可以不存在
            if(receipts.find(owner) == receipts.end()){
                //创建新的回执单
                receipt rcp;
                rcp.nonce_start = _account == nullptr? 0: _account->get_nonce();
                rcp.nonce.push_back(air.get_nonce()); //植入nonce
                receipts[owner] = std::move(rcp);
            }else{
                receipts[owner].nonce.push_back(air.get_nonce()); //植入nonce
            }
        }
        //检查回执单的合法性
        for (auto& receipt: receipts) {
            if (!receipt.second.check()){
                return false;
            }
        }
        return true;
    }

    auto connector_account::insert_block(block &blk, bool check)-> bool {
        //交易的逻辑是否正确
        if (!check_block_body(blk)){
            return false;
        }
        //简单的检查检测区块头
        if (blk.header.height != block_height + 1 || blk.header.prev_block_hash != top_block_hash){
            return false;
        }
        RollBack_Storage_TP storage;    //回滚操作
        bloom_filter test_filter = this->asset_bloom_filter;
        test_filter.set_block_height(blk.header.height);
        std::set<uint256> commit_accounts;
        //创建commit
        leveldb_account_commit level_commit;
        try{
            //先处理事务、插入事务
            for (auto &tsc: blk.body.transactions) {
                append_created_rollback(storage, tsc); //先创建回滚操作
                //账户必须存在
                account* _account = account_mpt.search_and_mark(tsc.get_sender());
                switch (tsc.get_transaction_operator()) {
                    case transaction_operator::Append:{
                        if (_account == nullptr) {
                            throw chain_db_exception("account not found!",db_error_code::account_no_exist);
                        }
                        uint64_t original_nonce = _account->get_nonce();
                        //记录原来的 nonce，失败了，需要回滚
                        uint64_t original_balance = _account->get_balance(tsc.get_asset_type());
                        //记录原来的余额，失败了，需要回滚
                        //修改为原子化操作
                        std::vector<std::tuple<bool,bool>> exists(tsc.outs.size(), {false,false});
                        size_t start = 0;
                        try{
                            _account->set_nonce(tsc.get_nonce() + 1);
                            _account->reduce_balance(tsc.get_asset_type(), tsc.get_count());
                            for (; start < tsc.outs.size(); start++) {
                                account* receiver = account_mpt.search_and_mark(tsc.outs[start].receiver);
                                commit_accounts.emplace(tsc.outs[start].receiver);
                                if (receiver == nullptr){
                                    try{
                                        //新账户
                                        account act(tsc.outs[start].receiver, 0);
                                        act.add_balance(tsc.get_asset_type(), tsc.outs[start].count);
                                        account_mpt.insert(tsc.outs[start].receiver ,act);
                                        exists[start] = {false, true };
                                    }catch(...){
                                        //直接回滚
                                        account_mpt.erase(tsc.outs[start].receiver);
                                        exists[start] = {false, false };
                                        throw chain_db_exception("the atomic operation failed!",db_error_code::atomic_operation_failed);
                                    }
                                }else{
                                    try{
                                        receiver->add_balance(tsc.get_asset_type(), tsc.outs[start].count);
                                        exists[start] = {true, true };;
                                    }catch(...){
                                        receiver->reduce_balance(tsc.get_asset_type(), tsc.outs[start].count);
                                        exists[start] = {true, false};;
                                        throw chain_db_exception("the atomic operation failed!",db_error_code::atomic_operation_failed);
                                    }
                                }
                            }
                        }
                        catch(...){
                            for (size_t j = 0; j < start; j++) {
                                bool exist = false; //账户是否存在
                                bool finish = false; //账户是否已经完成
                                std::tie(exist, finish) = exists[j];
                                if (finish){
                                    if (exist){
                                        account* receiver = account_mpt.search_and_mark(tsc.outs[j].receiver);
                                        receiver->reduce_balance(tsc.get_asset_type(), tsc.outs[j].count);
                                    }else{
                                        account_mpt.erase(tsc.outs[j].receiver);
                                    }
                                }
                            }
                            //执行回滚操作
                            _account->set_nonce(original_nonce);
                            _account->set_balance(tsc.get_asset_type(),original_balance);
                            throw chain_db_exception("the atomic operation failed!",db_error_code::atomic_operation_failed);
                            storage.pop_back();//删除回滚操作
                        }
                        commit_accounts.emplace(tsc.get_sender());
                        break;
                    }
                    case transaction_operator::Establish:{
                        //检测资产类型是否存在
                        if(!test_filter.test(tsc.get_asset_type())){
                            test_filter.insert(tsc.get_asset_type());
                            if(_account == nullptr){
                                //本次操作了 nonce应该是 1
                                account act(tsc.get_sender(), 1);
                                act.add_balance(tsc.get_asset_type(), environment_db::ASSET_MAX_COUNT);
                                try{
                                    account_mpt.insert(tsc.get_sender(), act);
                                }catch(...) {
                                    //为了安全，硬性删除一次
                                    account_mpt.erase(tsc.get_sender());
                                    //删除回滚操作
                                    storage.pop_back();
                                    throw chain_db_exception("the atomic operation failed!",db_error_code::atomic_operation_failed);
                                }
                            }
                            else{
                                _account->add_balance(tsc.get_asset_type(), environment_db::ASSET_MAX_COUNT);
                            }
                        }else{
                            //删除回滚操作
                            storage.pop_back();
                            throw chain_db_exception("asset already exist!",db_error_code::asset_already_exist);
                        }
                        commit_accounts.emplace(tsc.get_sender());
                        break;
                    }
                    default:
                        break;
                }
            }
            //处理affairs
            for (auto &air: blk.body.affairs) {
                if (!air.is_db_level_affair_type()){
                    continue;
                }
                //先准备回滚操作
                append_created_rollback(storage, air);
                account* _account = account_mpt.search_and_mark(air.get_owner());
                if (_account == nullptr){
                    try{
                        account act(air.get_owner(), 1);
                        account_mpt.insert(air.get_owner(), act);
                    } catch (...) {
                        account_mpt.erase(air.get_owner());
                        throw chain_db_exception("asset already exist!",db_error_code::asset_already_exist);
                    }
                }else{
                    _account->set_nonce(_account->get_nonce() + 1);
                }
                commit_accounts.emplace(air.get_owner());
            }
            //所有的内容都已经成功提交到mpt上面，重新进行hash计算，并且检查 mpt的根hash是否与 区块头的一致
            if (check){
                uint256 root_hash = this->account_mpt.get_hash();
                if (root_hash != blk.header.merkle_patricia_trie_root){
                    throw chain_db_exception("the merkle_patricia_trie_root is not equal to the account_mpt root hash!",db_error_code::invalid_patricia_merkle_tree_root_hash);
                }
            }
            //生成account的提交信息，提交到提交队列中
            for(const auto& key: commit_accounts){
                account* _act_ = account_mpt.search(key);
                if (_act_ == nullptr){
                    throw chain_db_exception("invalid account address!",db_error_code::invalid_account_address);
                }
                level_commit.commits[key].input(*_act_);
            }
            //前一个区块的高度和hash值
            level_commit.height = this->block_height;
            level_commit.block_hash = this->top_block_hash;
            level_commit.prev_mpt_root = this->merkle_patricia_trie_root_hash;
        }
        catch(const chain_db_exception &ex) {
            execute_rollback(storage);
            //继续抛出异常
            SPDLOG_ERROR("auto connector_account::insert_block(block {}, bool {}) error!", blk.header.get_block_hash().get_hex(), check);
            throw chain_db_exception(ex.what(),ex.get_db_error_code());
        }catch(const std::exception &ex) {
            execute_rollback(storage);
            return false;
        }catch(...){
            execute_rollback(storage);
            return false;
        }
        //提交账户信息
        this->commits.push_back(std::move(level_commit));
        //缓存回滚操作
        rollbacks.push_back(std::move(storage));
        //更新hash值
        this->merkle_patricia_trie_root_hash = this->account_mpt.get_hash();
        //更新区块高度、hash值
        this->block_height = blk.header.height;
        this->top_block_hash = blk.header.get_block_hash();
        test_filter.set_block_hash(this->top_block_hash);
        this->asset_bloom_filter = std::move(test_filter);
        //缓存旧的资产过滤器
        this->account_bloom_filters.emplace_back(this->asset_bloom_filter);
        //新资产过滤器
        return true;
    }

    auto connector_account::rollback(const uint32_t &step) -> bool {
        if (step >= rollbacks.size()){
            for (size_t i = 0; i < step; ++i) {
                const std::list<rollback_operation>& operations = rollbacks.back();
                execute_rollback(operations);
                rollbacks.pop_back();
                //扩展节点需要撤回
            }
            this->merkle_patricia_trie_root_hash = this->account_mpt.get_hash();
            this->account_mpt.get_change_store().clear();
            return true;
        }
        return false;
    }

    auto connector_account::append_created_rollback(connector_account::RollBack_Storage_TP &rtp, transaction &trans) -> void {
        switch (trans.get_transaction_operator()) {
            case transaction_operator::Append: {
                //发送者
                rollback_operation rop_sender {};
                rop_sender.target_address = trans.get_sender();
                rop_sender.value = trans.get_count();
                rop_sender.type = rollback_operation::UPDATE;
                rop_sender.update_type = rollback_operation::UPDATE_ADD; /* 减少反义词就是增加 */
                rop_sender.asset_type = trans.get_asset_type();
                rtp.push_back(rop_sender);
                //接收者
                for(out_entry & out : trans.outs){
                    rollback_operation rop {};
                    rop.asset_type = trans.get_asset_type();
                    rop.target_address = out.receiver;
                    rop.value = out.count;
                    if (account_mpt.find(out.receiver)){
                        /* 账户已经存在 */
                        rop.type = rollback_operation::UPDATE;
                        rop.update_type = rollback_operation::UPDATE_REDUCE;/* 追加反义词 就是减少 */
                        rtp.push_back(rop);
                    } else{
                        /* 账户不存在 */
                        rop.type = rollback_operation::DELETE_ACCOUNT;
                        rtp.push_back(rop);
                    }
                }
            }break;
            case transaction_operator::Establish:{
                //建立资产
                rollback_operation rop_est;
                rop_est.target_address = trans.get_sender();
                rop_est.value = trans.get_count();
                rop_est.asset_type = trans.get_asset_type();
                if (account_mpt.find(trans.get_sender())){
                    //账户已经存在了
                    rop_est.type = rollback_operation::UPDATE;
                    rop_est.update_type = rollback_operation::UPDATE_DELETE_ASSET;
                    /* 需要删除资产 */
                }else{
                    rop_est.type = rollback_operation::DELETE_ACCOUNT;
                }
                rtp.push_back(rop_est);
            }break;
            default:
                break;
        };
    }

    auto connector_account::append_created_rollback(connector_account::RollBack_Storage_TP &rtp, const affair &afi) -> void {
        //发送者
        rollback_operation rop_sender {};
        rop_sender.target_address = afi.get_owner();
        account *act_ = account_mpt.search(afi.get_owner());
        if (act_ == nullptr){
            rop_sender.type = rollback_operation::DELETE_ACCOUNT;
        }else{
            rop_sender.type = rollback_operation::ONLY_REDUCE_NONCE;
        }
        rtp.push_back(rop_sender);
    }

    auto connector_account::execute_rollback(const connector_account::RollBack_Storage_TP &rtp) -> bool {
        for(const rollback_operation &otp: rtp) {
            switch (otp.type) {
                case rollback_operation::INSERT:
                {
                    //我怎么也想不通为什么要删除账户，因为账户是存在的，只是没有资产了
                    //目前没有操作是需要啊删除用户，然后回滚插入用户的
                    break;
                }
                case rollback_operation::DELETE_ACCOUNT:
                {
                    account_mpt.erase(otp.target_address);
                    break;
                }
                case rollback_operation::UPDATE:
                {
                    account* _act = account_mpt.search_and_mark(otp.target_address);
                    switch (otp.update_type) {
                        case rollback_operation::UPDATE_ADD:
                        {
                            if (_act != nullptr){
                                _act->add_balance(otp.asset_type, otp.value);
                                _act->set_nonce(_act->get_nonce() - 1);
                            }
                            break;
                        }
                        case rollback_operation::UPDATE_DELETE_ASSET:
                        {
                            if (_act != nullptr){
                                _act->delete_asset(otp.asset_type);
                                _act->set_nonce(_act->get_nonce() - 1);
                            }
                            break;
                        }
                        case rollback_operation::UPDATE_REDUCE:
                        {
                            if (_act != nullptr){
                                _act->reduce_balance(otp.asset_type, otp.value);
                            }
                            break;
                        }
                        default:
                            break;
                    }
                    break;
                }
                case rollback_operation::ONLY_REDUCE_NONCE:
                {
                    account* _act = account_mpt.search_and_mark(otp.target_address);
                    if (_act!= nullptr){
                        _act->set_nonce(_act->get_nonce() - 1);
                    };
                }
                default:
                    break;
            }
        }
        return true;
    }

    auto connector_account::commit_extensions_node_to_leveldb(const connector_account::Extension_Element_TP &extension_nodes) -> bool {
        //先进行hash计算
        uint256 hash = account_mpt.get_hash();
        if (this->extensions_db != nullptr){
            leveldb::WriteBatch batch;
            for (const auto &extension : extension_nodes) {
                switch (extension.second.operator_type) {
                    case mpt_node_storage<merkle_patricia_trie<account>::CHILDREN_COUNT>::UPDATE_OP_MUSE:{
                        muse::serializer::BinarySerializer serializer;
                        serializer.input(extension.second);
                        leveldb::Slice slice(serializer.getBinaryStream(), serializer.byteCount());
                        batch.Put(extension.first, slice);
                        break;
                    }
                    case mpt_node_storage<merkle_patricia_trie<account>::CHILDREN_COUNT>::INSERT_OP_MUSE:{
                        muse::serializer::BinarySerializer serializer;
                        serializer.input(extension.second);
                        leveldb::Slice slice(serializer.getBinaryStream(), serializer.byteCount());
                        batch.Put(extension.first, slice);
                        break;
                    }
                    case mpt_node_storage<merkle_patricia_trie<account>::CHILDREN_COUNT>::DELETE_OP_MUSE:{
                        batch.Delete(extension.first);
                        break;
                    }
                    default:
                        break;
                }
            }
            leveldb::Status status = extensions_db->Write(leveldb::WriteOptions(), &batch);
            return status.ok();
        };
        return false;
    }

    connector_account::connector_account(leveldb::DB *_extensions_db, leveldb::DB *_accounts_db, leveldb::DB *_assets_db, const application_state& _state)
    :extensions_db(_extensions_db),
    accounts_db(_accounts_db),
    assets_db(_assets_db),
    block_height(_state.current_height),
    top_block_hash(_state.top_block_hash),
    account_mpt(_extensions_db,_accounts_db){
        if (block_height > 0){
            //区块hash + 区块高度
            if (this->assets_db != nullptr){
                std::string key( _state.top_block_hash.get_hex()) ;
                key += "___";
                key += std::to_string(_state.current_height);
                std::string out_data;
                leveldb::Status status = assets_db->Get(leveldb::ReadOptions(), key, &out_data);
                if (status.ok()){
                    bloom_filter_store store;
                    muse::serializer::BinaryDeserializeView viewer;
                    viewer.reset_bytestream(out_data.c_str(), out_data.size());
                    viewer.output(store);
                    this->asset_bloom_filter = store;
                    this->merkle_patricia_trie_root_hash = this->account_mpt.get_hash();
                }else{
                    SPDLOG_ERROR("connector_account constructor read assets bloom filter failed, des: {}", status.ToString());
                }
            }else{
                throw chain_db_exception("assets_db is nullptr", db_error_code::leveldb_pointer_is_nullptr);
            }
        }else{
            //首个区块为empty
            this->asset_bloom_filter.set_block_hash(this->top_block_hash);
            this->asset_bloom_filter.set_block_height(this->block_height);
            this->merkle_patricia_trie_root_hash = this->account_mpt.get_hash();
        }
    }

    auto connector_account::commit_account_to_leveldb(const leveldb_account_commit &lac) -> bool {
        if (this->accounts_db != nullptr){
            leveldb::WriteBatch batch;
            for(const auto& commit: lac.commits){
                leveldb::Slice value_slice(commit.second.getBinaryStream(), commit.second.byteCount());
                leveldb::Slice key_slice(reinterpret_cast<const char*>(commit.first.get_data_const()), uint256::Width);
                batch.Put(key_slice, value_slice);
            }
            leveldb::Status status = this->accounts_db->Write(leveldb::WriteOptions(), &batch);
            if (status.ok()){
                return true;
            }else{
                //记录错误原因
                return false;
            }
        }else{
            return false;
        }
    }

    /* bloom filter key is  hash_height */
    auto connector_account::commit_asset_bloom_filter_to_leveldb(const bloom_filter &blf) -> bool {
        if (this->assets_db != nullptr){
            bloom_filter_store store = blf.to_store();
            muse::serializer::BinarySerializer serializer;
            serializer.input(store);
            leveldb::Slice value_slice(serializer.getBinaryStream(), serializer.byteCount());
            //区块hash + 区块高度
            std::string key( store.block_hash.get_hex()) ;
            key += "___";
            key += std::to_string(blf.get_block_height());

            leveldb::Status status = this->assets_db->Put(leveldb::WriteOptions(), key, value_slice);
            if (status.ok()){
                return true;
            }
            SPDLOG_ERROR("connector_account::store_asset_bloom_filter_to_leveldb levelDB Error Des: {}", status.ToString());
            return false;
        }
        return false;
    }

    auto connector_account::package_next_block_MPTR(block &blk) noexcept -> bool {
        if (blk.header.prev_block_hash == top_block_hash){
            try {
                bool res = insert_block(blk, false);
                if (res){
                    Extension_Element_TP ee_tp;
                    //修改hash
                    blk.header.merkle_patricia_trie_root = account_mpt.get_hash();
                    auto& gc_store = this->account_mpt.get_change_store();
                    gc_store.swap(ee_tp);
                    //存储中间节点变化的数据
                    extensions.emplace_back(std::move(ee_tp));
                    return true;
                }
            } catch(chain_db_exception &db_ex) {
                SPDLOG_ERROR("connector_account::package_next_block_MPTR chain_db_exception {} : {}", std::to_string(db_ex.get_db_error_code()), db_ex.what());
                return false;
            } catch (std::exception &ex) {
                SPDLOG_ERROR("connector_account::package_next_block_MPTR std::exception {}", ex.what());
                return false;
            } catch(...){
                SPDLOG_ERROR("connector_account::package_next_block_MPTR no catch explicit!");
                return false;
            }
        }else{
            return false;
        }
        return false;
    }

    connector_account::connector_account(connector_account &&other) noexcept
    :account_mpt(std::move(other.account_mpt)),
    asset_bloom_filter(std::move(other.asset_bloom_filter)),
    merkle_patricia_trie_root_hash(std::move(other.merkle_patricia_trie_root_hash)),
    block_height(other.block_height),
    top_block_hash(std::move(other.top_block_hash)),
    account_bloom_filters(std::move(other.account_bloom_filters)),
    commits(std::move(other.commits)),
    rollbacks(std::move(other.rollbacks)),
    extensions(std::move(other.extensions)),
    extensions_db(other.extensions_db),
    accounts_db(other.accounts_db),
    assets_db(other.assets_db)
    {


    }

    auto connector_account::operator=(connector_account &&other) noexcept -> connector_account & {
        if (this != &other){
            account_mpt = std::move(other.account_mpt);
            asset_bloom_filter = std::move(other.asset_bloom_filter);
            merkle_patricia_trie_root_hash = std::move(other.merkle_patricia_trie_root_hash);
            block_height = other.block_height;
            top_block_hash = std::move(other.top_block_hash);
            account_bloom_filters = std::move(other.account_bloom_filters);
            commits = std::move(other.commits);
        }
        return *this;
    }

    auto connector_account::total_commit_to_leveldb() -> bool {
        if (this->extensions_db != nullptr){
            //中间扩展节点
            if (!this->extensions.empty()){
                while (!this->extensions.empty()){
                    auto extensions_tp = std::move(this->extensions.front());
                    this->extensions.pop_front();
                    if (!this->commit_extensions_node_to_leveldb(extensions_tp)){
                        SPDLOG_INFO("connector_account::total_commit_to_leveldb commit extensions node failed!");
                        return false;
                    }else{
                        SPDLOG_INFO("connector_account::total_commit_to_leveldb commit extensions node successfully!");
                    }
                }
            }
            //账户数据
            while (!this->commits.empty()){
                auto lac_tp = std::move(this->commits.front());
                this->commits.pop_front();
                if (!this->commit_account_to_leveldb(lac_tp)){
                    return false;
                }
            }
            //资产数据
            if (!this->account_bloom_filters.empty()){
                auto blf_tp = std::move(this->account_bloom_filters.front());
                this->account_bloom_filters.pop_front();
                if (!this->commit_asset_bloom_filter_to_leveldb(blf_tp)){
                    return false;
                }
            }
            //删除回滚点
            while (!this->rollbacks.empty()){
                this->rollbacks.pop_front();
            }
            return true;
        }
        return false;
    }

    auto connector_account::total_rollback() -> bool {
        bool result = rollback(1);
        if (result){
            this->merkle_patricia_trie_root_hash = this->account_mpt.get_hash();
            auto& commit = this->commits.front();
            if (this->merkle_patricia_trie_root_hash != commit.prev_mpt_root){
                SPDLOG_ERROR("The root hash of the Merkle tree cannot be restored. current height: ", commit.height + 1);
                throw chain_db_exception("rollback occur merkle_patricia_trie_root_hash cant recover before!", db_error_code::root_hash_of_the_MPT_cannot_be_restored);
            }
            this->block_height = this->block_height - 1;
            this->top_block_hash = commit.block_hash;
            commits.pop_back();
            this->extensions.pop_back();

            //获得原来的bloom_filter
            this->asset_bloom_filter = account_bloom_filters.back();
            this->account_bloom_filters.pop_back();
            return true;
        }

        return false;
    }

    auto connector_account::get_block_height() const -> uint64_t {
        return this->block_height;
    }

    auto connector_account::get_top_block_hash() const -> uint256 {
        return this->top_block_hash;
    }

    auto connector_account::find_account(const uint256 &address)->account* {
        return this->account_mpt.search(address);
    }

    connector_account::~connector_account(){
    };
}
