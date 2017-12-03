#ifndef RANDOM_AESNI_H
#define RANDOM_AESNI_H

#include <array>
#include <random>
#include <immintrin.h>

namespace ExtraGenerators
{
    class aesni_ctr_simple
    {
    private:
        __m128i key_;
        __m128i count_;

    public:
        typedef uint64_t result_type;

        explicit aesni_ctr_simple(result_type x = 1)
        {
            seed(x);
        }

        template<class Seq>
        explicit aesni_ctr_simple(Seq& seq)
        {
            seed(seq);
        }

        result_type operator()()
        {
            const auto ret = _mm_aesenc_si128(count_, key_);

            if (0 == ++count_.m128i_u64[0])
                ++count_.m128i_u64[1];

            return ret.m128i_u64[0];
        }

        static constexpr result_type min()
        {
            return numeric_limits<result_type>::min();
        }

        static constexpr result_type max()
        {
            return numeric_limits<result_type>::max();
        }

        static constexpr size_t seed_words = sizeof(__m128i) / sizeof(unsigned int);

        void seed(result_type s)
        {
            seed_seq seq{ s };

            seed(seq);
        }

        template<class Seq>
        void seed(Seq& seq)
        {
            array<unsigned int, seed_words> buffer;

            seq.generate(begin(buffer), end(buffer));

            auto pb = begin(buffer);

            for (auto i = 0; i < 2; ++i)
            {
                uint64_t v = *pb++;

                v |= static_cast<uint64_t>(*pb++) << 32;

                key_.m128i_i64[i] = v;

                v = *pb++;

                v |= static_cast<uint64_t>(*pb++) << 32;

                count_.m128i_i64[i] = v;
            }
        }

        void discard(unsigned long long count)
        {
        }
    };

    class aesni_ctr
    {
    private:
        __m128i key_;
        __m128i count_;
        __m128i value_;

        int i_;

    public:
        typedef uint64_t result_type;

        explicit aesni_ctr(result_type x = 1)
        {
            seed(x);
        }

        template<class Seq>
        explicit aesni_ctr(Seq& seq)
        {
            seed(seq);
        }

        result_type operator()()
        {
            if (i_ > 0)
            {
                value_ = _mm_aesenc_si128(count_, key_);

                if (0 == ++count_.m128i_u64[0])
                    ++count_.m128i_u64[1];

                i_ = 0;
            }

            return value_.m128i_u64[i_++];
        }

        static constexpr result_type min()
        {
            return numeric_limits<result_type>::min();
        }

        static constexpr result_type max()
        {
            return numeric_limits<result_type>::max();
        }

        static constexpr size_t seed_words = 2 * sizeof(__m128i) / sizeof(unsigned int);

        void seed(result_type s)
        {
            seed_seq seq{ s };

            seed(seq);
        }

        template<class Seq>
        void seed(Seq& seq)
        {
            array<unsigned int, seed_words> buffer;

            seq.generate(begin(buffer), end(buffer));

            auto pb = begin(buffer);

            for (auto i = 0; i < 2; ++i)
            {
                uint64_t v = *pb++;

                v |= static_cast<uint64_t>(*pb++) << 32;

                key_.m128i_i64[i] = v;

                v = *pb++;

                v |= static_cast<uint64_t>(*pb++) << 32;

                count_.m128i_i64[i] = v;
            }

            i_ = 0;
        }

        void discard(unsigned long long count)
        {
        }
    };

    template<int Rounds = 3>
    class aesni_ctr2
    {
    private:
        __m128i key_[Rounds];
        __m128i count_;

    public:
        typedef uint64_t result_type;

        explicit aesni_ctr2(result_type x = 1)
        {
            seed(x);
        }

        template<class Seq>
        explicit aesni_ctr2(Seq& seq)
        {
            seed(seq);
        }

        result_type operator()()
        {
            auto ret = _mm_xor_si128(count_, key_[0]);

            for (auto i = 0; i < Rounds - 2; ++i)
                ret = _mm_aesenc_si128(ret, key_[i + 1]);

            if (Rounds > 1)
                ret = _mm_aesenclast_si128(ret, key_[Rounds - 1]);

            if (0 == ++count_.m128i_u64[0])
                ++count_.m128i_u64[1];

            return ret.m128i_u64[0];
        }

        static constexpr result_type min()
        {
            return numeric_limits<result_type>::min();
        }

        static constexpr result_type max()
        {
            return numeric_limits<result_type>::max();
        }

        static constexpr size_t seed_words = sizeof(__m128i) / sizeof(unsigned int);

