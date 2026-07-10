// NONMATCHING: size 0x198 vs 0x1a0 (delta -8); missing r5 callee-save / 0x8e addne materialization / predicated ge-path
typedef signed char s8;
typedef short s16;
typedef unsigned short u16;
typedef unsigned char u8;
#define LA(p) (((long long)(int)(p)) & 0xFFFFFFFFFFFFFFFFLL)
extern s16 data_02082214[];
extern s8 data_ov092_0213208c[];
extern void func_ov092_021314d0(void *c);

void func_ov092_021316d8(char *c, int a1, int a2, int a3, s16 a4)
{
    int idx;
    int r5;
    s16 mv;
    s16 *ang;
    volatile s16 a4v = a4;

    idx = *(u16 *)(c + 0x564) + 1;
    idx = (int)(idx << 28) >> 16;
    idx = (int)((unsigned)(idx << 16) >> 16);
    idx = idx >> 4;
    idx = idx << 2;
    r5 = *(int *)(c + 0x55c);
    r5 = r5 + 0x3000;
    mv = *(s16 *)((char *)data_02082214 + idx);
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
                s16 *p = (s16 *)(c + 0x8e);
                *p = (s16)(*p + a3);
            } else {
                s16 *p = (s16 *)(c + 0x8e);
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
        if (k >= 0) {
            * (s16 *)((char *)ang + ((int)k << 1)) =
                (s16)(*(s16 *)((char *)ang + ((int)k << 1)) + a4v);
        } else {
            s16 *p = ang - ((int)k + 1);
            *p = (s16)(*p - a4v);
        }
    }

    if (*(u16 *)(c + 0x564) != 7)
        return;

    {
        s16 *ip = (s16 *)(c + 0x92);
        s16 *r3 = (s16 *)(c + 0x94);
        s16 *r2 = (s16 *)(c + 0x96);
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
