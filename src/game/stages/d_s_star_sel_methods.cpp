//cpp
/* leftovers:
 * - defer_codegen off stays, and the file stays in ROM order.
 *   Reordering moves the run.
 * - The destructor is written so D1 lands at 0x020addfc and D0 at
 *   0x020ade54. The extra D2 is unreferenced and dropped at link.
 *   It is the key function, so this file emits the vtable. The
 *   native vtable, typeinfo and profile now own their complete data range.
 * - common.h stays before dScStarSel_c.h. The other order grows
 *   Render by 0x18 bytes.
 * - The seven helpers carry coined names (GetCharacterSlot through
 *   UpdateCharacterSelect). Every caller is in this class, and the
 *   bodies only touch this object's fields.
 * - data_020a0e58 is PadData[], so the 4-byte record stride is in the
 *   type. A flat u16[i*2] form costs UpdateCharacterSelect seventeen words.
 *
 * dScStarSel_c, the star-select screen. ov003 .text
 * 0x020addfc..0x020b0580. Base is dScene_c.
 */
#pragma defer_codegen off
#include "common.h"
#include "PlayerInput.h"
#include "nitro/hw/registers.h"
#include "SpawnInfo.h"
#include "dScStarSel_c.h"
#include "decl_common.h"
#include "dFdBrightness_c.h"
#include "dFdColor_c.h"
#include "OAM.h"
#include "SaveData.h"
#include "Message.h"
#include "Sound.h"
#include "nitro/gx/gx_bgcnt.h"
#include "nitro/gx/gx.h"
#include "nitro/gx/gx_vramcnt.h"
#include "nitro/gx/gx_load.h"
#include "nitro/gx/g3x.h"
#include "nitro/gx/g3_util.h"
#include "GX.h"
#include "private/controller_input.h"
#include "math/MtxStack.h"
#include "nitro/fx/fx_trig.h"
#include "math/CalcLib.h"

extern unsigned char sStarSelectCharacterOrder[];
extern unsigned short sStarSelectDecimalDivisors[];
extern "C" unsigned char NumStars(void);

extern int data_0208ee44;
extern PadData data_020a0e58[];
extern "C" int IsStarCollectedInLevel(s8 levelID, s32 starID);
extern "C" void OamRender10_d_s_star_sel_methods(int, void *, int, int, int, int, int, int, int, int);
extern "C" void Matrix4x3_ApplyInPlaceToScale(Matrix4x3 *, s32, s32, s32);
extern s8 gNextSublevel;
extern s32 data_020a0db0;
extern u8 data_02092128[];
extern signed char gNumLives;
extern u8 data_ov001_020ab938;
extern u8 data_ov001_020ab940;
extern void *data_ov001_020abb18[];
extern void *data_ov001_020abcb4[];
extern u8 data_ov001_020abbb4;
extern u8 data_ov001_020abbf4;
/* SaveData::IsCharacterUnlocked was declared three ways across the legacy
 * shards -- `int (unsigned int)`, the same with the parameter named, and
 * `u32 (u32)`. u32 IS unsigned int here, so the parameter type never
 * actually differed; only the return spelling did, and every call site in
 * this file consumes it as `!= 0`. The declaration in SaveData.h is the one
 * they all already held. */

void CopyToViewMat(const Matrix4x3 *);

// ROM 0x020addfc, size 0x58; vtable slot 16. Written first so D1 and D0 are
// emitted first, as in the cartridge. The body is empty: tearing down the
// dExtSimpleModel_c pair (__cxa_vec_cleanup) and the base destructors is all
// compiler-generated.
dScStarSel_c::~dScStarSel_c() {}

// ROM 0x020ade54, size 0x6c; vtable slot 17. The deleting destructor has no
// source of its own: mwccarm emits it from the definition above, directly
// after D1, ending with dScene_c's inline operator delete.

// ROM 0x020adec0, size 0x90. The inverse of GetCursorCharacter: maps a
// character index to its strip slot, or 0 when it has none.
int dScStarSel_c::GetCharacterSlot(unsigned int chr) {
    unsigned char mode = mStripMode;
    if (mode == 4) {
        unsigned char *p = sStarSelectCharacterOrder;
        int i;
        for (i = 0; i < 3; i++) {
            if (chr == *p) {
                return i;
            }
            p++;
        }
    } else if (mode >= 2) {
        int slot = 0;
        int other;
        for (other = 0; other < 3; other++) {
            if (SaveData::IsCharacterUnlocked((unsigned int)other) != 0) {
                if ((unsigned int)other == chr) {
                    return slot;
                }
                slot++;
            }
        }
    }
    return 0;
}

// ROM 0x020adf50, size 0x78. Maps the strip slot mCharCursor to a character
// index: through sStarSelectCharacterOrder when mStripMode is 4, otherwise the
// mCharCursor-th unlocked character when mStripMode is 2 or more; else 0.
int dScStarSel_c::GetCursorCharacter() {
    unsigned char mode = mStripMode;
    if (mode == 4) {
        return sStarSelectCharacterOrder[mCharCursor];
    }
    if (mode >= 2) {
        int slot = 0;
        int chr = 0;
        for (; chr < 3; chr++) {
            if (SaveData::IsCharacterUnlocked((unsigned int)chr) != 0) {
                if (slot == mCharCursor) {
                    return chr;
                }
                slot++;
            }
        }
    }
    return 0;
}

// ROM 0x020adfc8, size 0xe8. Draws the coin record of the current level
// (SaveData::GetCoinRecord) as three digits drawn right to left, then the
// TIMES and COIN sprites.
void dScStarSel_c::RenderCoinScore() {
    int x = 0xb8;
    int lvl = SublevelToLevel(gNextSublevel);
    int coin = SaveData::GetCoinRecord(lvl);
    CalculateDigits(coin);
    int i;
    for (i = 2; i >= 0; i--) {
        signed char d = mDigits[i];
        if (d >= 0) {
            OAM::Render(0, OAM::NUMBERS[d], x, 0x4c, 8, -1, 0);
            x -= 9;
        }
    }
    OAM::Render(0, &OAM::TIMES, x, 0x54, -1, -1, 0);
    OAM::Render(0, &OAM::COIN, x - 0x10, 0x4c, -1, -1, 0);
}

// ROM 0x020ae0b0, size 0xf4. Draws the power-star count: NumStars() as three
// digits, drawn right to left, then the TIMES and POWER_STAR sprites.
void dScStarSel_c::RenderStarCount() {
    int x;
    int y;
    if (SublevelToLevel(gNextSublevel) >= 0xf) {
        y = 0xa0;
        x = 0xb8;
    } else {
        y = 0xac;
        x = 0xf4;
    }
    CalculateDigits(NumStars());
    {
        int i = 2;
        do {
            signed char d = mDigits[i];
            if (d >= 0) {
                OAM::Render(0, OAM::NUMBERS[d], x, y, 8, -1, 0);
                x -= 9;
            }
            i--;
        } while (i >= 0);
    }
    OAM::Render(0, &OAM::TIMES, x, y + 8, -1, -1, 0);
    OAM::Render(0, &OAM::POWER_STAR, x - 0x10, y + 8, -1, -1, 0);
}

