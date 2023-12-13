//
// Created by 14270 on 2024-01-23.
//

#include "connector_block.hpp"

namespace muse::chain{

    connector_block::connector_block(leveldb::DB *_leveldb_blocks, leveldb::DB *_leveldb_chain, const application_state &_state)
    :leveldb_blocks (_leveldb_blocks),
    leveldb_chain(_leveldb_chain),
    current_height (_state.current_height),
    initial_block_hash(_state.initial_block_hash),
    top_block_hash (_state.top_block_hash){

    }

    auto connector_block::update_connector(uint64_t _current_height, uint256 _top_block_hash) -> void {
        this->current_height = _current_height;
        this->top_block_hash = _top_block_hash;
    }

    auto connector_block::get_current_height() const -> uint64_t {
        return this->current_height;
    }

    auto connector_block::get_top_block_hash() const -> uint256 {
        return this->top_block_hash;
    }

    auto connector_block::package_next_block_header(block &blk, const chain_ini& _ini,const std::vector<uint256> &blacks) -> bool {
        /* 数量约定 */
        if (blk.body.transactions.size() < _ini.transaction_minimum_number_in_block){
            return false;
        }
        if (blk.body.transactions.size() > _ini.transaction_maximum_number_in_block){
            return false;
        }
        auto affair_count = std::count_if(blk.body.affairs.begin(), blk.body.affairs.end(), [](affair &air)->bool {
            return air.is_db_level_affair_type();
        });
        if (affair_count < _ini.affair_minimum_number_in_block){
            return false;
        }
        if (affair_count > _ini.affair_maximum_number_in_block){
            return false;
        }
        blk.header.prev_block_hash = top_block_hash;
        blk.header.height = current_height + 1;
        if (!blk.body.affairs.empty()){
            std::queue<uint256> uint256_queue;
            std::for_each(blk.body.affairs.begin(), blk.body.affairs.end(), [&uint256_queue](auto &affair){
                uint256_queue.push(affair.get_hash());
            });
            blk.header.affairs_merkle_root = muse::chain::merkle_tree::get_merkle_root(std::move(uint256_queue));
        }
        if (!blk.body.transactions.empty()){
            std::queue<uint256> uint256_queue;
            std::for_each(blk.body.transactions.begin(), blk.body.transactions.end(), [&uint256_queue](auto &trans){
                uint256_queue.push(trans.get_hash());
            });
            blk.header.transactions_merkle_root = muse::chain::merkle_tree::get_merkle_root(std::move(uint256_queue));
        }
        if (!blacks.empty()){
            blk.header.black_block_hashes = std::move(blacks);
        }
        return true;
    }

    auto connector_block::get_block_by_index(const uint64_t& height, bool &success) const -> block {
        block blk;
        if (leveldb_blocks == nullptr){
            success = false;
        }else{
            std::string state_data_out;
            leveldb::Slice key(reinterpret_cast<const char *>(&height), sizeof(uint64_t));
            auto status = leveldb_blocks->Get(leveldb::ReadOptions(), key, &state_data_out);
            if (status.ok()){
                muse::serializer::BinaryDeserializeView viewer;
                viewer.reset_bytestream(state_data_out.c_str(), state_data_out.size());
                viewer.output(blk);
                success = true;
            }else{
                SPDLOG_ERROR("get_block_by_hash Failed status error des: {}", status.ToString());
                success = false;
            }
        }
        return blk;
    }

    auto connector_block::store_index_hash(const uint64_t& height, uint256 hash) const -> bool {
        //500个区块，一个索引文件
        if (this->leveldb_chain != nullptr){
            leveldb::Slice key(reinterpret_cast<const char *>(&height), sizeof(uint64_t));
            leveldb::Slice value(reinterpret_cast<char *>(hash.get_data()), uint256::Width);
            auto status=  this->leveldb_chain->Put(leveldb::WriteOptions(), key, value);
            if (status.ok()){
                return true;
            }
            SPDLOG_WARN("store_index_hash Failed status error des: {}", status.ToString()); //记录失败原因
        }
        return false;
    }

    auto connector_block::load_hash_by_index(const uint64_t &height,bool &success) const -> uint256 {
        if (this->leveldb_chain != nullptr){
            leveldb::Slice key(reinterpret_cast<const char *>(&height), sizeof(uint64_t));
            std::string out;
            auto status=  this->leveldb_chain->Get(leveldb::ReadOptions(), key, &out);
            success = status.ok();
            //记录失败原因
            if (success){
                uint256 result(out.c_str(), out.size());
                return result;
            }
            SPDLOG_WARN("load_hash_by_index Failed status error des: {}", status.ToString());
        }
        return {};
    }

    auto connector_block::insert_block_save_to_disk() -> bool {
        if (block_queue.empty()){
            return false;
        }
        bool result = insert_block_to_levelDB(block_queue.front());
        if (result){
            const block& blk = block_queue.front();
            //更新  application_state
            muse::chain::application_state app_state;
            app_state.current_height = blk.header.height;
            app_state.top_block_hash = blk.header.get_block_hash();
            app_state.initial_block_hash = this->initial_block_hash;
            muse::serializer::BinarySerializer serializer;
            serializer.input(app_state);
            leveldb::Slice out(serializer.getBinaryStream(),serializer.byteCount());
            leveldb::Status stt = leveldb_chain->Put(leveldb::WriteOptions(), application_state::STORE_STATE_KEY, out);
            if (stt.ok()){
                block_queue.pop_front();
                return true;
            }
            SPDLOG_ERROR("levelDB Put Error, error des: {}", stt.ToString());
            throw std::logic_error("levelDB Write Error");
        }
        return false;
    }

    auto connector_block::insert_block_to_levelDB(block &blk) -> bool {
        if (this->leveldb_blocks != nullptr){
            uint256 blk_hash = blk.header.get_block_hash();
            //序列化
            muse::serializer::BinarySerializer serializer;
            serializer.input(blk);
            //可以减少复制操作
            leveldb::Slice out(serializer.getBinaryStream(),serializer.byteCount());
            leveldb::Slice key(reinterpret_cast<const char *>(&blk.header.height), sizeof(uint64_t));
            leveldb::Status stt = leveldb_blocks->Put(leveldb::WriteOptions(), key, out);
            //存储区块链状态
            if (stt.ok()){
                SPDLOG_INFO("insert block {} successfully!", blk_hash.get_hex());
                return true;
            }
            SPDLOG_ERROR("levelDB Write block Error, error des: {}", stt.ToString());
            throw std::logic_error("levelDB Write Error");
        }
        return false;
    }

    auto connector_block::input_block_cache(block &&blk) -> void {
        block_queue.push_back(std::move(blk));
    }

    auto connector_block::input_block_cache(const block &blk) -> void {
        block_queue.push_back(blk);
    }


}