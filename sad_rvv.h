#pragma once

#if defined(__riscv)

#include <riscv_vector.h>

int pixel_sad_16x16_rvv_optimized(uint8_t *pix1, intptr_t stride1,
                                                uint8_t *pix2, intptr_t stride2)

#endif // defined(__riscv)



