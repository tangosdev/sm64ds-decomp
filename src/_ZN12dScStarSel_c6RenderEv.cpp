//cpp
// @symbol _ZN12dScStarSel_c6RenderEv
// NONMATCHING: 13/593 at exact size (was 58). Real dScStarSel_c method over
// include/dScStarSel_c.h, vtable slot 9. Residue is the two-digit level number
// block: the ROM materialises the tens-digit x (0x77) right before the OAM::Render
// argument copy (mov r5,#0x77 / mov r2,r5 after the table load) while every
// spelling tried here emits it at the top of the block.
#pragma opt_loop_invariants off
#pragma opt_strength_reduction off
#include "common.h"
#include "dScStarSel_c.h"
#include "decl_common.h"

extern "C" {
int IsStarCollectedInLevel(s8 levelID, s32 starID);
u32 _ZN8SaveData19IsCharacterUnlockedEj(u32);
void _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(int, void *, int, int, int, int, void *);
void _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(int, void *, int, int, int, int, int, int, int, int);
void _ZN3OAM9RenderSubEP7OamAttrii(void *, int, int);
void _ZN3G3i13PerspectiveW_E5Fix12IiES1_S1_S1_S1_S1_bP9Matrix4x3(int, int, int, int, int, int, int, void *);
void _ZN3G3i7LookAt_EPK7Vector3S2_S2_bP9Matrix4x3(const void *, const void *, const void *, int, Matrix4x3 *);
void _Z13CopyToViewMatPK9Matrix4x3(const Matrix4x3 *);
void Matrix4x3_FromTranslation(Matrix4x3 *, s32, s32, s32);
void Matrix4x3_ApplyInPlaceToRotationY(Matrix4x3 *, s16);
void Matrix4x3_ApplyInPlaceToScale(Matrix4x3 *, s32, s32, s32);

extern s8 data_02092110;
extern s16 data_02082214[];
extern s32 data_020a0db0;
extern u8 data_0209caa0[];
extern Matrix4x3 data_020a0e68;
extern void *_ZN3OAM7NUMBERSE[];
extern u8 data_ov000_020ab938;
extern u8 data_ov000_020ab940;
extern void *data_ov000_020abb18[];
extern u8 data_ov000_020abd78;
extern u8 data_ov000_020abd80;
extern u8 data_ov000_020abb34;
extern u8 data_ov000_020abb54;
extern u8 data_ov000_020abb74;
extern u8 data_ov000_020abb94;
extern void *data_ov000_020abcb4[];
extern u8 data_ov000_020abbb4;
extern u8 data_ov000_020abbf4;
}

#define FB(p, o) (*(u8 *)((u8 *)(p) + (o)))
#define FH(p, o) (*(s16 *)((u8 *)(p) + (o)))
#define FW(p, o) (*(s32 *)((u8 *)(p) + (o)))
#define COS(a) data_02082214[((u16)(a) >> 4) * 2 + 1]