// ROM 0x020ae1a4, size 0x94. Splits value into three decimal digits in
// mDigits[0..2], dividing by sStarSelectDecimalDivisors[i] in turn; leading
// zeros are stored as -1 (blank) except the last digit.
void dScStarSel_c::CalculateDigits(int value) {
    int found = 0;
    int i;
    for (i = 0; i < 3; i++) {
        unsigned short place = sStarSelectDecimalDivisors[i];
        int digit = value / place;
        if (digit == 0 && found == 0 && i != 2) {
            mDigits[i] = -1;
        } else {
            found = 1;
            mDigits[i] = digit;
        }
        value = (unsigned short)(value % place);
    }
}

// ROM 0x020ae238, size 0x120. Draws the life counter: the LIFE_ICONS entry for
// mLifeIcon, the TIMES sprite, and the three digits of gNumLives.
void dScStarSel_c::RenderLifeCount() {
    int x;
    int y;
    if (SublevelToLevel(gNextSublevel) >= 0xf) {
        y = 0xa0;
        x = 0x50;
    } else {
        x = 0x10;
        y = 0xac;
    }
    OAM::Render(0, OAM::LIFE_ICONS[mLifeIcon], x, y + 8, -1, -1, 0x1000, 0x1000, 0, -1);
    OAM::Render(0, &OAM::TIMES, x + 0x10, y + 8, -1, -1, 0);
    CalculateDigits((unsigned short)gNumLives);
    {
        int i = 0;
        x += 0x18;
        do {
            signed char d = mDigits[i];
            if (d >= 0) {
                OAM::Render(0, OAM::NUMBERS[d], x, y, 8, -1, 0);
                x += 9;
            }
            i++;
        } while (i < 3);
    }
}

// ROM 0x020ae358, size 0x398. Per-frame selection handling: record hit
// tests and the D-pad.
//
// gActivePlayerSlot selects the active record in the 4-byte-stride tables at
// gTouchHeld..gTouchY. If byte 0 of that record is set and byte 1 (gTouchEdge)
// is set, its bytes 2 and 3 are tested against a box around the face at
// (0x80, mFaceY); a hit sets mCursorTarget 2, mPickedChar 3, mAnimMode 1, seeds
// mPickPressTimer and mExitDelay and plays sound data_0209caa0.file.mCharacter + 0x3c.
//
// Failing that (and only with mStripMode above 1 and data_0209caa0.file.mCharacter == 3),
// the three characters are scanned in order: for each unlocked i, bytes 2 and 3
// of the same record are tested against mCharX/mCharY, and a hit sets
// mCursorTarget 1, mCharCursor to its strip slot, and mPickedChar, data_02092128
// and data_02092114 to the character, mAnimMode 2, and plays the same sound.
//
// With no record active, the d-pad bits of data_020a0e58[0].held move the
// selection: mCharRepeatTimer counts down, and while it is zero a held left/right
// steps mCharCursor within [0, mStripMode - 2] and reloads the timer with 0x10
// or 8 depending on the active player's pressed word.
//
// Codegen note: the stride belongs in the TYPE, and the INDEX is what gets named.
// The 4-byte records are viewed as `[][4]` so each read refolds its own scale
// (`add r2, r4, r1, lsl #2`); flattening gTouchX/gTouchY to a bare `[]` with an
// explicit `* 4` costs 17 words, and the same is true of the PadData records. The last
// six words were a register transposition between the `c + i` and record-row
// address temps, and what closed it was naming the INDEX (`int ri`) for the second
// record read instead of naming the row POINTER: a named row pointer welds both
// reads onto one address temp and inverts the r2/r3 assignment, while the named
// index leaves each read to fold its own scale and hands the row temp r2.
void dScStarSel_c::UpdateCharacterSelect() {
    int idx = gActivePlayerSlot;
    int valid = 0;
    int i;
    if (((u8(*)[4])gTouchHeld)[idx][0] != 0) {
        valid = ((u8(*)[4])gTouchEdge)[idx][0] != 0;
    }
    if (valid != 0) {
        if ((((unsigned char)(((u8(*)[4])gTouchX)[idx][0] - 0x58)) < 0x50) &&
            (((unsigned char)((((u8(*)[4])gTouchY)[idx][0] - mFaceY) + 0x28)) < 0x50)) {
            mCursorTarget = 2;
            mPickedChar = 3;
            mPickPressTimer = (unsigned char)(data_0208ee44 * 6);
            mAnimMode = 1;
            mExitDelay = 0x10;
            func_02012790(data_0209caa0.file.mCharacter + 0x3c);
            return;
        }
        if (mStripMode <= 1) {
            return;
        }
        if (data_0209caa0.file.mCharacter != 3) {
            return;
        }
        for (i = 0; i < 3; i++) {
            if (SaveData::IsCharacterUnlocked(i) != 0) {
                int ri = gActivePlayerSlot;
                if (((unsigned short)((((u8(*)[4])gTouchHeld)[gActivePlayerSlot][2] - mCharX[i]) +
                                      0x18)) < 0x30) {
                    if (((unsigned short)((((u8(*)[4])gTouchHeld)[ri][3] - mCharY[i]) + 0x18)) <
                        0x2b) {
                        mCursorTarget = 1;
                        mCharCursor = (unsigned char)GetCharacterSlot(i);
                        data_02092128[0] = (unsigned char)i;
                        data_02092114 = (unsigned char)i;
                        mPickedChar = (unsigned char)i;
                        mPickPressTimer = (unsigned char)(data_0208ee44 * 3);
                        mAnimMode = 2;
                        mExitDelay = 0x10;
                        func_02012790(data_0209caa0.file.mCharacter + 0x3c);
                        return;
                    }
                }
            }
        }

        return;
    }
    if (data_0209caa0.file.controllerMode == 0) {
        unsigned short ctrl = data_020a0e58[0].held;
        if ((ctrl & 0x30) != 0) {
            unsigned short timer = mCharRepeatTimer;
            unsigned char next;
            if (timer != 0) {
                mCharRepeatTimer -= 1;
                return;
            }
            if (mCursorShown == 0) {
                return;
            }
            if (mCursorTarget != 1) {
                return;
            }
            if (mStripMode < 3) {
                return;
            }
            next = mCharCursor;
            if (ctrl & 0x20) {
                if (((data_020a0e58[0].pressed & 0x20) != 0) || (timer == 0)) {
                    mCharRepeatTimer = (data_020a0e58[idx].pressed & 0x20) ? (0x10) : (8);
                    if (mCharCursor != 0) {
                        next = next - 1;
                    }
                }
            } else if (ctrl & 0x10) {
                if (((data_020a0e58[0].pressed & 0x10) != 0) || (timer == 0)) {
                    mCharRepeatTimer = (data_020a0e58[idx].pressed & 0x10) ? (0x10) : (8);
                    if (mCharCursor != (mStripMode - 2)) {
                        next = next + 1;
                    }
                }
            }
            if (next == mCharCursor) {
                return;
            }
            mCharCursor = next;
            func_02012790(0x12e);
            return;
        }
    }

    mCharRepeatTimer = 0;
}

