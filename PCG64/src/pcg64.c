#include <stdio.h>
#include <stdint.h>
#include "pcg64.h"

/* PCG64 multiplier */
#define PCG64_MULTIPLIER                           \
    (((__uint128_t)2549297995355413924ULL << 64) | \
     4865540595714422341ULL)

/* Initialize generator */
void pcg64_seed(pcg64_t *rng, uint64_t seed, uint64_t stream)
{
    rng->state = 0;

    /* increment must be odd */
    rng->inc = ((__uint128_t)stream << 1) | 1;

    /* advance once */
    pcg64_random(rng);

    rng->state += seed;

    /* advance again */
    pcg64_random(rng);
}

/* Generate 64-bit random number */
uint64_t pcg64_random(pcg64_t *rng)
{
    rng->state = rng->state * PCG64_MULTIPLIER + rng->inc;

    __uint128_t state = rng->state;

    uint64_t x = (uint64_t)(state ^ (state >> 64));

    uint64_t rot = (uint64_t)(state >> 122);

    return (x >> rot) |
           (x << ((-rot) & 63));
}

/* Convert to double [0,1) */
double pcg64_double(pcg64_t *rng)
{
    return (pcg64_random(rng) >> 11) *
           (1.0 / 9007199254740992.0);
}

/* Generate an unsigned 64-bit integer */
uint64_t pcg64_uint64(pcg64_t *rng)
{
    return pcg64_random(rng);
}

/* Generate integer between min and max (inclusive) */
uint64_t pcg64_range(pcg64_t *rng, uint64_t min, uint64_t max)
{
    if (min > max)
        return 0; /* or handle as an error */

    uint64_t range = max - min + 1;

    if (range == 0)
        return pcg64_random(rng); /* full uint64_t range */

    uint64_t threshold = -range % range;
    uint64_t r;

    do
    {
        r = pcg64_random(rng);
    } while (r < threshold);

    return min + (r % range);
}

/* Generate N integers between min and max */
void pcg64_generate(
    pcg64_t *rng,
    uint64_t min,
    uint64_t max,
    int count)
{
    for (int i = 0; i < count; i++)
    {
        printf("%llu\n",
               (unsigned long long)pcg64_range(rng, min, max));
    }
}
