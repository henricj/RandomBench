#include "stdafx.h"

#if 0

#include "RngSupport.h"

#include "random_xorshift128plus.h"
#include "random_xoroshiro128plus.h"
#include "random_xorshift1024star.h"
//#include "random_rdrandengine.h"
#include "random_counterengine.h"
#include "random_zeroengine.h"

using namespace ExtraGenerators;

using namespace std;

#if defined(_MSC_VER)
#define NOINLINE __declspec(noinline)
#else
#define NOINLINE __attribute__ ((noinline))
#endif

namespace
{
// From http://stackoverflow.com/questions/28287064/how-not-to-optimize-away-mechanics-of-a-folly-function
#ifdef _MSC_VER

#pragma optimize("", off)

template<typename T>
void MarkUsed(T&& x)
{
}

#pragma optimize("", on)

#else
template<typename T>
void MarkUsed(T&& x) {
    asm volatile("" : "+r" (x));
}
#endif

typedef chrono::duration<double, nano> DoubleNanoseconds;

const int BLOCK_SIZE = 102;
const int REPETITIONS_PER_PASS = 10000;
const int PASSES = 7;
const int ALIGN_BYTES = 256;

#if __cplusplus >= 201402L || _MSC_VER >= 1900
#define HAVE_MAKE_UNIQUE_ARRAY
#endif

#ifdef HAVE_MAKE_UNIQUE_ARRAY
template<typename T, int alignment = 32>
class AlignedAllocated
{
    const int allocation_size = sizeof(T) + alignment;
    unique_ptr<char[]> memory_;
    void* p_;
    size_t size_;
public:
    AlignedAllocated() : memory_(make_unique<char[]>(allocation_size)), p_(memory_.get()), size_(allocation_size)
    { }

