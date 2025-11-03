///                                                                           
/// pot_t                                                                     
/// Copyright (c) 2025 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#pragma once
#include <bit>
#include <concepts>
#include <compare>

/// You decide whether POT types throw or not                                 
/// Very useful to catch arithmetic errors in your code in safe mode          
#ifdef LANGULUS_OPTION_SAFE_MODE
   #define lgls_has_assumptions
   #define lgls_if_safe(a) a
   #define lgls_if_unsafe(a)
   #define lgls_assume(CONDITION, MESSAGE) \
      if (not static_cast<bool>(CONDITION)) \
         throw ::std::runtime_error {MESSAGE};
   #define lgls_assume_and_optimize(CONDITION, MESSAGE) \
      if (not static_cast<bool>(CONDITION)) \
         throw ::std::runtime_error {MESSAGE}; \
      [[assume(CONDITION)]]
#else
   #define lgls_has_assumptions noexcept
   #define lgls_if_safe(a)
   #define lgls_if_unsafe(a) a
   #define lgls_assume(CONDITION, MESSAGE)
#endif

#if defined(_MSC_VER) and not defined(__clang__)
   #define lgls_inline __forceinline
   #define lgls_pure
#else
   #define lgls_inline __attribute__((always_inline)) inline
   #define lgls_pure __attribute__((pure))
#endif


namespace Langulus
{
   using ::std::size_t;

   struct potnz_t;
   struct pot_t;

   /// Power-of-two unsigned integer concept                                  
   template<class T>
   concept pot_unsigned_integral =
           ::std::same_as<::std::remove_cv_t<T>, potnz_t>
        or ::std::same_as<::std::remove_cv_t<T>, pot_t>;

   /// Expand the concept of unsigned_integral to include our pot types       
   template<class T>
   concept unsigned_integral = ::std::unsigned_integral<T> or pot_unsigned_integral<T>;


   ///                                                                        
   /// A non-zero unsigned power-of-two number, represented by the index of   
   /// the most significant bit, forming the following mapping:               
   ///            0_potnz -> 1u                                               
   ///            1_potnz -> 2u                                               
   ///            2_potnz -> 4u                                               
   ///            3_potnz -> 8u ...                                           
   /// The non-zero POT value is slightly faster on expansion, but slower on  
   /// compression. Useful if you plan on interacting with built-in integers. 
   struct potnz_t final {
      static constexpr bool zeroable = false;

      uint8_t bit;

      lgls_inline
      constexpr potnz_t() noexcept = default;

      lgls_inline
      constexpr potnz_t(potnz_t const&) noexcept = default;

      template<pot_unsigned_integral T> lgls_inline
      explicit constexpr potnz_t(T const& other) lgls_has_assumptions {
         lgls_assume_and_optimize(other.bit != 0, "potnz_t can't be constructed with zero");
         bit = other.bit - 1;
      }
      
      template<::std::unsigned_integral T> lgls_inline
      explicit constexpr potnz_t(T const& other) lgls_has_assumptions {
         lgls_assume(::std::has_single_bit(other), "potnz_t can't be constructed with value");
         bit = ::std::bit_width(other) - 1;
      }
      
      lgls_inline
      constexpr potnz_t& operator = (potnz_t const& rhs) noexcept = default;

      template<pot_unsigned_integral T> lgls_inline
      constexpr potnz_t& operator = (T const& rhs) lgls_has_assumptions {
         lgls_assume_and_optimize(rhs.bit != 0, "potnz_t can't be assigned with zero");
         bit = rhs.bit - 1;
         return *this;
      }

      template<::std::unsigned_integral T> lgls_inline
      constexpr potnz_t& operator = (T const& rhs) lgls_has_assumptions {
         lgls_assume(::std::has_single_bit(rhs), "potnz_t can't be assigned with value");
         bit = ::std::bit_width(rhs) - 1;
         return *this;
      }

      lgls_inline lgls_pure
      constexpr explicit operator bool () const noexcept {
         return true;
      }

