#include <stdio.h>
#include "pcg64.h"

int main(void)
{
    pcg64_t rng;
    pcg64_seed(&rng, 12345, 67891);

    printf("Random integer: %llu\n",
           (unsigned long long)pcg64_uint64(&rng));
    printf("Random double:  %.17f\n", pcg64_double(&rng));
    printf("Random in [10, 20]: %llu\n",
           (unsigned long long)pcg64_range(&rng, 10, 20));

    return 0;
}
