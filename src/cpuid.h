#ifndef __CPUID_H
#define __CPUID_H

#ifdef __x86_64
#define __cpuid(eax,ebx,ecx,edx)    \
    asm volatile(                   \
        "  xchgq  %%rbx,%q1\n"      \
        "  cpuid\n"                 \
        "  xchgq  %%rbx,%q1"        \
        : "=a"(eax), "=r" (ebx), "=c"(ecx), "=d"(edx) \
        : "a"(eax), "c"(ecx))
#else
// TODO 32bit calling convention
#endif


void cpuid_vendor_str(char * vendor_str);

/* F, CD, ER, PF
* Introduced with Xeon Phi x200 (Knights Landing) and Xeon E5-26xx V5 (Skylake EP/EX "Purley", expected in H2 2017), 
* with the last two (ER and PF) being specific to Knights Landing.
*
*/
int cpuid_support_avx();
int cpuid_support_avx2();
int cpuid_support_avx512_f();
int cpuid_support_avx512_pf();
int cpuid_support_avx512_er();
int cpuid_support_avx512_cd();

#endif