      lgls_inline lgls_pure
      constexpr bool operator ==  (potnz_t const& rhs) const noexcept = default;

      lgls_inline lgls_pure
      constexpr auto operator <=> (potnz_t const& rhs) const noexcept = default;

      template<::std::unsigned_integral T> lgls_inline lgls_if_unsafe(lgls_pure)
      constexpr explicit operator T () const lgls_has_assumptions {
         lgls_assume_and_optimize(bit + 1 < sizeof(T) * 8, "potnz_t is too big to fit into T");
         return T {1u} << bit;
      }

      lgls_inline lgls_if_unsafe(lgls_pure)
      constexpr uintptr_t mask() const lgls_has_assumptions {
         return operator uintptr_t () - 1;
      }
   };


   ///                                                                        
   /// An unsigned power-of-two number, represented by the index of the       
   /// most significant bit, forming the following mapping:                   
   ///          0_pot -> 0u                                                   
   ///          1_pot -> 1u                                                   
   ///          2_pot -> 2u                                                   
   ///          3_pot -> 4u ...                                               
   /// The non-zero POT value is slightly faster on compression, but slower on
   /// expansion. Useful if you plan on interacting with other pot_t.         
   struct pot_t final {
      static constexpr bool zeroable = true;

      uint8_t bit;

      lgls_inline
      constexpr pot_t() noexcept = default;

      lgls_inline
      constexpr pot_t(pot_t const&) noexcept = default;

      template<pot_unsigned_integral T> lgls_inline
      explicit constexpr pot_t(T const& other) lgls_has_assumptions {
         lgls_assume_and_optimize(other.bit != 255, "potnz_t is too big");
         bit = other.bit + 1;
      }
      
      template<::std::unsigned_integral T> lgls_inline
      explicit constexpr pot_t(T const& other) lgls_has_assumptions {
         lgls_assume(other == 0 or ::std::has_single_bit(other),
            "pot_t can't be constructed with value");
         bit = ::std::bit_width(other);
      }
      
      lgls_inline
      constexpr pot_t& operator = (pot_t const& rhs) noexcept = default;

      template<pot_unsigned_integral T> lgls_inline
      constexpr pot_t& operator = (T const& rhs) lgls_has_assumptions {
         lgls_assume_and_optimize(rhs.bit != 255, "potnz_t is too big");
         bit = rhs.bit + 1;
         return *this;
      }

      template<::std::unsigned_integral T> lgls_inline
      constexpr pot_t& operator = (T const& rhs) lgls_has_assumptions {
         lgls_assume(rhs == 0 or ::std::has_single_bit(rhs),
            "pot_t can't be constructed with value");
         bit = ::std::bit_width(rhs);
         return *this;
      }
      
      lgls_inline lgls_pure
      constexpr explicit operator bool () const noexcept {
         return static_cast<bool>(bit);
      }

      lgls_inline lgls_pure
      constexpr bool operator ==  (pot_t const& rhs) const noexcept = default;

      lgls_inline lgls_pure
      constexpr auto operator <=> (pot_t const& rhs) const noexcept = default;

      template<::std::unsigned_integral T>
      lgls_inline lgls_if_unsafe(lgls_pure)
      constexpr explicit operator T () const lgls_has_assumptions {
         if (bit == 0)
            return T {0};

         lgls_assume_and_optimize(bit < sizeof(T) * 8, "pot_t is too big to fit into T");
         return T {1} << (bit - 1);
      }

      lgls_inline lgls_if_unsafe(lgls_pure)
      constexpr uintptr_t mask() const lgls_has_assumptions {
         return operator uintptr_t () - 1;
      }
   };


   /// Literal operators                                                      
   lgls_inline lgls_if_unsafe(lgls_pure)
   constexpr potnz_t operator""_potnz(unsigned long long num) lgls_has_assumptions {
      return potnz_t(num);
   }

   lgls_inline lgls_if_unsafe(lgls_pure)
   constexpr pot_t operator""_pot(unsigned long long num) lgls_has_assumptions {
      return pot_t(num);
   }


