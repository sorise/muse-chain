#define OPENSSL_API_COMPAT 0x10100000L
#define LIBC_NAMESPACE _llvm_libc_muse_

#include <iostream>
#include <vector>

#include "rpc/rpc.hpp"
#include "package.hpp"
#include "application/application.hpp"
#include "util/timebox.hpp"
#include "util/logger.hpp"
#include "util/argparse.hpp"
#include "db/interface_db.hpp"

#include "nlohmann/json.hpp"

using namespace muse::chain;
using namespace nlohmann;

int main(int argc, char const *argv[]){
    //参数解析
    auto args = muse::utils::argparser("muse-blockchain program.");
    args.set_program_name("muse")
    .add_help_option()
    .add_option<std::string>("-p", "--password", "Please enter the password that resolves the node's private key.", "")
    .add_option<std::string>("-s", "--setting", "Please enter the path to the configuration file.", "")
    .parse(argc, argv);
    //私钥密码 和 配置文件地址
    auto password = args.get_option<std::string>("-p");
    auto path = args.get_option<std::string>("-s");

/*
    muse::chain::application chain;
    //加载配置文件
    bool success = chain.load_setting_file(path);
    if (!success){
        fmt::print("load setting file failed!");
        return 1;
    }
    //注册解压缩中间件
    muse::rpc::Disposition::Server_Configure(2,4,4096,5000ms,chain.get_chain_ini().log_file_path, true);
    //链初始化
    chain.initialize();
    //加载结点公私钥
    chain.load_node_keys(password);
    //插入数据块
    singleton_lazy_heap<interface_db>::get_ptr()->inject_levelDB(chain);
*/

//  fmt::print("{}\n", singleton_lazy_heap<interface_db>::get_reference().account_mpt_hash().get_hex());
    // create an array using push_back
    json j;
    j.push_back("foo");
    j.push_back(1);
    j.push_back(true);
    // also use emplace_back
    j.emplace_back(1.78);

    // iterate the array
    for (json::iterator it = j.begin(); it != j.end(); ++it) {
        std::cout << *it << '\n';
    }

// range-based for
    for (auto& element : j) {
        std::cout << element << '\n';
    }

// getter/setter
    const auto tmp = j[0].template get<std::string>();
    j[1] = 42;
    bool foo = j.at(2);

// comparison
    j == R"(["foo", 1, true, 1.78])"_json;  // true

// other stuff
    j.size();     // 4 entries
    j.empty();    // false
    j.type();     // json::value_t::array
    j.clear();    // the array is empty again

// convenience type checkers
    j.is_null();
    j.is_boolean();
    j.is_number();
    j.is_object();
    j.is_array();
    j.is_string();

// create an object
    json o;
    o["foo"] = 23;
    o["bar"] = false;
    o["baz"] = 3.141;

// also use emplace
    o.emplace("weather", "sunny");

// special iterator member functions for objects
    for (json::iterator it = o.begin(); it != o.end(); ++it) {
        std::cout << it.key() << " : " << it.value() << "\n";
    }

// the same code as range for
    for (auto& el : o.items()) {
        std::cout << el.key() << " : " << el.value() << "\n";
    }

// even easier with structured bindings (C++17)
    for (auto& [key, value] : o.items()) {
        std::cout << key << " : " << value << "\n";
    }

// find an entry
    if (o.contains("foo")) {
        // there is an entry with key "foo"
    }

// or via find and an iterator
    if (o.find("foo") != o.end()) {
        // there is an entry with key "foo"
    }

// or simpler using count()
    int foo_present = o.count("foo"); // 1
    int fob_present = o.count("fob"); // 0

// delete an entry
    o.erase("foo");
    return 0;
}