s32 dScStarSel_c::Render()
{
    s32 x;
    s32 i;
    s32 idx;
    Model *m;
    s32 yoff;
    Matrix4x3 mtx;

    func_ov003_020ae238((char *)this);
    func_ov003_020ae0b0((char *)this);

    if (SublevelToLevel(data_02092110) < 0xf) {
        u8 v;
        s32 c;
        func_ov003_020adfc8((char *)this);
        v = SublevelToLevel(data_02092110) + 1;
        if (v <= 0xf) {
            if (v < 10) {
                c = 0x7c;
            } else {
                _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(0, _ZN3OAM7NUMBERSE[v / 10], c = 0x77, 0x7a, 8, -1, 0);
                c += 9;
            }
            _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(0, _ZN3OAM7NUMBERSE[v % 10], c, 0x7a, 8, -1, 0);
        }
    }

    if (SublevelToLevel(data_02092110) <= 0xe) {
        _ZN3G3i13PerspectiveW_E5Fix12IiES1_S1_S1_S1_S1_bP9Matrix4x3(data_02082214[0x16], data_02082214[0x17], 0x1555, 0x1000, 0x1388000, 0x1000, 1, 0);
        _ZN3G3i7LookAt_EPK7Vector3S2_S2_bP9Matrix4x3(&data_ov003_020b132c, &data_ov003_020b1338, &data_ov003_020b1344, 1, &mtx);
        _Z13CopyToViewMatPK9Matrix4x3(&mtx);

        x = -(((s32)FB(this, 0x114) - 1) * 0x18000 / 2);
        for (i = 1; i <= FB(this, 0x114); i++) {
            Matrix4x3_FromTranslation(&data_020a0e68, x, 0x28000, 0);
            if (i - 1 == FB(this, 0x115)) {
                Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, (s16)(data_020a0db0 * 0x300));
                Matrix4x3_ApplyInPlaceToScale(&data_020a0e68, 0x1400, 0x1400, 0x1400);
            }
            idx = (IsStarCollectedInLevel(data_02092110, i) == 0) ? 1 : 0;
            m = &models[idx];
            m->mat4x3 = data_020a0e68;
            m->Render(0);
            x += 0x18000;
        }

        for (i = 0; i < FB(this, 0x114); i++) {
            if ((FB(this, 0x131) >> i) & 1) {
                if (i == FB(this, 0x115)) {
                    _ZN3OAM9RenderSubEP7OamAttrii(&data_ov000_020ab938, FB((u8 *)this + i, 0x11a), 0x18);
                } else {
                    _ZN3OAM9RenderSubEP7OamAttrii(&data_ov000_020ab940, FB((u8 *)this + i, 0x11a), 0x18);
                }
            }
            _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(0, data_ov000_020abb18[i], FB((u8 *)this + i, 0x11a), 8, -1, -1, 0);
        }

        if (FB(this, 0x135) != 0) {
            if (FB(this, 0x133) == 0) {
                _ZN3OAM9RenderSubEP7OamAttrii(&data_ov000_020abd78, FB((u8 *)this + FB(this, 0x115), 0x11a), 6);
            } else if (FB(this, 0x133) == 1) {
                u8 *sel = (u8 *)this + func_ov003_020adf50((char *)this);
                _ZN3OAM9RenderSubEP7OamAttrii(&data_ov000_020abd80, FB(sel, 0x124) - 0x24, FB(sel, 0x128) - 8);
            } else {
                _ZN3OAM9RenderSubEP7OamAttrii(&data_ov000_020abd80, 0x50, FB(this, 0x12b) + 8);
            }
        }

        if (FB(this, 0x139) != 0) {
        if (FB(this, 0x139) == 1) {
            switch (FB(this, 0x137)) {
            case 0:
                FW(this, 0x50) += 0x400;
                if (FW(this, 0x50) >= 0x1800) {
                    FW(this, 0x50) = 0x1800;
                    FB(this, 0x137) = 1;
                }
                break;
            case 1:
                FW(this, 0x50) -= 0x400;
                if (FW(this, 0x50) <= 0x1000) {
                    FW(this, 0x50) = 0x1000;
                    FB(this, 0x137) = 2;
                }
                break;
            case 2:
                FW(this, 0x50) -= 0x80;
                if (FB(this, 0x119) == 0) {
                    FB(this, 0x137) = 3;
                }
                break;
            case 3:
                FW(this, 0x50) += 0x100;
                FH(this, 0x110) += 0x1000;
                break;
            }
            FW(this, 0x54) = FW(this, 0x50);
        } else if (FB(this, 0x139) == 2) {
            FW(this, 0x58) = (0x1000 - COS((s16)(FH(this, 0x112) - 0x4000))) * 10 + 0x1000;
            FW(this, 0x5c) = 0x1000;
            FH(this, 0x112) = (FH(this, 0x112) + 0x1000) & 0x7fff;
            if (FH(this, 0x112) == 0x4000) {
                FB(this, 0x13a) ^= 1;
            }
            if (FW(this, 0x60) != 0) {
                FW(this, 0x60) += 0x1000;
                if (FW(this, 0x60) >= 0x10000) {
                    FW(this, 0x60) = 0;
                }
            }
        }
        }

        switch (data_0209caa0[0x41]) {
        case 0:
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(1, &data_ov000_020abb34, 0x80, FB(this, 0x12b), -1, -1, FW(this, 0x50), FW(this, 0x54), (u16)FH(this, 0x110), -1);
            break;
        case 1:
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(1, &data_ov000_020abb54, 0x80, FB(this, 0x12b), -1, -1, FW(this, 0x50), FW(this, 0x54), (u16)FH(this, 0x110), -1);
            break;
        case 2:
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(1, &data_ov000_020abb74, 0x80, FB(this, 0x12b), -1, -1, FW(this, 0x50), FW(this, 0x54), (u16)FH(this, 0x110), -1);
            break;
        case 3:
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(1, &data_ov000_020abb94, 0x80, FB(this, 0x12b), -1, -1, FW(this, 0x50), FW(this, 0x54), (u16)FH(this, 0x110), -1);
            for (i = 0; i < 3; i++) {
                yoff = 0;
                if (FB(this, 0x132) == i && FB(this, 0x118) != 0) {
                    yoff = 3;
                }
                if (_ZN8SaveData19IsCharacterUnlockedEj(i) != 0) {
                    if (FB(this, 0x139) == 2) {
                        if (i == func_ov003_020adf50((char *)this)) {
                            if (FB(this, 0x13a) == 0) {
                                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(1, data_ov000_020abcb4[i + 3], FB((u8 *)this + i, 0x124), FB((u8 *)this + i, 0x128), -1, -1, FW(this, 0x58), FW(this, 0x5c), 0, -1);
                                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(1, data_ov000_020abcb4[FB((u8 *)this + i, 0x12c)], FB((u8 *)this + i, 0x124), FB((u8 *)this + i, 0x128), -1, -1, FW(this, 0x58), FW(this, 0x5c), 0, -1);
                            } else {
                                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(1, data_ov000_020abcb4[FB((u8 *)this + (i ^ ((i != 0) ? 3 : 0)), 0x12c)], FB((u8 *)this + i, 0x124), FB((u8 *)this + i, 0x128), -1, -1, FW(this, 0x58), FW(this, 0x5c), 0, -1);
                            }
                        } else if (FW(this, 0x60) != 0) {
                            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(1, data_ov000_020abcb4[i + 3], FB((u8 *)this + i, 0x124), FB((u8 *)this + i, 0x128), -1, -1, FW(this, 0x60), FW(this, 0x60), 0, -1);
                            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(1, data_ov000_020abcb4[FB((u8 *)this + i, 0x12c)], FB((u8 *)this + i, 0x124), FB((u8 *)this + i, 0x128), -1, -1, FW(this, 0x60), FW(this, 0x60), 0, -1);
                        }
                    } else {
                        _ZN3OAM9RenderSubEP7OamAttrii(data_ov000_020abcb4[i + 3], FB((u8 *)this + i, 0x124), yoff + FB((u8 *)this + i, 0x128));
                        _ZN3OAM9RenderSubEP7OamAttrii(data_ov000_020abcb4[FB((u8 *)this + i, 0x12c)], FB((u8 *)this + i, 0x124), yoff + FB((u8 *)this + i, 0x128));
                    }
                }
            }
            break;
        }

        if (FB(this, 0x130) <= 1 || data_0209caa0[0x41] != 3) {
            if (SublevelToLevel(data_02092110) <= 0xe) {
                _ZN3OAM9RenderSubEP7OamAttrii(&data_ov000_020abbb4, 0x80, (FH(this, 0x10a) >> 8) + 0xa0);
            } else {
                _ZN3OAM9RenderSubEP7OamAttrii(&data_ov000_020abbf4, 0x80, (FH(this, 0x10a) >> 8) + 0xa0);
            }
        }
    }
    return 1;
}