    T* Create()
    {
        auto buffer_block = align(alignment, sizeof(T), p_, size_);

        if (nullptr == buffer_block)
            return nullptr;

        return new(buffer_block) T;
    }
};
#endif

template<typename V, typename W>
void
run_scale_loop(V* buffer, size_t length, function<W()> func, V scale, V offset)
{
    while (length-- > 0)
        buffer[length] = func() * scale - offset;
}

NOINLINE
void
use_rand16(size_t length)
{
    while (length--)
    {
        rand();
        rand();
        rand();
        rand();
        rand();
        rand();
        rand();
        rand();
        rand();
        rand();
        rand();
        rand();
        rand();
        rand();
        rand();
        rand();
    }
}

template<typename V>
NOINLINE
void
use_rand(V* buffer, size_t length)
{
    while (length--)
        buffer[length] = rand();
}

template<>
NOINLINE
void
use_rand<float>(float* buffer, size_t length)
{
    run_scale_loop<float, int>(buffer, length, rand, 2.f / RAND_MAX, 1.f);
}

template<>
NOINLINE
void
use_rand<double>(double* buffer, size_t length)
{
    run_scale_loop<double, int>(buffer, length, rand, 2. / RAND_MAX, 1.);
}

template<typename Engine>
void
NOINLINE
use_random16(Engine& engine, size_t length)
{
    while (length--)
    {
        engine();
        engine();
        engine();
        engine();
        engine();
        engine();
        engine();
        engine();
        engine();
        engine();
        engine();
        engine();
        engine();
        engine();
        engine();
        engine();
    }
}

template<typename Engine, typename Distribution>
void
NOINLINE
use_random(Engine& engine, Distribution& distribution, typename Distribution::result_type* buffer, size_t length)
{
    while (length--)
        buffer[length] = distribution(engine);
}

template<typename Engine, typename Iterator>
void
NOINLINE
use_random(Engine& engine, Iterator begin, Iterator end)
{
    for (auto it = begin; it != end; ++it)
        *it = engine();
}

template<typename Engine>
void
NOINLINE
use_random(Engine& engine, typename Engine::result_type* buffer, size_t length)
{
    while (length--)
        buffer[length] = engine();
}

template<typename Engine>
void
NOINLINE
use_random_scale(Engine& engine, double* buffer, size_t length)
{
    run_scale_loop<double, typename Engine::result_type>(buffer, length, engine, 2. / Engine::max(), 1.);
}

template<typename Engine>
void
NOINLINE
use_random_scale(Engine& engine, float* buffer, size_t length)
{
    while (length--)
        buffer[length] = engine() * 2.f / Engine::max() - 1.f;
    //run_scale_loop<float, typename Engine::result_type>(buffer, length, engine, 2.f / Engine::max(), 1.f);
}

template<typename Engine>
void MeasureEngineDiscard(Engine& eng, int passes)
{
    eng();

    for (auto i = 0; i < passes; ++i)
    {
        auto start = chrono::high_resolution_clock::now();

        for (auto j = 0; j < REPETITIONS_PER_PASS; ++j)
            use_rand16(BLOCK_SIZE);

        auto end = chrono::high_resolution_clock::now();

        auto rand_elapsed = chrono::duration_cast<DoubleNanoseconds>(end - start).count() / (16 * REPETITIONS_PER_PASS * BLOCK_SIZE);

        start = chrono::high_resolution_clock::now();

        for (auto j = 0; j < REPETITIONS_PER_PASS; ++j)
            use_random16(eng, BLOCK_SIZE);

        end = chrono::high_resolution_clock::now();

        auto random_elapsed = chrono::duration_cast<DoubleNanoseconds>(end - start).count() / (16 * REPETITIONS_PER_PASS * BLOCK_SIZE);

        cout << "discard rand: " << rand_elapsed << " random: " << random_elapsed << endl;
    }
}

template<typename Engine>
void MeasureEngine(Engine& eng, int passes)
{
#ifdef HAVE_MAKE_UNIQUE_ARRAY
    AlignedAllocated<array<typename Engine::result_type, BLOCK_SIZE>, ALIGN_BYTES> buffer_handle;

    auto buffer = *buffer_handle.Create();
#else
    array<typename Engine::result_type, BLOCK_SIZE> buffer;
#endif

    eng();

    for (auto i = 0; i < passes; ++i)
    {
        auto start = chrono::high_resolution_clock::now();

        for (auto j = 0; j < REPETITIONS_PER_PASS; ++j)
        {
            use_rand(&buffer[0], buffer.size());
            MarkUsed(buffer);
        }

        auto end = chrono::high_resolution_clock::now();

        auto rand_elapsed = chrono::duration_cast<DoubleNanoseconds>(end - start).count() / (REPETITIONS_PER_PASS * buffer.size());

        start = chrono::high_resolution_clock::now();

        for (auto j = 0; j < REPETITIONS_PER_PASS; ++j)
        {
            use_random(eng, &buffer[0], buffer.size());
            //use_random(eng, buffer.begin(), buffer.end());
            MarkUsed(buffer);
        }

        end = chrono::high_resolution_clock::now();

        auto random_elapsed = chrono::duration_cast<DoubleNanoseconds>(end - start).count() / (REPETITIONS_PER_PASS * buffer.size());

        cout << "raw rand: " << rand_elapsed << " random: " << random_elapsed << endl;
    }
}

template<typename Engine>
void MeasureEngineWithScale(Engine& eng, int passes)
{
#ifdef HAVE_MAKE_UNIQUE_ARRAY
    AlignedAllocated<array<float, BLOCK_SIZE>, ALIGN_BYTES> buffer_handle;

    auto buffer = *buffer_handle.Create();
#else
    array<float, BLOCK_SIZE> buffer;
#endif

    eng();

    for (auto i = 0; i < passes; ++i)
    {
        auto start = chrono::high_resolution_clock::now();

        for (auto j = 0; j < REPETITIONS_PER_PASS; ++j)
        {
            use_rand(&buffer[0], buffer.size());
            MarkUsed(buffer);
        }

        auto end = chrono::high_resolution_clock::now();

        auto rand_elapsed = chrono::duration_cast<DoubleNanoseconds>(end - start).count() / (REPETITIONS_PER_PASS * buffer.size());

        start = chrono::high_resolution_clock::now();

        for (auto j = 0; j < REPETITIONS_PER_PASS; ++j)
        {
            use_random_scale(eng, &buffer[0], buffer.size());
            MarkUsed(buffer);
        }

        end = chrono::high_resolution_clock::now();

        auto random_elapsed = chrono::duration_cast<DoubleNanoseconds>(end - start).count() / (REPETITIONS_PER_PASS * buffer.size());

        cout << "scale rand: " << rand_elapsed << " random: " << random_elapsed << endl;
    }
}

template<typename Engine, typename Distribution>
void MeasureWithDistribution(Engine& eng, Distribution& dist, int passes)
{
#ifdef HAVE_MAKE_UNIQUE_ARRAY
    AlignedAllocated<array<typename Distribution::result_type, BLOCK_SIZE>, ALIGN_BYTES> buffer_handle;

    auto buffer = *buffer_handle.Create();
#else
    array<typename Distribution::result_type, BLOCK_SIZE> buffer;
#endif

    eng();

    for (auto i = 0; i < passes; ++i)
    {
        auto start = chrono::high_resolution_clock::now();

        for (auto j = 0; j < REPETITIONS_PER_PASS; ++j)
        {
            use_rand(&buffer[0], buffer.size());
            MarkUsed(buffer);
        }

        auto end = chrono::high_resolution_clock::now();

        auto rand_elapsed = chrono::duration_cast<DoubleNanoseconds>(end - start).count() / (REPETITIONS_PER_PASS * buffer.size());

        start = chrono::high_resolution_clock::now();

        for (auto j = 0; j < REPETITIONS_PER_PASS; ++j)
        {
            use_random(eng, dist, &buffer[0], buffer.size());
            MarkUsed(buffer);
        }

        end = chrono::high_resolution_clock::now();

        auto random_elapsed = chrono::duration_cast<DoubleNanoseconds>(end - start).count() / (REPETITIONS_PER_PASS * buffer.size());

        cout << "dist rand: " << rand_elapsed << " random: " << random_elapsed << endl;
    }
}

template<typename Engine, typename Distribution>
void Measure(string name, Engine& engine, Distribution& distribution)
{
    cout << name << endl;

    MeasureEngineDiscard(engine, PASSES);
    MeasureEngine(engine, PASSES);
    MeasureEngineWithScale(engine, PASSES);
    MeasureWithDistribution(engine, distribution, PASSES);

    // Use the current state of both the engine and distribution
    // for something (we could assign it to a volatile).
    cout << "   value: " << distribution(engine) << endl;
}
}

