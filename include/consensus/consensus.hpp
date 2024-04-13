#ifndef MUSE_CHAIN_CONSENSUS_HPP
#define MUSE_CHAIN_CONSENSUS_HPP
#include <iostream>
#include <cstdio>

namespace muse::chain{
    enum class consensus_algorithm: uint8_t{
        PBFT = 0,
        SRRC = 1
    };

    class consensus {
    public:
        consensus() = default;

        void run(consensus_algorithm _algorithm);
    };
}

#endif //MUSE_CHAIN_CONSENSUS_HPP