// ROM 0x020ae6f0, size 0x4; vtable slot 12. Empty override.
void dScStarSel_c::OnPendingDestroy() {}

// ROM 0x020ae6f4, size 0x944; vtable slot 9. Draws the whole screen: level
// number, the star plates, the character strip, the cursor and the bottom sprite.
//
// The two-digit level-number block was the last residue (13 words): the cartridge
// materialises the tens-digit x (mov r5,#0x77) right before the OAM::Render argument
// copy, and starts the /10 magic-multiply chain one instruction earlier. Writing the
// test in its POSITIVE sense -- `if (levelNum >= 10) { render tens; numX += 9; } else numX = 0x7c;`
// instead of `if (levelNum < 10) numX = 0x7c; else { ... }` -- puts the tens branch first in the
// linearisation and the whole 13-word argument-setup block falls into the cartridge's
// order. The two branches are otherwise identical, so this is a source-shape fact and
// not a code change.
s32 dScStarSel_c::Render() {
    s32 x;
    s32 i;
    s32 idx;
    dExtSimpleModel_c *plate;
    s32 pressY;
    Matrix4x3 mtx;

    RenderLifeCount();
    RenderStarCount();

    if (SublevelToLevel(gNextSublevel) < 0xf) {
        u8 levelNum;
        s32 numX;
        RenderCoinScore();
        levelNum = SublevelToLevel(gNextSublevel) + 1;
        if (levelNum <= 0xf) {
            if (levelNum >= 10) {
                OAM::Render(0, OAM::NUMBERS[levelNum / 10], numX = 0x77, 0x7a, 8, -1, 0);
                numX += 9;
            } else {
                numX = 0x7c;
            }
            OAM::Render(0, OAM::NUMBERS[levelNum % 10], numX, 0x7a, 8, -1, 0);
        }
    }

    if (SublevelToLevel(gNextSublevel) <= 0xe) {
        G3i_PerspectiveW_(FX_SinIdx(0xb), FX_CosIdx(0xb), 0x1555, 0x1000, 0x1388000, 0x1000, 1, 0);
        G3i_LookAt_((const Vector3 *)&data_ov003_020b132c, (const Vector3 *)&data_ov003_020b1338,
                    (const Vector3 *)&data_ov003_020b1344, true, &mtx);
        CopyToViewMat(&mtx);

        /* The plates are 0x18000 apart, centred on x = 0, and sit at y = 0x28000. */
        x = -(((s32)mPlateCount - 1) * 0x18000 / 2);
        for (i = 1; i <= mPlateCount; i++) {
            Matrix4x3_FromTranslation(&data_020a0e68, x, 0x28000, 0);
            if (i - 1 == mSelectedPlate) {
                Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, (s16)(data_020a0db0 * 0x300));
                Matrix4x3_ApplyInPlaceToScale(&data_020a0e68, 0x1400, 0x1400, 0x1400);
            }
            idx = (IsStarCollectedInLevel(gNextSublevel, i) == 0) ? 1 : 0;
            plate = &models[idx];
            plate->mat4x3 = data_020a0e68;
            plate->Render(0);
            x += 0x18000;
        }

        for (i = 0; i < mPlateCount; i++) {
            if ((mPlateMask >> i) & 1) {
                if (i == mSelectedPlate) {
                    OAM::RenderSub((OamAttr *)&data_ov001_020ab938, mPlateX[i], 0x18);
                } else {
                    OAM::RenderSub((OamAttr *)&data_ov001_020ab940, mPlateX[i], 0x18);
                }
            }
            OAM::Render(0, (OamAttr *)data_ov001_020abb18[i], mPlateX[i], 8, -1, -1, 0);
        }

        if (mCursorShown != 0) {
            if (mCursorTarget == 0) {
                OAM::RenderSub(&OAM::POINTER_DOWN, mPlateX[mSelectedPlate], 6);
            } else if (mCursorTarget == 1) {
                int slot = GetCursorCharacter();
                OAM::RenderSub(&OAM::POINTER_RIGHT, mCharX[slot] - 0x24, mCharY[slot] - 8);
            } else {
                OAM::RenderSub(&OAM::POINTER_RIGHT, 0x50, mFaceY + 8);
            }
        }

        if (mAnimMode != 0) {
            if (mAnimMode == 1) {
                switch (mFaceAnimStep) {
                case 0:
                    mFaceScaleX += 0x400;
                    if (mFaceScaleX >= 0x1800) {
                        mFaceScaleX = 0x1800;
                        mFaceAnimStep = 1;
                    }
                    break;
                case 1:
                    mFaceScaleX -= 0x400;
                    if (mFaceScaleX <= 0x1000) {
                        mFaceScaleX = 0x1000;
                        mFaceAnimStep = 2;
                    }
                    break;
                case 2:
                    mFaceScaleX -= 0x80;
                    if (mExitDelay == 0) {
                        mFaceAnimStep = 3;
                    }
                    break;
                case 3:
                    mFaceScaleX += 0x100;
                    mFaceAngle += 0x1000;
                    break;
                }
                mFaceScaleY = mFaceScaleX;
            } else if (mAnimMode == 2) {
                mIconScaleX =
                    (0x1000 - FX_CosIdx((u16)(s16)(mPulseAngle - 0x4000) >> 4)) * 10 + 0x1000;
                mIconScaleY = 0x1000;
                mPulseAngle = (mPulseAngle + 0x1000) & 0x7fff;
                if (mPulseAngle == 0x4000) {
                    mAnimFlip ^= 1;
                }
                if (mIdleIconScale != 0) {
                    mIdleIconScale += 0x1000;
                    if (mIdleIconScale >= 0x10000) {
                        mIdleIconScale = 0;
                    }
                }
            }
        }

        switch (data_0209caa0.file.mCharacter) {
        case 0:
            OAM::Render(1, &OAM::PLAYER_SELECT_MARIO, 0x80, mFaceY, -1, -1, mFaceScaleX,
                        mFaceScaleY, (u16)mFaceAngle, -1);
            break;
        case 1:
            OAM::Render(1, &OAM::PLAYER_SELECT_LUIGI, 0x80, mFaceY, -1, -1, mFaceScaleX,
                        mFaceScaleY, (u16)mFaceAngle, -1);
            break;
        case 2:
            OAM::Render(1, &OAM::PLAYER_SELECT_WARIO, 0x80, mFaceY, -1, -1, mFaceScaleX,
                        mFaceScaleY, (u16)mFaceAngle, -1);
            break;
        case 3:
            OAM::Render(1, &OAM::PLAYER_SELECT_YOSHI, 0x80, mFaceY, -1, -1, mFaceScaleX,
                        mFaceScaleY, (u16)mFaceAngle, -1);
            for (i = 0; i < 3; i++) {
                pressY = 0;
                if (mPickedChar == i && mPickPressTimer != 0) {
                    pressY = 3;
                }
                if (SaveData::IsCharacterUnlocked(i) != 0) {
                    if (mAnimMode == 2) {
                        if (i == GetCursorCharacter()) {
                            if (mAnimFlip == 0) {
                                OAM::Render(1, (OamAttr *)data_ov001_020abcb4[i + 3], mCharX[i],
                                            mCharY[i], -1, -1, mIconScaleX, mIconScaleY, 0, -1);
                                OAM::Render(1, (OamAttr *)data_ov001_020abcb4[mCharSprite[i]],
                                            mCharX[i], mCharY[i], -1, -1, mIconScaleX, mIconScaleY,
                                            0, -1);
                            } else {
                                OAM::Render(
                                    1,
                                    (OamAttr *)
                                        data_ov001_020abcb4[mCharSprite[i ^ ((i != 0) ? 3 : 0)]],
                                    mCharX[i], mCharY[i], -1, -1, mIconScaleX, mIconScaleY, 0, -1);
                            }
                        } else if (mIdleIconScale != 0) {
                            OAM::Render(1, (OamAttr *)data_ov001_020abcb4[i + 3], mCharX[i],
                                        mCharY[i], -1, -1, mIdleIconScale, mIdleIconScale, 0, -1);
                            OAM::Render(1, (OamAttr *)data_ov001_020abcb4[mCharSprite[i]],
                                        mCharX[i], mCharY[i], -1, -1, mIdleIconScale,
                                        mIdleIconScale, 0, -1);
                        }
                    } else {
                        OAM::RenderSub((OamAttr *)data_ov001_020abcb4[i + 3], mCharX[i],
                                       pressY + mCharY[i]);
                        OAM::RenderSub((OamAttr *)data_ov001_020abcb4[mCharSprite[i]], mCharX[i],
                                       pressY + mCharY[i]);
                    }
                }
            }
            break;
        }

        if (mStripMode <= 1 || data_0209caa0.file.mCharacter != 3) {
            if (SublevelToLevel(gNextSublevel) <= 0xe) {
                OAM::RenderSub((OamAttr *)&data_ov001_020abbb4, 0x80, (mPromptYOffset >> 8) + 0xa0);
            } else {
                OAM::RenderSub((OamAttr *)&data_ov001_020abbf4, 0x80, (mPromptYOffset >> 8) + 0xa0);
            }
        }
    }
    return 1;
}

