#include "types.h"
typedef struct Vec { s32 x, y, z; } Vec;
typedef struct VecN { s16 x, y, z; } VecN;

#define REG_BEGIN (*(vu32 *)0x4000500)
#define REG_END   (*(vu32 *)0x4000504)
#define REG_NRM   (*(vu32 *)0x4000484)
#define REG_VTX   (*(vu32 *)0x400048C)

static inline void G3_Vtx(s16 x, s16 y, s16 z)
{
    REG_VTX = (u16)x | ((u16)y << 16);
    REG_VTX = (u16)z;
}

#define SEND_VTX(p) G3_Vtx((s16)((p)->x >> shift), (s16)((p)->y >> shift), (s16)((p)->z >> shift))

#define SEND_NRM(q) \
    REG_NRM = ((((q)->x) >> 3) & 0x3FF) | (((((q)->y) >> 3) & 0x3FF) << 10) | (((((q)->z) >> 3) & 0x3FF) << 20)

void func_ov007_020ca86c(Vec *vtx, VecN *nrm, s32 w, s32 h, s32 shift)
{
    s32 j, i;
    s32 dir;
    s32 w1;
    s32 rowbase;
    VecN *n;
    VecN *n2;
    Vec *p00, *p10, *p01, *p11;

    REG_BEGIN = 0;
    w1 = w + 1;
    dir = 1;
    for (i = 0; i < h; i++) {
        rowbase = i * w1;
        n = nrm + i * w * 2;
        for (j = 0; j < w; j++) {
            p00 = vtx + (rowbase + j);
            p10 = p00 + w1;
            p01 = p00 + 1;
            p11 = p10 + 1;
            n2 = n + 1;
            if (dir == 1) {
                SEND_NRM(n);
                SEND_VTX(p01);
                SEND_VTX(p00);
                SEND_VTX(p10);
                SEND_NRM(n2);
                SEND_VTX(p10);
                SEND_VTX(p11);
                SEND_VTX(p01);
            } else if (dir == -1) {
                SEND_NRM(n);
                SEND_VTX(p10);
                SEND_VTX(p11);
                SEND_VTX(p00);
                SEND_NRM(n2);
                SEND_VTX(p01);
                SEND_VTX(p00);
                SEND_VTX(p11);
            }
            n += 2;
        }
        dir = -dir;
    }
    REG_END = 0;
}
