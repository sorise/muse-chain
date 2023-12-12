//
// Created by remix on 23-9-23.
//
#ifndef MUSE_CHAIN_UINT256_HPP
#define MUSE_CHAIN_UINT256_HPP

#include <iostream>
#include <string>
#include<cstring>
#include <array>
#include <assert.h>
#include <algorithm>
#include <exception>
#include <system_error>
#include "../serializer/IbinarySerializable.h"
#include "../encryption/hash_handler.hpp"

namespace muse::chain{
    //多少个字节
    template<unsigned int Bits>
    class base_binary : public muse::serializer::IBinarySerializable {
    public:
        /* 字节数量 */
        const static constexpr int Width = Bits / 8;
    protected:
        static_assert(Bits % 8 == 0, "base_binary currently only supports whole bytes.");
        std::array<uint8_t, Width> data;
        static_assert(Width == sizeof(data), "bytes length check");
    public:
        constexpr base_binary() : data() {};
        constexpr explicit base_binary(uint8_t v) : data{v} {
            for (int i = 0; i < Width; ++i) {
                data[i] = v;
            }
        };

        explicit base_binary(const std::string& vch)
        {
            //判断vch的长度是否合法
            if (vch.size() <= 2) throw std::invalid_argument("the vch length is invalid");
            if (vch[0] != '0' || vch[1] != 'x') throw std::invalid_argument("the vch length is invalid");
            if (vch.size() - 2 > Width) throw std::invalid_argument("the vch length is invalid");

            assert(vch.size() <= Width);
            int half = 0;
            for (int i = 2; i < vch.size(); ++i) {
                uint16_t val = 0;
                //判断是否是16进制
                if (isdigit(vch[i])){
                    val = vch[i] - '0';
                } else if (islower(vch[i])){
                    val = vch[i] - 'a' + 10;
                } else if (isupper(vch[i])){
                    val = vch[i] - 'A' + 10;
                } else {
                    throw std::invalid_argument("the value of the string do not comply with hexadecimal specifications");
                }
                if (val >= 16) throw std::invalid_argument("the value of the string do not comply with hexadecimal specifications");

                int byte_place = half / 2;
                if (half % 2 == 0){
                    //如果是偶数，则是高位
                    data[byte_place] &= 0;
                    val = val << 4;
                }//如果是奇数，则是低位
                data[byte_place] |= val;
                half++;
            }
            //此次不应该加1
            for (int i = half/2; i < Width; ++i) {
                data[i] = 0;
            }
        }

        explicit base_binary(const char *_data, uint32_t data_length)
        {
            assert(data_length == Width);
            std::copy(_data,_data + data_length, data.begin());
        }

        explicit base_binary(const unsigned char *_data, uint32_t data_length)
        {
            assert(data_length == Width);
            std::copy(_data,_data + data_length, data.begin());
        }

        base_binary(const base_binary& other){
            //进行一次内存拷贝
            std::memcpy(data.data(), other.data.data(), Width);
        }

        [[nodiscard]] constexpr int Compare(const base_binary& other) const {
            return std::memcmp(data.data(), other.data.data(), Width);
        }

        friend constexpr bool operator==(const base_binary& a, const base_binary& b) { return a.Compare(b) == 0; };
        friend constexpr bool operator!=(const base_binary& a, const base_binary& b) { return a.Compare(b) != 0; };
        friend constexpr bool operator<(const base_binary& a, const base_binary& b) { return a.Compare(b) < 0; };

        [[nodiscard]] std::string get_hex() const{
            return hash_handler::convert_to_hex(data.data(), Width);
        }

        [[nodiscard]] unsigned char * get_data(){
            return data.data();
        }

        base_binary operator+(const base_binary& other){
            base_binary<Bits> r;
            for (int i = 0; i < Width; ++i) {
                r.data[i] = data[i] + other.data[i];
            }
            return r;
        }

        MUSE_IBinarySerializable(data);

        base_binary<256> get_sha3_256();
    };

    template<unsigned int Bits>
    base_binary<256> base_binary<Bits>::get_sha3_256() {
        base_binary<256> tick(0);
        hash_handler::sha3_256(reinterpret_cast<const char*>(this->data.data()), Width, tick.data.data());
        return tick;
    }

    template class base_binary<256>;

    using uint256 = base_binary<256>;
};
#endif //MUSE_CHAIN_UINT256_HPP