// ROM 0x020af038, size 0x834; vtable slot 6.
//
// The stylus-hit loop reads the touch record as one 4-byte row of gTouchHeld (the
// gTouchHeld/gTouchEdge/gTouchX/gTouchY layout PlayerInput.h describes). The
// record's y byte is not a named local: it is read in the loop condition straight off
// the row, and the compiler hoists that invariant read into the preheader itself,
// which is what gives it a fresh register. The slot index is a byte; a 32-bit index
// CSEs idx * 4 instead of refolding it for the edge read.
//
// Two scratch locals are shared between the character-pick branch and the stylus
// branch: `val` carries mCursorTarget in the first and the stylus-held
// flag in the second, `num` the picked character and then the plate count. The
// register allocator orders webs by where their variable is first assigned, so the
// sharing is what puts the plate count in r0 and the slot index in r2 at the loop;
// as four fresh locals they colour the other way round (re-measured: 8 words differ;
// splitting `val` alone still leaves 6).

extern dFdBrightness_c *gSceneFader;
extern dFdColor_c gSceneColorFader;

s32 dScStarSel_c::Behavior() {
    u8 *rec;
    u8 hit;
    int pressed;
    s32 cur;
    u32 found;
    s32 i;
    long tx;
    s32 num;
    u8 val;
    unsigned char idx;

    if (gSceneFader->IsAtStart() != 0) {
        DecIfAbove0_Byte(&mPlatePressTimer);
        DecIfAbove0_Byte(&mPickPressTimer);
        if (mExitDelay != 0 ||
            (SublevelToLevel(gNextSublevel) > 0xe && IsButtonInputValid() != 0)) {
            if (mExitDelay != 0) {
                mExitDelay -= data_0208ee44;
            }
            if (mExitDelay == 0 ||
                (SublevelToLevel(gNextSublevel) > 0xe && IsButtonInputValid() != 0)) {
                StartSceneFade(3, 0, 0);
                gSceneColorFader.color = 0x7fff;
                if (SublevelToLevel(gNextSublevel) <= 0xe) {
                    gNextStarId = mSelectedPlate + 1;
                } else {
                    gNextStarId = 1;
                }
            }
        } else if (mCursorShown != 0 && IsButtonInputValid() != 0) {
            val = mCursorTarget;
            if (val == 0 && mStripMode > 1 && data_0209caa0.file.mCharacter == 3) {
                mCursorTarget = 2;
                func_02012790(0x12e);
            } else if (mAnimMode == 0) {
                if (val == 1) {
                    num = GetCursorCharacter();
                    data_02092128[0] = num;
                    data_02092114 = num;
                    mPickedChar = num;
                    mPickPressTimer = data_0208ee44 * 3;
                    mAnimMode = 2;
                } else {
                    mCursorTarget = 2;
                    mPickedChar = 3;
                    mPickPressTimer = data_0208ee44 * 6;
                    mAnimMode = 1;
                }
                mExitDelay = 0x10;
                func_02012790(data_0209caa0.file.mCharacter + 0x3c);
            }
        } else if (SublevelToLevel(gNextSublevel) <= 0xe) {
            idx = gActivePlayerSlot;
            cur = mSelectedPlate;
            found = 0;
            val = ((u8(*)[4])gTouchHeld)[idx][0];
            if (val != 0) {
                num = mPlateCount;
                i = 0;
                if (num > 0) {
                    rec = ((u8(*)[4])gTouchHeld)[idx];
                    tx = rec[2];
                    do {
                        if ((u8)(tx - mPlateX[i] + 8) < 0x10 &&
                            ((u8(*)[4])gTouchHeld)[idx][3] < 0x28 && ((mPlateMask >> i) & 1)) {
                            hit = (val != 0 && ((u8(*)[4])gTouchEdge)[idx][0] != 0);
                            if (hit != 0 || cur != i) {
                                mPlatePressTimer = data_0208ee44 * 3;
                            }
                            if (cur != i) {
                                cur = i;
                                func_02012790(0x12e);
                            }
                            mCursorTarget = 0;
                            found = 1;
                            break;
                        }
                        i++;
                    } while (i < num);
                }
            }
            if (found == 0 && mCursorShown == 2 && mCursorTarget == 0 &&
                data_0209caa0.file.controllerMode == 0) {
                if (data_020a0e58[0].held & 0x20) {
                    pressed = data_020a0e58[0].pressed & 0x20;
                    if (pressed != 0 || mPlateRepeatTimer == 0) {
                        mPlateRepeatTimer = pressed ? 0x10 : 8;
                        if (cur != 0) {
                            u8 mask = mPlateMask;
                            cur--;
                            while (!((mask >> cur) & 1)) {
                                cur--;
                            }
                            func_02012790(0x12e);
                        }
                    }
                } else if (data_020a0e58[0].held & 0x10) {
                    pressed = data_020a0e58[0].pressed & 0x10;
                    if (pressed != 0 || mPlateRepeatTimer == 0) {
                        mPlateRepeatTimer = pressed ? 0x10 : 8;
                        if (cur < mPlateCount - 1) {
                            u8 mask = mPlateMask;
                            cur++;
                            while (!((mask >> cur) & 1)) {
                                cur++;
                            }
                            func_02012790(0x12e);
                        }
                    }
                }
            }
            if (mSelectedPlate != cur) {
                mSelectedPlate = cur;
                Message::DisplayStarNameForStarSelect((s16)cur);
                mPlatePressTimer = data_0208ee44 * 3;
            }
            if (mAnimMode == 0) {
                UpdateCharacterSelect();
            }
        }

        if (SublevelToLevel(gNextSublevel) <= 0xe) {
            if (mCursorShown == 0) {
                if (IsButtonInputValid() != 0 ||
                    (data_0209caa0.file.controllerMode == 0 &&
                     (data_020a0e58[gActivePlayerSlot].pressed & 0xf0))) {
                    func_02012790(0x12e);
                    if (data_0209caa0.file.controllerMode == 0 &&
                        (data_020a0e58[0].pressed & 0x30)) {
                        mCursorShown = 1;
                    } else {
                        mCursorShown = 2;
                    }
                    if (SublevelToLevel(gNextSublevel) <= 0xe) {
                        mCursorTarget = 0;
                    } else if (mStripMode > 1 && data_0209caa0.file.mCharacter == 3) {
                        mCursorTarget = 1;
                        if (mStripMode != 4) {
                            mCharCursor = 0;
                        } else {
                            mCharCursor = 1;
                        }
                    } else {
                        mCursorTarget = 2;
                    }
                }
            } else if (mCursorShown == 1) {
                if (data_020a0e58[0].pressed != 0) {
                    mCursorShown = 2;
                }
            } else if (data_0209caa0.file.controllerMode == 0 && (data_020a0e58[0].held & 0xc0)) {
                u16 keys = data_020a0e58[0].pressed;
                if (keys & 0x40) {
                    if (mCursorTarget != 0) {
                        mCursorTarget -= 1;
                        if (mCursorTarget == 1) {
                            if (mStripMode > 1 && data_0209caa0.file.mCharacter == 3) {
                                if (mStripMode != 4) {
                                    mCharCursor = 0;
                                } else {
                                    mCharCursor = 1;
                                }
                            } else {
                                mCursorTarget = 0;
                            }
                        }
                        func_02012790(0x12e);
                    }
                } else if (keys & 0x80) {
                    if (mCursorTarget != 2) {
                        mCursorTarget += 1;
                        if (mCursorTarget == 1) {
                            if (mStripMode > 1 && data_0209caa0.file.mCharacter == 3) {
                                if (mStripMode != 4) {
                                    mCharCursor = 0;
                                } else {
                                    mCharCursor = 1;
                                }
                            } else {
                                mCursorTarget = 2;
                            }
                        }
                        func_02012790(0x12e);
                    }
                }
            }

            if (mStripMode <= 1 || data_0209caa0.file.mCharacter != 3) {
                if (mBounceDelay != 0) {
                    mBounceDelay -= 1;
                    if (mBounceDelay == 0) {
                        mPromptYOffset = 0;
                        mBounceLaunch = -0x400;
                        mBounceVelocity = mBounceLaunch;
                        mBouncePhase = 1;
                    }
                } else {
                    if (!(mBouncePhase & 2)) {
                        mBounceVelocity += 0x100;
                        if (mBounceVelocity >= 0) {
                            mBouncePhase = 1;
                        }
                    } else {
                        mBounceVelocity -= 0x100;
                        if (mBounceVelocity >= 0) {
                            mBouncePhase = 3;
                        }
                    }
                    mPromptYOffset += mBounceVelocity;
                    if (mBouncePhase == 1 && mPromptYOffset >= 0) {
                        mBouncePhase = 2;
                    } else if (mBouncePhase == 3 && mPromptYOffset <= 0) {
                        mBounceLaunch += 0x80;
                        if (mBounceLaunch >= -0x180) {
                            mBounceDelay = 0x78;
                            mPromptYOffset = 0;
                        } else {
                            mBounceVelocity = mBounceLaunch;
                            mBouncePhase = 1;
                        }
                    }
                }
            }
        }
    }
    DecIfAbove0_Short(&mPlateRepeatTimer);
    return 1;
}