int main()
{
    Nyq::NyqEngine<> eng_default;
    Nyq::NyqEngine<xorshift128plus> eng_xorshift128plus;
    Nyq::NyqEngine<xoroshiro128plus> eng_xoroshiro128plus;
    Nyq::NyqEngine<xorshift1024star> eng_xorshift1024star;
    //Nyq::NyqEngine<minstd_rand> eng_minstd;
    //Nyq::NyqEngine<knuth_b> eng_knuth;
    //Nyq::NyqEngine<rdrandengine> eng_rdrand;
    Nyq::NyqEngine<counterengine> eng_counter;
    Nyq::NyqEngine<zeroengine> eng_zero;

    //auto eng = Nyq::CreateGenerator<std::minstd_rand>();
    Nyq::nyq_uniform_float_distribution dist{ -1, 1 };
    //Nyq::nyq_uniform_int_distribution dist{ 0, RAND_MAX };
    //Nyq::nyq_normal_float_distribution dist{ 0, 1 };

    Measure("default", eng_default, dist);

    Measure("xorshift128plus", eng_xorshift128plus, dist);

    Measure("xoroshiro128plus", eng_xoroshiro128plus, dist);

    Measure("xorshift1024star", eng_xorshift1024star, dist);

    //Measure("rdrand", eng_rdrand, dist);
#if false
    Measure("minstd", eng_minstd, dist);

    Measure("knuth", eng_knuth, dist);
#endif // false

    Measure("counter", eng_counter, dist);

    Measure("zero", eng_zero, dist);

    return 0;
}

#endif // 0
