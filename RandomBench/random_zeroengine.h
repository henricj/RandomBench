#ifndef RANDOM_ZEROENGINE_H
#define RANDOM_ZEROENGINE_H

#include <random>

namespace ExtraGenerators
{
using namespace std;

class zeroengine
{
public:
    typedef uint32_t result_type;

    explicit zeroengine(result_type x = 1)
    {
        seed(x);
    }

    template <class Seq>
    explicit zeroengine(Seq& seq)
    {
        seed(seq);
    }

    result_type operator()()
    {
        return 0;
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
    {
        seed_seq seq{ s };

        seed(seq);
    }

    template <class Seq>
    void seed(Seq& seq)
    { }

    void discard(unsigned long long count)
    { }
};
}

#endif // RANDOM_ZEROENGINE_H
