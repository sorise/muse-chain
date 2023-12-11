//
// Created by remix on 23-10-7.
//

#include "out_entry.hpp"

namespace muse::chain{
    out_entry::out_entry(): receiver(), count() {

    }
    out_entry::out_entry(const uint256 &other, const uint64_t &count):receiver(other), count(count) {

    }
    out_entry::out_entry(const out_entry &other): receiver(other.receiver), count(other.count) {

    }
}