        void seed(result_type s)
        {
            seed_seq seq{ s };

            seed(seq);
        }

        template<class Seq>
        void seed(Seq& seq)
        {
            array<unsigned int, seed_words> buffer;

            seq.generate(begin(buffer), end(buffer));

            auto pb = begin(buffer);

            for (auto i = 0; i < 2; ++i)
            {
                uint64_t v = *pb++;

                v |= static_cast<uint64_t>(*pb++) << 32;

                count_.m128i_i64[i] = v;

                for (auto j = 0; j < Rounds; ++j)
                {
                    v = *pb++;

                    v |= static_cast<uint64_t>(*pb++) << 32;

                    key_[j].m128i_i64[i] = v;
                }
            }
        }

        void discard(unsigned long long count)
        {
        }
    };

    template<int Rounds = 1>
    class aesni1
    {
    private:
        __m128i key_;
        __m128i value_;

        int i_ = 0;

        void stir()
        {
            for (auto i = 0; i < Rounds; ++i)
            {
                value_ = _mm_aesenc_si128(value_, key_);
            }

            i_ = 0;
        }

    public:
        typedef uint64_t result_type;

        explicit aesni1(result_type x = 1)
        {
            seed(x);
        }

        template<class Seq>
        explicit aesni1(Seq& seq)
        {
            seed(seq);
        }

        result_type operator()()
        {
            if (i_ >= 2)
                stir();

            return value_.m128i_i64[i_++];
        }

        static constexpr result_type min()
        {
            return numeric_limits<result_type>::min();
        }

        static constexpr result_type max()
        {
            return numeric_limits<result_type>::max();
        }

        static constexpr size_t seed_words = sizeof(decltype(value_)) + sizeof(decltype(key_)) / sizeof(unsigned int);

        void seed(result_type s)
        {
            seed_seq seq{ s };

            seed(seq);
        }

        template<class Seq>
        void seed(Seq& seq)
        {
            array<unsigned int, seed_words> buffer;

            seq.generate(begin(buffer), end(buffer));

            auto pb = begin(buffer);

            for (auto i = 0; i < 2; ++i)
            {
                uint64_t v = *pb++;

                v |= static_cast<uint64_t>(*pb++) << 32;

                value_.m128i_i64[i] = v;

                v = *pb++;

                v |= static_cast<uint64_t>(*pb++) << 32;

                key_.m128i_i64[i] = v;
            }

            stir();
        }

        void discard(unsigned long long count)
        {
        }
    };

    template<int Rounds = 1>
    class aesni2
    {
    private:
        constexpr static int elements = 2;
        std::array<__m128i, elements> s_;
        int i_ = 0;
        int j_ = 0;

        void stir()
        {
            for (auto i = 0; i < Rounds; ++i)
            {
                const auto s0 = s_[0];

                s_[0] = _mm_aesenc_si128(s_[1], s0);
                s_[1] = _mm_aesenc_si128(s0, s_[1]);
            }

            i_ = j_ = 0;
        }

    public:
        typedef uint64_t result_type;

        explicit aesni2(result_type x = 1)
        {
            seed(x);
        }

        template<class Seq>
        explicit aesni2(Seq& seq)
        {
            seed(seq);
        }

        result_type operator()()
        {
            if (j_ < 0)
            {
                j_ = 1;

                return s_[i_].m128i_i64[0];
            }

            const auto ret = s_[i_].m128i_i64[1];

            j_ = 0;

            if (++i_ >= s_.size())
                stir();

            return ret;
        }

        static constexpr result_type min()
        {
            return numeric_limits<result_type>::min();
        }

        static constexpr result_type max()
        {
            return numeric_limits<result_type>::max();
        }

        static constexpr size_t seed_words = elements * sizeof(decltype(s_)::value_type) / sizeof(unsigned int);

        void seed(result_type s)
        {
            seed_seq seq{ s };

            seed(seq);
        }

        template<class Seq>
        void seed(Seq& seq)
        {
            array<unsigned int, seed_words> buffer;

            seq.generate(begin(buffer), end(buffer));

            auto pb = begin(buffer);
            for (auto s = begin(s_); s != end(s_); ++s)
            {
                for (auto i = 0; i < 2; ++i)
                {
                    uint64_t v = *pb++;

                    v |= static_cast<uint64_t>(*pb++) << 32;

                    s->m128i_i64[i] = v;
                }
            }

            stir();
        }

        void discard(unsigned long long count)
        {
        }
    };

    template<int Rounds = 1>
    class aesni4
    {
    private:
        constexpr static int elements = 4;
        std::array<__m128i, elements> s_;
        int i_ = 0;
        int j_ = 0;

