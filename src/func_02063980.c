typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

typedef struct Sub {
    u8 pad[0x68];
} Sub;

typedef struct S {
    int f0;            /* 0x00 */
    int pad04;         /* 0x04 */
    u16 f8;            /* 0x08 */
    u16 padA;          /* 0x0a */
    u8 fC;             /* 0x0c */
    u8 padD[7];        /* 0x0d */
    u32 f14;           /* 0x14 */
    u8 pad18[5];       /* 0x18 */
    u8 f1d;            /* 0x1d */
    u8 pad1e[0x1f8 - 0x1e]; /* 0x1e */
    Sub elems[16];     /* 0x1f8 */
} S;

extern int func_02065bc0(void);
extern int func_02065bd0(void);
extern int func_02064f54(u8 *dst, u16 a1, u8 a2, int a3, int a4);
extern int func_0206470c(Sub *e);
extern int func_02064bac(u8 *dst, u16 a1, u8 a2, u32 a3);
extern int func_02064e18(u8 *dst, u16 a1, u8 a2, u32 a3, int a4);
extern int func_02064cd8(u8 *dst, u16 a1, u8 a2, u32 a3, int a4);
extern int func_02064a94(u8 *dst, u16 a1, u8 a2, u32 *a3, u8 a4);

int func_02063980(S *s, u8 *dst)
{
    int ret = 0;
    int i;

    switch (s->f0) {
    case 2:
        ret = func_02064f54(dst, s->f8, s->fC, func_02065bc0(), func_02065bd0());
        break;
    case 4:
    case 6:
    {
        Sub *e = s->elems;
        for (i = 0; i < 16; i++, e++) {
            int bit = 1 << i;
            if (s->f8 & (u16)bit) {
                int v = func_0206470c(e);
                if (v == -1) {
                    ret = func_02064bac(dst, (u16)bit, s->fC, s->f14);
                } else {
                    u16 mask = s->f8;
                    if (s->f0 == 4)
                        ret = func_02064e18(dst, mask, s->fC, s->f14, v);
                    else
                        ret = func_02064cd8(dst, mask, s->fC, s->f14, v);
                }
                if (ret != 0)
                    return ret;
            }
        }
    }
        break;
    case 10:
        ret = func_02064a94(dst, s->f8, s->fC, &s->f14, s->f1d);
        break;
    }
    return ret;
}