   /// Comparison                                                             
   template<pot_unsigned_integral LHS, unsigned_integral RHS>
   lgls_inline lgls_if_unsafe(lgls_pure)
   constexpr bool operator == (LHS const& lhs, RHS const& rhs) lgls_has_assumptions {
      return static_cast<RHS>(lhs) == rhs;
   }

   template<unsigned_integral LHS, pot_unsigned_integral RHS>
   lgls_inline lgls_if_unsafe(lgls_pure)
   constexpr bool operator == (LHS const& lhs, RHS const& rhs) lgls_has_assumptions {
      return lhs == static_cast<LHS>(rhs);
   }

   template<pot_unsigned_integral LHS, unsigned_integral RHS>
   lgls_inline lgls_if_unsafe(lgls_pure)
   constexpr auto operator <=> (LHS const& lhs, RHS const& rhs) lgls_has_assumptions {
      return static_cast<RHS>(lhs) <=> rhs;
   }

   template<unsigned_integral LHS, pot_unsigned_integral RHS>
   lgls_inline lgls_if_unsafe(lgls_pure)
   constexpr auto operator <=> (LHS const& lhs, RHS const& rhs) lgls_has_assumptions {
      return lhs <=> static_cast<LHS>(rhs);
   }

   /// Addition                                                               
   template<pot_unsigned_integral LHS, pot_unsigned_integral RHS>
   lgls_inline lgls_if_unsafe(lgls_pure)
   constexpr size_t operator + (LHS const& lhs, RHS const& rhs) lgls_has_assumptions {
      return static_cast<size_t>(lhs) + static_cast<size_t>(rhs);
   }

   template<pot_unsigned_integral LHS, unsigned_integral RHS>
   lgls_inline lgls_if_unsafe(lgls_pure)
   constexpr RHS operator + (LHS const& lhs, RHS const& rhs) lgls_has_assumptions {
      return static_cast<RHS>(lhs) + rhs;
   }

   template<unsigned_integral LHS, pot_unsigned_integral RHS>
   lgls_inline lgls_if_unsafe(lgls_pure)
   constexpr LHS operator + (LHS const& lhs, RHS const& rhs) lgls_has_assumptions {
      return lhs + static_cast<LHS>(rhs);
   }

   /// Subtraction                                                            
   template<pot_unsigned_integral LHS, pot_unsigned_integral RHS>
   lgls_inline lgls_if_unsafe(lgls_pure)
   constexpr size_t operator - (LHS const& lhs, RHS const& rhs) lgls_has_assumptions {
      return static_cast<size_t>(lhs) - static_cast<size_t>(rhs);
   }

   template<pot_unsigned_integral LHS, unsigned_integral RHS>
   lgls_inline lgls_if_unsafe(lgls_pure)
   constexpr RHS operator - (LHS const& lhs, RHS const& rhs) lgls_has_assumptions {
      return static_cast<RHS>(lhs) - rhs;
   }

   template<unsigned_integral LHS, pot_unsigned_integral RHS>
   lgls_inline lgls_if_unsafe(lgls_pure)
   constexpr LHS operator - (LHS const& lhs, RHS const& rhs) lgls_has_assumptions {
      return lhs - static_cast<LHS>(rhs);
   }

   /// Division                                                               
   template<pot_unsigned_integral LHS, pot_unsigned_integral RHS>
   lgls_inline lgls_if_unsafe(lgls_pure)
   constexpr LHS operator / (LHS const& lhs, RHS const& rhs) lgls_has_assumptions {
      LHS result;
      if constexpr (not LHS::zeroable) {
         if constexpr (not RHS::zeroable) {
            lgls_assume_and_optimize(rhs.bit <= lhs.bit, "division resulted in zero");
            result.bit = lhs.bit - rhs.bit;
         }
         else {
            lgls_assume_and_optimize(rhs.bit > 0, "division by zero");
            lgls_assume_and_optimize(rhs.bit - 1u <= lhs.bit, "division resulted in zero");
            result.bit = lhs.bit - rhs.bit - 1u;
         }
      }
      else {
         if constexpr (RHS::zeroable) {
            lgls_assume_and_optimize(rhs.bit > 0, "division by zero");
            if (rhs.bit > lhs.bit)
               result.bit = 0;
            else
               result.bit = lhs.bit - rhs.bit;
         }
         else {
            if (rhs.bit + 1u > lhs.bit)
               result.bit = 0;
            else
               result.bit = lhs.bit - rhs.bit + 1u;
         }
      }
      return result;
   }

