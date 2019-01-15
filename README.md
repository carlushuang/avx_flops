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
> Knights Landing (Xeon Phi x200):[1][9] AVX-512 F, CD, ER, PF
> Knights Mill (Xeon Phi x205):[7] AVX-512 F, CD, ER, PF, 4FMAPS, 4VNNIW, VPOPCNTDQ
> Skylake-SP, Skylake-X:[10][11][12] AVX-512 F, CD, VL, DQ, BW
> Cannon Lake:[7] AVX-512 F, CD, VL, DQ, BW, IFMA, VBMI
> Ice Lake:[7] AVX-512 F, CD, VL, DQ, BW, IFMA, VBMI, VBMI2, VPOPCNTDQ, BITALG, VNNI, VPCLMULQDQ, GFNI, VAES