// ROM 0x020af86c, size 0x34; vtable slot 3.
s32 dScStarSel_c::CleanupResources() {
    CleanCommonModelDataArr();
    Sound::UnsetPlayerVoiceGroup();
    GX_SetDispSelect(GX_DISP_SELECT_MAIN_SUB);
    return 1;
}

/* Runtime asset handles resolved through the overlay 0 file table. */
enum ZN12dScStarSel_c13InitResourcesEv_AssetId {
    ASSET_D_2D_BTM_ICON_NCG_BIN = 0x0229,
    ASSET_D_2D_BTM_ICON_NCL_BIN = 0x022a,
    ASSET_D_2D_PLAYER_SELECT_KOOPA_BG_NCG_BIN = 0x023f,
    ASSET_D_2D_PLAYER_SELECT_KOOPA_BG_NCL_BIN = 0x0240,
    ASSET_D_2D_PLAYER_SELECT_KOOPA_BG_NSC_BIN = 0x0241,
    ASSET_D_2D_PLAYER_SELECT_LUIGI_8BIT_NCG_BIN = 0x0242,
    ASSET_D_2D_PLAYER_SELECT_LUIGI_8BIT_NCL_BIN = 0x0243,
    ASSET_D_2D_PLAYER_SELECT_MARIO_8BIT_NCG_BIN = 0x0244,
    ASSET_D_2D_PLAYER_SELECT_MARIO_8BIT_NCL_BIN = 0x0245,
    ASSET_D_2D_PLAYER_SELECT_WARIO_8BIT_NCG_BIN = 0x0246,
    ASSET_D_2D_PLAYER_SELECT_WARIO_8BIT_NCL_BIN = 0x0247,
    ASSET_D_2D_PLAYER_SELECT_YOSHI_8BIT_NCG_BIN = 0x0248,
    ASSET_D_2D_PLAYER_SELECT_YOSHI_8BIT_NCL_BIN = 0x0249,
    ASSET_D_2D_SCENARIO_PLAYER_SELECT_COURSE_NUMBER_BG_NCG_BIN = 0x024a,
    ASSET_D_2D_SCENARIO_PLAYER_SELECT_COURSE_NUMBER_BG_NCL_BIN = 0x024b,
    ASSET_D_2D_SCENARIO_PLAYER_SELECT_COURSE_NUMBER_BG_NSC_BIN = 0x024c,
    ASSET_D_2D_SCENARIO_PLAYER_SELECT_NCG_BIN = 0x024d,
    ASSET_D_2D_SCENARIO_PLAYER_SELECT_NCL_BIN = 0x024e,
    ASSET_D_2D_SCENARIO_PLAYER_SELECT_COURSE_NUMBER_BG_F_NCG_BIN = 0x025f,
    ASSET_D_2D_SCENARIO_PLAYER_SELECT_COURSE_NUMBER_BG_G_NCG_BIN = 0x0260,
    ASSET_D_2D_SCENARIO_PLAYER_SELECT_COURSE_NUMBER_BG_I_NCG_BIN = 0x0261,
    ASSET_D_2D_SCENARIO_PLAYER_SELECT_COURSE_NUMBER_BG_S_NCG_BIN = 0x0262,
    ASSET_D_2D_SCENARIO_PLAYER_SELECT_F_NCG_BIN = 0x0263,
    ASSET_D_2D_SCENARIO_PLAYER_SELECT_G_NCG_BIN = 0x0264,
    ASSET_D_2D_SCENARIO_PLAYER_SELECT_I_NCG_BIN = 0x0265,
    ASSET_D_2D_SCENARIO_PLAYER_SELECT_S_NCG_BIN = 0x0266,
    ASSET_D_2D_CMN_ICON_E_NCG_BIN = 0x0269,
    ASSET_D_2D_CMN_ICON_F_NCG_BIN = 0x026a,
    ASSET_D_2D_CMN_ICON_G_NCG_BIN = 0x026c,
    ASSET_D_2D_CMN_ICON_I_NCG_BIN = 0x026e,
    ASSET_D_2D_CMN_ICON_S_NCG_BIN = 0x0270
};

