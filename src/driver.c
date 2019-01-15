#include "cpuid.h"
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <sys/time.h>
double current_sec()
{
    struct timeval time;
    if (gettimeofday(&time,NULL)){
        return 0;
    }
    return (double)time.tv_sec + (double)time.tv_usec * .000001;
}

typedef struct {
    uint64_t    loops;
    double      gflops;
    double      time_ms;
}bench_result_t;

#define AVX2_FMA_FP32_FLOP (10*2*8)
static void avx2_fma_fp32_kernel(uint64_t loop){
    asm volatile(
        "   movq        %0, %%rax               \n"
        "   vxorps      %%ymm0, %%ymm0, %%ymm0  \n"
        "   vxorps      %%ymm1, %%ymm1, %%ymm1  \n"
        "   vxorps      %%ymm2, %%ymm2, %%ymm2  \n"
        "   vxorps      %%ymm3, %%ymm3, %%ymm3  \n"
        "   vxorps      %%ymm4, %%ymm4, %%ymm4  \n"
        "   vxorps      %%ymm5, %%ymm5, %%ymm5  \n"
        "   vxorps      %%ymm6, %%ymm6, %%ymm6  \n"
        "   vxorps      %%ymm7, %%ymm7, %%ymm7  \n"
        "   vxorps      %%ymm8, %%ymm8, %%ymm8  \n"
        "   vxorps      %%ymm9, %%ymm9, %%ymm9  \n"
        "0:                                     \n"
        "   vfmadd231ps %%ymm0, %%ymm0, %%ymm0  \n"
        "   vfmadd231ps %%ymm1, %%ymm1, %%ymm1  \n"
        "   vfmadd231ps %%ymm2, %%ymm2, %%ymm2  \n"
        "   vfmadd231ps %%ymm3, %%ymm3, %%ymm3  \n"
        "   vfmadd231ps %%ymm4, %%ymm4, %%ymm4  \n"
        "   vfmadd231ps %%ymm5, %%ymm5, %%ymm5  \n"
        "   vfmadd231ps %%ymm6, %%ymm6, %%ymm6  \n"
        "   vfmadd231ps %%ymm7, %%ymm7, %%ymm7  \n"
        "   vfmadd231ps %%ymm8, %%ymm8, %%ymm8  \n"
        "   vfmadd231ps %%ymm9, %%ymm9, %%ymm9  \n"
        "   subq        $0x1,   %%rax           \n"
        "   jne     0b                          \n"
        :
        :"r"(loop)
        :   "ymm0","ymm1","ymm2","ymm3","ymm4",
            "ymm5","ymm6","ymm7","ymm8","ymm9" );
}

#define AVX2_FMA_FP64_FLOP (10*2*4)
static void avx2_fma_fp64_kernel(uint64_t loop){
    asm volatile(
        "   movq        %0, %%rax               \n"
        "   vxorpd      %%ymm0, %%ymm0, %%ymm0  \n"
        "   vxorpd      %%ymm1, %%ymm1, %%ymm1  \n"
        "   vxorpd      %%ymm2, %%ymm2, %%ymm2  \n"
        "   vxorpd      %%ymm3, %%ymm3, %%ymm3  \n"
        "   vxorpd      %%ymm4, %%ymm4, %%ymm4  \n"
        "   vxorpd      %%ymm5, %%ymm5, %%ymm5  \n"
        "   vxorpd      %%ymm6, %%ymm6, %%ymm6  \n"
        "   vxorpd      %%ymm7, %%ymm7, %%ymm7  \n"
        "   vxorpd      %%ymm8, %%ymm8, %%ymm8  \n"
        "   vxorpd      %%ymm9, %%ymm9, %%ymm9  \n"
        "0:                                     \n"
        "   vfmadd231pd %%ymm0, %%ymm0, %%ymm0  \n"
        "   vfmadd231pd %%ymm1, %%ymm1, %%ymm1  \n"
        "   vfmadd231pd %%ymm2, %%ymm2, %%ymm2  \n"
        "   vfmadd231pd %%ymm3, %%ymm3, %%ymm3  \n"
        "   vfmadd231pd %%ymm4, %%ymm4, %%ymm4  \n"
        "   vfmadd231pd %%ymm5, %%ymm5, %%ymm5  \n"
        "   vfmadd231pd %%ymm6, %%ymm6, %%ymm6  \n"
        "   vfmadd231pd %%ymm7, %%ymm7, %%ymm7  \n"
        "   vfmadd231pd %%ymm8, %%ymm8, %%ymm8  \n"
        "   vfmadd231pd %%ymm9, %%ymm9, %%ymm9  \n"
        "   subq        $0x1,   %%rax           \n"
        "   jne     0b                          \n"
        :
        :"r"(loop)
        :   "ymm0","ymm1","ymm2","ymm3","ymm4",
            "ymm5","ymm6","ymm7","ymm8","ymm9" );
}

