struct Vec3 { int x, y, z; };

#define LAUNDER(p) ((long long)(int)(p) & 0xFFFFFFFFFFFFFFFFLL)

extern void *_ZN5Actor10FindWithIDEj(unsigned int id);
extern int Vec3_Dist(const void *a, const void *b);
extern short Vec3_HorzAngle(const void *a, const void *b);
extern void *_ZN5Actor13ClosestPlayerEv(void *self);
extern void func_0201267c(int a, void *b);

void func_ov020_02112e94(char *r4)
{
    int sp[4];
    void *a;
    short d;
    void *p;
    int r3;
    int *av;
    unsigned short *p39e;

    a = _ZN5Actor10FindWithIDEj(*(unsigned int *)(r4 + 0x37c));
    if (a != 0) {
        av = (int *)(int)LAUNDER((char *)a + 0x5c);
        sp[0] = *av;
        sp[1] = av[1];
        sp[2] = av[2];
        if (Vec3_Dist(r4 + 0x5c, sp) >= 0xfa000)
            return;
        d = (short)(Vec3_HorzAngle(r4 + 0x5c, sp) - *(short *)(r4 + 0x8e) + 0x2000);
        if (d & 0x4000) {
            *(char **)(r4 + 0x3a4) = r4 + 0x90;
            if (d > 0)
                *(short *)(r4 + 0x3a2) = 0x4000;
            else
                *(short *)(r4 + 0x3a2) = (short)-0x4000;
        } else {
            *(char **)(r4 + 0x3a4) = r4 + 0x8c;
            if (d < 0)
                *(short *)(r4 + 0x3a2) = 0x5800;
            else
                *(short *)(r4 + 0x3a2) = (short)-0x4000;
        }
        if (*(short *)(r4 + 0x3a2) < 0)
            *(short *)(r4 + 0x398) = (short)0xfa24;
        else
            *(short *)(r4 + 0x398) = 0x5dc;
        *(int *)(r4 + 0x378) = 3;
        return;
    }

    if (*(unsigned short *)(r4 + 0x3a0) != 0) {
        p = _ZN5Actor13ClosestPlayerEv(r4);
        if (p != 0) {
            if (Vec3_Dist(r4 + 0x5c, (char *)p + 0x5c) < 0x1f4000)
                *(unsigned short *)(r4 + 0x3a0) = 0;
        }
        *(unsigned short *)(r4 + 0x39e) = 0;
        return;
    }

    p39e = (unsigned short *)(int)LAUNDER(r4 + 0x39e);
    *p39e = (unsigned short)(*p39e + 1);
    if (*(unsigned short *)(r4 + 0x39e) & 8) {
        if (*(short *)(r4 + 0x8c) >= 0) {
            r3 = -4;
        } else {
            func_0201267c(0x5f, r4 + 0x74);
            r3 = 4;
        }
        {
            int *px = (int *)(int)LAUNDER(r4 + 0x5c);
            *px = *px - (r3 << 12);
        }
        {
            int *pz = (int *)(int)LAUNDER(r4 + 0x64);
            *pz = *pz - (r3 << 12);
        }
        *(short *)(r4 + 0x90) = (short)(r3 * 0x32);
        *(short *)(r4 + 0x8c) = *(short *)(r4 + 0x90);
    } else {
        *(short *)(r4 + 0x90) = 0;
        *(short *)(r4 + 0x8c) = *(short *)(r4 + 0x90);
    }

    if (*(unsigned short *)(r4 + 0x39e) < 0x1e)
        return;
    *(int *)(r4 + 0x378) = 2;
    *(short *)(r4 + 0x398) = 0;
    *(short *)(r4 + 0x39a) = 0;
    *(short *)(r4 + 0x39c) = 0xc8;
    *(short *)(r4 + 0x3a0) = 0x28;
    *(short *)(r4 + 0x39e) = 0;
}