/* recovered: real C++ method over include/dScStarSel_c.h -- vtable slot 0.
 *
 * Loads the star-select screen: sound group, VRAM banks, the two dExtSimpleModel_c members
 * (models[0]/models[1]), the per-language BG/OBJ graphics, the per-character
 * palette set, then derives the star row (mPlateMask, mSelectedPlate,
 * mPlateCount, mPlateX) and the character row (mCharX/mCharY/mCharSprite,
 * mStripMode).
 *
 * The star x positions are a u8 running value: the row is centred, so it starts
 * at 0x80 (odd count) or 0x92 (even count) minus half the count times the 0x23
 * pitch, and each star adds one pitch. Written as one u8 with if/else constant
 * arms, which is what keeps the value in the register the ternary lands in (r3)
 * and lets the loop's +0x23 temp take a fresh r1. */

extern "C" void Enable3dEngines(void);
extern "C" void *LoadFile(int);
extern "C" void DecompressLZ16(void *, void *);
extern "C" int IsStarCollected(int, int);

extern u8 gMainScreenLayers;
extern u8 gSubScreenLayers;

s32 dScStarSel_c::InitResources() {
    void *file;
    s32 starNo;
    u8 collected;
    s32 anyMissing;
    u8 plate;
    u32 totalStars;
    u8 level;
    u8 count;
    u8 plateX;
    s32 i;
    s32 chr;
    u8 unlocked;
    s32 row;
    u8 charY;
    u8 charSprite;
    s32 placed;
    s32 stepIdx;
    s8 *yStep;
    s32 nextChr;
    u8 charX;

    LoadTextNarcs();
    LoadArchive(0);
    Enable3dEngines();
    Sound::LoadInitialGroup(3);
    if (gNextSublevel == 0x24 || gNextSublevel == 0x26 || gNextSublevel == 0x28) {
        Sound::LoadAndSetMusic_Layer1(0x24);
        func_0201277c(0xB8);
    } else {
        Sound::LoadAndSetMusic_Layer1(0x16);
    }
    GX_SetDispSelect(GX_DISP_SELECT_MAIN_SUB);
    ::Initialise3dGraphics(0);
    G3X_SetClearColor(0x7FFF, 0x1F, 0x7FFF, 0x3F, false);
    GX::DisableAllBanks();
    GX_SetBankForTex(GX_VRAM_TEX_01_AB);
    GX_SetBankForTexPltt(GX_VRAM_TEXPLTT_01234_EF);
    GX_SetBankForBG(GX_VRAM_BG_128_C);
    GX_SetBankForOBJ(GX_VRAM_OBJ_16_G);
    GX_SetBankForSubBG(GX_VRAM_SUB_BG_32_H);
    GX_SetBankForSubOBJ(GX_VRAM_SUB_OBJ_128_D);
    GX_SetBankForSubOBJExtPltt(GX_VRAM_SUB_OBJEXTPLTT_0_I);
    GX_SetOBJVRamModeChar(GX_OBJVRAMMODE_CHAR_2D);
    GXS_SetOBJVRamModeChar(GX_OBJVRAMMODE_CHAR_2D);
    GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BG0_AS_3D);
    GXS_SetGraphicsMode(GX_BGMODE_0);
    GX_SetBGScrOffset(1);
    GX_SetPower(GX_POWER_ALL);
    GX_DispOn();
    GXS_DispOn();
    G3X_SetFog(false, 0, 2, 0x1000);
    InitialiseVramGlobals();
    models[0].LoadAndSetFile(0x8015, 1, -1);
    models[1].LoadAndSetFile(0x8017, 1, -1);
    G2_SetBG0Priority(2);
    gMainScreenLayers = DISPLAY_LAYER_BG0 | DISPLAY_LAYER_BG3 | DISPLAY_LAYER_OBJ;
    G2_SetBG3Priority(0);
    G2_SetBG3ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0xb800,
                         GX_BG_CHARBASE_0x00000);
    G2_BG3Mosaic(0);
    LoadFont(0);
    if (SublevelToLevel(gNextSublevel) >= 0xF) {
        G2_SetBG2Priority(1);
        G2_SetBG2ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0xb000,
                             GX_BG_CHARBASE_0x10000);
        G2_BG2Mosaic(0);
        file = LoadFile(ASSET_D_2D_PLAYER_SELECT_KOOPA_BG_NCG_BIN);
        DecompressLZ16(file, G2_GetBG2CharPtr());
        Deallocate(file);
        file = LoadFile(ASSET_D_2D_PLAYER_SELECT_KOOPA_BG_NCL_BIN);
        GX_LoadBGPltt(file, 0, 0x1A0);
        GXS_LoadBGPltt(file, 0, 2);
        Deallocate(file);
        file = LoadFile(ASSET_D_2D_PLAYER_SELECT_KOOPA_BG_NSC_BIN);
        DecompressLZ16(file, G2_GetBG2ScrPtr());
        Deallocate(file);
        SetBg2Offset(0, 0);
        gMainScreenLayers |= DISPLAY_LAYER_BG2;
        gMainScreenLayers &= ~DISPLAY_LAYER_BG0;
    } else {
        G2_SetBG2Priority(1);
        G2_SetBG2ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0xb000,
                             GX_BG_CHARBASE_0x10000);
        G2_BG2Mosaic(0);
        if (GetOwnerLanguage() == 5) {
            file = LoadFile(ASSET_D_2D_SCENARIO_PLAYER_SELECT_COURSE_NUMBER_BG_S_NCG_BIN);
        } else if (GetOwnerLanguage() == 4) {
            file = LoadFile(ASSET_D_2D_SCENARIO_PLAYER_SELECT_COURSE_NUMBER_BG_I_NCG_BIN);
        } else if (GetOwnerLanguage() == 3) {
            file = LoadFile(ASSET_D_2D_SCENARIO_PLAYER_SELECT_COURSE_NUMBER_BG_G_NCG_BIN);
        } else if (GetOwnerLanguage() == 2) {
            file = LoadFile(ASSET_D_2D_SCENARIO_PLAYER_SELECT_COURSE_NUMBER_BG_F_NCG_BIN);
        } else {
            file = LoadFile(ASSET_D_2D_SCENARIO_PLAYER_SELECT_COURSE_NUMBER_BG_NCG_BIN);
        }
        DecompressLZ16(file, G2_GetBG2CharPtr());
        Deallocate(file);
        file = LoadFile(ASSET_D_2D_SCENARIO_PLAYER_SELECT_COURSE_NUMBER_BG_NCL_BIN);
        GX_LoadBGPltt(file, 0, 0x1A0);
        Deallocate(file);
        file = LoadFile(ASSET_D_2D_SCENARIO_PLAYER_SELECT_COURSE_NUMBER_BG_NSC_BIN);
        DecompressLZ16(file, G2_GetBG2ScrPtr());
        Deallocate(file);
        SetBg2Offset(0, -4);
        gMainScreenLayers |= DISPLAY_LAYER_BG2;
        GXS_LoadBGPltt(&sStarSelectSubBackdropColor, 0, 2);
    }
    if (GetOwnerLanguage() == 5) {
        file = LoadFile(ASSET_D_2D_CMN_ICON_S_NCG_BIN);
    } else if (GetOwnerLanguage() == 4) {
        file = LoadFile(ASSET_D_2D_CMN_ICON_I_NCG_BIN);
    } else if (GetOwnerLanguage() == 3) {
        file = LoadFile(ASSET_D_2D_CMN_ICON_G_NCG_BIN);
    } else if (GetOwnerLanguage() == 2) {
        file = LoadFile(ASSET_D_2D_CMN_ICON_F_NCG_BIN);
    } else {
        file = LoadFile(ASSET_D_2D_CMN_ICON_E_NCG_BIN);
    }
    DecompressLZ16(file, G2_GetOBJCharPtr());
    Deallocate(file);
    file = LoadFile(ASSET_D_2D_BTM_ICON_NCG_BIN);
    DecompressLZ16(file, (u8 *)G2_GetOBJCharPtr() + 0x2000);
    Deallocate(file);
    if (GetOwnerLanguage() == 5) {
        file = LoadFile(0xB00A);
    } else if (GetOwnerLanguage() == 4) {
        file = LoadFile(0xAC0A);
    } else if (GetOwnerLanguage() == 3) {
        file = LoadFile(0xA80A);
    } else if (GetOwnerLanguage() == 2) {
        file = LoadFile(0xA40A);
    } else {
        file = LoadFile(0xA00A);
    }
    GX_LoadOBJPltt(file, 0, 0x120);
    Deallocate(file);
    file = LoadFile(ASSET_D_2D_BTM_ICON_NCL_BIN);
    GX_LoadOBJPltt(file, 0x120, 0xE0);
    Deallocate(file);
    if (GetOwnerLanguage() == 5) {
        file = LoadFile(ASSET_D_2D_SCENARIO_PLAYER_SELECT_S_NCG_BIN);
    } else if (GetOwnerLanguage() == 4) {
        file = LoadFile(ASSET_D_2D_SCENARIO_PLAYER_SELECT_I_NCG_BIN);
    } else if (GetOwnerLanguage() == 3) {
        file = LoadFile(ASSET_D_2D_SCENARIO_PLAYER_SELECT_G_NCG_BIN);
    } else if (GetOwnerLanguage() == 2) {
        file = LoadFile(ASSET_D_2D_SCENARIO_PLAYER_SELECT_F_NCG_BIN);
    } else {
        file = LoadFile(ASSET_D_2D_SCENARIO_PLAYER_SELECT_NCG_BIN);
    }
    DecompressLZ16(file, G2S_GetOBJCharPtr());
    Deallocate(file);
    file = LoadFile(ASSET_D_2D_SCENARIO_PLAYER_SELECT_NCL_BIN);
    GXS_LoadOBJPltt(file, 0, 0x200);
    Deallocate(file);
    data_02092128[0] = data_0209caa0.file.mCharacter;
    switch (data_0209caa0.file.mCharacter) {
    case 0:
        file = LoadFile(ASSET_D_2D_PLAYER_SELECT_MARIO_8BIT_NCG_BIN);
        DecompressLZ16(file, (u8 *)G2S_GetOBJCharPtr() + 0x4000);
        Deallocate(file);
        file = LoadFile(ASSET_D_2D_PLAYER_SELECT_MARIO_8BIT_NCL_BIN);
        GXS_BeginLoadOBJExtPltt();
        GXS_LoadOBJExtPltt(file, 0, 0x200);
        GXS_EndLoadOBJExtPltt();
        Deallocate(file);
        break;
    case 1:
        file = LoadFile(ASSET_D_2D_PLAYER_SELECT_LUIGI_8BIT_NCG_BIN);
        DecompressLZ16(file, (u8 *)G2S_GetOBJCharPtr() + 0x4000);
        Deallocate(file);
        file = LoadFile(ASSET_D_2D_PLAYER_SELECT_LUIGI_8BIT_NCL_BIN);
        GXS_BeginLoadOBJExtPltt();
        GXS_LoadOBJExtPltt(file, 0, 0x200);
        GXS_EndLoadOBJExtPltt();
        Deallocate(file);
        break;
    case 2:
        file = LoadFile(ASSET_D_2D_PLAYER_SELECT_WARIO_8BIT_NCG_BIN);
        DecompressLZ16(file, (u8 *)G2S_GetOBJCharPtr() + 0x4000);
        Deallocate(file);
        file = LoadFile(ASSET_D_2D_PLAYER_SELECT_WARIO_8BIT_NCL_BIN);
        GXS_BeginLoadOBJExtPltt();
        GXS_LoadOBJExtPltt(file, 0, 0x200);
        GXS_EndLoadOBJExtPltt();
        Deallocate(file);
        break;
    case 3:
        file = LoadFile(ASSET_D_2D_PLAYER_SELECT_YOSHI_8BIT_NCG_BIN);
        DecompressLZ16(file, (u8 *)G2S_GetOBJCharPtr() + 0x4000);
        Deallocate(file);
        file = LoadFile(ASSET_D_2D_PLAYER_SELECT_YOSHI_8BIT_NCL_BIN);
        GXS_BeginLoadOBJExtPltt();
        GXS_LoadOBJExtPltt(file, 0, 0x200);
        GXS_EndLoadOBJExtPltt();
        Deallocate(file);
        break;
    }

    level = (u8)SublevelToLevel(gNextSublevel);
    if ((u32)level <= 0xE) {
        collected = 0;
        anyMissing = 0;
        plate = 0;
        totalStars = CountStarsCollectedInLevelToDisplay(level);
        if (IsStarCollected(level, 0) != 0) {
            totalStars = (u32)(u8)(totalStars - 1);
        }
        mSelectedPlate = 0xFF;
        mPlateMask = 0;
        starNo = 1;
        if ((u32)collected < totalStars) {
            do {
                if (IsStarCollected(level, starNo) != 0) {
                    mPlateMask |= (u8)(1 << plate);
                    collected += 1;
                } else {
                    if (mSelectedPlate == 0xFF) {
                        mSelectedPlate = plate;
                        mPlateMask |= (u8)(1 << plate);
                    }
                    anyMissing = 1;
                }
                plate += 1;
                starNo += 1;
            } while ((u32)collected < totalStars);
        }
        mPlateCount = plate;
        if ((anyMissing == 0) && (totalStars != 7)) {
            mPlateCount += 1;
            if (mSelectedPlate == 0xFF) {
                mSelectedPlate = plate;
                mPlateMask |= (u8)(1 << plate);
            }
        }
        if (mSelectedPlate == 0xFF) {
            mSelectedPlate = 0;
        }
        count = mPlateCount;
        if (count & 1)
            plateX = 0x80;
        else
            plateX = 0x92;
        plateX -= ((s32)count >> 1) * 0x23;
        for (i = 0; i < (s32)mPlateCount; i++) {
            mPlateX[i] = plateX;
            plateX += 0x23;
        }
    }

    if (SublevelToLevel(gNextSublevel) <= 0x14) {
        Message::DisplayCourseNameForStarSelect((u32)(s16)SublevelToLevel(gNextSublevel));
        if (SublevelToLevel(gNextSublevel) <= 0xE) {
            Message::DisplayStarNameForStarSelect(mSelectedPlate);
        }
        SetBg3Offset(0, -4);
    } else {
        gMainScreenLayers &= ~DISPLAY_LAYER_BG3;
    }

    chr = 0;
    mStripMode = 0;
    do {
        if (SaveData::IsCharacterUnlocked(chr) != 0) {
            mStripMode++;
        }
        chr += 1;
    } while (chr < 4);

    if (SublevelToLevel(gNextSublevel) <= 0xE) {
        if ((mStripMode != 1) && (data_0209caa0.file.mCharacter == 3)) {
            mFaceY = 0x88;
            mBounceDelay = 0;
        } else {
            mFaceY = 0x60;
            mBounceDelay = 0;
        }
        unlocked = mStripMode;
        row = unlocked - 1;
        charX = sStarSelectCharXStart[row];
        charY = sStarSelectCharYStart[row];
        if ((u32)unlocked > 1U) {
            placed = 0;
            nextChr = 0;
            charSprite = sStarSelectCharSpriteStart[row];
            stepIdx = (unlocked - 2) * 4;
            if (row > 0) {
                yStep = &sStarSelectCharYStep[stepIdx];
                do {
                    if (SaveData::IsCharacterUnlocked(nextChr) != 0) {
                        mCharX[nextChr] = charX;
                        mCharY[nextChr] = charY;
                        mCharSprite[nextChr] = charSprite;
                        placed += 1;
                        charX = (u8)(charX + sStarSelectCharXStep[stepIdx]);
                        charY = (u8)(charY + *yStep);
                        charSprite = (u8)(charSprite + 1);
                        yStep += 1;
                        stepIdx += 1;
                    }
                    nextChr += 1;
                } while (placed < (s32)(mStripMode - 1));
            }
        }
    }

    mPickedChar = 0xFF;
    mCursorShown = 0;
    mCursorTarget = 0;
    mFaceScaleX = 0x1000;
    mFaceScaleY = 0x1000;
    mIdleIconScale = 0x1000;
    mFaceAngle = 0;
    mAnimMode = 0;
    mFaceAnimStep = 0;
    mFaceAngle = 0;
    mBounceDelay = 0x78;
    mAnimFlip = 0;
    gSubScreenLayers = 0x10;
    GX_SetVisiblePlane(gMainScreenLayers);
    REG_DISPCNT_SUB = (REG_DISPCNT_SUB & ~0x1f00) | 0x1000;
    mLifeIcon = data_0209caa0.file.mCharacter;
    data_0208ee44 = 1;
    return 1;
}

