//
// Created by 14270 on 2024-01-01.
//

#include "bloom_filter.hpp"
#include "bloom_filter_store.hpp"


#include <utility>

namespace muse::chain{
    auto bloom_filter::murmur_hash2_x64 (const void * key, uint32_t len, uint32_t seed ) -> uint64_t
    {
        const uint64_t m = 0xc6a4a7935bd1e995;
        const int r = 47;
        uint64_t h = seed ^ (len * m);
        const uint64_t * data = (const uint64_t *)key;
        const uint64_t * end = data + (len/8);
        while(data != end)
        {
            uint64_t k = *data++;
            k *= m;
            k ^= k >> r;
            k *= m;
            h ^= k;
            h *= m;
        }
        const uint8_t * data2 = (const uint8_t*)data;
        switch(len & 7)
        {
            case 7: h ^= ((uint64_t)data2[6]) << 48;
            case 6: h ^= ((uint64_t)data2[5]) << 40;
            case 5: h ^= ((uint64_t)data2[4]) << 32;
            case 4: h ^= ((uint64_t)data2[3]) << 24;
            case 3: h ^= ((uint64_t)data2[2]) << 16;
            case 2: h ^= ((uint64_t)data2[1]) << 8;
            case 1: h ^= ((uint64_t)data2[0]);
                h *= m;
        };
        h ^= h >> r;
        h *= m;
        h ^= h >> r;
        return h;
    }

    auto bloom_filter::md5_x64(const void * key, uint32_t len, uint32_t seed) -> uint64_t{
        constexpr const uint32_t length = 17;
        unsigned char store[length]{'\0'};
        hash_handler::md5(static_cast<const char *>(key), len, store);
        uint64_t top = (*reinterpret_cast<uint64_t*>(store)) * seed;
        uint64_t bottom = (*reinterpret_cast<uint64_t*>(store + 8)) * seed;
        return top ^ bottom;
    }

    auto bloom_filter::get_hash() -> uint256 {
        bloom_filter_store store = this->to_store();
        uint256 result;
        hash_handler::sha3_256(store.bloom_filter_data.c_str(), store.bloom_filter_data.size(), result.get_data());
        return result;
    }

    auto bloom_filter::insert(const uint32_t& value) -> bool {
        std::array<uint64_t, BLOOM_FILTER_HASH_COUNT> place = get_bits_places(value);
        bool flag = true;
        for(uint32_t i = 0; i < BLOOM_FILTER_HASH_COUNT; ++i){
            flag = flag && asset.test(place.at(i));
        };
        //如果已经存在了
        if (flag){
            return false;
        };
        for(uint32_t i = 0; i < BLOOM_FILTER_HASH_COUNT; ++i){
            asset.set(place.at(i), true);
        };
        return true;
    }

    auto bloom_filter::get_bits_places(const uint32_t& value) -> std::array<uint64_t, BLOOM_FILTER_HASH_COUNT> {
        std::array<uint64_t, BLOOM_FILTER_HASH_COUNT> result {0};
        result[0] = murmur_hash2_x64(&value, sizeof(value), seeds[0]) % BLOOM_FILTER_SIZE;
        result[1] = murmur_hash2_x64(&value, sizeof(value), seeds[1]) % BLOOM_FILTER_SIZE;
        result[2] = murmur_hash2_x64(&value, sizeof(value), seeds[2]) % BLOOM_FILTER_SIZE;
        result[3] = murmur_hash2_x64(&value, sizeof(value), seeds[3]) % BLOOM_FILTER_SIZE;
        result[4] = md5_x64(&value, sizeof(value), seeds[4]) % BLOOM_FILTER_SIZE;
        result[5] = md5_x64(&value, sizeof(value), seeds[5]) % BLOOM_FILTER_SIZE;
        result[6] = md5_x64(&value, sizeof(value), seeds[6]) % BLOOM_FILTER_SIZE;
        result[7] = md5_x64(&value, sizeof(value), seeds[7]) % BLOOM_FILTER_SIZE;
        return result;
    }

    auto bloom_filter::test(const uint32_t &value) const -> bool {
        std::array<uint64_t, BLOOM_FILTER_HASH_COUNT> place = get_bits_places(value);
        return asset.test(place[0]) && asset.test(place[1]) && asset.test(place[2]) && asset.test(place[3]) &&
               asset.test(place[4]) && asset.test(place[5]) && asset.test(place[6]) && asset.test(place[7]);
    }

    bloom_filter::bloom_filter(const bloom_filter &other)
    :asset(other.asset),
    block_height(other.block_height),
    block_hash(other.block_hash)
    {

    }

    auto bloom_filter::to_string() const -> std::string{
        return asset.to_string();
    }

    auto bloom_filter::to_store() const -> bloom_filter_store {
        //创建一个缓冲区用于存储位数据，并初始化为零
        std::string buffer((asset.size() + 7) / 8, '\0');
        //将bitset的数据转换为字节序列
        for (size_t i = 0; i < asset.size(); ++i) {
            if (asset[i]) {
                buffer[i / 8] |= (1 << (i % 8));
            }
        }

        bloom_filter_store result{
            this->block_height,
            this->block_hash,
            buffer
        };
        return result;
    }

    auto bloom_filter::operator=(const bloom_filter &other) -> bloom_filter & {
        if (&other != this){
            this->asset = other.asset;
            this->block_height = other.block_height;
            this->block_hash = other.block_hash;
        }
        return *this;
    }

    bloom_filter::bloom_filter(bloom_filter &&other) noexcept
    :asset(other.asset), block_height(other.block_height), block_hash(other.block_hash){

    }


    auto bloom_filter::operator=(bloom_filter &&other) noexcept -> bloom_filter & {
        if (&other!= this){
            std::swap(asset, other.asset);
            std::swap(block_height, other.block_height);
            std::swap(block_hash, other.block_hash);
        }
        return *this;
    }

    bloom_filter::bloom_filter(): block_height(0), block_hash(0) {

    }

    bloom_filter::bloom_filter(const bloom_filter_store &other)
    :block_height(other.block_height),
    block_hash(other.block_hash){
        //将bitset的数据转换为字节序列
        // 将字节数据转换回std::bitset对象的位数据
        for (size_t i = 0; i < BLOOM_FILTER_SIZE; ++i) {
            if (other.bloom_filter_data[i / 8] & (1 << (i % 8))) {
                asset.set(i);
            }
        }
    }

    auto bloom_filter::operator==(const bloom_filter &other) const -> bool {
        for (size_t i = 0; i < BLOOM_FILTER_SIZE; ++i) {
            if (this->asset.test(i) != other.asset.test(i)){
                return false;
            }
        }
        return true;
    }

    auto bloom_filter::get_block_height() const -> const uint64_t & {
        return this->block_height;
    }

    auto bloom_filter::get_block_hash() const -> const uint256 & {
        return this->block_hash;
    }

    auto bloom_filter::set_block_height(const uint64_t &height) -> void {
        this->block_height = height;
    }

    auto bloom_filter::set_block_hash(const uint256 &hash) -> void {
        this->block_hash = hash;
    }

    auto bloom_filter::operator=(const bloom_filter_store &store) -> void {
        std::bitset<BLOOM_FILTER_SIZE> assets_new;
        std::swap<std::bitset<BLOOM_FILTER_SIZE>>(this->asset, assets_new);
        for (size_t i = 0; i < BLOOM_FILTER_SIZE; ++i) {
            if (store.bloom_filter_data[i / 8] & (1 << (i % 8))) {
                asset.set(i);
            }
        }
        this->block_height = store.block_height;
        this->block_hash = store.block_hash;
    }

}