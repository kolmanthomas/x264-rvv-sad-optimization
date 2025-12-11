#include <stdlib.h>
#include <time.h>
#include <stdio.h>

#include "sad_scalar.h"
#include "sad_rvv.h"

void randomize_block_16x16(uint8_t *block, intptr_t i_stride_block)
{
    for(int y = 0; y < 16; y++)
    {
        for(int x = 0; x < 16; x++)
        {
            block[x] = rand() % 256;
        }
        block += i_stride_block;
    }
}

int main(int argc, char *argv[])
{
    // Optionally receive seed and iteration count from args.
    long rng_seed;
    if (argc > 1)
    {
        rng_seed = strtol(argv[1], NULL, 10);
    }
    else
    {
        rng_seed = time(NULL);
    }

    printf("RNG seed is %ld\n", rng_seed);
    srand(rng_seed);

    // Display RVV and intrinsics support.
#if defined(__riscv_v)
    printf("RVV %u.%u.%u\n", __riscv_v / 1000000, __riscv_v % 1000000 / 1000, __riscv_v % 1000);
#else
    printf("RVV not supported\n");
#endif

#if defined(__riscv_v_intrinsic)
    printf("RVV intrinsics support = %d\n", __riscv_v_intrinsic);
#endif

    const int width = 16;
    const int height = 16;
    const int stride = 16;

    uint8_t *block1 = (uint8_t*)malloc(width * height * sizeof(uint8_t));
    uint8_t *block2 = (uint8_t*)malloc(width * height * sizeof(uint8_t));

    randomize_block_16x16(block1, stride);
    randomize_block_16x16(block2, stride);
    
    int sad_value = pixel_sad_16x16_scalar(block1, stride, block2, stride);
        
#if defined(__riscv) && defined(__riscv_v_intrinsic)
    int optimized_sad_value = pixel_sad_16x16_rvv_optimized(block1, stride, block2, stride);

    if (sad_value != optimized_sad_value)
    {
        printf("Different SAD results: scalar = %d, rvv = %d\n", sad_value, optimized_sad_value);
    }
#endif

    free(block1);
    free(block2);
    return 0;
}

