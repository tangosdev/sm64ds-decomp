typedef unsigned int u32;
typedef int Fix12i;
typedef short s16;

extern u32 _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    u32 slot, u32 unk, Fix12i x, Fix12i y, Fix12i z, void *rot, void *callback);

extern int data_ov084_02130e24[];
extern s16 data_02082214[];
extern int data_ov084_0213029c[];
extern int data_ov084_021302c4[];

void func_ov084_0212d42c(char *self)
{
    int space[3];
    int idx, fac, spd, s, m, x, y, z, zero, bias;
    long long prod;
    volatile int *ytbl;
    volatile int *xtbl;
    s16 *st;

    if (*(int *)(self + 0x170) != data_ov084_02130e24[1])
        return;
    idx = (int)((unsigned)(*(int *)(self + 0x168) << 4) >> 16);
    if (idx >= 0xa)
        return;

    x = *(int *)(self + 0x5c);
    st = data_02082214;
    *(volatile int *)&space[0] = x;
    y = *(int *)(self + 0x60);
    xtbl = data_ov084_0213029c;
    *(volatile int *)&space[1] = y;
    z = *(int *)(self + 0x64);
    bias = 0x800;
    *(volatile int *)&space[2] = z;

    {
        unsigned short ang = *(unsigned short *)(self + 0x8e);
        fac = xtbl[idx];
        spd = *(int *)(self + 0x204);
        s = st[(ang >> 4) << 1];
        ytbl = data_ov084_021302c4;
        zero = 0;
        m = fac * s;
        prod = (long long)m * spd + bias;
        x = x + (int)(prod >> 12);
        *(volatile int *)&space[0] = x;
    }
    {
        unsigned short ang = *(unsigned short *)(self + 0x8e);
        s = st[((ang >> 4) << 1) + 1];
        spd = *(int *)(self + 0x204);
        m = fac * s;
        prod = (long long)m * spd + bias;
        z = z + (int)(prod >> 12);
        *(volatile int *)&space[2] = z;
    }
    y = y + *(int *)(self + 0x204) * ytbl[idx];
    *(volatile int *)&space[1] = y;
    *(u32 *)(self + 0x224) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        *(u32 *)(self + 0x224), 0xfb, space[0], space[1], z, (void *)zero, (void *)zero);
}
