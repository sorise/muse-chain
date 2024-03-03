#include <benchmark/benchmark.h>
#include <array>
#include "serializer/binarySerializer.h"
#include "db/uint256.hpp"
#include "db/transaction.hpp"
#include "db/affair.hpp"
#include "db/block.hpp"


static void BinarySerializer_affair(benchmark::State& state) {
    muse::chain::affair afi;
    muse::chain::uint256 sender("0x123");
    afi.set_owner(sender);
    std::string test_string(state.range(0),'1');
    afi.set_data(test_string.c_str(), test_string.size());
    std::string sig(88,'1');
    afi.set_signature(sig);
    muse::serializer::BinarySerializer serializer_;
    for (auto _ : state){
        //创建一个二进制序列化器
        muse::serializer::BinarySerializer serializer;
        //序列化
        serializer.input(afi);
    }
}

BENCHMARK(BinarySerializer_affair)->Arg(0);
BENCHMARK(BinarySerializer_affair)->Arg(5);
BENCHMARK(BinarySerializer_affair)->Arg(10);
BENCHMARK(BinarySerializer_affair)->Arg(20);
BENCHMARK(BinarySerializer_affair)->Arg(40);
BENCHMARK(BinarySerializer_affair)->Arg(80);
BENCHMARK(BinarySerializer_affair)->Arg(160);
BENCHMARK(BinarySerializer_affair)->Arg(320);
BENCHMARK(BinarySerializer_affair)->Arg(640);
BENCHMARK(BinarySerializer_affair)->Arg(1280);
BENCHMARK(BinarySerializer_affair)->Arg(2560);



BENCHMARK_MAIN();