project(avx_flops)
cmake_minimum_required( VERSION 2.8 )

set(src src/cpuid.c src/driver.c)

set(CMAKE_C_FLAGS " -m64 -O3 ${CMAKE_C_FLAGS}")
add_executable(avx_flops ${src} )