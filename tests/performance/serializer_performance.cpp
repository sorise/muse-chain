#include <benchmark/benchmark.h>
#include <array>
#include "serializer/binarySerializer.h"
#include "db/uint256.hpp"
#include "db/transaction.hpp"
#include "db/affair.hpp"
#include "db/block.hpp"


static void BinarySerializer_tri_afi(benchmark::State& state) {
    muse::chain::block blk;
    for (int i = 0; i < state.range(0); ++i) {
        muse::chain::transaction trs;
        muse::chain::uint256 sender("0x123" + std::to_string(i));
        trs.outs.emplace_back(muse::chain::uint256("0x4694"), 100 );
        trs.set_sender(sender);
        std::string sig(88,'5');
        blk.body.transactions.push_back(trs);
        trs.set_signature(sig);
    }
    for (int i = 0; i < state.range(0); ++i) {
        muse::chain::affair afi;
        muse::chain::uint256 sender("0x123" + std::to_string(i));
        afi.set_owner(sender);
        std::string test_string(128,'1');
        afi.set_data(test_string.c_str(), test_string.size());
        std::string sig(88,'1');
        blk.body.affairs.push_back(afi);
        afi.set_signature(sig);
    }
    blk.signature = std::string(88,'c');
    for (auto _ : state){
        //创建一个二进制序列化器
        muse::serializer::BinarySerializer serializer;
        //序列化
        serializer.input(blk);
    }
}

BENCHMARK(BinarySerializer_tri_afi)->Arg(0);
BENCHMARK(BinarySerializer_tri_afi)->Arg(5);
BENCHMARK(BinarySerializer_tri_afi)->Arg(10);
BENCHMARK(BinarySerializer_tri_afi)->Arg(20);
BENCHMARK(BinarySerializer_tri_afi)->Arg(40);
BENCHMARK(BinarySerializer_tri_afi)->Arg(50);
BENCHMARK(BinarySerializer_tri_afi)->Arg(100);
BENCHMARK(BinarySerializer_tri_afi)->Arg(200);
BENCHMARK(BinarySerializer_tri_afi)->Arg(400);
BENCHMARK(BinarySerializer_tri_afi)->Arg(500);
BENCHMARK(BinarySerializer_tri_afi)->Arg(1000);



BENCHMARK_MAIN();