#include "types.h"
#define LA(p) (p)
extern s16 data_02082214[];
extern s8 data_ov092_0213208c[];
extern void func_ov092_021314d0(void *c);

void func_ov092_021316d8(char *c, int a1, int a2, int a3, s16 a4)
{
    int idx;
    int r5;
    s16 mv;
    s16 *ang;

    {
        s16 t = (s16)((*(u16 *)(c + 0x564) + 1) << 12);
        idx = ((u16)t >> 4) * 2;
    }
    r5 = *(int *)(c + 0x55c);
    r5 = r5 + 0x3000;
    mv = data_02082214[idx];
    *(int *)(c + 0x60) = r5 + ((mv * 0x6793a) >> 12);

    {
        int *px = (int *)LA(c + 0x5c);
        *px = *px + a2;
    }
    ang = (s16 *)(c + 0x8c);
    {
        int *pz = (int *)LA(c + 0x64);
        *pz = *pz + a1;
    }

    if (a3) {
        u8 f = *(u8 *)(c + 0x575);
        if ((f & 0x10) != 0) {
            if ((f & 0x20) != 0) {
                s16 *p = (s16 *)LA(c + 0x8e);
                *p = (s16)(*p + a3);
            } else {
                s16 *p = (s16 *)LA(c + 0x8e);
                *p = (s16)(*p - a3);
            }
        } else {
            if ((f & 0x20) != 0)
                *ang = (s16)(*ang - a3);
            else
                *ang = (s16)(*ang + a3);
        }
    } else {
        u8 f = *(u8 *)(c + 0x575);
        s8 k = data_ov092_0213208c[f];
        if (k < 0) {
            s16 *p = ang - ((int)k + 1);
            *p = (s16)(*p - a4);
        } else {
            s16 t = ang[k];
            ang[k] = (s16)(t + a4);
        }
    }

    if (*(u16 *)(c + 0x564) != 7)
        return;

    {
        s16 *ip = (s16 *)LA(c + 0x92);
        s16 *r3 = (s16 *)LA(c + 0x94);
        s16 *r2 = (s16 *)LA(c + 0x96);
        *ip = (s16)(*ip + *(s16 *)(c + 0x8c));
        *r3 = (s16)(*r3 + *(s16 *)(c + 0x8e));
        *r2 = (s16)(*r2 + *(s16 *)(c + 0x90));
    }

    if (*(u8 *)(c + 0x574) == 3)
        *(int *)(c + 0x568) = 0;
    else
        func_ov092_021314d0(c);
    *(u8 *)(c + 0x577) = 1;
}
