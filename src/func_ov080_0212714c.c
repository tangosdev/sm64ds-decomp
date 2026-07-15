typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;

extern int _ZN8Platform20UpdateKillByMegaCharEsss5Fix12IiE(void *self, s16 a, s16 b, s16 c, int d);
extern int DecIfAbove0_Byte(u8 *p);
extern int Vec3_Dist(void *a, void *b);
extern short Vec3_HorzAngle(void *a, void *b);
extern int AngleDiff(int a, int b);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(void *a, void *b);
extern void Vec3_Sub(void *out, void *a, void *b);
extern int LenVec3(void *v);
extern void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int, void *);
extern u16 DecIfAbove0_Short(u16 *p);
extern void func_ov080_02127094(char *t);
extern int _ZN8Platform21IsClsnInRangeOnScreenE5Fix12IiES1_(void *, int, int);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(void *);
extern int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(void *, int, int);

int func_ov080_0212714c(char *c, int *p2)
{
    int tmp[3];
    int len;
    int ang;
    s16 v;
    int horz;

    if (_ZN8Platform20UpdateKillByMegaCharEsss5Fix12IiE(c, (s16)-0x1500, 0, 0, 0) != 0)
        return 1;
    if ((int)((*(int *)(c + 0xb0) & 8) != 0) != 0)
        return 1;

    if (*(u8 *)(c + 0x342) == 0) {
        if (DecIfAbove0_Byte((u8 *)((int)c + 0x33e))) {
            if (*(s16 *)((c + 0x300) + 0x3c) > 0) {
                s16 *p = (s16 *)(int)(((long long)(int)(c + 0x33c)) & 0xFFFFFFFFFFFFFFFFLL);
                *p = (s16)(*p - 0x10);
            } else {
                s16 *p = (s16 *)(int)(((long long)(int)(c + 0x33c)) & 0xFFFFFFFFFFFFFFFFLL);
                *p = (s16)(*p + 0x10);
            }
            v = *(s16 *)((c + 0x300) + 0x3c);
            if (v <= 0x10 && v >= (s16)-0x10)
                *(s16 *)((c + 0x300) + 0x3c) = 0;
        } else {
            v = *(s16 *)((c + 0x300) + 0x3c);
            if (v != 0x100) {
                if (v > 0x100) {
                    s16 *p = (s16 *)(int)(((long long)(int)(c + 0x33c)) & 0xFFFFFFFFFFFFFFFFLL);
                    *p = (s16)(*p - 0x10);
                } else {
                    s16 *p = (s16 *)(int)(((long long)(int)(c + 0x33c)) & 0xFFFFFFFFFFFFFFFFLL);
                    *p = (s16)(*p + 0x10);
                }
                v = *(s16 *)((c + 0x300) + 0x3c);
                if (v < 0x110 && v > 0xf0)
                    *(s16 *)((c + 0x300) + 0x3c) = 0x100;
            }
        }
        if (Vec3_Dist(c + 0x5c, c + 0x320) < 0x2000)
            *(s16 *)((c + 0x300) + 0x3c) = 0;
    } else {
        horz = Vec3_HorzAngle(c + 0x5c, c + 0x32c);
        AngleDiff(horz, *(s16 *)(c + 0x8e));
        ang = AngleDiff(horz, *(s16 *)(c + 0x8e));
        if (ang > 0x4000) {
            s16 *p = (s16 *)(int)(((long long)(int)(c + 0x33c)) & 0xFFFFFFFFFFFFFFFFLL);
            *p = (s16)(*p - 0x10);
        } else {
            s16 *p = (s16 *)(int)(((long long)(int)(c + 0x33c)) & 0xFFFFFFFFFFFFFFFFLL);
            *p = (s16)(*p + 0x10);
        }
        v = *(s16 *)((c + 0x300) + 0x3c);
        {
            int lim = 0x200;
            int nlim = -lim;
            if (v < nlim)
                v = (s16)nlim;
            else if (v > lim)
                v = (s16)lim;
        }
        *(s16 *)((c + 0x300) + 0x3c) = v;
    }

    {
        int d = *(s16 *)((c + 0x300) + 0x3c);
        int t = d + ((unsigned)(d >> 5) >> 26);
        *(int *)(c + 0x98) = (t >> 6) << 12;
    }
    _ZN5Actor9UpdatePosEP12CylinderClsn(c, 0);
    Vec3_Sub(tmp, c + 0x5c, c + 0x320);
    len = LenVec3(tmp);
    ang = Vec3_HorzAngle(c + 0x5c, c + 0x320);
    ang = AngleDiff(ang, *(s16 *)(c + 0x8e));
    if (ang >= 0x7f00)
        ang = 0x8000;
    if (ang <= 0x100)
        ang = 0;
    if (len > *p2) {
        *(int *)(c + 0x5c) = *(int *)(c + 0x68);
        *(int *)(c + 0x60) = *(int *)(c + 0x6c);
        *(int *)(c + 0x64) = *(int *)(c + 0x70);
    }
    if (ang == *(int *)(c + 0x338) || ang == 0x8000) {
        *(int *)(c + 0x5c) = *(int *)(c + 0x68);
        *(int *)(c + 0x60) = *(int *)(c + 0x6c);
        *(int *)(c + 0x64) = *(int *)(c + 0x70);
        *(u8 *)(c + 0x33e) = 0xa;
    }
    {
        s16 *px = (s16 *)(int)(((long long)(int)(c + 0x8c)) & 0xFFFFFFFFFFFFFFFFLL);
        *px = (s16)(*px + *(s16 *)((c + 0x300) + 0x3c));
    }
    {
        int mask = 0x1fff;
        if ((*(s16 *)(c + 0x8c) & mask) < 0x210 && *(s16 *)((c + 0x300) + 0x3c) != 0) {
            if (*(u16 *)((c + 0x300) + 0x40) == 0) {
                _ZN5Sound9PlayBank3EjRK7Vector3(0x25, c + 0x74);
                *(u16 *)((c + 0x300) + 0x40) = 0xe;
            }
        } else {
            DecIfAbove0_Short((u16 *)(c + 0x340));
        }
    }
    func_ov080_02127094(c);
    {
        int is66 = (int)(*(u16 *)(c + 0xc) == 0x66);
        if (is66 != 0) {
            if (_ZN8Platform21IsClsnInRangeOnScreenE5Fix12IiES1_(c, 0, 0) != 0)
                _ZN8Platform19UpdateClsnPosAndRotEv(c);
        } else {
            if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(c, 0, 0) != 0)
                _ZN8Platform19UpdateClsnPosAndRotEv(c);
        }
    }
    *(u8 *)(c + 0x342) = 0;
    return 1;
}
