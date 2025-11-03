C++23 type-safe power-of-two unsigned integers for your compression needs. Includes two types:
 - `Langulus::potnz_t`: a non-zero unsigned power-of-two integer. Slightly faster on expansion, but slower on compression. Useful if you plan on interacting with built-in integers.
 - `Langulus::pot_t`: a zeroable unsigned power-of-two integer. Slightly faster on compression, but slower on expansion. Useful if you plan on interacting with other pot_t.

-----------------

✅ Always inlined<br>
✅ No side effects (when compiled with G++/Clang)<br>
✅ Type-safe<br>
✅ Supports constexpr<br>
✅ Behaves like a built-in integer - will never default-initialize to 0<br>
✅ Will throw on arithmetic error/bad initialization when `LANGULUS_OPTION_SAFE_MODE` is defined

-----------------

### Download the library directly from cmake, and link it:
```cmake
include(FetchContent)
FetchContent_Declare(LangulusPot
    GIT_REPOSITORY  https://github.com/Epixu/pot_t.git
    GIT_TAG         main
    EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(LangulusPot)

target_link_libraries(YourTarget PUBLIC LangulusPot)
```

### Usage:
```c++
#include <Langulus/Pot.hpp>

using namespace Langulus;

// not allowed - avoids implicit coversions and subtle errors
// constexpr pot_t value  = 1024;
constexpr pot_t   value   = 1024_pot;   // ok
constexpr pot_t   value2  = 1024_potnz; // ok
constexpr potnz_t nzvalue = 64_potnz;   // ok

static_assert(1024_pot == 1024_pot);
static_assert(1024_pot == 1024_potnz);

// not allowed, integer must be unsigned - avoids implicit coversions
// static_assert(1024_pot == 1024);     
static_assert(1024_pot == 1024u);       // ok
static_assert(1024_pot != 1025u);
static_assert(1024_pot >  64u);

constexpr size_t         cast = static_cast<size_t>(1024_pot);
constexpr size_t     expanded = 1024_pot + 64_pot;
constexpr pot_t  not_expanded = 1024_pot * 64_pot;

static_assert(    expanded == 1024u + 64u);
static_assert(not_expanded == 1024u * 64u);

// will throw if LANGULUS_OPTION_SAFE_MODE cmake option is enabled:
potnz_t nzvalue = 0;
// same applies for arithmetic operations that result in zero and involve potnz_t

// will throw if LANGULUS_OPTION_SAFE_MODE cmake option is enabled:
uint8_t small = static_cast<uint8_t>(1024_pot);
uint8_t small = static_cast<uint8_t>(128_pot);  // ok
```
