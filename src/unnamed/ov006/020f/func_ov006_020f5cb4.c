#include "types.h"
typedef struct Car {
    /* 0x00 */ s32 x;
    /* 0x04 */ u8 unk04[0x12 - 0x04];
    /* 0x12 */ u8 b12;
    /* 0x13 */ u8 b13;
    /* 0x14 */ u8 b14;
    /* 0x15 */ u8 b15;
    /* 0x16 */ u8 b16;
    /* 0x17 */ u8 b17;
} Car;

typedef struct Ctx {
    /* 0x0000 */ Car raw[1];
    /* 0x0018 */ u8 unk18[0x51a8 - 0x18];
    /* 0x51a8 */ Car cars[25];
    /* 0x5400 */ u8 unk5400[0xa];
    /* 0x540a */ u8 mode;
} Ctx;

extern s32 data_ov006_0212e930[];

void func_ov006_020f5cb4(Ctx *ctx, int idx)
{
    s32 lim;
    s32 rem;
    s32 k;
    s32 *px;
    s32 n;
    u8 *pb;

    ctx->cars[idx].x -= 0x10000;
    if (ctx->cars[idx].b16 == 0) {
        lim = data_ov006_0212e930[ctx->mode];
        rem = idx;
        if (idx >= lim) {
            n = lim;
            do {
                rem -= n;
            } while (rem >= n);
        }
        if (rem == 0) {
            ctx->cars[idx].b16 = 1;
            return;
        }
        n = rem + 1;
        k = 1;
        if (n > 1) {
            do {
                px = (s32 *)((u8 *)&ctx->raw[idx] + 0x51a8);
                pb = (u8 *)&ctx->raw[idx] + 0x51be;
                if (ctx->cars[idx - k].b13 != 0) {
                    if (((*px - ctx->cars[idx - k].x) >> 12) <= 4) {
                        *pb = 1;
                        ctx->cars[idx - k].b14 = 6;
                    }
                }
                k++;
            } while (k < n);
        }
    }
    if ((ctx->cars[idx].x >> 12) > -0x18) {
        return;
    }
    *((u8 *)ctx + idx * 24 + 0x51bb) = 0;
    *((u8 *)ctx + idx * 24 + 0x51ba) = 0;
}