#define AVX512_FMA_FP32_FLOP (10*2*16)
static void avx512_fma_fp32_kernel(uint64_t loop){
    asm volatile(
        "   movq        %0, %%rax               \n"
        "   vxorps      %%zmm0, %%zmm0, %%zmm0  \n"
        "   vxorps      %%zmm1, %%zmm1, %%zmm1  \n"
        "   vxorps      %%zmm2, %%zmm2, %%zmm2  \n"
        "   vxorps      %%zmm3, %%zmm3, %%zmm3  \n"
        "   vxorps      %%zmm4, %%zmm4, %%zmm4  \n"
        "   vxorps      %%zmm5, %%zmm5, %%zmm5  \n"
        "   vxorps      %%zmm6, %%zmm6, %%zmm6  \n"
        "   vxorps      %%zmm7, %%zmm7, %%zmm7  \n"
        "   vxorps      %%zmm8, %%zmm8, %%zmm8  \n"
        "   vxorps      %%zmm9, %%zmm9, %%zmm9  \n"
        "0:                                     \n"
        "   vfmadd231ps %%zmm0, %%zmm0, %%zmm0  \n"
        "   vfmadd231ps %%zmm1, %%zmm1, %%zmm1  \n"
        "   vfmadd231ps %%zmm2, %%zmm2, %%zmm2  \n"
        "   vfmadd231ps %%zmm3, %%zmm3, %%zmm3  \n"
        "   vfmadd231ps %%zmm4, %%zmm4, %%zmm4  \n"
        "   vfmadd231ps %%zmm5, %%zmm5, %%zmm5  \n"
        "   vfmadd231ps %%zmm6, %%zmm6, %%zmm6  \n"
        "   vfmadd231ps %%zmm7, %%zmm7, %%zmm7  \n"
        "   vfmadd231ps %%zmm8, %%zmm8, %%zmm8  \n"
        "   vfmadd231ps %%zmm9, %%zmm9, %%zmm9  \n"
        "   subq        $0x1,   %%rax           \n"
        "   jne     0b                          \n"
        :
        :"r"(loop)
        :   "zmm0","zmm1","zmm2","zmm3","zmm4",
            "zmm5","zmm6","zmm7","zmm8","zmm9" );
}

