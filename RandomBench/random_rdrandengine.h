#ifndef RANDOM_RDRANDENGINE_H
#define RANDOM_RDRANDENGINE_H

#include <array>
#include <random>
#include <immintrin.h>

namespace ExtraGenerators
{
using namespace std;

class rdrandengine
{
public:
    typedef uint64_t result_type;

    explicit rdrandengine(result_type x = 1)
    {     }

    template <class Seq>
    explicit rdrandengine(Seq& seq)
    {
    }

    result_type operator()() const
    {
        for (auto retry = 10; retry > 0; --retry)
        {
            result_type n;

            if (_rdrand64_step(&n))
                return n;
        }

        throw "Broken hardware...";
    }

    static constexpr result_type min()
    {
        return numeric_limits<result_type>::min();
    }

    static constexpr result_type max()
    {
        return numeric_limits<result_type>::max();
    }

    static constexpr size_t seed_words = 1;

    void seed(result_type s)
    { }

    template <class Seq>
    void seed(Seq& seq)
    { }

    void discard(unsigned long long count)
    { }
};
}

#endif // RANDOM_RDRANDENGINE_H