        void stir()
        {
            for (auto i = 0; i < Rounds; ++i)
            {
                const auto s0 = s_[0];

                s_[0] = _mm_aesenc_si128(s_[1], s0);
                s_[1] = _mm_aesenc_si128(s_[2], s_[1]);
                s_[2] = _mm_aesenc_si128(s_[3], s_[2]);
                s_[3] = _mm_aesenc_si128(s0, s_[3]);
            }

            i_ = j_ = 0;
        }

    public:
        typedef uint64_t result_type;

        explicit aesni4(result_type x = 1)
        {
            seed(x);
        }

        template<class Seq>
        explicit aesni4(Seq& seq)
        {
            seed(seq);
        }

        result_type operator()()
        {
            if (j_ < 0)
            {
                j_ = 1;

                return s_[i_].m128i_i64[0];
            }

            const auto ret = s_[i_].m128i_i64[1];

            j_ = 0;

            if (++i_ >= 8)
                stir();

            return ret;
        }

        static constexpr result_type min()
        {
            return numeric_limits<result_type>::min();
        }

        static constexpr result_type max()
        {
            return numeric_limits<result_type>::max();
        }

        static constexpr size_t seed_words = elements * sizeof(decltype(s_)::value_type) / sizeof(unsigned int);

        void seed(result_type s)
        {
            seed_seq seq{ s };

            seed(seq);
        }

        template<class Seq>
        void seed(Seq& seq)
        {
            array<unsigned int, seed_words> buffer;

            seq.generate(begin(buffer), end(buffer));

            auto pb = begin(buffer);
            for (auto s = begin(s_); s != end(s_); ++s)
            {
                for (auto i = 0; i < 2; ++i)
                {
                    uint64_t v = *pb++;

                    v |= static_cast<uint64_t>(*pb++) << 32;

                    s->m128i_i64[i] = v;
                }
            }

            stir();
        }

        void discard(unsigned long long count)
        {
        }
    };

    template<int Rounds = 1>
    class aesni8
    {
    private:
        constexpr static int elements = 8;
        std::array<__m128i, elements> s_;
        int i_ = 0;
        int j_ = 0;

        void stir()
        {
            for (auto i = 0; i < Rounds; ++i)
            {
                const auto s0 = s_[0];

                s_[0] = _mm_aesenc_si128(s_[1], s0);
                s_[1] = _mm_aesenc_si128(s_[2], s_[1]);
                s_[2] = _mm_aesenc_si128(s_[3], s_[2]);
                s_[3] = _mm_aesenc_si128(s_[4], s_[3]);
                s_[4] = _mm_aesenc_si128(s_[5], s_[4]);
                s_[5] = _mm_aesenc_si128(s_[6], s_[5]);
                s_[6] = _mm_aesenc_si128(s_[7], s_[6]);
                s_[7] = _mm_aesenc_si128(s0, s_[7]);
            }

            i_ = j_ = 0;
        }

    public:
        typedef uint64_t result_type;

        explicit aesni8(result_type x = 1)
        {
            seed(x);
        }

        template<class Seq>
        explicit aesni8(Seq& seq)
        {
            seed(seq);
        }

        result_type operator()()
        {
            if (j_ <= 0)
            {
                j_ = 1;

                return s_[i_].m128i_i64[0];
            }

            const auto ret = s_[i_].m128i_i64[1];

            j_ = 0;

            if (++i_ >= s_.size())
                stir();

            return ret;
        }

        static constexpr result_type min()
        {
            return numeric_limits<result_type>::min();
        }

        static constexpr result_type max()
        {
            return numeric_limits<result_type>::max();
        }

        static constexpr size_t seed_words = elements * sizeof(decltype(s_)::value_type) / sizeof(unsigned int);

        void seed(result_type s)
        {
            seed_seq seq{ s };

            seed(seq);
        }

        template<class Seq>
        void seed(Seq& seq)
        {
            array<unsigned int, seed_words> buffer;

            seq.generate(begin(buffer), end(buffer));

            auto pb = begin(buffer);
            for (auto s = begin(s_); s != end(s_); ++s)
            {
                for (auto i = 0; i < 2; ++i)
                {
                    uint64_t v = *pb++;

                    v |= static_cast<uint64_t>(*pb++) << 32;

                    s->m128i_i64[i] = v;
                }
            }

            stir();
        }

        void discard(unsigned long long count)
        {
        }
    };

    template<int Rounds = 1>
    class aesni16
    {
    private:
        constexpr static int elements = 16;
        std::array<__m128i, elements> s_;
        int i_ = 0;
        int j_ = 0;

