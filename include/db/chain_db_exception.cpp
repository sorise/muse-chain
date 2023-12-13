//
// Created by 14270 on 2024-01-02.
//

#include "chain_db_exception.hpp"

namespace muse::chain{

    chain_db_exception::chain_db_exception(const std::string &arg, db_error_code err)
    :std::runtime_error(arg),
    error_code(err){

    }

    auto chain_db_exception::get_db_error_code() const -> db_error_code {
        return error_code;
    }

    auto chain_db_exception::operator=(const chain_db_exception &other) -> chain_db_exception & {
        if (this != &other){
            this->error_code = other.error_code;
        }
        return *this;
    }

    chain_db_exception::chain_db_exception(const chain_db_exception &_c_d_ex):
    std::runtime_error(_c_d_ex.what()),
    error_code(_c_d_ex.error_code){

    }
}