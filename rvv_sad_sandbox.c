#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

#include <asm/hwprobe.h>
#include <asm/unistd.h>

#include "sad_scalar.h"
#include "sad_rvv.h"
#include "sad_autovec.h"

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

void probe_riscv()
{
	struct riscv_hwprobe requests[] = {{RISCV_HWPROBE_KEY_MVENDORID},
					   {RISCV_HWPROBE_KEY_MARCHID},
					   {RISCV_HWPROBE_KEY_MIMPID},
					   {RISCV_HWPROBE_KEY_CPUPERF_0},
					   {RISCV_HWPROBE_KEY_IMA_EXT_0}};

	int ret = syscall(__NR_riscv_hwprobe, &requests,
			  sizeof(requests) / sizeof(struct riscv_hwprobe), 0,
			  NULL, 0);
	if (ret) {
		fprintf(stderr, "Syscall failed with %d: %s\n", ret,
 			strerror(errno));
	}

	bool has_misaligned_fast =
	    (requests[3].value & RISCV_HWPROBE_MISALIGNED_FAST) ==
	    RISCV_HWPROBE_MISALIGNED_FAST;
	printf("Vendor ID: %llx\n", requests[0].value);
	printf("MARCH ID: %llx\n", requests[1].value);
	printf("MIMPL ID: %llx\n", requests[2].value);
	printf("HasMisalignedFast: %s\n", has_misaligned_fast ? "yes" : "no");
	__u64 extensions = requests[4].value;
	printf("Extensions:\n");
	if (extensions & RISCV_HWPROBE_IMA_FD)
		printf("\tFD\n");
	if (extensions & RISCV_HWPROBE_IMA_C)
		printf("\tC\n");
	if (extensions & RISCV_HWPROBE_IMA_V)
		printf("\tV\n");
	if (extensions & RISCV_HWPROBE_EXT_ZBA)
		printf("\tZBA\n");
	if (extensions & RISCV_HWPROBE_EXT_ZBB)
		printf("\tZBB\n");
	if (extensions & RISCV_HWPROBE_EXT_ZBS)
		printf("\tZBS\n");

    #ifdef __riscv_vector
        printf("RISC-V Vector extension supported (version %ld)\n", __riscv_vector);
        #if __riscv_vector >= 1000000
            printf("Vector 1.0 or later supported\n");
        #endif
    #else
        printf("RISC-V Vector extension not supported\n");
    #endif
}

int main(int argc, char *argv[])
{
    probe_riscv();

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

    // Optionally receive number of times to repeat from args.
    long n = 1;
    if (argc > 2)
    {
        n = strtol(argv[2], NULL, 10);
    }

    // Display RVV and intrinsics support.
#if defined(__riscv_v)
    printf("RVV v%u.%u.%u\n", __riscv_v / 1000000, __riscv_v % 1000000 / 1000, __riscv_v % 1000);
#else
    printf("RVV not supported\n");
#endif

#if defined(__riscv_v_intrinsic)
    printf("RVV intrinsics v%u.%u.%u\n", __riscv_v_intrinsic / 1000000, __riscv_v_intrinsic % 1000000 / 1000, __riscv_v_intrinsic % 1000);
#endif

    const int height = 16;
    const int stride1 = 16;
    const int stride2 = 32;

    uint8_t *block1 = (uint8_t*)malloc(stride1 * height * sizeof(uint8_t));
    uint8_t *block2 = (uint8_t*)malloc(stride2 * height * sizeof(uint8_t));

    for (int i = 0; i < n; i++)
    {
        randomize_block_16x16(block1, stride1);
        randomize_block_16x16(block2, stride2);
        
        int sad_value = pixel_sad_16x16_scalar(block1, stride1, block2, stride2);

    #if defined(__riscv) && defined(__riscv_v_intrinsic)
        int optimized_sad_value = pixel_sad_16x16_rvv_optimized(block1, stride1, block2, stride2);
        int autovec_sad_result = pixel_sad_16x16_autovec(block1, stride1, block2, stride2);

        if (sad_value != optimized_sad_value || sad_value != autovec_sad_result)
        {
            printf("Different SAD results: scalar = %d, rvv = %d, autovec = %d\n", sad_value, optimized_sad_value, autovec_sad_result);
            break;
        }
    #endif
    }

    free(block1);
    free(block2);
    return 0;
}

