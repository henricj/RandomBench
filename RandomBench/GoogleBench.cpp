﻿#include "stdafx.h"

#include "RngSupport.h"

#include "random_xorshift128plus.h"
#include "random_xoroshiro128plus.h"
#include "random_xorshift1024star.h"
#include "random_rdrandengine.h"
#include "random_aesni.h"
#include "random_counterengine.h"
#include "random_zeroengine.h"

#include <ppltasks.h>


template<typename Engine>
static void BM_engine(benchmark::State& state)
{
    Engine eng;

    for (auto _ : state)
        benchmark::DoNotOptimize(eng());
}

#if 0
BENCHMARK_TEMPLATE(BM_engine, ExtraGenerators::xorshift128plus);
BENCHMARK_TEMPLATE(BM_engine, ExtraGenerators::xoroshiro128plus);
BENCHMARK_TEMPLATE(BM_engine, ExtraGenerators::xorshift1024star);
BENCHMARK_TEMPLATE(BM_engine, ExtraGenerators::rdrandengine);
BENCHMARK_TEMPLATE(BM_engine, ExtraGenerators::aesni2<>);
BENCHMARK_TEMPLATE(BM_engine, ExtraGenerators::aesni2<16>);
BENCHMARK_TEMPLATE(BM_engine, ExtraGenerators::aesni4<>);
BENCHMARK_TEMPLATE(BM_engine, ExtraGenerators::aesni4<16>);
BENCHMARK_TEMPLATE(BM_engine, ExtraGenerators::aesni8<>);
BENCHMARK_TEMPLATE(BM_engine, ExtraGenerators::aesni8<16>);
BENCHMARK_TEMPLATE(BM_engine, ExtraGenerators::counterengine);
BENCHMARK_TEMPLATE(BM_engine, ExtraGenerators::zeroengine);
BENCHMARK_TEMPLATE(BM_engine, std::minstd_rand0);
BENCHMARK_TEMPLATE(BM_engine, std::mt19937);
BENCHMARK_TEMPLATE(BM_engine, std::mt19937_64);
#endif // 0

template<typename Engine>
static void BM_nyq_engine(benchmark::State& state)
{
    Nyq::NyqEngine<Engine> eng;

    for (auto _ : state)
        benchmark::DoNotOptimize(eng());
}

