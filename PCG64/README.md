# PCG64 XSL-RR (128/64)

A small C implementation of a PCG-style pseudorandom number generator with a 128-bit internal state and 64-bit output. The API includes seeding, floating-point generation, inclusive integer ranges, and a helper that prints generated values.

> **Note:** This generator is intended for general-purpose, non-cryptographic use. Do not use it for passwords, keys, tokens, or other security-sensitive purposes.

## Features:

- 64-bit random output from a 128-bit state
- Seed and stream parameters for initializing generators
- `double` values in the range `[0, 1)`
- Inclusive integer ranges using rejection sampling
- A helper for printing a sequence of values

## Files:

| File         | Description                                     |
| ------------ | ----------------------------------------------- |
| `pcg64.h`    | Generator type and public function declarations |
| `pcg64.c`    | Generator implementation                        |
| `libpcg64.a` | Static PCG64 Pre-Built Library                  |

## Requirements:

The implementation uses the compiler extension `__uint128_t`. Use a compiler that supports 128-bit integer types, such as GCC or Clang.

## Build:

Compile the implementation as an object file:

```bash
gcc -std=gnu11 -O2 -Wall -Wextra -Iinclude src/pcg64.c -o pcg64.o
```

To build an executable, link `pcg64.c` with a source file that provides `main()`:

```bash
gcc -std=gnu11 -O2 -Wall -Wextra -Iinclude src/example.c src/pcg64.c -o bin/main
```

Link the pre-built static library:

```bash
gcc -std=gnu11 -O2 -Wall -Wextra -Iinclude src/example.c -Llib -lpcg64 -o bin/main
```

## Quick Start:

```c
#include <stdio.h>
#include "pcg64.h"

int main(void)
{
    pcg64_t rng;
    pcg64_seed(&rng, 12345, 67890);

    printf("Random integer: %llu\n",
           (unsigned long long)pcg64_uint64(&rng));
    printf("Random double:  %.17f\n", pcg64_double(&rng));
    printf("Random in [10, 20]: %llu\n",
           (unsigned long long)pcg64_range(&rng, 10, 20));

    return 0;
}
```

Save this as `example.c`, then build and run it:

```bash
gcc -std=gnu11 -O2 -Wall -Wextra example.c pcg64.c -o example
./main
```

The same seed and stream produce the same sequence of values.

## API:

### `pcg64_seed`:

```c
void pcg64_seed(pcg64_t *rng, uint64_t seed, uint64_t stream);
```

Initializes a generator using a seed and stream value. The stream value is converted to an odd increment, as required by the generator.

### `pcg64_random`:

```c
uint64_t pcg64_random(pcg64_t *rng);
```

Advances the generator and returns a 64-bit pseudorandom value.

### `pcg64_uint64`:

```c
uint64_t pcg64_uint64(pcg64_t *rng);
```

Convenience wrapper that returns the next 64-bit value, equivalent to `pcg64_random()`.

### `pcg64_double`:

```c
double pcg64_double(pcg64_t *rng);
```

Returns a floating-point value in `[0, 1)`.

### `pcg64_range`:

```c
uint64_t pcg64_range(pcg64_t *rng, uint64_t min, uint64_t max);
```

Returns an integer in the inclusive range `[min, max]`. If `min > max`, the function returns `0`. The full `uint64_t` range is supported.

### `pcg64_generate`:

```c
void pcg64_generate(
    pcg64_t *rng,
    uint64_t min,
    uint64_t max,
    int count
);
```

Generates and prints `count` values in the inclusive range `[min, max]`, one per line. Output is written to standard output.

## Generator State:

The public generator type stores its internal state and stream increment:

```c
typedef struct
{
    __uint128_t state;
    __uint128_t inc;
} pcg64_t;
```

Keep each generator instance separate when independent sequences are needed. Do not share a generator between threads without external synchronization.

## Limitations:

1) Non-standard type:

   - `__uint128_t` is GCC/Clang-specific and not portable ISO C.
   - It will fail on many compilers / platforms.

2) Missing full PCG64 specification details:

   - This is not a complete "PCG64" implementation in the strictest sense.
   - It uses a simplified state layout and output function, but not all
     PCG64 variants or validation vectors are covered.

3) Seeding limitations:

   - Seed and stream are only 64-bit values.
   - The full PCG64 state space is broader than this API exposes.

4) Range generation issue:

   - if `(min > max) returns 0`, which may hide user error instead of
     reporting it.
   - The code also does not handle invalid ranges in a robust way.

5) No statistical validation:

   - It has not been tested against known PCG64 outputs or extensive
     randomness tests.
   - A simple implementation may still have subtle bugs.

6) Not cryptographic:

   - This generator is not suitable for security-sensitive use, keys,
     nonces, tokens, or anything requiring unpredictability against
     attackers.

7) Not thread-safe by default:

   - A single `rng` object is not safe to use concurrently without external
     synchronization.

8) Output function is unusual for some users:

   - The rotation logic is valid for PCG output, but it is easy to
     implement incorrectly.
   - If a developer expects a different PCG64 variant, behavior may differ.

9) `pcg64_generate()` is not general-purpose:

   - It prints directly to stdout and is tightly coupled to I/O.
   - It is less reusable than a generator that returns values to caller code.

10) Compiler assumptions:

    - Requires a compiler that supports 128-bit integers and correct
      64-bit integer operations.