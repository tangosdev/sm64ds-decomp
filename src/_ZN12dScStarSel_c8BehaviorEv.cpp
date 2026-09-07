//cpp
// @symbol _ZN12dScStarSel_c8BehaviorEv
// NONMATCHING: 27/525 at exact size (was 100). Real dScStarSel_c method over
// include/dScStarSel_c.h, vtable slot 6. Every remaining divergence is register
// naming inside the touch-hit search loop (the ROM keeps found in lr, i in ip,
// the touch record pointer in r6); declaration order moves it between 27 and 35.
#pragma opt_loop_invariants off
#pragma opt_strength_reduction off
#include "common.h"
#include "dScStarSel_c.h"
#include "decl_common.h"
#include "Message.h"

struct VObj {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual int v5();
};

extern "C" {
u8 DecIfAbove0_Byte(u8 *p);
u16 DecIfAbove0_Short(u16 *p);
void func_02012790(int idx);

extern VObj *data_0209f5bc;
extern s8 data_02092110;
extern s32 data_0208ee44;
extern u16 data_0209f5e8[];
extern u8 data_02092128;
extern u8 data_0209caa0[];
extern u8 data_020a0e40;
extern u8 data_020a0de8[];
extern u8 data_020a0de9[];
extern u16 data_020a0e58[];
extern u16 data_020a0e5a[];
}

#define FB(p, o) (*(u8 *)((u8 *)(p) + (o)))
#define FH(p, o) (*(s16 *)((u8 *)(p) + (o)))
#define FU(p, o) (*(u16 *)((u8 *)(p) + (o)))

