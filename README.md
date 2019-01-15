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
# not in some intel system with Intel Turbo Boost, the max freq may still have turbo effect.
# can set to the second largest freq
```