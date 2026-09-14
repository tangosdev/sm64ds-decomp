//cpp
// @symbol _ZN12dScMgSlot1_c8BehaviorEv
/* recovered: dScMgSlot1_c::Behavior, Mario Slot's per-frame state machine (vtable slot 6). */
// NONMATCHING: div 19 of 519 words. mwccarm 2004/b56, --module ov006,
// @ 0x0210c9e0 size 0x81c. Residue class: colouring (regperm, no nameable web left after CRK2-H's exhaustive harvest).
// Draft from nearmiss/db.jsonl (stored divergence 19), landed 2026-09-14 under Tango's ruling that the functionally-equivalent C
// drafts live on main with an honest banner so the port and readers have source. Logic is
// verified only as far as the residue class implies: register names and instruction order
// for the shape-exact rows, NOT an independent execution audit for the others. Counts as
// decompiled, not matched; tools/enroll.py leaves it out of the ROM build, which keeps the
// original bytes for this range. A byte-exact match replaces this file and drops the banner.
#include "dScMgSlot1_c.h"

extern "C" {
extern u8 DecIfAbove0_Byte(u8 *p);
extern int Sound_PlayIfNotActive(int handle, int a, int b, int c);
extern void func_02012718(void *a, int b);
extern unsigned int func_02012790(unsigned int a);
extern int func_0202ec9c(void *fader, int a);
extern void func_ov004_020ad79c(int a, int b);
extern void func_ov004_020adb1c(int a);
extern void func_ov004_020ae274(void *c);
extern void func_ov006_0210c180(void *o);
extern void func_ov006_0210c1a8(void *o);
extern void func_ov006_0210c218(void *o, s16 x, s16 y);
extern void func_ov006_0210c278(void *o);
extern void func_ov006_0210c2c0(void *o, int v);
extern void func_ov006_0210c2d4(void *o);
extern int func_ov006_0210c500(void *self);
extern u8 data_020a0e40;
extern u8 data_020a0de8[];
extern u8 data_020a0de9[];
extern int data_ov006_0213e600[];
extern int data_ov006_0213e63c[][2];
extern u8 data_ov006_0213e4d8[];
extern s16 data_ov006_0213e654[][2];
extern s16 data_ov006_0213e656[][2];
extern s16 data_ov006_0213e4f8[][2];
extern s16 data_ov006_0213e4fa[][2];
extern struct FaderBrightness data_0209f61c;
}

namespace Sound { void PlayBank2_2D(unsigned int); }

