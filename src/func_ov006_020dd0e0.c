#include "types.h"

extern u8 data_020a0e40;
extern u8 data_020a0de8[];
extern u8 data_020a0de9[];
extern u8 data_020a0dea[];
extern u8 data_020a0deb[];
extern char *data_ov004_020beb68;

extern void func_ov006_020dcb1c(char *c, int i);
extern void func_ov006_020dd4b0(char *c, int i);
extern void func_020127a4(int r0, int r1, int r2, int r3);
extern void _ZN5Sound12PlayBank2_2DEj(unsigned int n);

void func_ov006_020dd0e0(char *c, int i)
{
    u8 k;
    int hit;
    int off;
    int dx;
    int dy;
    int pan;
    int stars;

    if (*(s32 *)(c + 0x51c8) != 2) {
        return;
    }
    k = data_020a0e40;
    hit = 0;
    if (data_020a0de8[k * 4] != 0) {
        hit = data_020a0de9[k * 4] != 0;
    }
    if (hit == 0) {
        return;
    }

    off = i * 0x18;
    dx = data_020a0dea[k * 4] - (*(s32 *)(c + 0x4ac0 + off) >> 12);
    dy = data_020a0deb[k * 4] - (*(s32 *)(c + 0x4ac4 + off) >> 12);
    if (dx <= -0x10) {
        return;
    }
    if (dx >= 0x10) {
        return;
    }
    if (dy <= -0x10) {
        return;
    }
    if (dy >= 0x10) {
        return;
    }

    *(u8 *)(c + 0x4ad5 + off) = 1;
    if (*(u8 *)(c + 0x4ad3 + off) != 0) {
        *(u8 *)(c + 0x4ad0 + off) = 1;
        *(s32 *)(c + 0x4ac8 + off) = *(s32 *)(c + 0x4ac4 + off);
        *(s32 *)(c + 0x4acc + off) = -0x3000;
        *(u8 *)(c + 0x4ad6 + off) = 0;
        func_ov006_020dd4b0(c, i);
        return;
    }

    func_ov006_020dcb1c(c, i);
    *(u8 *)(c + 0x4ad2 + off) = 0;
    *(s32 *)(c + 0x51c8) = 3;
    *(s32 *)(c + 0x51cc) = 0x40;
    pan = ((*(s32 *)(c + 0x4ac0 + off) >> 12) - 0x80) >> 1;
    if (pan >= 0x3c) {
        pan = 0x3c;
    }
    if (pan <= -0x3c) {
        pan = -0x3c;
    }
    func_020127a4(2, 0xee, 0xffff, pan);
    _ZN5Sound12PlayBank2_2DEj(0xf2);
    *(u8 *)(c + 0x4ad0 + off) = 2;
    stars = data_ov004_020beb68 != 0 ? *(s32 *)(data_ov004_020beb68 + 0xa8) : 0;
    if (stars > *(s32 *)(c + 0x51d4)) {
        *(u8 *)(c + 0x51db) = 1;
    } else {
        *(u8 *)(c + 0x51db) = 0;
    }
}
