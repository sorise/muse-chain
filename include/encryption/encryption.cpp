//
// Created by remix on 23-11-21.
//

#include "encryption.hpp"
namespace muse::chain{
    std::shared_ptr<hash_base> encryption::get_hash_base() {
        static std::shared_ptr<hash_base> instance(new sha3_256(), [](sha3_256 *ptr){
            delete ptr;
        });
        return instance;
    }
}