   template<pot_unsigned_integral LHS, unsigned_integral RHS>
   lgls_inline lgls_if_unsafe(lgls_pure)
   constexpr RHS operator / (LHS const& lhs, RHS const& rhs) lgls_has_assumptions {
      return static_cast<RHS>(lhs) / rhs;
   }

   template<unsigned_integral LHS, pot_unsigned_integral RHS>
   lgls_inline lgls_if_unsafe(lgls_pure)
   constexpr LHS operator / (LHS const& lhs, RHS const& rhs) lgls_has_assumptions {
      return lhs / static_cast<LHS>(rhs);
   }

   /// Multiplication                                                         
   template<pot_unsigned_integral LHS, pot_unsigned_integral RHS>
   lgls_inline lgls_if_unsafe(lgls_pure)
   constexpr LHS operator * (LHS const& lhs, RHS const& rhs) lgls_has_assumptions {
      LHS result;
      if constexpr (not LHS::zeroable) {
         if constexpr (not RHS::zeroable) {
            lgls_assume_and_optimize(lhs.bit + rhs.bit <= 255u, "multiplication overflowed");
            result.bit = lhs.bit + rhs.bit;
         }
         else {
            lgls_assume_and_optimize(rhs.bit, "multiplication resulted in zero");
            lgls_assume_and_optimize(lhs.bit + (rhs.bit-1u) <= 255u, "multiplication overflowed");
            result.bit = lhs.bit + rhs.bit - 1u;
         }
      }
      else {
         if constexpr (RHS::zeroable) {
            lgls_assume_and_optimize(lhs.bit + rhs.bit <= 255u, "multiplication overflowed");
            result.bit = lhs.bit + rhs.bit;
         }
         else {
            lgls_assume_and_optimize(lhs.bit + rhs.bit + 1 <= 255u, "multiplication overflowed");
            result.bit = lhs.bit + rhs.bit + 1;
         }
      }
      return result;
   }

   template<pot_unsigned_integral LHS, unsigned_integral RHS>
   lgls_inline lgls_if_unsafe(lgls_pure)
   constexpr RHS operator * (LHS const& lhs, RHS const& rhs) lgls_has_assumptions {
      return static_cast<RHS>(lhs) * rhs;
   }

   template<unsigned_integral LHS, pot_unsigned_integral RHS>
   lgls_inline lgls_if_unsafe(lgls_pure)
   constexpr LHS operator * (LHS const& lhs, RHS const& rhs) lgls_has_assumptions {
      return lhs * static_cast<LHS>(rhs);
   }

   /// Shifting right                                                         
   template<pot_unsigned_integral LHS, pot_unsigned_integral RHS> lgls_inline
   constexpr potnz_t& operator >>= (LHS& lhs, RHS const& rhs) lgls_has_assumptions {
      if constexpr (LHS::zeroable) {
         if (lhs.bit == 0)
            return lhs;
      }

      lgls_assume_and_optimize(rhs.bit <= lhs.bit, "right shift underflowed");
      lhs.bit -= rhs.bit;
      return lhs;
   }

   /// Shifting left                                                          
   template<pot_unsigned_integral LHS, pot_unsigned_integral RHS> lgls_inline
   constexpr potnz_t& operator <<= (LHS& lhs, RHS const& rhs) lgls_has_assumptions {
      if constexpr (LHS::zeroable) {
         if (lhs.bit == 0)
            return lhs;
      }

      lgls_assume_and_optimize(rhs.bit + lhs.bit <= 255u, "left shift overflowed");
      lhs.bit += rhs.bit;
      return lhs;
   }
}