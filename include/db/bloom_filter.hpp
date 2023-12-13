//
// Created by 14270 on 2024-01-01.
//

#ifndef GLORIA_BLOOM_FILTER_HPP
#define GLORIA_BLOOM_FILTER_HPP

#include <bitset>
#include <iostream>

#include "encryption/hash_handler.hpp"
#include "IGetHashSha3256.hpp"
#include "bloom_filter_store.hpp"

namespace muse::chain{
    //64K 50000,1 in 152
    class bloom_filter: public IGetHashSha3256{
    public:
        //哈希函数个数
        constexpr static const uint32_t BLOOM_FILTER_HASH_COUNT = 8;

        constexpr static const uint32_t BLOOM_FILTER_SIZE = 524288; //65K

        constexpr static const std::array<uint64_t, BLOOM_FILTER_HASH_COUNT> seeds{
              95207074, 95671760, 71937200, 69017398,
              79226824, 56713949, 29781914, 45285662
        };
    private:
        std::bitset<BLOOM_FILTER_SIZE> asset;

        uint64_t block_height;              //区块高度

        uint256 block_hash;                 //区块hash

        /* 内容写入到文件中 */
        template <size_t N>
        auto write_bitset_to_file(const std::bitset<N>& bits, const std::string& filename) -> void;

        template <size_t N>
        auto read_bitset_from_file(const std::string& filename) -> std::bitset<N>;

        static auto get_bits_places(const uint32_t& value) -> std::array<uint64_t, BLOOM_FILTER_HASH_COUNT>;

        static auto murmur_hash2_x64 ( const void * key, uint32_t len, uint32_t seed ) -> uint64_t;

        static auto md5_x64( const void * key, uint32_t len, uint32_t seed) -> uint64_t ;
    public:
        bloom_filter();

        bloom_filter(const bloom_filter& other);

        explicit bloom_filter(const bloom_filter_store& other);

        bloom_filter(bloom_filter&& other) noexcept ;

        auto operator=(const bloom_filter& other) -> bloom_filter&;

        auto operator=(bloom_filter&& other) noexcept -> bloom_filter&;

        auto operator=(const bloom_filter_store& store) -> void;

        auto operator==(const bloom_filter& other) const -> bool;

        virtual ~bloom_filter() = default;

        auto get_hash() -> uint256 override;

        /* 插入一个值 */
        auto insert(const uint32_t& value) -> bool;

        [[nodiscard]] auto test(const uint32_t& value) const -> bool;

        [[nodiscard]] auto to_string() const -> std::string;

        [[nodiscard]] auto to_store() const -> bloom_filter_store;

        [[nodiscard]] auto get_block_height() const -> const uint64_t&;

        [[nodiscard]] auto get_block_hash() const -> const uint256&;

        auto set_block_height(const uint64_t &height) -> void;

        auto set_block_hash(const uint256 &hash) -> void;

    };


    template<size_t N>
    auto bloom_filter::write_bitset_to_file(const std::bitset<N> &bits, const std::string &filename) -> void {
        // 创建一个缓冲区用于存储位数据，并初始化为零
        std::vector<char> buffer((bits.size() + 7) / 8, 0);

        // 将bitset的数据转换为字节序列
        for (size_t i = 0; i < bits.size(); ++i) {
            if (bits[i]) {
                buffer[i / 8] |= (1 << (i % 8));
            }
        }

        // 打开文件进行写操作
        std::ofstream file(filename, std::ios::out | std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file for writing.");
        }

        // 将字节数据写入文件
        file.write(buffer.data(), buffer.size());

        // 关闭文件
        file.close();
    }

    template<size_t N>
    auto bloom_filter::read_bitset_from_file(const std::string &filename) -> std::bitset<N>  {
        // 打开文件进行读操作
        std::ifstream file(filename, std::ios::in | std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file for reading.");
        }

        // 确定文件大小
        file.seekg(0, std::ios::end);
        size_t fileSize = file.tellg();
        file.seekg(0, std::ios::beg);

        // 创建一个缓冲区用于存储文件内容，并初始化为零
        std::vector<char> buffer(fileSize, 0);

        // 读取文件内容到缓冲区
        file.read(buffer.data(), fileSize);

        // 关闭文件
        file.close();

        // 创建一个空的std::bitset对象
        std::bitset<N> bits;

        // 将字节数据转换回std::bitset对象的位数据
        for (size_t i = 0; i < bits.size(); ++i) {
            if (buffer[i / 8] & (1 << (i % 8))) {
                bits.set(i);
            }
        }

        // 返回反序列化后的std::bitset对象
        return bits;
    }
}

#endif //GLORIA_BLOOM_FILTER_HPP
