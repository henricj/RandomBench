#ifndef RANDOM_LCG_AES_H
#define RANDOM_LCG_AES_H

#include <array>
#include <memory>
#include <random>

namespace ExtraGenerators
{
   class lcg_aes
   {
      static constexpr int size_ = 2;

      __m128i s_[size_];
      __m128i c_[size_];

      __m128i out_[size_];
      __m128i a_;
      __m128i count_;

      int index_;
      int word_;
   public:
      typedef uint64_t result_type;

      explicit lcg_aes(result_type x = 1)
      {
         //align_state();

         init_a();

         seed(x);
      }

      template<class Seq>
      explicit lcg_aes(Seq& seq)
      {
         //align_state();
         init_a();

         seed(seq);
      }

      result_type operator()() noexcept
      {
         if (word_ >= 4)
         {
            if (++index_ >= size_)
            {
               const auto count_key = _mm_aesenc_si128(count_, count_);
               auto a = a_;
               auto prev = _mm_shuffle_epi32(out_[size_ - 1], _MM_SHUFFLE(1, 0, 3, 2));
               for (auto i = 0; i < size_; ++i)
               {
                  const auto key1 = _mm_aesenc_si128(prev, count_key);
                  const auto key0 = _mm_aesenc_si128(s_[i], prev);
                  s_[i] = _mm_add_epi32(_mm_mullo_epi32(a, s_[i]), c_[i]);
                  a = _mm_shuffle_epi32(a, _MM_SHUFFLE(0, 3, 2, 1));
                  prev = _mm_shuffle_epi32(out_[i], _MM_SHUFFLE(1, 0, 3, 2));
                  out_[i] = _mm_aesenc_si128(_mm_xor_si128(out_[i], key0), key1);
               }

               increment(count_);

               index_ = word_ = 0;
            }
         }

         return out_[index_].m128i_u32[word_++];
      }

      static constexpr result_type min()
      {
         return std::numeric_limits<result_type>::min();
      }

      static constexpr result_type max()
      {
         return std::numeric_limits<result_type>::max();
      }

      static constexpr size_t seed_words = ((1 + size_ * 3) * sizeof(__m128i)) / sizeof(unsigned int);

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

         auto p = begin(buffer);

         for (auto i = 0; i < size_; ++i)
         {
            for (auto j = 0; j < 4; ++j)
            {
               s_[i].m128i_u32[j] = *p++;
               c_[i].m128i_u32[j] = *p++ | 1;
               out_[i].m128i_u32[j] = *p++;
            }
         }
         for (auto j = 0; j < 4; ++j)
         {
            count_.m128i_u32[j] = *p++;
         }

         index_ = word_ = size_;
      }

      void discard(unsigned long long count)
      {
         while (count-- > 0)
            (*this)();
      }

   private:
      // https://stackoverflow.com/a/9350285
      static __m128i increment(__m128i x) {
         const __m128i ONE = _mm_setr_epi32(1, 0, 0, 0);
         const __m128i ZERO = _mm_setzero_si128();

         x = _mm_add_epi64(x, ONE);
         __m128i t = _mm_cmpeq_epi64(x, ZERO);
         t = _mm_unpacklo_epi64(ZERO, t);
         x = _mm_sub_epi64(x, t);

         return x;
      }
      
      void init_a()
      {
         a_ = _mm_set_epi32(214013, 17405, 214013, 69069);
      }
   };
}

#endif // RANDOM_LCG_AES_H
