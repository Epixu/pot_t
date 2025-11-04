///                                                                           
/// pot_t                                                                     
/// Copyright (c) 2025 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#include <catch2/catch.hpp>
#include <Langulus/Pot.hpp>

using namespace Langulus;

namespace Catch
{
   template<>
   struct StringMaker<::Langulus::pot_t> {
      static ::std::string convert(::Langulus::pot_t const& value) {
         return std::to_string(static_cast<size_t>(value));
      }
   };
}

SCENARIO("Testing pot_t", "[pot_t]") {
   constexpr potnz_t nzvalue = 64_potnz;
   constexpr pot_t   value   = 1024_pot;

   static_assert(nzvalue == 64u);
   static_assert(nzvalue != 65u);
   static_assert(nzvalue >  63u);

   static_assert(value == 1024u);
   static_assert(value != 1025u);
   static_assert(value >  64u);

#ifdef LANGULUS_OPTION_SAFE_MODE
   REQUIRE_THROWS(0_potnz);
   REQUIRE_NOTHROW(1_potnz);
   REQUIRE_NOTHROW(0_pot);
   REQUIRE_NOTHROW(1_pot);
#endif
}
