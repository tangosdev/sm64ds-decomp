//cpp
#include "dScMgAmida_c.h"

extern "C" {
int RandomIntInternal(int *seed);
extern int data_0209d4b8;
extern s32 data_ov006_02141640[];
extern s32 data_ov006_02141650[];
extern u8 data_ov006_0212e1a8[];
}

extern "C" void func_ov006_020d36a4(dScMgAmida_c *self)
{
    int i;
    int j;
    int k;
    int r;
    int t;
    u32 u;
    int again;
    u8 mark0[4];
    u8 mark1[4];

    self->unk_53dd = 0;
    if (self->Unk36() != 0) {
        again = 1;
        for (i = 0; i < 4; i++) {
            self->unk_4694[i] = i;
        }
        do {
            for (k = 0; k < 4; k++) {
                u = (u32)RandomIntInternal(&data_0209d4b8) >> 16;
                r = ((u & 0x7fff) * 4) >> 15;
                t = self->unk_4694[k];
                self->unk_4694[k] = self->unk_4694[r];
                self->unk_4694[r] = t;
            }
            if (self->mRoundCount == 0) {
                again = 0;
                switch (self->mPatternIndex) {
                case 0:
                    self->unk_46a4[0] = 0;
                    self->unk_46a4[1] = 0;
                    self->unk_46a4[2] = 3;
                    self->unk_46a4[3] = 3;
                    break;
                case 1:
                case 3:
                case 5:
                    self->unk_46a4[0] = 0;
                    self->unk_46a4[1] = 0;
                    self->unk_46a4[2] = 1;
                    self->unk_46a4[3] = 3;
                    break;
                default:
                    for (i = 0; i < 4; i++) {
                        self->unk_46a4[i] = i;
                    }
                    break;
                }
                for (j = 0; j < 4; j++) {
                    data_ov006_02141640[self->unk_4694[j]] = self->unk_46a4[j];
                }
            } else {
                for (i = 0; i < 4; i++) {
                    data_ov006_02141650[i] = data_ov006_02141640[i];
                }
                for (j = 0; j < 4; j++) {
                    data_ov006_02141640[self->unk_4694[j]] = self->unk_46a4[j];
                }
                for (j = 0; j < 4; j++) {
                    if (data_ov006_02141650[j] != data_ov006_02141640[j]) {
                        again = 0;
                        break;
                    }
                }
            }
        } while (again == 1);
    } else if (self->unk_46c8 == 1) {
        if (self->mRoundCount == 0) {
            r = ((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 4) >> 15;
            t = self->unk_4694[0];
            self->unk_4694[0] = self->unk_4694[r];
            self->unk_4694[r] = t;
        } else {
            r = (((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 3) >> 15) + 1;
            t = self->unk_4694[0];
            self->unk_4694[0] = self->unk_4694[r];
            self->unk_4694[r] = t;
        }
    } else {
        again = 1;
        for (int n = 0; n < 4; n++) {
            mark1[n] = 0;
        }
        for (j = 0; j < self->unk_46c8; j++) {
            mark1[self->unk_4694[j]] = 1;
        }
        do {
            for (int n = 0; n < 4; n++) {
                mark0[n] = 0;
            }
            for (i = 0; i < 4; i++) {
                self->unk_4694[i] = i;
            }
            for (k = 0; k < 4; k++) {
                u = (u32)RandomIntInternal(&data_0209d4b8) >> 16;
                r = ((u & 0x7fff) * 4) >> 15;
                t = self->unk_4694[k];
                self->unk_4694[k] = self->unk_4694[r];
                self->unk_4694[r] = t;
            }
            for (j = 0; j < self->unk_46c8; j++) {
                mark0[self->unk_4694[j]] = 1;
            }
            if (self->mRoundCount == 0) {
                again = 0;
            } else {
                for (int n = 0; n < 4; n++) {
                    if (mark0[n] != mark1[n]) {
                        again = 0;
                        break;
                    }
                }
            }
        } while (again == 1);
    }

    for (i = 0; i < 4; i++) {
        if (self->Unk36() != 0) {
            self->unk_46b8[i] = 0;
        } else {
            self->unk_46b8[i] = i * *(s32 *)(data_ov006_0212e1a8 + self->mPatternIndex * 0x1c + 0x14) * 0x3c;
        }
        self->unk_4660[i][0] = (self->unk_4694[i] << 6) + 0x20;
        if (self->Unk36() != 0) {
            self->unk_4660[i][1] = -0xcc;
        } else {
            self->unk_4660[i][1] = -0xd4;
        }
        self->unk_4684[i] = -1;
        self->unk_4680[i] = 0;
        self->unk_46b4[i] = 0;
    }
    self->unk_46cc = 0;
    self->mRoundTimer = 0;
}
