#ifndef RNGSUPPORT_H
#define RNGSUPPORT_H

#ifdef __cplusplus

#if __cplusplus >= 201103L || (defined _MSC_VER && _MSC_VER >= 1600)
#define NYQ_USE_RANDOM_HEADER
#endif

#if defined NYQ_USE_RANDOM_HEADER

#include <vector>
#include <random>

namespace Nyq
{
using namespace std;

typedef mt19937 nyq_generator;
typedef seed_seq nyq_seed_seq;
typedef uniform_real_distribution<float> nyq_uniform_float_distribution;
typedef normal_distribution<float> nyq_normal_float_distribution;
typedef uniform_real_distribution<double> nyq_uniform_double_distribution;
typedef normal_distribution<double> nyq_normal_double_distribution;
typedef uniform_int_distribution<int> nyq_uniform_int_distribution;

template<class RNG>
struct nominal_seed_size
{
    static constexpr size_t seed_words()
    {
        return RNG::seed_words;
    }
};

template<>
struct nominal_seed_size<mt19937>
{
    static constexpr size_t seed_words()
    {
        return mt19937::state_size;
    }
};

template<>
struct nominal_seed_size<mt19937_64>
{
    static constexpr size_t seed_words()
    {
        return mt19937_64::state_size;
    }
};

template<>
struct nominal_seed_size<minstd_rand>
{
    static constexpr size_t seed_words()
    {
        return 1;
    }
};

template<>
struct nominal_seed_size<knuth_b>
{
    static constexpr size_t seed_words()
    {
        return 1;
    }
};

class RngSupportBase
{
protected:
    static vector<unsigned int> CreateRootSeedVector(size_t size);
};

template<class RNG = nyq_generator>
class RngSupport : RngSupportBase
{
private:
    friend vector<unsigned int> CreateRootSeedVector(size_t size);

    static RNG CreateRootGenerator()
    {
        auto seed_data = CreateRootSeedVector(nominal_seed_size<RNG>::seed_words());

        seed_seq seed(begin(seed_data), end(seed_data));

        RNG generator(seed);

        return generator;
    }

    static RNG& GetRootGenerator()
    {
        static thread_local auto generator = CreateRootGenerator();

        return generator;
    }

public:
    template<class RealFwdIt, class Real>
    static void RandomFillUniform(RealFwdIt first, RealFwdIt last, Real low, Real high)
    {
        RNG& generator = GetRootGenerator();

        uniform_real_distribution<Real> uniform{ low, high };

        for (; first != last; ++first)
            *first = uniform(generator);
    }

    template<class RealFwdIt, class Real>
    static void RandomFillNormal(RealFwdIt first, RealFwdIt last, Real mean, Real stDev)
    {
        RNG& generator = GetRootGenerator();

        normal_distribution<Real> uniform{ mean, stDev };

        for (; first != last; ++first)
            *first = uniform(generator);
    }

    template<class RealFwdIt, class Real>
    static bool RandomFillClampedNormal(RealFwdIt first, RealFwdIt last, Real mean, Real stDev, Real low, Real high)
    {
        RNG& generator = GetRootGenerator();

        normal_distribution<Real> uniform{ mean, stDev };

        for (; first != last; ++first)
        {
            int retry = 10;

            for (;;)
            {
                Real x = uniform(generator);

                if (x <= high && x >= low)
                {
                    *first = x;
                    break;
                }

                if (--retry <= 0)
                    return false;
            }
        }

        return true;
    }

    template<class Real>
    static Real RandomUniformReal(Real low, Real high)
    {
        auto& generator = GetRootGenerator();

        uniform_real_distribution<Real> uniform{ low, high };

        return uniform(generator);
    }

    template<class Int>
    static Int RandomUniformInt(Int low, Int high)
    {
        auto& generator = GetRootGenerator();

        uniform_int_distribution<Int> uniform{ low, high };

        return uniform(generator);
    }

    static vector<unsigned int> CreateSeedVector(vector<unsigned int>::size_type size)
    {
        vector<unsigned int> seed;

        seed.reserve(size);

        auto rng = GetRootGenerator();

        for (; size > 0; --size)
            seed.push_back(static_cast<unsigned int>(rng()));

        return seed;
    }

}; // class RngSupport

template <class RNG = nyq_generator>
static RNG CreateGenerator(size_t seed_words = 0)
{
    if (seed_words < 1)
        seed_words = nominal_seed_size<RNG>::seed_words();

    auto seed_data = RngSupport<RNG>::CreateSeedVector(seed_words);

    nyq_seed_seq seq(begin(seed_data), end(seed_data));

    RNG generator(seq);

    return generator;
}

template <class RNG = nyq_generator>
static void ReseedGenerator(RNG& generator, size_t seed_words = 0)
{
    auto seed_data = RngSupport<RNG>::CreateSeedVector(seed_words);

    nyq_seed_seq seq(begin(seed_data), end(seed_data));

    generator.seed(seq);
}

template <class RNG = nyq_generator>
class NyqEngine : public RNG
{
public:
    explicit NyqEngine(int seed_words = 0) : RNG(CreateGenerator<RNG>(seed_words))
    { }
};

} // namespace Nyq

#endif // NYQ_USE_RANDOM_HEADER

extern "C" {
#endif // __cplusplus

    void RandomFillUniformFloat(float* p, size_t count, float low, float high);
    void RandomFillNormalFloat(float* p, size_t count, float mean, float stDev);
    int RandomFillClampedNormalFloat(float* p, size_t count, float mean, float stDev, float low, float high);
    float RandomUniformFloat(float low, float high);

    void RandomFillUniformDouble(double* p, size_t count, double low, double high);
    void RandomFillNormalDouble(double* p, size_t count, double mean, double stDev);
    int RandomFillClampedNormalDouble(double* p, size_t count, double mean, double stDev, double low, double high);
    double RandomUniformDouble(double low, double high);

    int RandomUniformInt(int lowInclusive, int highExclusive);

#ifdef __cplusplus
} //   extern "C"
#endif

#endif // RNGSUPPORT_H