        void stir()
        {
            for (auto i = 0; i < Rounds; ++i)
            {
                const auto s0 = s_[0];

                s_[0] = _mm_aesenc_si128(s_[1], s0);
                s_[1] = _mm_aesenc_si128(s_[2], s_[1]);
                s_[2] = _mm_aesenc_si128(s_[3], s_[2]);
                s_[3] = _mm_aesenc_si128(s_[4], s_[3]);
                s_[4] = _mm_aesenc_si128(s_[5], s_[4]);
                s_[5] = _mm_aesenc_si128(s_[6], s_[5]);
                s_[6] = _mm_aesenc_si128(s_[7], s_[6]);
                s_[7] = _mm_aesenc_si128(s_[8], s_[7]);
                s_[8] = _mm_aesenc_si128(s_[9], s_[8]);
                s_[9] = _mm_aesenc_si128(s_[10], s_[9]);
                s_[10] = _mm_aesenc_si128(s_[11], s_[10]);
                s_[11] = _mm_aesenc_si128(s_[12], s_[11]);
                s_[12] = _mm_aesenc_si128(s_[13], s_[12]);
                s_[13] = _mm_aesenc_si128(s_[14], s_[13]);
                s_[14] = _mm_aesenc_si128(s_[15], s_[14]);
                s_[15] = _mm_aesenc_si128(s0, s_[15]);
            }

            i_ = j_ = 0;
        }

    public:
        typedef uint64_t result_type;

        explicit aesni16(result_type x = 1)
        {
            seed(x);
        }

        template<class Seq>
        explicit aesni16(Seq& seq)
        {
            seed(seq);
        }

        result_type operator()()
        {
            if (j_ <= 0)
            {
                j_ = 1;

                return s_[i_].m128i_i64[0];
            }

            const auto ret = s_[i_].m128i_i64[1];

            j_ = 0;

            if (++i_ >= s_.size())
                stir();

            return ret;
        }

        static constexpr result_type min()
        {
            return numeric_limits<result_type>::min();
        }

        static constexpr result_type max()
        {
            return numeric_limits<result_type>::max();
        }

        static constexpr size_t seed_words = elements * sizeof(decltype(s_)::value_type) / sizeof(unsigned int);

        void seed(result_type s)
        {
            seed_seq seq{ s };

            seed(seq);
        }

        template<class Seq>
        void seed(Seq& seq)
        {
            array<unsigned int, seed_words> buffer;

            seq.generate(begin(buffer), end(buffer));

            auto pb = begin(buffer);
            for (auto s = begin(s_); s != end(s_); ++s)
            {
                for (auto i = 0; i < 2; ++i)
                {
                    uint64_t v = *pb++;

                    v |= static_cast<uint64_t>(*pb++) << 32;

                    s->m128i_i64[i] = v;
                }
            }

            stir();
        }

        void discard(unsigned long long count)
        {
        }
    };

    template<int Elements, int Rounds = 1>
    class aesni_n
    {
    private:
        std::array<__m128i, Elements> s_;
        int i_ = 0;
        int j_ = 0;

        void stir()
        {
            for (auto i = 0; i < Rounds; ++i)
            {
                const auto s0 = s_[0];

                for (auto j = 0; j < Elements - 1; ++j)
                    s_[j] = _mm_aesenc_si128(s_[j + 1], s0);
                s_[Elements - 1] = _mm_aesenc_si128(s0, s_[Elements - 1]);
            }

            i_ = j_ = 0;
        }

    public:
        typedef uint64_t result_type;

        explicit aesni_n(result_type x = 1)
        {
            seed(x);
        }

        template<class Seq>
        explicit aesni_n(Seq& seq)
        {
            seed(seq);
        }

        result_type operator()()
        {
            if (j_ <= 0)
            {
                j_ = 1;

                return s_[i_].m128i_u64[0];
            }

            const auto ret = s_[i_].m128i_u64[1];

            j_ = 0;

            if (++i_ >= s_.size())
                stir();

            return ret;
        }

        static constexpr result_type min()
        {
            return numeric_limits<result_type>::min();
        }

        static constexpr result_type max()
        {
            return numeric_limits<result_type>::max();
        }

        static constexpr size_t seed_words = Elements * sizeof(decltype(s_)::value_type) / sizeof(unsigned int);

        void seed(result_type s)
        {
            seed_seq seq{ s };

            seed(seq);
        }

