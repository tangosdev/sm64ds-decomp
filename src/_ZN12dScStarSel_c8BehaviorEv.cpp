//cpp
// @symbol _ZN12dScStarSel_c8BehaviorEv
// NONMATCHING: 19/525 at exact size 0x834. Real dScStarSel_c method over
// include/dScStarSel_c.h, vtable slot 6. Declaring the two touch-record globals with
// their real 4-byte stride (u8 [][4]) is what took 20 to 19: with the stride in the
// type, the index scale folds into each addressing mode instead of being CSE'd into
// one live temp, so the second read refolds it off the surviving index exactly as the
// ROM does (+0x28c ldrb r0,[r1,r0,lsl#2]). See notes 6bv lever 2.
//
// The banner that stood here claimed 27 against a source that measures 48 today; this
// body is the near-miss DB's row (#2382), which measures 19 in the same run. Do not
// re-derive from the 27 shape.
//
// ALL 19 ARE THE 6bs DEAD-REGISTER DELTA, from exactly two roots, and the rest is
// knock-on. The ROM's compiler will not reuse a register that died on the previous
// instruction; every build we own takes it because it is the lowest free one.
//   root 1  +0x228  the pool address of data_020a0e40 dies at the load of `idx`.
//           ROM: ldrb r2,[r0] (skips r0).  Here: ldrb r0,[r0].  That one choice
//           rotates `idx` and `n` against each other -- 8 words, including the
//           latch cmp and both data_020a0de8/9 reads.
//   root 2  +0x264  `rec` dies at the load of `ty`.
//           ROM: ldrb r7,[r6,#3] (skips r6, then spends r6 on the loop scratch born
//           one instruction later).  Here: ldrb r6,[r6,#3], scratch to r7 -- 11 words.
// Every instruction shape, every immediate and the whole frame already agree, so this
// is the build delta of notes 6bs, not a spelling that has not been found. Per that
// section the only construct that skips a dead register is a volatile-fed separate
// local, which materialises a stack slot and changes the size; it is not admissible
// and was not banked.
//
// MEASURED INERT at this shape (run m100 lane H1, on top of the earlier lane's decl-
// order hill-climbs, single-type sweep, pragma table, statement shuffles and pointer
// splits) -- 170 cells that compiled, nothing under 19:
//   * 130 scope-depth cells moving cur/ty/idx/rec/found/i/tx/n/touched into the
//     `if (touched)` and `if (n > 0)` blocks singly, in pairs and in triples. This is
//     6bu lever 7 applied to exactly the swap it describes, and it does not reach it:
//     any cell that moves `ty` costs, and the best tie is 19.
//   * 24 cells of loop-condition shape x `ty` width. u8/u16/u32 tie at 19 and s32
//     costs 1; a named u8 or s32 temp for the first condition ties; the
//     `& (1 << i)` and reversed-subtract spellings of the guard tie. (The four
//     nested-if cells in that batch did not compile and are not counted.)
//   * 16 cells of the idx/n pair: both first-write orders, `n` hoisted above `idx`,
//     beside it, and below `i = 0`, crossed with u8/s32 for each. This is the
//     first-write-rank lever aimed at the rank tie it is meant for, and it is inert:
//     the type of `idx` does not matter at all (u8 and s32 score identically), and
//     hoisting `n`'s write above the `touched` guard costs 3. A rank tie would have
//     moved; this does not, which is the positive evidence that root 1 is 6bs and
//     not a naming problem.
// Cross-build: 2004/b56 is the ONLY installed build that even reaches 0x834 here
// (1.2 lands 2008-2012, 2.0 1952, dsi 1764-1784), so the version axis is closed too.
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
extern u8 data_020a0de8[][4];
extern u8 data_020a0de9[][4];
extern u16 data_020a0e58[];
extern u16 data_020a0e5a[];
}

#define FB(p, o) (*(u8 *)((u8 *)(p) + (o)))
#define FH(p, o) (*(s16 *)((u8 *)(p) + (o)))
#define FU(p, o) (*(u16 *)((u8 *)(p) + (o)))

s32 dScStarSel_c::Behavior()
{
    s32 cur;
    u8 ty;
    u8 idx;
    u8 *rec;
    s32 found;
    s32 i;
    u8 tx;
    s32 n;
    s32 pressed;
    u8 touched;
    s32 hit;

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
            touched = data_020a0de8[idx][0];
            if (touched != 0) {
                n = FB(this, 0x114);
                i = 0;
                if (n > 0) {
                    rec = data_020a0de8[idx];
                    tx = rec[2];
                    ty = rec[3];
                    do {
                    if ((u8)(tx - FB((u8 *)this + i, 0x11a) + 8) < 0x10 && ty < 0x28 && ((FB(this, 0x131) >> i) & 1)) {
                        hit = (touched != 0 && data_020a0de9[idx][0] != 0);
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
