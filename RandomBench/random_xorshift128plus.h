#ifndef RANDOM_XORSHIFT128PLUS_H
#define RANDOM_XORSHIFT128PLUS_H

#include <array>
#include <random>

namespace ExtraGenerators
{
using namespace std;

class xorshift128plus
{
    // From http://xorshift.di.unimi.it/
    // http://xorshift.di.unimi.it/xorshift128plus.c
    // "Written in 2014-2016 by Sebastiano Vigna (vigna@acm.org)"
    uint64_t s0_;
    uint64_t s1_;
public:
    typedef uint64_t result_type;

    explicit xorshift128plus(result_type x = 1)
    {
        seed(x);
    }

    template <class Seq>
    explicit xorshift128plus(Seq& seq)
    {
        seed(seq);
    }

    result_type operator()()
    {
        result_type s1 = s0_;
        const result_type s0 = s1_;

        s0_ = s0;

        s1 ^= s1 << 23;

        s1_ = s1 ^ s0 ^ (s1 >> 18) ^ (s0 >> 5);

        return s1_ + s0;
    }

    static constexpr result_type min()
    {
        return numeric_limits<result_type>::min();
    }

    static constexpr result_type max()
    {
        return numeric_limits<result_type>::max();
    }

    static constexpr size_t seed_words = (sizeof(xorshift128plus::s0_) + sizeof(xorshift128plus::s1_)) / sizeof(unsigned int);

    void seed(result_type s)
    {
        seed_seq seq{ s };

        seed(seq);
    }

    template <class Seq>
    void seed(Seq& seq)
    {
        array<unsigned int, seed_words> buffer;

        seq.generate(begin(buffer), end(buffer));

        s0_ = buffer[0];
        s0_ <<= 32;
        s0_ |= buffer[1];

        s1_ = buffer[2];
        s1_ <<= 32;
        s1_ |= buffer[3];
    }

    void discard(unsigned long long count)
    {
        (*this)();
    }
};
}

#endif // RANDOM_XORSHIFT128PLUS_H
