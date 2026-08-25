//cpp
// @symbol _ZN13dScMgFlower_c8BehaviorEv
#include "types.h"
#include "decl_common.h"
#include "dScMgFlower_c.h"
/* dScMgFlower_c::Behavior -- vtable slot 6.
 *
 * Attributed by the ROM's vtable; one of the three slots (0, 6, 9) where Flower's
 * table really does differ from dScMgSingle3DBase_c's. The four addresses that
 * once carried `recovered name: dScMgFlower_c_*` for slots 2/5/7/10 were the
 * parent's and moved up in commit 4f7406b9c -- see include/dScMgFlower_c.h.
 *
 * The whole minigame: pick the flower nearest the cursor, drag it, drop it, score
 * the pair, and run the between-rounds state machine. mArray at 0x4f38 is the
 * 0x16-entry flower table, spelled here by raw offset because its element type is
 * not reconstructed yet.
 *
 * LA/LB ARE LAUNDERS, NOT CASTS. Each forces the address through an integer so
 * mwccarm re-materialises it instead of common-subexpressing the field address --
 * removing one is a byte-level change. The two pragmas are load-bearing for the
 * same reason. Only the declarations move: the body is byte-for-byte the one that
 * already matched, with `this` in place of the char* parameter. */

extern "C" {
typedef struct V2 {
    int x;
    int z;
} V2;

typedef struct Cell {
    int x;
    int z;
    char pad[0x18];
} Cell;

void Vec2_Sub(V2 *out, V2 *a, V2 *b);
int Vec2_Len(V2 *p);
void FreeGfxSlotsById(int a);
extern u8 data_020a0e40[];
extern u8 data_020a0de8[];
extern u8 data_020a0de9[];
extern u8 data_020a0dea[];
extern u8 data_020a0deb[];
}

#define LB(p) ((int)((int)(p)))
#define LA(p) ((int)(p))

#pragma opt_strength_reduction off
#pragma opt_common_subs off