        template<class Seq>
        void seed(Seq& seq)
        {
            array<unsigned int, seed_words> buffer;

            seq.generate(begin(buffer), end(buffer));

            auto pb = begin(buffer);
            for (auto s = begin(s_); s != end(s_); ++s)
            {
                for (auto i = 0; i < 2; ++i)
                {
                    uint64_t v = *pb++;

                    v |= static_cast<uint64_t>(*pb++) << 32;

                    s->m128i_u64[i] = v;
                }
            }

            stir();
        }

        void discard(unsigned long long count)
        {
        }
    };

    template<int Elements, int Rounds = 1>
    class aesni_n_u32
    {
    private:
        std::array<__m128i, Elements> s_;
        int i_ = 0;
        int j_ = 0;

        void stir()
        {
            for (auto i = 0; i < Rounds; ++i)
            {
                const auto s0 = s_[0];
                auto previous = s0;

                for (auto j = 0; j < Elements - 2; ++j)
                {
                    const auto value = s_[j + 1];

                    s_[j] = _mm_aesenc_si128(value, previous);

                    previous = value;
                }

                s_[Elements - 1] = _mm_aesenc_si128(s0, previous);
            }

            i_ = j_ = 0;
        }

    public:
        typedef uint32_t result_type;

        explicit aesni_n_u32(result_type x = 1)
        {
            seed(x);
        }

        template<class Seq>
        explicit aesni_n_u32(Seq& seq)
        {
            seed(seq);
        }

        result_type operator()()
        {
            const auto ret = s_[i_].m128i_u32[j_++];

            if (j_ <= 3)
                return ret;

            j_ = 0;

            if (++i_ >= s_.size())
                stir();

            return ret;
        }

        static constexpr result_type min()
        {
            return numeric_limits<result_type>::min();
        }

        static constexpr result_type max()
        {
            return numeric_limits<result_type>::max();
        }

        static constexpr size_t seed_words = Elements * sizeof(decltype(s_)::value_type) / sizeof(unsigned int);

        void seed(result_type s)
        {
            seed_seq seq{ s };

            seed(seq);
        }

        template<class Seq>
        void seed(Seq& seq)
        {
            array<unsigned int, seed_words> buffer;

            seq.generate(begin(buffer), end(buffer));

            auto pb = begin(buffer);
            for (auto s = begin(s_); s != end(s_); ++s)
            {
                for (auto i = 0; i < 2; ++i)
                {
                    uint64_t v = *pb++;

                    v |= static_cast<uint64_t>(*pb++) << 32;

                    s->m128i_u64[i] = v;
                }
            }

            stir();
        }

        void discard(unsigned long long count)
        {
        }
    };

    template<int Elements, int Rounds = 1>
    class aesni_n_u32_a
    {
    private:
        alignas(16) std::array<uint32_t, Elements * sizeof(__m128i) / sizeof(uint32_t) > s_;
        int i_ = 0;

        void stir()
        {
            for (auto i = 0; i < Rounds; ++i)
            {
                auto *p = reinterpret_cast<__m128i*>(&s_[0]);

                const auto s0 = _mm_load_si128(p++);
                auto previous = s0;

                for (auto j = 0; j < Elements - 2; ++j)
                {
                    const auto value = _mm_load_si128(p);

                    _mm_store_si128(p++, _mm_aesenc_si128(value, previous));

                    previous = value;
                }

                assert(&s_[(Elements - 1) * sizeof(__m128i) / sizeof(s_[0])] == reinterpret_cast<uint32_t*>(p));

                _mm_store_si128(p, _mm_aesenc_si128(s0, previous));
            }

            i_ = 0;
        }

    public:
        typedef uint32_t result_type;

        explicit aesni_n_u32_a(result_type x = 1)
        {
            seed(x);
        }

        template<class Seq>
        explicit aesni_n_u32_a(Seq& seq)
        {
            seed(seq);
        }

        result_type operator()()
        {
            const auto ret = s_[i_++];

            if (++i_ >= s_.size())
                stir();

            return ret;
        }

        static constexpr result_type min()
        {
            return numeric_limits<result_type>::min();
        }

        static constexpr result_type max()
        {
            return numeric_limits<result_type>::max();
        }

        static constexpr size_t seed_words = Elements * sizeof(decltype(s_)::value_type) / sizeof(unsigned int);

        void seed(result_type s)
        {
            seed_seq seq{ s };

            seed(seq);
        }

        template<class Seq>
        void seed(Seq& seq)
        {
            seq.generate(begin(s_), end(s_));

            stir();
        }

        void discard(unsigned long long count)
        {
        }
    };
}

#endif // RANDOM_AESNI_H
