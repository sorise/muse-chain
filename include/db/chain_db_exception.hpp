//
// Created by 14270 on 2024-01-02.
//

#ifndef GLORIA_CHAIN_DB_EXCEPTION_HPP
#define GLORIA_CHAIN_DB_EXCEPTION_HPP
#include <exception>
#include <stdexcept>



namespace muse::chain{
    #define ERROR_CODE_DB_TYPE int

    enum db_error_code: uint8_t {
        asset_already_exist = 1,  /* 资产已经存在 */
        establish_transaction_no_output = 2,  /* 资产已经存在 */
        account_no_exist = 3,  /* 账户不存在 */
        memory_not_enough = 4,  /* 内存不足 */
        atomic_operation_failed = 5,  /* 原子操作失败 */
        transaction_operator_error = 6, /* 不支持的交易类型*/
        invalid_account_address = 7,  /* 账户地址错误 */
        invalid_patricia_merkle_tree_root_hash = 8, /* merkle树根错误 */
        leveldb_pointer_is_nullptr = 9, /* 数据库指针为空指针 */
        root_hash_of_the_MPT_cannot_be_restored = 10,
    };

    class chain_db_exception: public std::runtime_error {
    public:
        /* 构造函数 */
        chain_db_exception(const std::string &arg, db_error_code err);
        /* 复制构造函数 */
        chain_db_exception(const chain_db_exception& _c_d_ex);
        //移动构造函数
        chain_db_exception(chain_db_exception&& _c_d_ex) noexcept = delete;

        ~chain_db_exception() override = default;

        auto operator=(const chain_db_exception& _c_d_ex) -> chain_db_exception &;

        auto operator=(chain_db_exception&& _c_d_ex) noexcept -> chain_db_exception & = delete;

        //返回错误号
        [[nodiscard]] auto get_db_error_code() const -> db_error_code;
    private:
        db_error_code error_code;
    };
}
#endif //GLORIA_CHAIN_DB_EXCEPTION_HPP
