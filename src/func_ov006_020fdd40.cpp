//cpp
// @symbol func_ov006_020fdd40
#include "dScMgPachinko_c.h"

extern "C" {
extern int RandomIntInternal(int *seed);
extern int data_0209d4b8;
extern int data_ov006_0212eb80[];
extern u8 data_ov006_0213d974[];
extern s16 data_02082214[];
}

#define FMUL(a, b) ((int)(((long long)(a) * (b) + 0x800) >> 12))

extern "C" void func_ov006_020fdd40(dScMgPachinko_c *self)
{
    int i;
    int ones;
    int tens;

    if (self->unk_5c24 != 0) {
        self->unk_5c24--;
        if ((s16)self->unk_5c24 < 0) self->unk_5c24 = 0;
        return;
    }
    self->unk_5c24 = 0x50;
    for (i = 0; i < 0x1e; i++) {
        int ra;
        int rb;
        if (self->mBall[i].active != 0) continue;
        self->mBall[i].active = 1;
        ra = RandomIntInternal(&data_0209d4b8);
        rb = RandomIntInternal(&data_0209d4b8);
        self->mBall[i].x = (data_ov006_0212eb80[(((u32)rb >> 16) & 0x7fff) * 5 >> 15] + ((((u32)ra >> 16) & 0x7fff) * 4 >> 15) * 8) << 12;
        self->mBall[i].y = -0x100000;
        self->mBall[i].unk08 = 0;
        self->mBall[i].unk0c = 0;
        self->mBall[i].unk2f = 0;
        self->mBall[i].unk30 = 0;
        ra = RandomIntInternal(&data_0209d4b8);
        self->mBall[i].unk28 = ((((u32)ra >> 16) & 0x7fff) * 8 >> 15) << 4;
        self->mBall[i].unk24 = 0;
        self->mBall[i].unk1c = 0;
        self->mBall[i].unk36 = self->unk_5c28;
        self->unk_5c26++;
        if (self->unk_5c28 == 0) break;
        {
            int r = (((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 16 >> 15;
            int lvl = self->unk_5c28 >> 1;
            if (lvl >= 4) lvl = 4;
            if (data_ov006_0213d974[r + lvl * 4] == 0) break;
        }
        if ((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 2 >> 15) {
            self->mBall[i].x = 0x100000;
            self->mBall[i].angle = 0xc000 - (((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 0xc >> 15) + 1 << 9);
        } else {
            self->mBall[i].x = 0;
            self->mBall[i].angle = (((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 0xc >> 15) + 1 << 9) + 0xc000;
        }
        self->mBall[i].unk18 = 0;
        self->mBall[i].unk10 = 0;
        self->mBall[i].unk14 = 0;
        self->mBall[i].unk24 = self->mBall[i].angle - 0x4000;
        self->mBall[i].y = -0x60000;
        self->mBall[i].unk08 = FMUL(data_02082214[(self->mBall[i].angle >> 4) * 2 + 1], 0xe80);
        self->mBall[i].unk0c = FMUL(data_02082214[(self->mBall[i].angle >> 4) * 2], 0xe80);
        self->mBall[i].unk2f = 3;
        self->mBall[i].unk2d = 1;
        self->mBall[i].unk33 = 0;
        self->mBall[i].unk20 = 0x1000;
        self->mBall[i].unk28 = 0x10;
        break;
    }
    ones = self->unk_5c26;
    for (tens = 0; ones >= 0xa; tens++) ones -= 0xa;
    if (tens != 0 && ones == 0) self->unk_5c28++;
    if (self->unk_5c28 > 0x28) self->unk_5c28 = 0x27;
    self->unk_5c24 -= self->unk_5c28 << 2;
    if ((s16)self->unk_5c24 <= 0x20) self->unk_5c24 = 0x20;
}