//BENCHMARK_TEMPLATE(BM_nyq_engine, ExtraGenerators::xorshift128plus);
BENCHMARK_TEMPLATE(BM_nyq_engine, ExtraGenerators::xoroshiro128plus);
BENCHMARK_TEMPLATE(BM_nyq_engine, ExtraGenerators::xorshift1024star);
//BENCHMARK_TEMPLATE(BM_nyq_engine, ExtraGenerators::rdrandengine);
//BENCHMARK_TEMPLATE(BM_nyq_engine, ExtraGenerators::aesni1<>);
//BENCHMARK_TEMPLATE(BM_nyq_engine, ExtraGenerators::aesni2<>);
//BENCHMARK_TEMPLATE(BM_nyq_engine, ExtraGenerators::aesni2<14>);
//BENCHMARK_TEMPLATE(BM_nyq_engine, ExtraGenerators::aesni4<>);
//BENCHMARK_TEMPLATE(BM_nyq_engine, ExtraGenerators::aesni4<16>);
BENCHMARK_TEMPLATE(BM_nyq_engine, ExtraGenerators::aesni8<>);
BENCHMARK_TEMPLATE(BM_nyq_engine, ExtraGenerators::aesni8<2>);
BENCHMARK_TEMPLATE(BM_nyq_engine, ExtraGenerators::aesni8<3>);
BENCHMARK_TEMPLATE(BM_nyq_engine, ExtraGenerators::aesni8<4>);
BENCHMARK_TEMPLATE(BM_nyq_engine, ExtraGenerators::aesni8<10>);
BENCHMARK_TEMPLATE(BM_nyq_engine, ExtraGenerators::aesni8<12>);
BENCHMARK_TEMPLATE(BM_nyq_engine, ExtraGenerators::aesni8<14>);
BENCHMARK_TEMPLATE(BM_nyq_engine, ExtraGenerators::aesni16<>);
BENCHMARK_TEMPLATE(BM_nyq_engine, ExtraGenerators::aesni16<2>);
BENCHMARK_TEMPLATE(BM_nyq_engine, ExtraGenerators::aesni16<3>);
BENCHMARK_TEMPLATE(BM_nyq_engine, ExtraGenerators::aesni16<4>);
BENCHMARK_TEMPLATE(BM_nyq_engine, ExtraGenerators::aesni16<10>);
BENCHMARK_TEMPLATE(BM_nyq_engine, ExtraGenerators::aesni16<12>);
BENCHMARK_TEMPLATE(BM_nyq_engine, ExtraGenerators::aesni16<14>);
BENCHMARK_TEMPLATE(BM_nyq_engine, ExtraGenerators::aesni_n<8, 1>);
BENCHMARK_TEMPLATE(BM_nyq_engine, ExtraGenerators::aesni_n<8, 2>);
BENCHMARK_TEMPLATE(BM_nyq_engine, ExtraGenerators::aesni_n<8, 3>);
BENCHMARK_TEMPLATE(BM_nyq_engine, ExtraGenerators::aesni_n<8, 4>);
BENCHMARK_TEMPLATE(BM_nyq_engine, ExtraGenerators::aesni_n<8, 8>);
BENCHMARK_TEMPLATE(BM_nyq_engine, ExtraGenerators::aesni_n<8, 10>);
BENCHMARK_TEMPLATE(BM_nyq_engine, ExtraGenerators::aesni_n<16, 1>);
BENCHMARK_TEMPLATE(BM_nyq_engine, ExtraGenerators::aesni_n<16, 2>);
BENCHMARK_TEMPLATE(BM_nyq_engine, ExtraGenerators::aesni_n<16, 4>);
BENCHMARK_TEMPLATE(BM_nyq_engine, ExtraGenerators::aesni_n<16, 8>);
BENCHMARK_TEMPLATE(BM_nyq_engine, ExtraGenerators::aesni_n<64, 1>);
BENCHMARK_TEMPLATE(BM_nyq_engine, ExtraGenerators::aesni_n_u32<8, 1>);
BENCHMARK_TEMPLATE(BM_nyq_engine, ExtraGenerators::aesni_n_u32<8, 2>);
BENCHMARK_TEMPLATE(BM_nyq_engine, ExtraGenerators::aesni_n_u32<8, 3>);
BENCHMARK_TEMPLATE(BM_nyq_engine, ExtraGenerators::aesni_n_u32<8, 4>);
BENCHMARK_TEMPLATE(BM_nyq_engine, ExtraGenerators::aesni_n_u32<8, 8>);
BENCHMARK_TEMPLATE(BM_nyq_engine, ExtraGenerators::aesni_n_u32<8, 10>);
BENCHMARK_TEMPLATE(BM_nyq_engine, ExtraGenerators::aesni_n_u32_a<8, 1>);
BENCHMARK_TEMPLATE(BM_nyq_engine, ExtraGenerators::aesni_n_u32_a<8, 2>);
BENCHMARK_TEMPLATE(BM_nyq_engine, ExtraGenerators::aesni_n_u32_a<8, 10>);
BENCHMARK_TEMPLATE(BM_nyq_engine, ExtraGenerators::aesni_n_u32<16, 1>);
BENCHMARK_TEMPLATE(BM_nyq_engine, ExtraGenerators::aesni_n_u32<16, 2>);
BENCHMARK_TEMPLATE(BM_nyq_engine, ExtraGenerators::aesni_n_u32<16, 4>);
BENCHMARK_TEMPLATE(BM_nyq_engine, ExtraGenerators::aesni_n_u32<16, 8>);
BENCHMARK_TEMPLATE(BM_nyq_engine, ExtraGenerators::aesni_n_u32<16, 10>);
BENCHMARK_TEMPLATE(BM_nyq_engine, ExtraGenerators::aesni_n_u32<64, 1>);
BENCHMARK_TEMPLATE(BM_nyq_engine, ExtraGenerators::aesni_n_u32<64, 2>);
BENCHMARK_TEMPLATE(BM_nyq_engine, ExtraGenerators::aesni_n_u32<64, 3>);
BENCHMARK_TEMPLATE(BM_nyq_engine, ExtraGenerators::aesni_n_u32<64, 8>);
BENCHMARK_TEMPLATE(BM_nyq_engine, ExtraGenerators::aesni_n_u32<64, 10>);
BENCHMARK_TEMPLATE(BM_nyq_engine, ExtraGenerators::aesni_n_u32<64, 12>);
BENCHMARK_TEMPLATE(BM_nyq_engine, ExtraGenerators::aesni_n_u32<64, 14>);
BENCHMARK_TEMPLATE(BM_nyq_engine, ExtraGenerators::aesni_n_u32<64, 16>);
BENCHMARK_TEMPLATE(BM_nyq_engine, ExtraGenerators::aesni_ctr);
//BENCHMARK_TEMPLATE(BM_nyq_engine, ExtraGenerators::aesni_ctr2<3>);
//BENCHMARK_TEMPLATE(BM_nyq_engine, ExtraGenerators::counterengine);
//BENCHMARK_TEMPLATE(BM_nyq_engine, ExtraGenerators::zeroengine);
//BENCHMARK_TEMPLATE(BM_nyq_engine, std::minstd_rand0);
//BENCHMARK_TEMPLATE(BM_nyq_engine, std::mt19937);
//BENCHMARK_TEMPLATE(BM_nyq_engine, std::mt19937_64);

