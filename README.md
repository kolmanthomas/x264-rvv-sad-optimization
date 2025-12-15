# x264 SAD 16x16 optimization sandbox

This repository serves as a sandbox environment to test and profile multiple RISC-V implementations of [x264's](https://code.videolan.org/videolan/x264) `pixel_sad_16x16()`.

So far, it compares the following implementations:

- Scalar, compiled with -O3.
- Scalar source, autovectorized for RISC-V Vector (RVV).
- Vector, using RVV intrinsics.

## Get started

To build the executable, run:

```bash
make all
```

Then run `./out/rvv_sad_sandbox`.

## Simple profiling

To get simple execution time profiling, you can use [GNU gprof](https://ftp.gnu.org/old-gnu/Manuals/gprof-2.9.1/html_mono/gprof.html).

Run the following command to build the sandbox for profiling:

```bash
make profile
```

Then, execute the generated binary `./out/rvv_sad_sandbox`. Optionally, specify the number of times to execute all SAD implementations. For example, to run each implementation 1000 times:

```bash
./out/rvv_sad_sandbox 1 1000
```

(All CLI args to the program are positional, for now.)

Then, run the following gprof command to get basic timings in `prof.txt`:

```bash
gprof -ppixel_sad_16x16_autovec -ppixel_sad_16x16_scalar -ppixel_sad_16x16_rvv_optimized -b ./out/rvv_sad_sandbox > prof.txt
```

## License

See `COPYING`.