/* Reconstructed source-style name: SM64DS proves dScStarSel_c through RTTI,
 * allocation size, vtable identity, and the STAR_SELECT registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: StarSelect_Spawn. */
dScStarSel_c *dScStarSel_c_classInit(void) {
    return new dScStarSel_c;
}

/* Sub-screen BG palette entry 0, the backdrop colour (0x7fff, white):
   InitResources loads these two bytes with GXS_LoadBGPltt(.., 0, 2). */
u16 sStarSelectSubBackdropColor = 32767;
/* The inverse lookup visits three entries; mode 4 bounds the cursor to 0..2. */
u8 sStarSelectCharacterOrder[3] = {1, 0, 2};
/* The character strip's layout, indexed by the unlocked count minus one.
   With two or more characters unlocked, InitResources gives the first of
   them its mCharY, mCharSprite and mCharX from these three tables. */
u8 sStarSelectCharYStart[4] = {0, 72, 72, 72};
u8 sStarSelectCharSpriteStart[4] = {0, 0, 1, 0};
u8 sStarSelectCharXStart[4] = {0, 128, 92, 128};
/* Divisors for the hundreds, tens and units digits. */
u16 sStarSelectDecimalDivisors[3] = {100, 10, 1};
/* Each later unlocked character's mCharY and mCharX step, four entries per
   unlocked count of two, three and four. Four-slot strides are observed;
   unused slots do not prove original bounds. */
s8 sStarSelectCharYStep[12] = {0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, 0};
s16 sStarSelectCharXStep[12] = {0, 0, 0, 0, 72, 0, 0, 0, -68, 136, 0, 0};
BaseSpawnInfo<dScStarSel_c> g_profile_STAR_SELECT = {dScStarSel_c_classInit, 4, 7};
