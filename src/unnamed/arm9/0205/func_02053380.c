typedef short s16;
typedef long long s64;
typedef unsigned long long u64;

void func_02053380(s16 *in, s16 *out)
{
    u64 sq;
    volatile unsigned short *ime;
    unsigned short saved;
    int sqrtResult;
    s64 divResult;
    s64 scale;
    s64 prod;

    sq = (long long)(in[0] * in[0]);
    sq += (long long)(in[1] * in[1]);
    sq += (long long)(in[2] * in[2]);

    *(volatile unsigned short *)0x4000280 = 2;
    *(volatile u64 *)0x4000290 = (u64)0x1000000 << 32;
    *(volatile u64 *)0x4000298 = sq;

    ime = (volatile unsigned short *)0x4000208;
    saved = *ime;
    *ime = 0;
    *(volatile unsigned short *)0x40002b0 = 1;
    *(volatile u64 *)0x40002b8 = sq << 2;
    *ime;
    *ime = saved;

    while (*(volatile unsigned short *)0x40002b0 & 0x8000) {}
    sqrtResult = *(volatile int *)0x40002b4;
    while (*(volatile unsigned short *)0x4000280 & 0x8000) {}
    divResult = *(volatile s64 *)0x40002a0;

    scale = divResult * (s64)sqrtResult;

    prod = scale * (s64)in[0];
    out[0] = (s16)((prod + (1LL << 44)) >> 45);
    prod = scale * (s64)in[1];
    out[1] = (s16)((prod + (1LL << 44)) >> 45);
    prod = scale * (s64)in[2];
    out[2] = (s16)((prod + (1LL << 44)) >> 45);
}