s32 dScMgFlower_c::Behavior()
{
    char *c = (char *)this;
    struct dScMgFlower_c *self = this;
    int i;
    int k;
    int t;
    V2 d1;
    V2 d2;
    V2 d3;

    if (self->mPromptBlinkCount == 0) {
        self->mPromptEnabled = 1;
        self->mPromptBlinkCount = 1;
        self->mPromptBlinkTimer = 0;
    }
    if (self->mHintTimer > 0) {
        if (self->mHeldPetal == -1) {
            *(int *)LA(c + 0x5fd0) -= 1;
            if (self->mHintTimer <= 0) {
                if (self->mPetalsLeft >= 1)
                    self->mFaceSprite = 2;
            }
        } else {
            self->mHintTimer = 0x3c;
        }
    }
    switch (self->mState) {
    case 0:
        self->mPrevCursorX = self->mCursorX;
        self->mPrevCursorY = self->mCursorY;
        k = data_020a0e40[0];
        if (data_020a0de8[k * 4] != 0) {
            int b = data_020a0deb[k * 4];
            int a = data_020a0dea[k * 4];
            self->mCursorX = (b ? a : a) << 12;
            self->mCursorY = b << 12;
        }
        if (self->mHoldTimer > 0x14) {
            if (self->mPetalsLeft > 0) {
                for (i = 0; i < self->mPetalsLeft; i++) {
                    *(u8 *)(c + i * 0x20 + 0x4f39) = 1;
                    *(u8 *)(c + i * 0x20 + 0x4f3b) = 1;
                }
                self->mPetalsLeft = 0;
                _ZN5Sound12PlayBank2_2DEj(0x10d);
                FreeGfxSlotsById(0x1d);
            }
        } else if (self->mHeldPetal < 0) {
            int t;
            if (data_020a0de8[k * 4] != 0 && data_020a0de9[k * 4] != 0)
                t = 1;
            else
                t = 0;
            if (t != 0) {
                int ii;
                V2 *q = (V2 *)(c + 0x4f3c);
                for (ii = 0; ii < 0x16; ii++) {
                    if (*(u8 *)(c + ii * 0x20 + 0x4f38) != 0 &&
                        *(u8 *)(c + ii * 0x20 + 0x4f3a) != 1 &&
                        *(u8 *)(c + ii * 0x20 + 0x4f39) != 1) {
                        int v;
                        Vec2_Sub(&d1, q, (V2 *)(c + 0x5fb8));
                        v = Vec2_Len(&d1) < 0x18000 ? 1 : 0;
                        if (v != 0) {
                            _ZN5Sound12PlayBank2_2DEj(0x109);
                            *(u8 *)(c + ii * 0x20 + 0x4f3a) = 1;
                            *(int *)(c + ii * 0x20 + 0x4f44) = 0;
                            *(int *)(c + ii * 0x20 + 0x4f48) = 0;
                            self->mHeldPetal = ii;
                            *(int *)(c + ii * 0x20 + 0x4f4c) = self->mCursorX;
                            *(int *)(c + ii * 0x20 + 0x4f50) = self->mCursorY;
                            FreeGfxSlotsById(0x1d);
                            self->mFaceSprite = 2;
                            break;
                        }
                    }
                    q = (V2 *)((char *)q + 0x20);
                }
                if (self->mHeldPetal < 0) {
                    int i2;
                    V2 *q2 = (V2 *)(c + 0x4f3c);
                    for (i2 = 0; i2 < 0x16; i2++) {
                        if (*(u8 *)(c + i2 * 0x20 + 0x4f38) != 0 &&
                            *(u8 *)(c + i2 * 0x20 + 0x4f3a) != 1) {
                            int v2;
                            Vec2_Sub(&d2, q2, (V2 *)(c + 0x5fb8));
                            v2 = Vec2_Len(&d2) < 0x18000 ? 1 : 0;
                            if (v2 != 0) {
                                *(u8 *)(c + i2 * 0x20 + 0x4f3a) = 1;
                                *(int *)(c + i2 * 0x20 + 0x4f44) = 0;
                                *(int *)(c + i2 * 0x20 + 0x4f48) = 0;
                                self->mHeldPetal = i2;
                                *(int *)(c + i2 * 0x20 + 0x4f4c) = self->mCursorX;
                                *(int *)(c + i2 * 0x20 + 0x4f50) = self->mCursorY;
                                break;
                            }
                        }
                        q2 = (V2 *)((char *)q2 + 0x20);
                    }
                }
            }
        } else {
            t = 0;
            if (data_020a0de8[k * 4] != 0) {
                Cell *cells;
                int j;
                Vec2_Sub(&d3, (V2 *)(c + 0x5fb8), (V2 *)(c + 0x5fc0));
                cells = (Cell *)(c + 0x4f3c);
                j = self->mHeldPetal;
                cells[j].x += d3.x;
                *(int *)LB((char *)cells + j * 0x20 + 4) += d3.z;
            } else {
                if (*(u8 *)(c + self->mHeldPetal * 0x20 + 0x4f39) == 0) {
                    *(u8 *)(c + self->mHeldPetal * 0x20 + 0x4f39) = 1;
                    if (*(u8 *)(c + self->mHeldPetal * 0x20 + 0x4f3b) == 0) {
                        t = 1;
                        *(u8 *)(c + self->mHeldPetal * 0x20 + 0x4f3b) = 1;
                        func_ov006_020c3990(c + 0x51f8);
                    }
                }
                *(u8 *)(c + self->mHeldPetal * 0x20 + 0x4f3a) = 0;
                self->mHeldPetal = -1;
            }
            if (t != 0) {
                *(int *)LA(c + 0x5fd8) -= 1;
                if (self->mPetalToggle == 1) {
                    self->mPetalToggle = 0;
                    if (self->mPetalsLeft >= 1) {
                        FreeGfxSlotsById(0x1d);
                        func_ov004_020b0cac(0x13, 0x80, 0x18, 0, -1, 0xd);
                        self->mFaceSprite = 3;
                        _ZN5Sound12PlayBank2_2DEj(0x104);
                    }
                } else {
                    self->mPetalToggle = 1;
                    if (self->mPetalsLeft >= 1) {
                        FreeGfxSlotsById(0x1d);
                        func_ov004_020b0cac(0x10, 0x80, 0x18, 0, -1, 0xd);
                        self->mFaceSprite = 1;
                        _ZN5Sound12PlayBank2_2DEj(0x103);
                    }
                }
                self->mHintTimer = 0x3c;
            }
        }
        if (self->mPetalsLeft <= 0 && self->unk_5fcd == 1) {
            if (self->mResultTimer > 0) {
                *(int *)LA(c + 0x5fd4) -= 1;
                if (self->mResultTimer <= 0) {
                    self->mState = 1;
                    func_ov004_020b0a54(0xc);
                }
                if (func_ov006_020c3b80(c + 0x51f8) != 0) {
                    if (self->mPetalToggle == 1)
                        func_ov006_020c3908(c + 0x51f8);
                    else
                        func_ov006_020c38b0(c + 0x51f8);
                }
            } else {
                FreeGfxSlotsById(0x1d);
                if (self->mHoldTimer > 0x14) {
                    if (self->unk_5fcd == 1) {
                        _ZN5Sound12PlayBank2_2DEj(0x106);
                        self->mFaceSprite = 4;
                        self->mResultTimer = 0x3c;
                    }
                } else {
                if (self->mPetalToggle == 1) {
                    self->mFaceSprite = 0;
                    *(int *)LA(c + 0x5fdc) += 1;
                    self->mLoseStreak = 0;
                    if (self->mWinStreak >= 3) {
                        func_ov004_020b0cac(0x12, 0x80, 0x18, 0, -1, 0xd);
                        _ZN5Sound12PlayBank2_2DEj(0x107);
                        *(int *)LA(c + 0x5ff0) += 3;
                        if (self->mScore > 0x270f)
                            self->mScore = 0x270f;
                    } else {
                        func_ov004_020b0cac(0x10, 0x80, 0x18, 0, -1, 0xd);
                        _ZN5Sound12PlayBank2_2DEj(0x108);
                        *(int *)LA(c + 0x5ff0) += 1;
                        if (self->mScore > 0x270f)
                            self->mScore = 0x270f;
                    }
                } else {
                    self->mFaceSprite = 4;
                    *(int *)LA(c + 0x5fe0) += 1;
                    self->mWinStreak = 0;
                    if (self->mLoseStreak >= 3) {
                        func_ov004_020b0cac(0x11, 0x80, 0x18, 0, -1, 0xd);
                        _ZN5Sound12PlayBank2_2DEj(0x105);
                    } else {
                        func_ov004_020b0cac(0x13, 0x80, 0x18, 0, -1, 0xd);
                        _ZN5Sound12PlayBank2_2DEj(0x106);
                    }
                }
                self->mResultTimer = 0x3c;
                }
            }
        }
        func_ov006_0212a654(c);
        break;
    case 1:
        if (func_ov006_020c3b80(c + 0x51f8) != 0) {
            if (self->mPetalToggle == 1)
                func_ov006_020c3908(c + 0x51f8);
            else
                func_ov006_020c38b0(c + 0x51f8);
        }
        self->mPromptEnabled = 0;
        func_ov006_0212a654(c);
        break;
    default:
        break;
    }
    func_ov006_020c3d18(c + 0x51f8);
    return 1;
}
