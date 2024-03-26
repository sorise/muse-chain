//
// Created by remix on 24-3-26.
//

#include "interface_db.hpp"

namespace muse::chain{
    std::unique_ptr<connector_block> interface_db::cbk = nullptr;

    std::unique_ptr<connector_account> interface_db::cat = nullptr;


}