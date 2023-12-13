//
// Created by 14270 on 2024-01-05.
//

#include "rollback_operation.hpp"

namespace muse::chain{

    auto rollback_operation::to_string() const -> std::string {
        std::string str;
        switch (type) {
            case INSERT:
            {
                str += "INSERT \n";
            }
                break;
            case DELETE_ACCOUNT:
            {
                str += "DELETE_ACCOUNT \n";
                str += target_address.get_hex()  + "\n";
                str += "count: " + std::to_string(value)  + "\n";
            }
                break;
            case UPDATE:
            {
                str += "UPDATE ";
                if (update_type == UPDATE_ADD){
                    str += "ADD \n";
                    str += target_address.get_hex()  + "\n";
                    str += "count: " + std::to_string(value)  + "\n";
                    str += "asset type: " + std::to_string(asset_type)  + "\n";
                }else if(update_type == UPDATE_REDUCE){
                    str += "REDUCE \n";
                    str += target_address.get_hex()  + "\n";
                    str += "count: " + std::to_string(value)  + "\n";
                    str += "asset type: " + std::to_string(asset_type)  + "\n";
                }else if(update_type == UPDATE_DELETE_ASSET){
                    str += "DELETE_ASSET \n";
                    str += target_address.get_hex()  + "\n";
                    str += "asset type: " + std::to_string(asset_type)  + "\n";
                }
            }
                break;
        }
        return str;
    }

}