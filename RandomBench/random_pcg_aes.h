#ifndef RANDOM_XOROSHIRO128PLUS_AES_H
#define RANDOM_XOROSHIRO128PLUS_AES_H

#include <array>
#include <memory>
#include <random>

namespace ExtraGenerators
{
#if defined(_MSC_VER) && _MSC_VER >= 1800
#define RESTRICT __restrict
#elif 0 // Check for GCC and clang versions
#define RESTRICT __restrict__
#else
#define RESTRICT
#endif
   class lcgaes
   {
      __m128i s_;
      __m128i prev_;

   public:
      typedef uint64_t result_type;

      explicit lcgaes(result_type x = 1)
      {
         //align_state();

         seed(x);
      }

      template<class Seq>
      explicit lcgaes(Seq& seq)
      {
         //align_state();

         seed(seq);
      }

      result_type operator()() noexcept
      {
         const auto out = prev_.m128i_u64[0];
         //const auto result = s0 + s1;

         prev_ = _mm_aesenc_si128(prev_, s_);

         const auto s1 = s_.m128i_u64[0] ^ s_.m128i_u64[1];
         s_.m128i_u64[0] = rotl(s_.m128i_u64[0], 55) ^ s1 ^ (s1 << 14);
         s_.m128i_u64[1] = rotl(s1, 36);

         return out;
      }

      static constexpr result_type min()
      {
         return std::numeric_limits<result_type>::min();
      }

      static constexpr result_type max()
      {
         return std::numeric_limits<result_type>::max();
      }

      static constexpr size_t seed_words = (2 * sizeof(__m128i)) / sizeof(unsigned int);

      void seed(result_type s)
      {
         std::seed_seq seq{ s };

         seed(seq);
      }

      template<class Seq>
      void seed(Seq& seq)
      {
         std::array<unsigned int, seed_words> buffer{};

         seq.generate(begin(buffer), end(buffer));

         for (auto i = 0; i < 4; ++i)
         {
            s_.m128i_u32[i] = buffer[i];
            prev_.m128i_u32[i] = buffer[i + 4];
         }
      }

      void discard(unsigned long long count)
      {
         while (count-- > 0)
            (*this)();
      }
   };

    class xoroshiro128plus_aes
    {
        // From http://xorshift.di.unimi.it/
        // http://xorshift.di.unimi.it/xoroshiro128plus.c
        // "Written in 2016 by David Blackman and Sebastiano Vigna (vigna@acm.org)"

        alignas(16) std::array<uint64_t, 2> s_;
        alignas(16) std::array<uint64_t, 2> prev_;

        //std::array<uint64_t, 3> raw_;
        //uint64_t* RESTRICT s_;

        //void align_state() noexcept
        //{
        //    auto sz = sizeof(raw_);
        //    void* p = &raw_[0];

        //    std::align(16, 2 * sizeof(uint64_t), p, sz);

        //    s_ = reinterpret_cast<uint64_t *>(p);
        //}

        static uint64_t rotl(const uint64_t x, int k) noexcept
        {
            return _rotl64(x, k);
            //return (x << k) | (x >> (64 - k));
        }

    public:
        typedef uint64_t result_type;

        explicit xoroshiro128plus_aes(result_type x = 1)
        {
            //align_state();

            seed(x);
        }

        template<class Seq>
        explicit xoroshiro128plus_aes(Seq& seq)
        {
            //align_state();

            seed(seq);
        }

        result_type operator()() noexcept
        {
            const auto s0 = s_[0];
            auto s1 = s_[1];
            //const auto result = s0 + s1;

            const auto out = prev_[0];

            const auto x = _mm_load_si128(reinterpret_cast<__m128i* RESTRICT>(&s_[0]));
            const auto prev = _mm_load_si128(reinterpret_cast<__m128i* RESTRICT>(&prev_[0]));

            s1 ^= s0;
            s_[0] = rotl(s0, 55) ^ s1 ^ (s1 << 14);
            s_[1] = rotl(s1, 36);

            *reinterpret_cast<__m128i* RESTRICT>(&prev_[0]) = _mm_aesenc_si128(prev, x);

            return out;
        }

        static constexpr result_type min()
        {
            return std::numeric_limits<result_type>::min();
        }

        static constexpr result_type max()
        {
            return std::numeric_limits<result_type>::max();
        }

        static constexpr size_t seed_words = (4 * sizeof(uint64_t)) / sizeof(unsigned int);

        void seed(result_type s)
        {
            std::seed_seq seq{ s };

            seed(seq);
        }

        template<class Seq>
        void seed(Seq& seq)
        {
            std::array<unsigned int, seed_words> buffer{};

            seq.generate(begin(buffer), end(buffer));

            for (auto i = 0; i < 4; ++i)
            {
               s_[i] = buffer[i];
               prev_[i] = buffer[i + 4];
            }
        }

        void discard(unsigned long long count)
        {
            while (count-- > 0)
                (*this)();
        }
    };
}

#endif // RANDOM_XOROSHIRO128PLUS_AES_H
