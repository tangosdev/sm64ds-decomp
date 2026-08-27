/* matched pure C — was HAND-ASM regperm floor; y-early preload cracks r4/r5 */
extern short data_02082214[];
typedef struct { int x, y, z; } Vec3;
void func_ov002_020bf13c(char* c)
{
    volatile int pad[4];
    int vel; int* dst; short s, cs; int ang; int r6, flags;
    Vec3 v;
    (void)&pad;
    if (*(int*)(c + 8) != 3) return;
    dst = *(int**)(c + 0x578);
    if (dst == 0) return;
    vel = *(int*)(c + 0x98);
    ang = *(short*)(c + 0x8e);
    {
        int idx = (unsigned short)(short)(ang + 0x8000) >> 4;
        s = data_02082214[idx * 2];
        cs = data_02082214[idx * 2 + 1];
    }
    v.x = *(int*)(c + 0x5c);
    v.y = *(int*)(c + 0x60);
    v.z = *(int*)(c + 0x64);
    v.x = (short)s * (short)0x78 + v.x;
    v.z = (short)cs * (short)0x78 + v.z;
    r6 = vel >> 5;
    if (r6 > 0x1000) r6 = 0x1000;
    if (r6 < 0) r6 = 0;
    {
        unsigned char f6de = *(unsigned char*)(c + 0x6de);
        int ip = (r6 + 0x1000) >> 8;
        flags = ip & 0xff;
        if (f6de != 0)
            flags = (flags | 0x80) & 0xff;
        if (vel != 0)
            flags = (flags | 0x40) & 0xff;
        dst[0] = v.x;
        dst[1] = v.y + 0xc000;
        dst[2] = v.z;
        *(unsigned char*)(*(char**)(c + 0x57c)) = (unsigned char)flags;
    }
}
