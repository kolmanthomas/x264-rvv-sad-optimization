#pragma once

#include <stdint.h>
#include <stdlib.h>

typedef uint8_t pixel;

extern inline int pixel_sad_16x16_scalar( pixel *pix1, intptr_t i_stride_pix1,
                                   pixel *pix2, intptr_t i_stride_pix2 )
{
    int i_sum = 0;
    for( int y = 0; y < 16; y++ )
    {
        for( int x = 0; x < 16; x++ )
        {
            i_sum += abs( pix1[x] - pix2[x] );
        }
        pix1 += i_stride_pix1;
        pix2 += i_stride_pix2;
    }
    return i_sum;
}
