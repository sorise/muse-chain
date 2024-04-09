#ifndef MUSE_CHAIN_INTERFACE_DB_HPP
#define MUSE_CHAIN_INTERFACE_DB_HPP

#include <memory>
#include <shared_mutex>
#include "util/singleton.hpp"
#include "connector_block.hpp"
#include "connector_account.hpp"
#include "bloom_filter.hpp"

namespace muse::chain{
    //对外接口，需要实现线程安全
    class interface_db {
    private:
        //区块数据
        std::unique_ptr<connector_block> cbk {nullptr};
        //数据层指针
        std::unique_ptr<connector_account> cat {nullptr};
        //区块 levelDB 数据库指针
        std::unique_ptr<leveldb::DB> blocks_db {nullptr};
        //链状态 levelDB 数据库指针
        std::unique_ptr<leveldb::DB> chain_db {nullptr};
        //MPT 分支节点 levelDB 数据库指针
        std::unique_ptr<leveldb::DB> extensions_db {nullptr};
        //MPT 叶子节点 levelDB 数据库指针
        std::unique_ptr<leveldb::DB> accounts_db {nullptr};
        //资产 levelDB 数据库指针
        std::unique_ptr<leveldb::DB> assets_db {nullptr};

        std::shared_mutex connector_block_mtx;
        std::shared_mutex connector_account_mtx;
    public:
        auto inject_levelDB(muse::chain::application &chain) ->void;
        /* 检查某个资产是否存在 */
        auto check_asset_exist(const uint32_t &asset_id, uint256 block_hash) -> bool;

        /* 检查是否可以生产一个区块 */
        auto check_transaction_pool() -> bool;

        /* 验证一个区块是否合法*/
        bool check_block_legitimacy(block &blk);

        /* 验证一个交易的合法性 */
        bool check_transaction_legitimacy(transaction& tsn);

        /* 检查一个事务的合法性 */
        bool check_affair_legitimacy(affair& afi);

        auto account_mpt_hash() -> uint256;

        auto get_chain_height() -> uint64_t;

        auto transaction_pool_waiting_count() -> uint64_t;
    };
}
#endif //MUSE_CHAIN_INTERFACE_DB_HPP