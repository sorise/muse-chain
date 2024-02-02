

创建 20个密钥对，然后存放到文件中！
```cpp
for (int i = 0; i < 20; ++i) {
    auto key = ecc_secp256k1::get_key();
    std::string pri_file_name("/home/remix/chain/initial_pri_keys/private_key");
    pri_file_name += "_";
    pri_file_name += std::to_string(i);
    pri_file_name += ".pem";

    std::string pub_file_name("/home/remix/chain/initial_pri_keys/public_key");
    pub_file_name += "_";
    pub_file_name += std::to_string(i);
    pub_file_name += ".pub";

    bool pri_result = ecc_secp256k1::store_private_key_pem(key.get(), pri_file_name, "123456");
    bool pub_result = ecc_secp256k1::store_public_key_pem(key.get(), pub_file_name);
    std::cout << std::boolalpha << pri_result << "[:]" << pub_result << "\n";
}
```