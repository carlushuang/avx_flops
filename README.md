# avx_flops
Benchmark cpu flops using avx instructions, same idea as https://github.com/pigirons/cpufp

# build
```
sh rebuild.sh
```

# bench
it's better force cpu frequency to a steak value, not let DVFS/DFS change freq on the run.

maybe set the userspace governor:
```
echo userspace > /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor
echo [freq] > /sys/devices/system/cpu/cpu0/cpufreq/scaling_setspeed
# note in some intel system with Intel Turbo Boost, the max freq may still have turbo effect.
# hence may need set to the second largest freq
```

# port & lantency
vfmadd(vfmadd132ss/sd, vfmadd231ss/sd, ...)

   arch       |  avx256 | avx512 
 -------------|---------|---------
  haswell     |  p01, 5 |   n/a
  broadwell   |  p01, 5 |   n/a
  skylake     |  p01, 4 |   n/a
  skylakeX    |  p01, 4 | p05, 4
  zen         |  p01, 5 |   n/a

* above data is from [agner's](https://www.agner.org/optimize/) instruction_tables.pdf. 
* "p01, 5" means can issue both port 0 and port 1, lantency is 5 clock cycle

hence have 10 vfmadd instruction at inner loop is optimal and can cover above arch

cpu with avx512:
* Knights Landing (Xeon Phi x200):[1][9] AVX-512 F, CD, ER, PF
* Knights Mill (Xeon Phi x205):[7] AVX-512 F, CD, ER, PF, 4FMAPS, 4VNNIW, VPOPCNTDQ
* Skylake-SP, Skylake-X:[10][11][12] AVX-512 F, CD, VL, DQ, BW
* Cannon Lake:[7] AVX-512 F, CD, VL, DQ, BW, IFMA, VBMI
* Ice Lake:[7] AVX-512 F, CD, VL, DQ, BW, IFMA, VBMI, VBMI2, VPOPCNTDQ, BITALG, VNNI, VPCLMULQDQ, GFNI, VAES

some test info
```
# Intel(R) Xeon(R) Gold 6142 CPU @ 2.60GHz (no freq turbo, fix at 2.6G)
# theoritical peak is 2(port)*8(8xfp32 for 256bit)*2(add+mul)*2.6GHz = 83.2 gflops
GenuineIntel
CPU Feature: avx:1 avx2:1 avx512f:1 avx512pf:0 avx512er:0 avx512cd:1
avx256 fma fp32, 83.15 gflops
avx256 fma fp64, 41.58 gflops
avx512 fma fp32, 166.31 gflops
avx512 fma fp64, 83.16 gflops

# AMD Ryzen 7 2700X Eight-Core Processor, strange?
AuthenticAMD
CPU Feature: avx:1 avx2:1 avx512f:0 
avx256 fma fp32, 68.63 gflops
avx256 fma fp64, 34.31 gflops

```