template<typename Engine>
static void BM_engine_block(benchmark::State& state)
{
    Nyq::NyqEngine<Engine> eng;

    std::vector<decltype(eng)::result_type> buffer;

    for (auto _ : state)
    {
        state.PauseTiming();

        buffer.resize(state.range());

        state.ResumeTiming();

        std::generate(std::begin(buffer), std::end(buffer), eng);

        benchmark::ClobberMemory();
    }

    state.SetItemsProcessed(int64_t(state.iterations()) * int64_t(state.range(0)));
}

const int start_range = 1024;
const int end_range = 1 << 14;
//
////BENCHMARK_TEMPLATE(BM_engine_block, ExtraGenerators::xorshift128plus)->Range(start_range, end_range);
//BENCHMARK_TEMPLATE(BM_engine_block, ExtraGenerators::xoroshiro128plus)->Range(start_range, end_range);
//BENCHMARK_TEMPLATE(BM_engine_block, ExtraGenerators::xorshift1024star)->Range(start_range, end_range);
//BENCHMARK_TEMPLATE(BM_engine_block, ExtraGenerators::rdrandengine)->Range(start_range, end_range);
//BENCHMARK_TEMPLATE(BM_engine_block, ExtraGenerators::aesni1<>)->Range(start_range, end_range);
//BENCHMARK_TEMPLATE(BM_engine_block, ExtraGenerators::aesni2<>)->Range(start_range, end_range);
//BENCHMARK_TEMPLATE(BM_engine_block, ExtraGenerators::aesni2<16>)->Range(start_range, end_range);
////BENCHMARK_TEMPLATE(BM_engine_block, ExtraGenerators::aesni4<>)->Range(start_range, end_range);
////BENCHMARK_TEMPLATE(BM_engine_block, ExtraGenerators::aesni4<16>)->Range(start_range, end_range);
//BENCHMARK_TEMPLATE(BM_engine_block, ExtraGenerators::aesni8<>)->Range(start_range, end_range);
//BENCHMARK_TEMPLATE(BM_engine_block, ExtraGenerators::aesni8<16>)->Range(start_range, end_range);
////BENCHMARK_TEMPLATE(BM_engine_block, ExtraGenerators::aesni_ctr)->Range(start_range, end_range);
//BENCHMARK_TEMPLATE(BM_engine_block, ExtraGenerators::aesni_ctr2<3>)->Range(start_range, end_range);
//BENCHMARK_TEMPLATE(BM_engine_block, ExtraGenerators::counterengine)->Range(start_range, end_range);
//BENCHMARK_TEMPLATE(BM_engine_block, ExtraGenerators::zeroengine)->Range(start_range, end_range);
//BENCHMARK_TEMPLATE(BM_engine_block, std::minstd_rand0)->Range(start_range, end_range);
//BENCHMARK_TEMPLATE(BM_engine_block, std::mt19937)->Range(start_range, end_range);
//BENCHMARK_TEMPLATE(BM_engine_block, std::mt19937_64)->Range(start_range, end_range);
//
BENCHMARK_MAIN();