#define AVX512_FMA_FP64_FLOP (10*2*8)
static void avx512_fma_fp64_kernel(uint64_t loop){
    asm volatile(
        "   movq        %0, %%rax               \n"
        "   vxorpd      %%zmm0, %%zmm0, %%zmm0  \n"
        "   vxorpd      %%zmm1, %%zmm1, %%zmm1  \n"
        "   vxorpd      %%zmm2, %%zmm2, %%zmm2  \n"
        "   vxorpd      %%zmm3, %%zmm3, %%zmm3  \n"
        "   vxorpd      %%zmm4, %%zmm4, %%zmm4  \n"
        "   vxorpd      %%zmm5, %%zmm5, %%zmm5  \n"
        "   vxorpd      %%zmm6, %%zmm6, %%zmm6  \n"
        "   vxorpd      %%zmm7, %%zmm7, %%zmm7  \n"
        "   vxorpd      %%zmm8, %%zmm8, %%zmm8  \n"
        "   vxorpd      %%zmm9, %%zmm9, %%zmm9  \n"
        "0:                                     \n"
        "   vfmadd231pd %%zmm0, %%zmm0, %%zmm0  \n"
        "   vfmadd231pd %%zmm1, %%zmm1, %%zmm1  \n"
        "   vfmadd231pd %%zmm2, %%zmm2, %%zmm2  \n"
        "   vfmadd231pd %%zmm3, %%zmm3, %%zmm3  \n"
        "   vfmadd231pd %%zmm4, %%zmm4, %%zmm4  \n"
        "   vfmadd231pd %%zmm5, %%zmm5, %%zmm5  \n"
        "   vfmadd231pd %%zmm6, %%zmm6, %%zmm6  \n"
        "   vfmadd231pd %%zmm7, %%zmm7, %%zmm7  \n"
        "   vfmadd231pd %%zmm8, %%zmm8, %%zmm8  \n"
        "   vfmadd231pd %%zmm9, %%zmm9, %%zmm9  \n"
        "   subq        $0x1,   %%rax           \n"
        "   jne     0b                          \n"
        :
        :"r"(loop)
        :   "zmm0","zmm1","zmm2","zmm3","zmm4",
            "zmm5","zmm6","zmm7","zmm8","zmm9" );
}

bench_result_t bench_fma(uint64_t loop,
    void(*kernel)(uint64_t),
    uint64_t flop_per_loop)
{
    bench_result_t br;
    kernel(10);     //warmup
    double st = current_sec();
    kernel(loop);
    double cost = current_sec() - st;
    br.loops = loop;
    br.gflops = flop_per_loop*loop * 1e-9 / cost ;
    //br.time_ms = cost / loop *1e-3;
    return br;
}


#define LOOP 0xc0000000
int main(){
    char cpu_vendor_str[13];
    cpuid_vendor_str(cpu_vendor_str);
    printf("%s\n", cpu_vendor_str);

    printf("CPU Feature: avx:%d avx2:%d", cpuid_support_avx(), cpuid_support_avx2());
    int avx512f = cpuid_support_avx512_f();
    printf(" avx512f:%d ", avx512f);
    if(avx512f){
        printf("avx512pf:%d avx512er:%d avx512cd:%d",
            cpuid_support_avx512_pf(),
            cpuid_support_avx512_er(),
            cpuid_support_avx512_cd());
    }
    printf("\n");

    if(cpuid_support_avx2()){
        bench_result_t r_avx2_fma;
        r_avx2_fma = bench_fma(LOOP, avx2_fma_fp32_kernel, AVX2_FMA_FP32_FLOP);
        printf("avx2 fma fp32 gflops:%.2f\n", r_avx2_fma.gflops);

        r_avx2_fma = bench_fma(LOOP, avx2_fma_fp64_kernel, AVX2_FMA_FP64_FLOP);
        printf("avx2 fma fp64 gflops:%.2f\n", r_avx2_fma.gflops);
    }

    if(cpuid_support_avx512_f()){
        bench_result_t r_avx512_fma;
        r_avx512_fma = bench_fma(LOOP, avx512_fma_fp32_kernel, AVX512_FMA_FP32_FLOP);
        printf("avx512 fma fp32 gflops:%.2f\n", r_avx512_fma.gflops);

        r_avx512_fma = bench_fma(LOOP, avx512_fma_fp64_kernel, AVX512_FMA_FP64_FLOP);
        printf("avx512 fma fp64 gflops:%.2f\n", r_avx512_fma.gflops);
    }
}
