#pragma once

#if defined(__riscv)

#include <riscv_vector.h>

extern inline int pixel_sad_16x16_rvv_optimized(uint8_t *pix1, intptr_t stride1,
                                                uint8_t *pix2, intptr_t stride2)
{
    // Fixed vector length for 16 bytes
    size_t vl = __riscv_vsetvl_e8m1(16);

    // 32-bit accumulator vector initialized to 0
    size_t vl_acc = __riscv_vsetvlmax_e32m4();
    vuint32m4_t v_acc = __riscv_vmv_v_x_u32m4(0, vl_acc);

    // Process 2 rows per iteration: 16 rows / 2 = 8 iterations
    for (int y = 0; y < 8; y++) {
        // --- Row A ---
        vuint8m1_t v_p1_a = __riscv_vle8_v_u8m1(pix1, vl);
        vuint8m1_t v_p2_a = __riscv_vle8_v_u8m1(pix2, vl);

        vuint8m1_t v_max_a = __riscv_vmaxu_vv_u8m1(v_p1_a, v_p2_a, vl);
        vuint8m1_t v_min_a = __riscv_vminu_vv_u8m1(v_p1_a, v_p2_a, vl);
        vuint8m1_t v_diff_a = __riscv_vsub_vv_u8m1(v_max_a, v_min_a, vl);

        // --- Row B (next row) ---
        vuint8m1_t v_p1_b = __riscv_vle8_v_u8m1(pix1 + stride1, vl);
        vuint8m1_t v_p2_b = __riscv_vle8_v_u8m1(pix2 + stride2, vl);

        vuint8m1_t v_max_b = __riscv_vmaxu_vv_u8m1(v_p1_b, v_p2_b, vl);
        vuint8m1_t v_min_b = __riscv_vminu_vv_u8m1(v_p1_b, v_p2_b, vl);
        vuint8m1_t v_diff_b = __riscv_vsub_vv_u8m1(v_max_b, v_min_b, vl);
        //
        // Widen A and B to 16-bit
        vuint16m2_t v_diff_16_a = __riscv_vwcvtu_x_x_v_u16m2(v_diff_a, vl);
        vuint16m2_t v_diff_16_b = __riscv_vwcvtu_x_x_v_u16m2(v_diff_b, vl);

        // Accumulate both rows into 32-bit accumulator
        v_acc = __riscv_vwaddu_wv_u32m4(v_acc, v_diff_16_a, vl);
        v_acc = __riscv_vwaddu_wv_u32m4(v_acc, v_diff_16_b, vl);

        // Advance by 2 rows
        pix1 += 2 * stride1;
        pix2 += 2 * stride2;
    }

    // Final horizontal reduction
    vuint32m1_t v_sum = __riscv_vmv_v_x_u32m1(0, vl_acc);
    v_sum = __riscv_vredsum_vs_u32m4_u32m1(v_acc, v_sum, vl_acc);
    return (int)__riscv_vmv_x_s_u32m1_u32(v_sum);
}

#endif // defined(__riscv)



