//
// Created by 14270 on 2024-01-05.
//

#include "receipt.hpp"

namespace muse::chain{

    auto receipt::check_nonce() -> bool {
        if (nonce.empty()) {
            return true;
        }
        if (nonce.size() > 1){
            std::sort(nonce.begin(), nonce.end());
            if (nonce.front()!= nonce_start + 1) {
                return false;
            }
            for (int i = 1; i < nonce.size(); ++i) {
                if (nonce[i]!= nonce[i - 1] + 1) {
                    return false;
                }
            }
        }else{
            return nonce.front() == nonce_start + 1;
        }
        return true;
    }

    auto receipt::check() -> bool {
        if (!check_nonce()) return false;
        return std::all_of(balances.begin(), balances.end(), [](auto &balance)->bool {
            return balance.second.first >= balance.second.second;
        });
    }

    receipt::receipt(const uint64_t &start):nonce_start(start) {

    }

    receipt::receipt():nonce_start(0) {

    }

    auto receipt::operator=(const receipt &rcp) -> receipt & {
        this->nonce_start = rcp.nonce_start;
        this->nonce = rcp.nonce;
        this->balances = rcp.balances;
        return *this;
    }

    receipt::receipt(receipt &&other) noexcept
    :balances(std::move(other.balances)),nonce_start(other.nonce_start),nonce(std::move(other.nonce)){

    }

    auto receipt::operator=(receipt &&other) noexcept -> receipt & {
        if (this != &other){
            this->nonce_start = other.nonce_start;
            this->balances = std::move(other.balances);
            this->nonce = std::move(other.nonce);
        }
        return *this;
    }

    auto receipt::input_and_check(transaction &trans) -> bool {
        return true;
    }
}