s32 dScStarSel_c::Behavior()
{
    s32 cur;
    u8 idx;
    u8 touched;
    u8 ty;
    s32 i;
    u8 tx;
    s32 found;
    u8 *rec;
    s32 n;
    s32 hit;
    s32 pressed;

    if (data_0209f5bc->v5() != 0) {
        DecIfAbove0_Byte(&FB(this, 0x117));
        DecIfAbove0_Byte(&FB(this, 0x118));
        if (FB(this, 0x119) != 0 || (SublevelToLevel(data_02092110) > 0xe && IsButtonInputValid() != 0)) {
            if (FB(this, 0x119) != 0) {
                FB(this, 0x119) -= data_0208ee44;
            }
            if (FB(this, 0x119) == 0 || (SublevelToLevel(data_02092110) > 0xe && IsButtonInputValid() != 0)) {
                StartSceneFade(3, 0, 0);
                data_0209f5e8[6] = 0x7fff;
                if (SublevelToLevel(data_02092110) <= 0xe) {
                    data_0209f1f0 = FB(this, 0x115) + 1;
                } else {
                    data_0209f1f0 = 1;
                }
            }
        } else if (FB(this, 0x135) != 0 && IsButtonInputValid() != 0) {
            u8 mode = FB(this, 0x133);
            if (mode == 0 && FB(this, 0x130) > 1 && data_0209caa0[0x41] == 3) {
                FB(this, 0x133) = 2;
                func_02012790(0x12e);
            } else if (FB(this, 0x139) == 0) {
                if (mode == 1) {
                    u8 ch = func_ov003_020adf50((char *)this);
                    data_02092128 = ch;
                    data_02092114 = ch;
                    FB(this, 0x132) = ch;
                    FB(this, 0x118) = data_0208ee44 * 3;
                    FB(this, 0x139) = 2;
                } else {
                    FB(this, 0x133) = 2;
                    FB(this, 0x132) = 3;
                    FB(this, 0x118) = data_0208ee44 * 6;
                    FB(this, 0x139) = 1;
                }
                FB(this, 0x119) = 0x10;
                func_02012790(data_0209caa0[0x41] + 0x3c);
            }
        } else if (SublevelToLevel(data_02092110) <= 0xe) {
            idx = data_020a0e40;
            cur = FB(this, 0x115);
            found = 0;
            touched = data_020a0de8[idx * 4];
            if (touched != 0) {
                n = FB(this, 0x114);
                i = 0;
                if (n > 0) {
                    rec = &data_020a0de8[idx * 4];
                    tx = rec[2];
                    ty = rec[3];
                    do {
                    if ((u8)(tx - FB((u8 *)this + i, 0x11a) + 8) < 0x10 && ty < 0x28 && ((FB(this, 0x131) >> i) & 1)) {
                        hit = (touched != 0 && data_020a0de9[idx * 4] != 0);
                        if (hit != 0 || cur != i) {
                            FB(this, 0x117) = data_0208ee44 * 3;
                        }
                        if (cur != i) {
                            cur = i;
                            func_02012790(0x12e);
                        }
                        FB(this, 0x133) = 0;
                        found = 1;
                        break;
                    }
                    i++;
                    } while (i < n);
                }
            }
            if (found == 0 && FB(this, 0x135) == 2 && FB(this, 0x133) == 0 && data_0209caa0[0x42] == 0) {
                if (data_020a0e58[0] & 0x20) {
                    pressed = data_020a0e58[1] & 0x20;
                    if (pressed != 0 || FU(this, 0x104) == 0) {
                        FU(this, 0x104) = pressed ? 0x10 : 8;
                        if (cur != 0) {
                            u8 mask = FB(this, 0x131);
                            cur--;
                            while (!((mask >> cur) & 1)) {
                                cur--;
                            }
                            func_02012790(0x12e);
                        }
                    }
                } else if (data_020a0e58[0] & 0x10) {
                    pressed = data_020a0e58[1] & 0x10;
                    if (pressed != 0 || FU(this, 0x104) == 0) {
                        FU(this, 0x104) = pressed ? 0x10 : 8;
                        if (cur < FB(this, 0x114) - 1) {
                            u8 mask = FB(this, 0x131);
                            cur++;
                            while (!((mask >> cur) & 1)) {
                                cur++;
                            }
                            func_02012790(0x12e);
                        }
                    }
                }
            }
            if (FB(this, 0x115) != cur) {
                FB(this, 0x115) = cur;
                Message::DisplayStarNameForStarSelect((s16)cur);
                FB(this, 0x117) = data_0208ee44 * 3;
            }
            if (FB(this, 0x139) == 0) {
                func_ov003_020ae358((char *)this);
            }
        }

        if (SublevelToLevel(data_02092110) <= 0xe) {
            if (FB(this, 0x135) == 0) {
                if (IsButtonInputValid() != 0 || (data_0209caa0[0x42] == 0 && (data_020a0e5a[data_020a0e40 * 2] & 0xf0))) {
                    func_02012790(0x12e);
                    if (data_0209caa0[0x42] == 0 && (data_020a0e58[1] & 0x30)) {
                        FB(this, 0x135) = 1;
                    } else {
                        FB(this, 0x135) = 2;
                    }
                    if (SublevelToLevel(data_02092110) <= 0xe) {
                        FB(this, 0x133) = 0;
                    } else if (FB(this, 0x130) > 1 && data_0209caa0[0x41] == 3) {
                        FB(this, 0x133) = 1;
                        if (FB(this, 0x130) != 4) {
                            FB(this, 0x134) = 0;
                        } else {
                            FB(this, 0x134) = 1;
                        }
                    } else {
                        FB(this, 0x133) = 2;
                    }
                }
            } else if (FB(this, 0x135) == 1) {
                if (data_020a0e58[1] != 0) {
                    FB(this, 0x135) = 2;
                }
            } else if (data_0209caa0[0x42] == 0 && (data_020a0e58[0] & 0xc0)) {
                u16 keys = data_020a0e58[1];
                if (keys & 0x40) {
                    if (FB(this, 0x133) != 0) {
                        FB(this, 0x133) -= 1;
                        if (FB(this, 0x133) == 1) {
                            if (FB(this, 0x130) > 1 && data_0209caa0[0x41] == 3) {
                                if (FB(this, 0x130) != 4) {
                                    FB(this, 0x134) = 0;
                                } else {
                                    FB(this, 0x134) = 1;
                                }
                            } else {
                                FB(this, 0x133) = 0;
                            }
                        }
                        func_02012790(0x12e);
                    }
                } else if (keys & 0x80) {
                    if (FB(this, 0x133) != 2) {
                        FB(this, 0x133) += 1;
                        if (FB(this, 0x133) == 1) {
                            if (FB(this, 0x130) > 1 && data_0209caa0[0x41] == 3) {
                                if (FB(this, 0x130) != 4) {
                                    FB(this, 0x134) = 0;
                                } else {
                                    FB(this, 0x134) = 1;
                                }
                            } else {
                                FB(this, 0x133) = 2;
                            }
                        }
                        func_02012790(0x12e);
                    }
                }
            }

            if (FB(this, 0x130) <= 1 || data_0209caa0[0x41] != 3) {
                if (FU(this, 0x108) != 0) {
                    FU(this, 0x108) -= 1;
                    if (FU(this, 0x108) == 0) {
                        FH(this, 0x10a) = 0;
                        FH(this, 0x10e) = -0x400;
                        FH(this, 0x10c) = FH(this, 0x10e);
                        FB(this, 0x136) = 1;
                    }
                } else {
                    if (!(FB(this, 0x136) & 2)) {
                        FH(this, 0x10c) += 0x100;
                        if (FH(this, 0x10c) >= 0) {
                            FB(this, 0x136) = 1;
                        }
                    } else {
                        FH(this, 0x10c) -= 0x100;
                        if (FH(this, 0x10c) >= 0) {
                            FB(this, 0x136) = 3;
                        }
                    }
                    FH(this, 0x10a) += FH(this, 0x10c);
                    if (FB(this, 0x136) == 1 && FH(this, 0x10a) >= 0) {
                        FB(this, 0x136) = 2;
                    } else if (FB(this, 0x136) == 3 && FH(this, 0x10a) <= 0) {
                        FH(this, 0x10e) += 0x80;
                        if (FH(this, 0x10e) >= -0x180) {
                            FU(this, 0x108) = 0x78;
                            FH(this, 0x10a) = 0;
                        } else {
                            FH(this, 0x10c) = FH(this, 0x10e);
                            FB(this, 0x136) = 1;
                        }
                    }
                }
            }
        }
    }
    DecIfAbove0_Short(&FU(this, 0x104));
    return 1;
}
