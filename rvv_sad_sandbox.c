#include <stdlib.h>
#include <time.h>
#include <stdio.h>

#include "sad_scalar.h"
#include "sad_rvv.h"

int main()
{
    srand(time(NULL));

    const int width = 16;
    const int height = 16;
    const int stride = 16;

    uint8_t *block1 = (uint8_t*)malloc(width * height * sizeof(uint8_t));
    uint8_t *block2 = (uint8_t*)malloc(width * height * sizeof(uint8_t));

    for(int y = 0; y < height; y++)
      {
          for(int x = 0; x < width; x++)
          {
              block1[y * stride + x] = rand() % 256;
              block2[y * stride + x] = rand() % 256;
          }
      }

    int sad_value = pixel_sad_16x16_scalar(block1, stride, block2, stride);

#if defined(__riscv)
    int optimized_sad_value = pixel_sad_16x16_rvv_optimized(block1, stride, block2, stride);
#endif

    printf("SAD between block1 and block2: %d\n", sad_value);
    printf("Average absolute difference per pixel: %.2f\n", sad_value / 256.0);
#if defined(__riscv)
    printf("Optimized SAD between block1 and block2: %d\n", sad_value);
    printf("Average absolute difference (for optimized SAD) per pixel: %.2f\n", optimized_sad_value / 256.0);
#endif

    free(block1);
    free(block2);
    return 0;
}

