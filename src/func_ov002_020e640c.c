typedef int s32;
typedef short s16;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef struct { s32 x, y, z; } Vector3;

extern int _ZNK6Player14GetBodyModelIDEjb(void *p, u32 a, int b);
extern int _Z15ApproachLinear2Rsss(s16 *p, s16 t, s16 step);
extern void Vec3_RotateYAndTranslate(Vector3 *dst, const Vector3 *src, s16 ang, const Vector3 *t);

void func_ov002_020e640c(char *c)
{
    char *md;
    char *mp;
    char *r4p;
    int id;
    Vector3 d;

    id = _ZNK6Player14GetBodyModelIDEjb(c, *(u32 *)(c + 8) & 0xff, 0);
    {
        int *p = (int *)(int)(((long long)(int)(*(s32 *)(c + 0xdc + id * 4) + 8)) & 0xFFFFFFFFFFFFFFFFLL);
        md = (char *)p[2];
        mp = (char *)p[0];
    }
    r4p = *(char **)(mp + 8);

    *(u16 *)(int)(((long long)(int)(md + 0x1bc)) & 0xFFFFFFFFFFFFFFFFLL) += *(s16 *)(c + 0x75e);
    *(u16 *)(int)(((long long)(unsigned int)(md + 0x1be)) & 0xFFFFFFFFFFFFFFFFLL) += *(s16 *)(c + 0x760);

    {
        int b = *(u8 *)(c + 0x742);
        if (b == 0) {
        } else if (b == 1) {
            *(u16 *)(int)(((long long)(int)(md + 0x326)) & 0xFFFFFFFFFFFFFFFFLL) += *(u16 *)(c + 0x762);
            *(u16 *)(int)(((long long)(unsigned int)(md + 0x328)) & 0xFFFFFFFFFFFFFFFFLL) += *(u16 *)(c + 0x764);
            *(u16 *)(int)(((long long)(int)(md + 0x32a)) & 0xFFFFFFFFFFFFFFFFLL) += *(u16 *)(c + 0x766);
        } else if (b == 2) {
            *(s16 *)(md + 0x326) = *(s16 *)(c + 0x762);
            *(s16 *)(md + 0x328) = *(s16 *)(c + 0x764);
            *(s16 *)(md + 0x32a) = *(s16 *)(c + 0x766);
        } else if (b == 3) {
            *(s16 *)(md + 0x32a) = (s16)(*(s16 *)(c + 0x762) + 0x4000);
            *(s16 *)(md + 0x328) = *(s16 *)(c + 0x764);
            *(s16 *)(md + 0x326) = *(s16 *)(c + 0x766);
            *(s16 *)(md + 0x1be) = *(s16 *)(c + 0x75c);
            *(s16 *)(md + 0x1ba) = *(s16 *)(c + 0x75e);
            *(s16 *)(md + 0x1bc) = *(s16 *)(c + 0x760);
        } else if (b == 4) {
            _Z15ApproachLinear2Rsss((s16 *)(c + 0x762), (s16)(*(u16 *)(md + 0x32a) - 0x4000), 0x400);
            _Z15ApproachLinear2Rsss((s16 *)(c + 0x764), *(s16 *)(md + 0x328), 0x400);
            _Z15ApproachLinear2Rsss((s16 *)(c + 0x766), *(s16 *)(md + 0x326), 0x400);
            *(s16 *)(md + 0x32a) = (s16)(*(s16 *)(c + 0x762) + 0x4000);
            *(s16 *)(md + 0x328) = *(s16 *)(c + 0x764);
            *(s16 *)(md + 0x326) = *(s16 *)(c + 0x766);
        }
    }

    *(u16 *)(c + 0x580) = *(u16 *)(md + 0x326);
    *(u16 *)(c + 0x582) = *(u16 *)(md + 0x328);
    *(u16 *)(c + 0x584) = *(u16 *)(md + 0x32a);

    *(s32 *)(md + 0x20) = *(s32 *)(r4p + 0x24);
    *(s32 *)(md + 0x24) = *(s32 *)(r4p + 0x28);
    *(s32 *)(md + 0x28) = *(s32 *)(r4p + 0x2c);

    {
        int b = *(u8 *)(c + 0x743);
        if (b != 0) {
            if ((b & 1) == 1) {
                *(s32 *)(c + 0x744) = *(s32 *)(md + 0x54);
                if (*(u8 *)(c + 0x743) < 3)
                    *(s32 *)(c + 0x748) = 0;
                else
                    *(s32 *)(c + 0x748) = *(s32 *)(md + 0x58);
                *(s32 *)(c + 0x74c) = *(s32 *)(md + 0x5c);
                *(s32 *)(c + 0x750) = *(s32 *)(c + 0x744);
                *(s32 *)(c + 0x754) = *(s32 *)(c + 0x748);
                *(s32 *)(c + 0x758) = *(s32 *)(c + 0x74c);
                {
                    u8 *q = (u8 *)(((long long)(int)(c + 0x743)) & 0xFFFFFFFFFFFFFFFFLL);
                    (*q)++;
                }
            } else {
                d.x = (*(s32 *)(md + 0x54) - *(s32 *)(c + 0x744)) << 3;
                if (*(u8 *)(c + 0x743) < 3)
                    d.y = 0;
                else
                    d.y = (*(s32 *)(md + 0x58) - *(s32 *)(c + 0x748)) << 3;
                d.z = (*(s32 *)(md + 0x5c) - *(s32 *)(c + 0x74c)) << 3;
                Vec3_RotateYAndTranslate((Vector3 *)(c + 0x5c), (Vector3 *)(c + 0x5c), *(s16 *)(c + 0x8e), &d);
                *(s32 *)(c + 0x744) = *(s32 *)(md + 0x54);
                if (*(u8 *)(c + 0x743) >= 3)
                    *(s32 *)(c + 0x748) = *(s32 *)(md + 0x58);
                *(s32 *)(c + 0x74c) = *(s32 *)(md + 0x5c);
            }
            *(s32 *)(md + 0x54) = 0;
            if (*(u8 *)(c + 0x743) >= 3)
                *(s32 *)(md + 0x58) = 0;
            *(s32 *)(md + 0x5c) = 0;
        }
    }

    md += 0x68;
    r4p += 0x80;
    {
        s16 i = 2;
        if (*(u32 *)(mp + 4) > 2) {
            do {
            *(s32 *)(md + 0x20) = *(s32 *)(r4p + 0x24);
            *(s32 *)(md + 0x24) = *(s32 *)(r4p + 0x28);
            *(s32 *)(md + 0x28) = *(s32 *)(r4p + 0x2c);
                i = (s16)(i + 1);
                md += 0x34;
                r4p += 0x40;
            } while ((u32)i < *(u32 *)(mp + 4));
        }
    }
}
