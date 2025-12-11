#pragma once

#include <stdint.h>
#include <stdlib.h>

int pixel_sad_16x16_scalar( uint8_t *pix1, intptr_t i_stride_pix1,
                                          uint8_t *pix2, intptr_t i_stride_pix2 );
