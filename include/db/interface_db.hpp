//
// Created by remix on 24-3-26.
//

#ifndef MUSE_CHAIN_INTERFACE_DB_HPP
#define MUSE_CHAIN_INTERFACE_DB_HPP

#include <memory>

#include "connector_block.hpp"
#include "connector_account.hpp"
#include "bloom_filter.hpp"

namespace muse::chain{
    class interface_db {
    private:
        //数据层指针
        static std::unique_ptr<connector_block> cbk;

        static std::unique_ptr<connector_account> cat;
    public:
        /* 检查某个资产是否存在 */
        auto check_asset_exist(const uint32_t &asset_id, uint256 block_hash) -> bool;

        /* 检查是否可以生产一个区块 */
        static bool check_transaction_pool();

        /* 验证一个区块是否合法*/
        static bool check_block_legitimacy(block &blk);

        /* 验证一个交易的合法性 */
        static bool check_transaction_legitimacy(transaction& tsn);

        /* 检查一个事务的合法性 */
        static bool check_affair_legitimacy(affair& afi);
    };
}


#endif //MUSE_CHAIN_INTERFACE_DB_HPP
