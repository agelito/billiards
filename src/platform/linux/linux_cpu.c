// linux_cpu.c

static inline uint64
cpu_get_rdtsc()
{
    uint32 rdtsc_low, rdtsc_high;
    __asm__ __volatile__ ("rdtsc" : "=a" (rdtsc_low), "=d" (rdtsc_high));
    return ((uint64)rdtsc_high << 32) | rdtsc_low;
}
