#include "interface_db.hpp"

namespace muse::chain{
    auto interface_db::check_asset_exist(const uint32_t &asset_id, uint256 block_hash) -> bool {
        return false;
    }

    auto interface_db::account_mpt_hash() -> uint256 {
        return cat->account_mpt.get_hash();
    }

    auto interface_db::check_transaction_pool() -> bool {
        return false;
    }

    bool interface_db::check_block_legitimacy(block &blk) {
        return false;
    }

    bool interface_db::check_transaction_legitimacy(transaction &tsn) {
        return false;
    }

    bool interface_db::check_affair_legitimacy(affair &afi) {
        return false;
    }

    auto interface_db::inject_levelDB( application &chain) -> void {
        //区块 levelDB 数据库指针
        this->blocks_db = std::unique_ptr<leveldb::DB>(muse::chain::application::get_blocks_db(chain.get_chain_ini().block_db_path));
        //链状态 levelDB 数据库指针
        this->chain_db = std::unique_ptr<leveldb::DB>(muse::chain::application::get_chain_db(chain.get_chain_ini().chain_state_db_path));
        //分支节点 levelDB 数据库指针
        this->extensions_db = std::unique_ptr<leveldb::DB>(muse::chain::application::get_extensions_db(chain.get_chain_ini().extensions_db_path));
        //MPT 叶子节点 levelDB 数据库指针
        this->accounts_db = std::unique_ptr<leveldb::DB>(muse::chain::application::get_accounts_db(chain.get_chain_ini().account_db_path));
        //资产 levelDB 数据库指针
        this->assets_db = std::unique_ptr<leveldb::DB>(muse::chain::application::get_assets_db(chain.get_chain_ini().assets_db_path));
        //判断 是否存在空节点
        if (blocks_db == nullptr ||chain_db == nullptr || extensions_db == nullptr || accounts_db == nullptr || assets_db == nullptr){
            SPDLOG_ERROR("levelDB db pointer is empty!");
        }
        //管理区块数据的外层接口
        this->cbk = std::make_unique<connector_block>(blocks_db.get(),chain_db.get(),chain.get_application_state());
        this->cat = std::make_unique<connector_account>(extensions_db.get(),accounts_db.get(),assets_db.get(), chain.get_application_state());
    }
}