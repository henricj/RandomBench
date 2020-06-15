#ifndef RANDOM_CTR_AES_H
#define RANDOM_CTR_AES_H

#include <array>
#include <memory>
#include <random>

namespace ExtraGenerators
{
   class ctr_aes
   {
      static constexpr int size_ = 4;
      static constexpr int rounds_ = 4;

      __m128i key_[rounds_ + 1];
      __m128i out_[size_];
      __m128i count_;

      int index_;
      int word_;
   public:
      typedef uint64_t result_type;

      explicit ctr_aes(result_type x = 1)
      {
         seed(x);
      }

      template<class Seq>
      explicit ctr_aes(Seq& seq)
      {
         seed(seq);
      }

      result_type operator()() noexcept
      {
         if (word_ >= 4)
         {
            if (++index_ >= size_)
            {
               out_[0] = _mm_xor_si128(count_, key_[0]);
               increment(count_);
               out_[0] = _mm_aesenc_si128(out_[0], key_[1]);

               out_[1] = _mm_xor_si128(count_, key_[0]);
               increment(count_);
               out_[1] = _mm_aesenc_si128(out_[1], key_[1]);

               out_[2] = _mm_xor_si128(count_, key_[0]);
               increment(count_);
               out_[2] = _mm_aesenc_si128(out_[2], key_[1]);

               out_[3] = _mm_xor_si128(count_, key_[0]);
               increment(count_);
               out_[3] = _mm_aesenc_si128(out_[3], key_[1]);

               out_[0] = _mm_aesenc_si128(out_[0], key_[2]);
               out_[1] = _mm_aesenc_si128(out_[1], key_[2]);
               out_[2] = _mm_aesenc_si128(out_[2], key_[2]);
               out_[3] = _mm_aesenc_si128(out_[3], key_[2]);

               out_[0] = _mm_aesenc_si128(out_[0], key_[3]);
               out_[1] = _mm_aesenc_si128(out_[1], key_[3]);
               out_[2] = _mm_aesenc_si128(out_[2], key_[3]);
               out_[3] = _mm_aesenc_si128(out_[3], key_[3]);

               out_[0] = _mm_aesenclast_si128(out_[0], key_[4]);
               out_[1] = _mm_aesenclast_si128(out_[1], key_[4]);
               out_[2] = _mm_aesenclast_si128(out_[2], key_[4]);
               out_[3] = _mm_aesenclast_si128(out_[3], key_[4]);

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

      static constexpr size_t seed_words = ((1 + 1 + rounds_) * sizeof(__m128i)) / sizeof(unsigned int);

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

         for (auto & k : key_)
         {
            for (auto& c : k.m128i_u32)
            {
               c = *p++;
            }
         }

         for (auto& c : count_.m128i_u32)
         {
            c = *p++;
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
   };
}

#endif // RANDOM_CTR_AES_H
