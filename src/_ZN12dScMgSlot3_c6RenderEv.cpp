//cpp
// @symbol _ZN12dScMgSlot3_c6RenderEv
#include "types.h"
#include "decl_common.h"
#include "dScMgSlot3_c.h"
/* dScMgSlot3_c::Render -- vtable slot 9.
 *
 * Attributed by the ROM's vtable: the second of the two slots where this class's
 * table differs from dScMgSingle3DBase_c's. The old file's `recovered name:`
 * comment agreed, and here it is right.
 *
 * Draws the three reels -- the win-line pass at state 6 and the scrolling pass
 * otherwise -- then the payout markers, the HUD, and the two swinging lamps whose
 * angles are mLamp1Angle/mLamp2Angle.
 *
 * The pragma is load-bearing, not tidying; the `(int)` launder on the two indexed
 * reads at 0x4fe4/0x4ff4 is there for the same reason. Only the receiver changed:
 * `self` is `this` now instead of a char* parameter cast back to the class. */

extern "C" {
void _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(int b, void *attr, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9);
void Hud_RenderSprite(void *a0, int a1, int a2, int a3, int a4);
void func_ov004_020b1bc8(char *a0, int a1, int a2, int a3);
void func_ov004_020b1e34(char *a0, int a1, int a2, int a3);
int GetGameLanguage(void);
}

typedef struct T4fe4 {
    char pad[0x4fe4];
    int vals[3];
} T4fe4;

extern "C" char *data_ov006_0213e5ec[];

#pragma opt_strength_reduction off

s32 dScMgSlot3_c::Render()
{
    char *c = (char *)this;
    struct dScMgSlot3_c *self = this;
    int i, j;

    func_ov006_020c1eb4(c + 0x4660);
    func_ov006_020c201c(c + 0x4f38);

    if (self->mReelDrawY > 0) {
        for (i = 0; i < 3; i++) {
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, data_ov006_0213e9a4[self->mWinSymbol * 3 + i], 0x80, self->mReelDrawY + 0x10, -1, 2, 0x1000, 0x1000, 0, 1);
        }
        for (i = 0; i < 3; i++) {
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, data_ov006_0213e9a4[self->unk_503c * 3 + i], 0x80, self->mReelDrawY + 0x60, -1, 2, 0x1000, 0x1000, 0, 1);
        }
    } else {
        for (i = 0; i < 3; i++) {
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, data_ov006_0213e9a4[self->mWinSymbol * 3 + i], 0x80, 0x60, -1, 2, 0x1000, 0x1000, 0, 1);
        }
    }

    if (self->mState != 7) {
        if (self->mState == 6) {
            int i2, j2;
            char *p = c;
            for (i2 = 0; i2 < 3; i2++) {
                int y;
                for (j2 = 0, y = 0x30; j2 < 3; j2++) {
                    Hud_RenderSprite(data_ov006_0213e9a4[*(u8 *)(p + j2 + 0x5031) * 3 + i2],
                                        y - (*(int *)((int)c + i2 * 4 + 0x4ff4) >> 12), 0x60, -1, -1);
                    y += 0x50;
                }
                p += 3;
            }
        } else {
            char *p = c;
            int row, rem;
            int i2, j2;
            for (i2 = 0; i2 < 3; i2++) {
                int a = *(int *)(((int)c + i2 * 4 + 0x4fe4)) >> 12;
                int y;
                row = a / 0x50;
                rem = a % 0x50;
                for (j2 = 0, y = 0x30; j2 < 4; j2++) {
                    Hud_RenderSprite(data_ov006_0213e9a4[*(u8 *)(p + row + 0x501c) * 3 + i2],
                                        y - rem, 0x60, -1, -1);
                    row = (row + 1) % self->mStripLength;
                    y += 0x50;
                }
                p += 5;
            }
        }
    }

    if (self->unk_503d < 3 && (self->mFrameCounter & 0x20)) {
        int slot = 3;
        int ok = 1;
        int i3, j3;
        char *p = c;
        for (i3 = 0; i3 < 3; i3++) {
            if (*(u8 *)(c + i3 + 0x502e) != 0) {
                if (slot < 3) {
                    if (self->mWinSymbol != *(u8 *)(p + slot + 0x5031))
                        ok = 0;
                } else {
                    u8 t = self->mWinSymbol;
                    for (j3 = 0; j3 < 3; j3++) {
                        if (t == *(u8 *)(p + j3 + 0x5031)) {
                            slot = j3;
                            break;
                        }
                        if (j3 == 2)
                            ok = 0;
                    }
                }
            }
            if (ok == 0)
                break;
            p += 3;
        }
        if (ok != 0 && slot < 3) {
            int count = 0;
            int i4;
            int y = 0x4c;
            for (i4 = 0; i4 < 3; i4++) {
                if (*(u8 *)(c + i4 + 0x502e) != 0) {
                    Hud_RenderSprite(&data_ov006_0213e5dc, slot * 0x50 + 0x30, y, -1, -1);
                    count++;
                }
                y += 0x18;
            }
            if ((self->mFrameCounter & 0x3f) == 0x20) {
                _ZN5Sound12PlayBank2_2DEj(count > 1 ? 0x1ac : 0x1ab);
            }
        }
    }

    func_ov004_020b1bc8(c, 0xc, 0xc, 0);
    func_ov004_020b1e34(c, 0xe0, 0x14, 1);

    if (self->mState == 3 && self->mWinColumn >= 0) {
        func_ov004_020af948(*(void **)(data_ov006_0213e5ec[GetGameLanguage()] + 8), self->mWinColumn * 0x50 + 0x20, 0x28, 0);
        func_ov004_020af948(*(void **)(data_ov006_0213e5ec[GetGameLanguage()] + 0x34), self->mWinColumn * 0x50 + 0x30, 0x28, 0);
        if (self->mWinColumn == 1) {
            func_ov004_020b2444(self->mWinColumn * 0x50 + 0x40, 0x28, 6, 0, 0, 0, 0x14);
        } else {
            func_ov004_020b2444(self->mWinColumn * 0x50 + 0x40, 0x28, 3, 0, 0, 0, 0x14);
        }
    } else if (self->mState == 4 && self->mWinColumn < 0) {
        func_ov004_020af948(*(void **)(data_ov006_0213e5ec[GetGameLanguage()] + 8), 0x70, 0x28, 0);
        func_ov004_020af948(*(void **)(data_ov006_0213e5ec[GetGameLanguage()] + 0x38), 0x80, 0x28, 0);
        func_ov004_020b2444(0x90, 0x28, 3, 0, 0, 0, 0x28);
    }

    func_ov004_020afb20(data_ov006_0213ac30, 0x18, 0x30, -1, 0, 0x1000, self->mLamp1Angle);
    func_ov004_020afb20(data_ov006_0213ac3c, 0x40, 0x10, -1, 0, 0x1000, self->mLamp2Angle);

    return 1;
}
