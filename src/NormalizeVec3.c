typedef unsigned short u16;
typedef unsigned int u32;
typedef long long s64;
typedef unsigned long long u64;

void NormalizeVec3(int *v, int *out)
{
    s64 sq;
    volatile u16 *ime;
    u16 saved;
    int len;
    s64 recip;
    s64 scale;
    s64 prod;

    sq = (s64)v[0] * v[0];
    sq += (s64)v[1] * v[1];
    sq += (s64)v[2] * v[2];

    *(volatile u16 *)0x4000280 = 2;
    *(volatile u64 *)0x4000290 = (u64)0x1000000 << 32;
    *(volatile u64 *)0x4000298 = sq;

    ime = (volatile u16 *)0x4000208;
    saved = *ime;
    *ime = 0;
    *(volatile u16 *)0x40002b0 = 1;
    *(volatile u64 *)0x40002b8 = (u64)sq << 2;
    *ime;
    *ime = saved;

    while (*(volatile u16 *)0x40002b0 & 0x8000) {}
    len = *(volatile int *)0x40002b4;
    while (*(volatile u16 *)0x4000280 & 0x8000) {}
    recip = *(volatile s64 *)0x40002a0;

    scale = recip * (s64)len;

    prod = scale * (s64)v[0];
    out[0] = (int)((prod + (1LL << 44)) >> 45);
    prod = scale * (s64)v[1];
    out[1] = (int)((prod + (1LL << 44)) >> 45);
    prod = scale * (s64)v[2];
    out[2] = (int)((prod + (1LL << 44)) >> 45);
}
