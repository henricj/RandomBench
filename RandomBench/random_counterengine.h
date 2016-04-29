#ifndef RANDOM_COUNTERENGINE_H
#define RANDOM_COUNTERENGINE_H

#include <array>
#include <random>

namespace ExtraGenerators
{
using namespace std;

class counterengine
{
    uint32_t count_;
public:
    typedef uint32_t result_type;

    explicit counterengine(result_type x = 1)
    {
        seed(x);
    }

    template <class Seq>
    explicit counterengine(Seq& seq)
    {
        seed(seq);
    }

    result_type operator()()
    {
        return ++count_;
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
    {
        array<unsigned int, 1> buffer;

        seq.generate(begin(buffer), end(buffer));

        count_ = buffer[0];
    }

    void discard(unsigned long long count)
    {
        count_ = static_cast<uint32_t>(count_ + count);
    }
};
}

#endif // RANDOM_COUNTERENGINE_H