s32 dScMgSlot1_c::Behavior()
{
    char *c = (char *)this;
    int i;

    switch (unk_46b4) {
    case 0:
        for (i = 0; i < 3; i++) {
            unk_4702[i] = 1;
        }
        *(u8 *)(c + 0x4705) = 0;
        unk_470a = 0;
        unk_470b = 0;
        unk_46b4 = 1;
        func_ov006_0210c2d4(&mBetIcon);
        if (mPromptBlinkCount == 0) {
            mPromptEnabled = 1;
            mPromptBlinkCount = 1;
            mPromptBlinkTimer = 0;
        }
        *(u8 *)(c + 0x470c) = 0x3c;
        break;
    case 1:
        if (DecIfAbove0_Byte((u8 *)(c + 0x470c)) == 0) {
            Sound::PlayBank2_2D(0x160);
            *(u8 *)(c + 0x470c) = 0x1e;
            unk_46b4 = 2;
        }
        break;
    case 2:
        if (DecIfAbove0_Byte((u8 *)(c + 0x470c)) == 0) {
            unk_46b4 = 3;
        }
        break;
    case 3: {
        int moving = 0;
        for (i = 0; i < 3; i++) {
            if (unk_4702[i] == 1 || unk_46a4[i] != 0) {
                moving++;
                unk_46a4[i] -= data_ov006_0213e600[*(u8 *)(c + 0x4708) + i];
                if ((int)unk_46a4[i] < 0) {
                    if (unk_4702[i] == 1) {
                        unk_46ff[i]++;
                        unk_46ff[i] %= 21;
                        unk_46a4[i] += 0x40000;
                    } else {
                        unk_46a4[i] = 0;
                    }
                }
            }
        }
        if (moving != 0) {
            *(int *)(c + 0x46b0) = Sound_PlayIfNotActive(*(int *)(c + 0x46b0), 2, 0x161, 0);
        }
        if (*(u8 *)(c + 0x4705) < 3) {
            int hit = 0;
            u8 idx = data_020a0e40;
            if (data_020a0de8[idx * 4] != 0) {
                if (data_020a0de9[idx * 4] != 0) {
                    hit = 1;
                }
            }
            if (hit != 0) {
                u16 off = idx * 4;
                u8 *q = &data_020a0de8[off];
                int tx = q[2];
                int ty = q[3];
                for (i = 0; i < 3; i++) {
                    if (unk_4702[i] == 1) {
                        int dy = ty - data_ov006_0213e63c[i][1];
                        int dx = tx - data_ov006_0213e63c[i][0];
                        if (dx <= 0x20 && dx >= -0x20 && dy <= 0x20 && dy >= -0x20) {
                            unk_4702[i] = 0;
                            (*(u8 *)(c + 0x4705))++;
                            if (mBetIcon.unk_011 != 0) {
                                mBetIcon.unk_011 = 0;
                                func_ov004_020ad79c(unk_0a8, mHudScore);
                            }
                            func_02012718((void *) 0x162, data_ov006_0213e63c[i][0] << 12);
                            break;
                        }
                    }
                }
            }
        } else {
            int stopped = 1;
            for (i = 0; i < 3; i++) {
                if (unk_46a4[i] != 0) {
                    stopped = 0;
                }
            }
            if (stopped == 1) {
                unk_470a = func_ov006_0210c500(this);
                *(int *)(c + 0x46b8) = 0;
                if (unk_470a != 0) {
                    *(int *)(c + 0x46b8) += data_ov006_0213e4d8[unk_4709];
                }
                if (unk_470b != 0 && unk_470b < 3) {
                    *(int *)(c + 0x46b8) += unk_470b << 1;
                }
                func_ov004_020ad79c(unk_0a8 + *(int *)(c + 0x46b8) * mBetIcon.unk_01c, mHudScore);
                unk_46b4 = 4;
                *(u8 *)(c + 0x470c) = 0x3c;
            }
        }
        break;
    }
    case 4:
        if (DecIfAbove0_Byte((u8 *)(c + 0x470c)) == 0) {
            if (unk_470a != 0) {
                func_ov006_0210c218(c + 0x4684, data_ov006_0213e654[unk_4709][0], data_ov006_0213e656[unk_4709][0]);
            }
            if (unk_470b != 0 && unk_470b < 3) {
                func_ov006_0210c218(c + 0x4690, data_ov006_0213e4f8[unk_470b - 1][0], data_ov006_0213e4fa[unk_470b - 1][0]);
            }
            if (*(int *)(c + 0x46b8) != 0) {
                unk_46b4 = 5;
                *(u8 *)(c + 0x470c) = 0x64;
                func_ov006_0210c180(c + 0x469c);
                func_02012790(0x26);
            } else {
                unk_46b4 = 7;
                *(u8 *)(c + 0x470c) = 0x3c;
                func_02012790(0xe);
            }
            mPromptEnabled = 0;
        }
        break;
    case 5:
        if (DecIfAbove0_Byte((u8 *)(c + 0x470c)) == 0) {
            func_ov006_0210c2c0(&mBetIcon, *(int *)(c + 0x46b8));
            *(u8 *)(c + 0x470c) = 0x3c;
            unk_46b4 = 6;
        }
        break;
    case 6: {
        int done = (mBetIcon.unk_020 == 0) ? 1 : 0;
        if (done != 0) {
            func_ov004_020adb1c(unk_0a8);
            if (DecIfAbove0_Byte((u8 *)(c + 0x470c)) == 0) {
                OnYoshiTryEat(4);
            }
        }
        break;
    }
    case 7:
        if (DecIfAbove0_Byte((u8 *)(c + 0x470c)) == 0) {
            if (unk_0a8 > 0) {
                OnYoshiTryEat(5);
            } else {
                int j;
                char *p = c;
                for (j = 0; j < 3; j++) {
                    unk_46a4[j] += 0x40000;
                    unk_46ff[j]++;
                    unk_46ff[j] %= 21;
                    *(u8 *)(p + unk_46ff[j] + 0x46c0) = 0xd;
                    p += 0x15;
                }
                unk_46b4 = 9;
            }
        }
        break;
    case 9:
    case 11: {
        int idx = (unk_46b4 - 9) >> 1;
        unk_46a4[idx] -= 0x2000;
        if ((int)unk_46a4[idx] < 0) {
            unk_46a4[idx] = 0;
            *(u8 *)(c + 0x470c) = 0xa;
            unk_46b4++;
            func_02012718((void *) 0x164, data_ov006_0213e63c[idx][0] << 12);
            if (idx == 0) {
                func_ov004_020ae274((void *) 2);
            }
        }
        break;
    }
    case 13:
        unk_46a4[2] -= 0x2000;
        if ((int)unk_46a4[2] < 0) {
            unk_46a4[2] = 0;
            *(u8 *)(c + 0x470c) = 0x3c;
            unk_46b4 = 8;
            func_02012718((void *) 0x164, data_ov006_0213e63c[2][0] << 12);
        }
        break;
    case 8:
        if (DecIfAbove0_Byte((u8 *)(c + 0x470c)) == 0) {
            dScene_c::SetFaders(&data_0209f61c);
            func_0202ec9c(&data_0209f61c, 2);
            dScene_c::StartSceneFade(5, 0, 0);
        }
        break;
    case 10:
    case 12:
        if (DecIfAbove0_Byte((u8 *)(c + 0x470c)) == 0) {
            unk_46b4++;
        }
        break;
    }
    mBetIcon.Behavior();
    func_ov006_0210c278(c + 0x4684);
    func_ov006_0210c278(c + 0x4690);
    func_ov006_0210c1a8(c + 0x469c);
    return 1;
}
