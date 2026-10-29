//cpp
/* leftovers:
 * - defer_codegen off stays, and the file stays in ROM order.
 *   Reordering moves the run.
 * - The destructor is written so D1 lands at 0x020addfc and D0 at
 *   0x020ade54. The extra D2 is unreferenced and dropped at link.
 *   It is the key function, so this file emits the vtable. The
 *   native vtable, typeinfo and profile now own their complete data range.
 * - opt_strength_reduction off is file-global. Without it Render
 *   grows by 0x10 bytes.
 * - common.h stays before dScStarSel_c.h. The other order grows
 *   Render by 0x18 bytes.
 * - opt_loop_invariants off binds InitResources alone, between a
 *   push and a pop. Without it the star-row loop hoists its +0x23 step.
 * - The seven helpers stay func_ov003_*. Every caller is in this
 *   class, and the bodies only touch this object's fields.
 * - data_020a0e5a stays u16[][2]. The flat u16[i*2] form costs
 *   func_ov003_020ae358 seventeen words.
 * - The bytes after the dExtSimpleModel_c pair stay offset macros. The header
 *   still holds that tail as opaque storage.
 *
 * dScStarSel_c, the star-select screen. ov003 .text
 * 0x020addfc..0x020b0580. Base is dScene_c.
 */
#pragma defer_codegen off
#pragma opt_strength_reduction off
#include "common.h"
#include "SpawnInfo.h"
#include "dScStarSel_c.h"
#include "decl_common.h"
#include "dFdBrightness_c.h"
#include "OAM.h"
#include "SaveData.h"
#include "Message.h"
#include "Sound.h"
#include "G3X.h"
#include "G3i.h"
#include "G2.h"
#include "GX.h"

/* Raw this-relative accessors, and the cosine table lookup used by Render. */
#define FB(p, o) (*(u8 *)((u8 *)(p) + (o)))
#define FH(p, o) (*(s16 *)((u8 *)(p) + (o)))
#define FW(p, o) (*(s32 *)((u8 *)(p) + (o)))
#define FU(p, o) (*(u16 *)((u8 *)(p) + (o)))
#define COS(a) data_02082214[((u16)(a) >> 4) * 2 + 1]

/* Named views onto the storage between the dExtSimpleModel_c pair and the end of the
 * object. Fixed-point values are 20.12 (0x1000 = 1.0), as the OAM::Render
 * scale arguments and the 0x1000 steps below show. */
#define SCALE_X(p)        FW(p, 0x50)   /* scale of the panel sprite at (0x80, PANEL_Y) */
#define SCALE_Y(p)        FW(p, 0x54)
#define ICON_SCALE_X(p)   FW(p, 0x58)   /* scale of the icon under CHAR_CURSOR */
#define ICON_SCALE_Y(p)   FW(p, 0x5c)
#define IDLE_ICON_SCALE(p) FW(p, 0x60)  /* scale of the other character icons */
/* D-pad auto-repeat on the star plates: Behavior reloads it with 0x10 on a
   press and 8 while held, and counts it down every frame. */
#define PLATE_REPEAT_TIMER(p) FU(p, 0x104)
#define REPEAT_TIMER(p)   (*(u16 *)((u8 *)(p) + 0x106))   /* D-pad auto-repeat */
/* The bottom sprite's bounce in Behavior: BOUNCE_DELAY frames (0x78) pass,
   then BOUNCE_VELOCITY is launched at BOUNCE_LAUNCH (first -0x400) and
   steps BOTTOM_Y_OFFSET until it lands; each phase-3 landing raises BOUNCE_LAUNCH
   by 0x80 until it reaches -0x180 and the delay restarts. BOUNCE_PHASE is
   1 for the arc above the rest line, 2 or 3 for the rebound below it. */
#define BOUNCE_DELAY(p)    FU(p, 0x108)
#define BOUNCE_VELOCITY(p) FH(p, 0x10c)
#define BOUNCE_LAUNCH(p)   FH(p, 0x10e)
#define BOUNCE_PHASE(p)    FB(p, 0x136)
#define BOTTOM_Y_OFFSET(p) FH(p, 0x10a) /* 8.8 offset of the bottom sprite's y */
#define SPIN_ANGLE(p)     FH(p, 0x110)
#define PULSE_ANGLE(p)    FH(p, 0x112)
#define PLATE_COUNT(p)    FB(p, 0x114)  /* star plates drawn by Render */
#define SELECTED_PLATE(p) FB(p, 0x115)
#define LIFE_ICON(p)      FB(p, 0x116)  /* index into OAM::LIFE_ICONS */
/* Frames the picked character's icon is drawn pressed (Render's pressY). */
#define PICK_PRESS_TIMER(p) FB(p, 0x118)
/* Set to 0x10 when a choice is made; Behavior counts it down and fades the
   scene out at zero. */
#define EXIT_DELAY(p)     FB(p, 0x119)
#define PLATE_X(p, i)     FB((u8 *)(p) + (i), 0x11a)
#define DIGIT(p, i)       (*(s8 *)((u8 *)(p) + (i) + 0x121))  /* 3 digits, -1 = blank */
#define CHAR_X(p, i)      FB((u8 *)(p) + (i), 0x124)          /* per character */
#define CHAR_Y(p, i)      FB((u8 *)(p) + (i), 0x128)
#define PANEL_Y(p)        FB(p, 0x12b)
#define CHAR_SPRITE(p, i) FB((u8 *)(p) + (i), 0x12c)
#define STRIP_MODE(p)     FB(p, 0x130)  /* strip slots + 1; 4 = fixed order from sStarSelectCharacterOrder */
#define PLATE_MASK(p)     FB(p, 0x131)  /* bit i: extra sprite behind plate i */
#define PICKED_CHAR(p)    FB(p, 0x132)
#define CURSOR_MODE(p)    FB(p, 0x133)  /* 0 plates, 1 character strip, else fixed */
#define CHAR_CURSOR(p)    FB(p, 0x134)  /* slot within the character strip */
#define CURSOR_ON(p)      FB(p, 0x135)
#define ANIM_MODE(p)      FB(p, 0x139)  /* 1 or 2 selects the animation below */
#define ANIM_FLIP(p)      FB(p, 0x13a)
/* Step of the panel's ANIM_MODE 1 grow, shrink, wait and spin sequence. */
#define PANEL_ANIM_STEP(p) FB(p, 0x137)

extern "C" {
extern unsigned char sStarSelectCharacterOrder[];
extern unsigned short sStarSelectDecimalDivisors[];
extern unsigned char NumStars(void);
// local extern: this file needs a record-view spelling of one of the touch lanes (the ROM scales the slot in the addressing mode), which conflicts with PlayerInput.h; the header is not included and all five symbols are declared here.
extern unsigned char gActivePlayerSlot;
// local extern: see above.
extern unsigned char gTouchHeld[][4];
// local extern: see above.
extern unsigned char gTouchEdge[][4];
// local extern: see above.
extern unsigned char gTouchX[][4];
// local extern: see above.
extern unsigned char gTouchY[][4];
extern int data_0208ee44;
extern unsigned short data_020a0e5a[][2];
extern u16 data_020a0e58[];
int IsStarCollectedInLevel(s8 levelID, s32 starID);
void OamRender10_d_s_star_sel_methods(int, void *, int, int, int, int, int, int, int, int);
void _Z13CopyToViewMatPK9Matrix4x3(const Matrix4x3 *);
void Matrix4x3_FromTranslation(Matrix4x3 *, s32, s32, s32);
void Matrix4x3_ApplyInPlaceToRotationY(Matrix4x3 *, s16);
void Matrix4x3_ApplyInPlaceToScale(Matrix4x3 *, s32, s32, s32);
extern s8 data_02092110;
extern s16 data_02082214[];
extern s32 data_020a0db0;
extern u8 data_0209caa0[0x50];
extern Matrix4x3 data_020a0e68;
extern u8 data_02092128;
extern signed char data_0209f2f4[];
extern u8 data_ov001_020ab938;
extern u8 data_ov001_020ab940;
extern void *data_ov001_020abb18[];
extern u8 data_ov001_020abd78;
extern u8 data_ov001_020abd80;
extern u8 data_ov001_020abb34;
extern u8 data_ov001_020abb54;
extern u8 data_ov001_020abb74;
extern u8 data_ov001_020abb94;
extern void *data_ov001_020abcb4[];
extern u8 data_ov001_020abbb4;
extern u8 data_ov001_020abbf4;
/* SaveData::IsCharacterUnlocked was declared three ways across the legacy
 * shards -- `int (unsigned int)`, the same with the parameter named, and
 * `u32 (u32)`. u32 IS unsigned int here, so the parameter type never
 * actually differed; only the return spelling did, and every call site in
 * this file consumes it as `!= 0`. The declaration in SaveData.h is the one
 * they all already held. */
}

// @symbol _ZN12dScStarSel_cD1Ev
// ROM 0x020addfc, size 0x58; vtable slot 16. Written first so D1 and D0 are
// emitted first, as in the cartridge. The body is empty: tearing down the
// dExtSimpleModel_c pair (__cxa_vec_cleanup) and the base destructors is all
// compiler-generated.
dScStarSel_c::~dScStarSel_c() {}

// @symbol _ZN12dScStarSel_cD0Ev
// ROM 0x020ade54, size 0x6c; vtable slot 17. The deleting destructor has no
// source of its own: mwccarm emits it from the definition above, directly
// after D1, ending with dScene_c's inline operator delete.

// @symbol _ZN12dScStarSel_c19func_ov003_020adec0Ej
// ROM 0x020adec0, size 0x90. The inverse of func_ov003_020adf50: maps a
// character index to its strip slot, or 0 when it has none.
int dScStarSel_c::func_ov003_020adec0(unsigned int chr) {
    unsigned char mode = STRIP_MODE(this);
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

// @symbol _ZN12dScStarSel_c19func_ov003_020adf50Ev
// ROM 0x020adf50, size 0x78. Maps the strip slot CHAR_CURSOR to a character
// index: through sStarSelectCharacterOrder when STRIP_MODE is 4, otherwise the
// CHAR_CURSOR-th unlocked character when STRIP_MODE is 2 or more; else 0.
int dScStarSel_c::func_ov003_020adf50() {
    unsigned char mode = STRIP_MODE(this);
    if (mode == 4) {
        return sStarSelectCharacterOrder[CHAR_CURSOR(this)];
    }
    if (mode >= 2) {
        int slot = 0;
        int chr = 0;
        for (; chr < 3; chr++) {
            if (SaveData::IsCharacterUnlocked((unsigned int)chr) != 0) {
                if (slot == CHAR_CURSOR(this)) {
                    return chr;
                }
                slot++;
            }
        }
    }
    return 0;
}

// @symbol _ZN12dScStarSel_c19func_ov003_020adfc8Ev
// ROM 0x020adfc8, size 0xe8. Draws the coin record of the current level
// (SaveData::GetCoinRecord) as three digits drawn right to left, then the
// TIMES and COIN sprites.
void dScStarSel_c::func_ov003_020adfc8() {
    int x = 0xb8;
    int lvl = SublevelToLevel(data_02092110);
    int coin = SaveData::GetCoinRecord(lvl);
    func_ov003_020ae1a4(coin);
    int i;
    for (i = 2; i >= 0; i--) {
        signed char d = DIGIT(this, i);
        if (d >= 0) {
            OAM::Render(0, OAM::NUMBERS[d], x, 0x4c, 8, -1, 0);
            x -= 9;
        }
    }
    OAM::Render(0, &OAM::TIMES, x, 0x54, -1, -1, 0);
    OAM::Render(0, &OAM::COIN, x - 0x10, 0x4c, -1, -1, 0);
}

// @symbol _ZN12dScStarSel_c19func_ov003_020ae0b0Ev
// ROM 0x020ae0b0, size 0xf4. Draws the power-star count: NumStars() as three
// digits, drawn right to left, then the TIMES and POWER_STAR sprites.
void dScStarSel_c::func_ov003_020ae0b0() {
    int x;
    int y;
    if (SublevelToLevel(data_02092110) >= 0xf) {
        y = 0xa0;
        x = 0xb8;
    } else {
        y = 0xac;
        x = 0xf4;
    }
    func_ov003_020ae1a4(NumStars());
    {
        int i = 2;
        do {
            signed char d = DIGIT(this, i);
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

// @symbol _ZN12dScStarSel_c19func_ov003_020ae1a4Ei
// ROM 0x020ae1a4, size 0x94. Splits value into three decimal digits in
// DIGIT(this, 0..2), dividing by sStarSelectDecimalDivisors[i] in turn; leading
// zeros are stored as -1 (blank) except the last digit.
void dScStarSel_c::func_ov003_020ae1a4(int value) {
    int found = 0;
    int one = 1;
    int blank = -1;
    int i;
    for (i = 0; i < 3; i++) {
        unsigned short place = sStarSelectDecimalDivisors[i];
        int digit = value / place;
        if (digit == 0 && found == 0 && i != 2) {
            DIGIT(this, i) = (char)blank;
        } else {
            found = one;
            DIGIT(this, i) = (char)digit;
        }
        value = (unsigned short)(value % place);
    }
}

// @symbol _ZN12dScStarSel_c19func_ov003_020ae238Ev
// ROM 0x020ae238, size 0x120. Draws the life counter: the LIFE_ICONS entry for
// LIFE_ICON, the TIMES sprite, and the three digits of data_0209f2f4[0].
void dScStarSel_c::func_ov003_020ae238() {
    int x;
    int y;
    if (SublevelToLevel(data_02092110) >= 0xf) {
        y = 0xa0;
        x = 0x50;
    } else {
        x = 0x10;
        y = 0xac;
    }
    OAM::Render(0, OAM::LIFE_ICONS[LIFE_ICON(this)], x, y + 8, -1, -1, 0x1000, 0x1000, 0, -1);
    OAM::Render(0, &OAM::TIMES, x + 0x10, y + 8, -1, -1, 0);
    func_ov003_020ae1a4((unsigned short)data_0209f2f4[0]);
    {
        int i = 0;
        x += 0x18;
        do {
            signed char d = DIGIT(this, i);
            if (d >= 0) {
                OAM::Render(0, OAM::NUMBERS[d], x, y, 8, -1, 0);
                x += 9;
            }
            i++;
        } while (i < 3);
    }
}

// @symbol _ZN12dScStarSel_c19func_ov003_020ae358Ev
// ROM 0x020ae358, size 0x398. Per-frame selection handling: record hit
// tests and the D-pad.
//
// gActivePlayerSlot selects the active record in the 4-byte-stride tables at
// gTouchHeld..gTouchY. If byte 0 of that record is set and byte 1 (gTouchEdge)
// is set, its bytes 2 and 3 are tested against a box around (0x80, PANEL_Y); a
// hit sets CURSOR_MODE 2, PICKED_CHAR 3, ANIM_MODE 1, seeds the timers at
// c+0x118/0x119 and plays sound data_0209caa0[0x41] + 0x3c.
//
// Failing that (and only with STRIP_MODE above 1 and data_0209caa0[0x41] == 3),
// the three characters are scanned in order: for each unlocked i, bytes 2 and 3
// of the same record are tested against CHAR_X/CHAR_Y, and a hit sets
// CURSOR_MODE 1, CHAR_CURSOR to its strip slot, and PICKED_CHAR, data_02092128
// and data_02092114 to the character, ANIM_MODE 2, and plays the same sound.
//
// With no record active, the d-pad half of the control word
// data_020a0e58 moves the selection: REPEAT_TIMER counts down, and while it is
// zero a held left/right steps CHAR_CURSOR within [0, STRIP_MODE - 2] and reloads
// the timer with 0x10 or 8 depending on data_020a0e5a.
//
// Codegen note: the stride belongs in the TYPE, and the INDEX is what gets named.
// The 4-byte records are declared `[][4]` so each read refolds its own scale
// (`add r2, r4, r1, lsl #2`); flattening gTouchX/gTouchY to a bare `[]` with an
// explicit `* 4` costs 17 words, and the same is true of data_020a0e5a. The last
// six words were a register transposition between the `c + i` and record-row
// address temps, and what closed it was naming the INDEX (`int ri`) for the second
// record read instead of naming the row POINTER: a named row pointer welds both
// reads onto one address temp and inverts the r2/r3 assignment, while the named
// index leaves each read to fold its own scale and hands the row temp r2.
void dScStarSel_c::func_ov003_020ae358() {
    int idx = gActivePlayerSlot;
    int valid = 0;
    int i;
    if (gTouchHeld[idx][0] != 0) {
        valid = gTouchEdge[idx][0] != 0;
    }
    if (valid != 0) {
        if ((((unsigned char)(gTouchX[idx][0] - 0x58)) < 0x50) &&
            (((unsigned char)((gTouchY[idx][0] - PANEL_Y(this)) + 0x28)) < 0x50)) {
            CURSOR_MODE(this) = 2;
            PICKED_CHAR(this) = 3;
            PICK_PRESS_TIMER(this) = (unsigned char)(data_0208ee44 * 6);
            ANIM_MODE(this) = 1;
            EXIT_DELAY(this) = 0x10;
            func_02012790(data_0209caa0[0x41] + 0x3c);
            return;
        }
        if (STRIP_MODE(this) <= 1) {
            return;
        }
        if (data_0209caa0[0x41] != 3) {
            return;
        }
        for (i = 0; i < 3; i++) {
            if (SaveData::IsCharacterUnlocked(i) != 0) {
                int ri = gActivePlayerSlot;
                if (((unsigned short)((gTouchHeld[gActivePlayerSlot][2] - CHAR_X(this, i)) +
                                      0x18)) < 0x30) {
                    if (((unsigned short)((gTouchHeld[ri][3] - CHAR_Y(this, i)) + 0x18)) < 0x2b) {
                        CURSOR_MODE(this) = 1;
                        CHAR_CURSOR(this) = (unsigned char)func_ov003_020adec0(i);
                        data_02092128 = (unsigned char)i;
                        data_02092114 = (unsigned char)i;
                        PICKED_CHAR(this) = (unsigned char)i;
                        PICK_PRESS_TIMER(this) = (unsigned char)(data_0208ee44 * 3);
                        ANIM_MODE(this) = 2;
                        EXIT_DELAY(this) = 0x10;
                        func_02012790(data_0209caa0[0x41] + 0x3c);
                        return;
                    }
                }
            }
        }

        return;
    }
    if (data_0209caa0[0x42] == 0) {
        unsigned short ctrl = data_020a0e58[0];
        if ((ctrl & 0x30) != 0) {
            unsigned short timer = REPEAT_TIMER(this);
            unsigned char next;
            if (timer != 0) {
                REPEAT_TIMER(this) -= 1;
                return;
            }
            if (CURSOR_ON(this) == 0) {
                return;
            }
            if (CURSOR_MODE(this) != 1) {
                return;
            }
            if (STRIP_MODE(this) < 3) {
                return;
            }
            next = CHAR_CURSOR(this);
            if (ctrl & 0x20) {
                if (((data_020a0e58[1] & 0x20) != 0) || (timer == 0)) {
                    REPEAT_TIMER(this) = (data_020a0e5a[idx][0] & 0x20) ? (0x10) : (8);
                    if (CHAR_CURSOR(this) != 0) {
                        next = next - 1;
                    }
                }
            } else if (ctrl & 0x10) {
                if (((data_020a0e58[1] & 0x10) != 0) || (timer == 0)) {
                    REPEAT_TIMER(this) = (data_020a0e5a[idx][0] & 0x10) ? (0x10) : (8);
                    if (CHAR_CURSOR(this) != (STRIP_MODE(this) - 2)) {
                        next = next + 1;
                    }
                }
            }
            if (next == CHAR_CURSOR(this)) {
                return;
            }
            CHAR_CURSOR(this) = next;
            func_02012790(0x12e);
            return;
        }
    }

    REPEAT_TIMER(this) = 0;
}

// @symbol _ZN12dScStarSel_c16OnPendingDestroyEv
// ROM 0x020ae6f0, size 0x4; vtable slot 12. Empty override.
void dScStarSel_c::OnPendingDestroy() {}

// @symbol _ZN12dScStarSel_c6RenderEv
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

    func_ov003_020ae238();
    func_ov003_020ae0b0();

    if (SublevelToLevel(data_02092110) < 0xf) {
        u8 levelNum;
        s32 numX;
        func_ov003_020adfc8();
        levelNum = SublevelToLevel(data_02092110) + 1;
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

    if (SublevelToLevel(data_02092110) <= 0xe) {
        G3i::PerspectiveW_(data_02082214[0x16], data_02082214[0x17], 0x1555, 0x1000, 0x1388000,
                           0x1000, 1, 0);
        G3i::LookAt_((const Vector3 *)&data_ov003_020b132c, (const Vector3 *)&data_ov003_020b1338,
                     (const Vector3 *)&data_ov003_020b1344, true, &mtx);
        _Z13CopyToViewMatPK9Matrix4x3(&mtx);

        /* The plates are 0x18000 apart, centred on x = 0, and sit at y = 0x28000. */
        x = -(((s32)PLATE_COUNT(this) - 1) * 0x18000 / 2);
        for (i = 1; i <= PLATE_COUNT(this); i++) {
            Matrix4x3_FromTranslation(&data_020a0e68, x, 0x28000, 0);
            if (i - 1 == SELECTED_PLATE(this)) {
                Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, (s16)(data_020a0db0 * 0x300));
                Matrix4x3_ApplyInPlaceToScale(&data_020a0e68, 0x1400, 0x1400, 0x1400);
            }
            idx = (IsStarCollectedInLevel(data_02092110, i) == 0) ? 1 : 0;
            plate = &models[idx];
            plate->mat4x3 = data_020a0e68;
            plate->Render(0);
            x += 0x18000;
        }

        for (i = 0; i < PLATE_COUNT(this); i++) {
            if ((PLATE_MASK(this) >> i) & 1) {
                if (i == SELECTED_PLATE(this)) {
                    OAM::RenderSub((OamAttr *)&data_ov001_020ab938, PLATE_X(this, i), 0x18);
                } else {
                    OAM::RenderSub((OamAttr *)&data_ov001_020ab940, PLATE_X(this, i), 0x18);
                }
            }
            OAM::Render(0, (OamAttr *)data_ov001_020abb18[i], PLATE_X(this, i), 8, -1, -1, 0);
        }

        if (CURSOR_ON(this) != 0) {
            if (CURSOR_MODE(this) == 0) {
                OAM::RenderSub((OamAttr *)&data_ov001_020abd78, PLATE_X(this, SELECTED_PLATE(this)),
                               6);
            } else if (CURSOR_MODE(this) == 1) {
                int slot = func_ov003_020adf50();
                OAM::RenderSub((OamAttr *)&data_ov001_020abd80, CHAR_X(this, slot) - 0x24,
                               CHAR_Y(this, slot) - 8);
            } else {
                OAM::RenderSub((OamAttr *)&data_ov001_020abd80, 0x50, PANEL_Y(this) + 8);
            }
        }

        if (ANIM_MODE(this) != 0) {
            if (ANIM_MODE(this) == 1) {
                switch (PANEL_ANIM_STEP(this)) {
                case 0:
                    SCALE_X(this) += 0x400;
                    if (SCALE_X(this) >= 0x1800) {
                        SCALE_X(this) = 0x1800;
                        PANEL_ANIM_STEP(this) = 1;
                    }
                    break;
                case 1:
                    SCALE_X(this) -= 0x400;
                    if (SCALE_X(this) <= 0x1000) {
                        SCALE_X(this) = 0x1000;
                        PANEL_ANIM_STEP(this) = 2;
                    }
                    break;
                case 2:
                    SCALE_X(this) -= 0x80;
                    if (EXIT_DELAY(this) == 0) {
                        PANEL_ANIM_STEP(this) = 3;
                    }
                    break;
                case 3:
                    SCALE_X(this) += 0x100;
                    SPIN_ANGLE(this) += 0x1000;
                    break;
                }
                SCALE_Y(this) = SCALE_X(this);
            } else if (ANIM_MODE(this) == 2) {
                ICON_SCALE_X(this) =
                    (0x1000 - COS((s16)(PULSE_ANGLE(this) - 0x4000))) * 10 + 0x1000;
                ICON_SCALE_Y(this) = 0x1000;
                PULSE_ANGLE(this) = (PULSE_ANGLE(this) + 0x1000) & 0x7fff;
                if (PULSE_ANGLE(this) == 0x4000) {
                    ANIM_FLIP(this) ^= 1;
                }
                if (IDLE_ICON_SCALE(this) != 0) {
                    IDLE_ICON_SCALE(this) += 0x1000;
                    if (IDLE_ICON_SCALE(this) >= 0x10000) {
                        IDLE_ICON_SCALE(this) = 0;
                    }
                }
            }
        }

        switch (data_0209caa0[0x41]) {
        case 0:
            OAM::Render(1, (OamAttr *)&data_ov001_020abb34, 0x80, PANEL_Y(this), -1, -1,
                        SCALE_X(this), SCALE_Y(this), (u16)SPIN_ANGLE(this), -1);
            break;
        case 1:
            OAM::Render(1, (OamAttr *)&data_ov001_020abb54, 0x80, PANEL_Y(this), -1, -1,
                        SCALE_X(this), SCALE_Y(this), (u16)SPIN_ANGLE(this), -1);
            break;
        case 2:
            OAM::Render(1, (OamAttr *)&data_ov001_020abb74, 0x80, PANEL_Y(this), -1, -1,
                        SCALE_X(this), SCALE_Y(this), (u16)SPIN_ANGLE(this), -1);
            break;
        case 3:
            OAM::Render(1, (OamAttr *)&data_ov001_020abb94, 0x80, PANEL_Y(this), -1, -1,
                        SCALE_X(this), SCALE_Y(this), (u16)SPIN_ANGLE(this), -1);
            for (i = 0; i < 3; i++) {
                pressY = 0;
                if (PICKED_CHAR(this) == i && PICK_PRESS_TIMER(this) != 0) {
                    pressY = 3;
                }
                if (SaveData::IsCharacterUnlocked(i) != 0) {
                    if (ANIM_MODE(this) == 2) {
                        if (i == func_ov003_020adf50()) {
                            if (ANIM_FLIP(this) == 0) {
                                OAM::Render(1, (OamAttr *)data_ov001_020abcb4[i + 3],
                                            CHAR_X(this, i), CHAR_Y(this, i), -1, -1,
                                            ICON_SCALE_X(this), ICON_SCALE_Y(this), 0, -1);
                                OAM::Render(1, (OamAttr *)data_ov001_020abcb4[CHAR_SPRITE(this, i)],
                                            CHAR_X(this, i), CHAR_Y(this, i), -1, -1,
                                            ICON_SCALE_X(this), ICON_SCALE_Y(this), 0, -1);
                            } else {
                                OAM::Render(1,
                                            (OamAttr *)data_ov001_020abcb4[CHAR_SPRITE(
                                                this, i ^ ((i != 0) ? 3 : 0))],
                                            CHAR_X(this, i), CHAR_Y(this, i), -1, -1,
                                            ICON_SCALE_X(this), ICON_SCALE_Y(this), 0, -1);
                            }
                        } else if (IDLE_ICON_SCALE(this) != 0) {
                            OAM::Render(1, (OamAttr *)data_ov001_020abcb4[i + 3], CHAR_X(this, i),
                                        CHAR_Y(this, i), -1, -1, IDLE_ICON_SCALE(this),
                                        IDLE_ICON_SCALE(this), 0, -1);
                            OAM::Render(1, (OamAttr *)data_ov001_020abcb4[CHAR_SPRITE(this, i)],
                                        CHAR_X(this, i), CHAR_Y(this, i), -1, -1,
                                        IDLE_ICON_SCALE(this), IDLE_ICON_SCALE(this), 0, -1);
                        }
                    } else {
                        OAM::RenderSub((OamAttr *)data_ov001_020abcb4[i + 3], CHAR_X(this, i),
                                       pressY + CHAR_Y(this, i));
                        OAM::RenderSub((OamAttr *)data_ov001_020abcb4[CHAR_SPRITE(this, i)],
                                       CHAR_X(this, i), pressY + CHAR_Y(this, i));
                    }
                }
            }
            break;
        }

        if (STRIP_MODE(this) <= 1 || data_0209caa0[0x41] != 3) {
            if (SublevelToLevel(data_02092110) <= 0xe) {
                OAM::RenderSub((OamAttr *)&data_ov001_020abbb4, 0x80,
                               (BOTTOM_Y_OFFSET(this) >> 8) + 0xa0);
            } else {
                OAM::RenderSub((OamAttr *)&data_ov001_020abbf4, 0x80,
                               (BOTTOM_Y_OFFSET(this) >> 8) + 0xa0);
            }
        }
    }
    return 1;
}

// @symbol _ZN12dScStarSel_c8BehaviorEv
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
// branch: `val` carries the cursor mode (c+0x133) in the first and the stylus-held
// flag in the second, `num` the picked character and then the plate count. The
// register allocator orders webs by where their variable is first assigned, so the
// sharing is what puts the plate count in r0 and the slot index in r2 at the loop;
// as four fresh locals they colour the other way round. The file's
// opt_strength_reduction off keeps the frame at 0x834 (with it on, r8 is pushed).

extern "C" {
u8 DecIfAbove0_Byte(u8 *p);
u16 DecIfAbove0_Short(u16 *p);

extern dFdBrightness_c *data_0209f5bc;
extern u16 data_0209f5e8[];
}

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

    if (data_0209f5bc->IsAtStart() != 0) {
        DecIfAbove0_Byte(&FB(this, 0x117));
        DecIfAbove0_Byte(&PICK_PRESS_TIMER(this));
        if (EXIT_DELAY(this) != 0 ||
            (SublevelToLevel(data_02092110) > 0xe && IsButtonInputValid() != 0)) {
            if (EXIT_DELAY(this) != 0) {
                EXIT_DELAY(this) -= data_0208ee44;
            }
            if (EXIT_DELAY(this) == 0 ||
                (SublevelToLevel(data_02092110) > 0xe && IsButtonInputValid() != 0)) {
                StartSceneFade(3, 0, 0);
                data_0209f5e8[6] = 0x7fff;
                if (SublevelToLevel(data_02092110) <= 0xe) {
                    data_0209f1f0 = SELECTED_PLATE(this) + 1;
                } else {
                    data_0209f1f0 = 1;
                }
            }
        } else if (CURSOR_ON(this) != 0 && IsButtonInputValid() != 0) {
            val = CURSOR_MODE(this);
            if (val == 0 && STRIP_MODE(this) > 1 && data_0209caa0[0x41] == 3) {
                CURSOR_MODE(this) = 2;
                func_02012790(0x12e);
            } else if (ANIM_MODE(this) == 0) {
                if (val == 1) {
                    num = func_ov003_020adf50();
                    data_02092128 = num;
                    data_02092114 = num;
                    PICKED_CHAR(this) = num;
                    PICK_PRESS_TIMER(this) = data_0208ee44 * 3;
                    ANIM_MODE(this) = 2;
                } else {
                    CURSOR_MODE(this) = 2;
                    PICKED_CHAR(this) = 3;
                    PICK_PRESS_TIMER(this) = data_0208ee44 * 6;
                    ANIM_MODE(this) = 1;
                }
                EXIT_DELAY(this) = 0x10;
                func_02012790(data_0209caa0[0x41] + 0x3c);
            }
        } else if (SublevelToLevel(data_02092110) <= 0xe) {
            idx = gActivePlayerSlot;
            cur = SELECTED_PLATE(this);
            found = 0;
            val = gTouchHeld[idx][0];
            if (val != 0) {
                num = PLATE_COUNT(this);
                i = 0;
                if (num > 0) {
                    rec = gTouchHeld[idx];
                    tx = rec[2];
                    do {
                        if ((u8)(tx - PLATE_X(this, i) + 8) < 0x10 && gTouchHeld[idx][3] < 0x28 &&
                            ((PLATE_MASK(this) >> i) & 1)) {
                            hit = (val != 0 && gTouchEdge[idx][0] != 0);
                            if (hit != 0 || cur != i) {
                                FB(this, 0x117) = data_0208ee44 * 3;
                            }
                            if (cur != i) {
                                cur = i;
                                func_02012790(0x12e);
                            }
                            CURSOR_MODE(this) = 0;
                            found = 1;
                            break;
                        }
                        i++;
                    } while (i < num);
                }
            }
            if (found == 0 && CURSOR_ON(this) == 2 && CURSOR_MODE(this) == 0 &&
                data_0209caa0[0x42] == 0) {
                if (data_020a0e58[0] & 0x20) {
                    pressed = data_020a0e58[1] & 0x20;
                    if (pressed != 0 || PLATE_REPEAT_TIMER(this) == 0) {
                        PLATE_REPEAT_TIMER(this) = pressed ? 0x10 : 8;
                        if (cur != 0) {
                            u8 mask = PLATE_MASK(this);
                            cur--;
                            while (!((mask >> cur) & 1)) {
                                cur--;
                            }
                            func_02012790(0x12e);
                        }
                    }
                } else if (data_020a0e58[0] & 0x10) {
                    pressed = data_020a0e58[1] & 0x10;
                    if (pressed != 0 || PLATE_REPEAT_TIMER(this) == 0) {
                        PLATE_REPEAT_TIMER(this) = pressed ? 0x10 : 8;
                        if (cur < PLATE_COUNT(this) - 1) {
                            u8 mask = PLATE_MASK(this);
                            cur++;
                            while (!((mask >> cur) & 1)) {
                                cur++;
                            }
                            func_02012790(0x12e);
                        }
                    }
                }
            }
            if (SELECTED_PLATE(this) != cur) {
                SELECTED_PLATE(this) = cur;
                Message::DisplayStarNameForStarSelect((s16)cur);
                FB(this, 0x117) = data_0208ee44 * 3;
            }
            if (ANIM_MODE(this) == 0) {
                func_ov003_020ae358();
            }
        }

        if (SublevelToLevel(data_02092110) <= 0xe) {
            if (CURSOR_ON(this) == 0) {
                if (IsButtonInputValid() != 0 ||
                    (data_0209caa0[0x42] == 0 && (data_020a0e5a[gActivePlayerSlot][0] & 0xf0))) {
                    func_02012790(0x12e);
                    if (data_0209caa0[0x42] == 0 && (data_020a0e58[1] & 0x30)) {
                        CURSOR_ON(this) = 1;
                    } else {
                        CURSOR_ON(this) = 2;
                    }
                    if (SublevelToLevel(data_02092110) <= 0xe) {
                        CURSOR_MODE(this) = 0;
                    } else if (STRIP_MODE(this) > 1 && data_0209caa0[0x41] == 3) {
                        CURSOR_MODE(this) = 1;
                        if (STRIP_MODE(this) != 4) {
                            CHAR_CURSOR(this) = 0;
                        } else {
                            CHAR_CURSOR(this) = 1;
                        }
                    } else {
                        CURSOR_MODE(this) = 2;
                    }
                }
            } else if (CURSOR_ON(this) == 1) {
                if (data_020a0e58[1] != 0) {
                    CURSOR_ON(this) = 2;
                }
            } else if (data_0209caa0[0x42] == 0 && (data_020a0e58[0] & 0xc0)) {
                u16 keys = data_020a0e58[1];
                if (keys & 0x40) {
                    if (CURSOR_MODE(this) != 0) {
                        CURSOR_MODE(this) -= 1;
                        if (CURSOR_MODE(this) == 1) {
                            if (STRIP_MODE(this) > 1 && data_0209caa0[0x41] == 3) {
                                if (STRIP_MODE(this) != 4) {
                                    CHAR_CURSOR(this) = 0;
                                } else {
                                    CHAR_CURSOR(this) = 1;
                                }
                            } else {
                                CURSOR_MODE(this) = 0;
                            }
                        }
                        func_02012790(0x12e);
                    }
                } else if (keys & 0x80) {
                    if (CURSOR_MODE(this) != 2) {
                        CURSOR_MODE(this) += 1;
                        if (CURSOR_MODE(this) == 1) {
                            if (STRIP_MODE(this) > 1 && data_0209caa0[0x41] == 3) {
                                if (STRIP_MODE(this) != 4) {
                                    CHAR_CURSOR(this) = 0;
                                } else {
                                    CHAR_CURSOR(this) = 1;
                                }
                            } else {
                                CURSOR_MODE(this) = 2;
                            }
                        }
                        func_02012790(0x12e);
                    }
                }
            }

            if (STRIP_MODE(this) <= 1 || data_0209caa0[0x41] != 3) {
                if (BOUNCE_DELAY(this) != 0) {
                    BOUNCE_DELAY(this) -= 1;
                    if (BOUNCE_DELAY(this) == 0) {
                        BOTTOM_Y_OFFSET(this) = 0;
                        BOUNCE_LAUNCH(this) = -0x400;
                        BOUNCE_VELOCITY(this) = BOUNCE_LAUNCH(this);
                        BOUNCE_PHASE(this) = 1;
                    }
                } else {
                    if (!(BOUNCE_PHASE(this) & 2)) {
                        BOUNCE_VELOCITY(this) += 0x100;
                        if (BOUNCE_VELOCITY(this) >= 0) {
                            BOUNCE_PHASE(this) = 1;
                        }
                    } else {
                        BOUNCE_VELOCITY(this) -= 0x100;
                        if (BOUNCE_VELOCITY(this) >= 0) {
                            BOUNCE_PHASE(this) = 3;
                        }
                    }
                    BOTTOM_Y_OFFSET(this) += BOUNCE_VELOCITY(this);
                    if (BOUNCE_PHASE(this) == 1 && BOTTOM_Y_OFFSET(this) >= 0) {
                        BOUNCE_PHASE(this) = 2;
                    } else if (BOUNCE_PHASE(this) == 3 && BOTTOM_Y_OFFSET(this) <= 0) {
                        BOUNCE_LAUNCH(this) += 0x80;
                        if (BOUNCE_LAUNCH(this) >= -0x180) {
                            BOUNCE_DELAY(this) = 0x78;
                            BOTTOM_Y_OFFSET(this) = 0;
                        } else {
                            BOUNCE_VELOCITY(this) = BOUNCE_LAUNCH(this);
                            BOUNCE_PHASE(this) = 1;
                        }
                    }
                }
            }
        }
    }
    DecIfAbove0_Short(&PLATE_REPEAT_TIMER(this));
    return 1;
}

// @symbol _ZN12dScStarSel_c16CleanupResourcesEv
// ROM 0x020af86c, size 0x34; vtable slot 3.
s32 dScStarSel_c::CleanupResources() {
    CleanCommonModelDataArr();
    Sound::UnsetPlayerVoiceGroup();
    unsigned short *p = (unsigned short *)0x4000304;
    *p = *p | 0x8000;
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

// @symbol _ZN12dScStarSel_c13InitResourcesEv
/* recovered: real C++ method over include/dScStarSel_c.h -- vtable slot 0.
 *
 * Loads the star-select screen: sound group, VRAM banks, the two dExtSimpleModel_c members
 * (models[0]/models[1]), the per-language BG/OBJ graphics, the per-character
 * palette set, then derives the star row (PLATE_MASK, SELECTED_PLATE,
 * PLATE_COUNT, PLATE_X) and the character row (CHAR_X/CHAR_Y/CHAR_SPRITE,
 * STRIP_MODE). Those fields fall inside dScStarSel_c::unk_104, which the header
 * still holds as opaque storage, so they are reached through the views above.
 *
 * The star x positions are a u8 running value: the row is centred, so it starts
 * at 0x80 (odd count) or 0x92 (even count) minus half the count times the 0x23
 * pitch, and each star adds one pitch. Written as one u8 with if/else constant
 * arms, which is what keeps the value in the register the ternary lands in (r3)
 * and lets the loop's +0x23 temp take a fresh r1. */
#pragma push
#pragma opt_loop_invariants off

extern "C" {
void Enable3dEngines(void);
void *LoadFile(int);
void DecompressLZ16(void *, u32);
int IsStarCollected(int, int);

extern u8 data_0209d45c;
extern u8 data_0209d454;
}

#define REG16(a) (*(vu16 *)(a))
#define REG32(a) (*(vu32 *)(a))

s32 dScStarSel_c::InitResources() {
    void *file;
    s32 loopI;
    u8 accB;
    s32 anyUnfilled;
    u8 bitPos;
    u32 totalStars;
    u8 level;
    s32 sp8;
    s32 spC;
    s32 varOne;
    u8 *bitsPtr;
    u8 tmpU8;
    u8 colorVal;
    s32 idx2;
    s32 charIdx;
    u8 charCount;
    s32 charCountM1;
    u8 tabSL;
    u8 tabSB;
    s32 varFp;
    s32 idx8;
    s8 *tablePtr;
    s32 varR6;
    u8 tabR5;

    LoadTextNarcs();
    LoadArchive(0);
    Enable3dEngines();
    Sound::LoadInitialGroup(3);
    if (data_02092110 == 0x24 || data_02092110 == 0x26 || data_02092110 == 0x28) {
        Sound::LoadAndSetMusic_Layer1(0x24);
        func_0201277c(0xB8);
    } else {
        Sound::LoadAndSetMusic_Layer1(0x16);
    }
    REG16(0x4000304) |= 0x8000;
    ::Initialise3dGraphics(0);
    G3X::SetClearColor(0x7FFF, 0x1F, 0x7FFF, 0x3F, false);
    GX::DisableAllBanks();
    GX::SetBankForTex(3);
    GX::SetBankForTexPltt(0x30);
    GX::SetBankForBG(4);
    GX::SetBankForOBJ(0x40);
    GX::SetBankForSubBG(0x80);
    GX::SetBankForSubOBJ(8);
    GX::SetBankForSubOBJExtPltt(0x100);
    REG32(0x4000000) &= 0xFFCFFFEF;
    REG32(0x4001000) &= 0xFFCFFFEF;
    GX::SetGraphicsMode(1, 0, 1);
    GXS::SetGraphicsMode(0);
    REG32(0x4000000) = (REG32(0x4000000) & ~0x38000000) | 0x08000000;
    REG16(0x4000304) = (REG16(0x4000304) & 0xFFFFFDF1) | 0x20E;
    GX::DispOn();
    REG32(0x4001000) |= 0x10000;
    G3X::SetFog(false, 0, 2, 0x1000);
    InitialiseVramGlobals();
    models[0].LoadAndSetFile(0x8015, 1, -1);
    models[1].LoadAndSetFile(0x8017, 1, -1);
    REG16(0x4000008) = (REG16(0x4000008) & ~3) | 2;
    data_0209d45c = 0x19;
    REG16(0x400000E) &= ~3;
    REG16(0x400000E) = (REG16(0x400000E) & 0x43) | 0x1700;
    REG16(0x400000E) &= ~0x40;
    LoadFont(0);
    if (SublevelToLevel(data_02092110) >= 0xF) {
        REG16(0x400000C) = (REG16(0x400000C) & ~3) | 1;
        REG16(0x400000C) = (REG16(0x400000C) & 0x43) | 0x1610;
        REG16(0x400000C) &= ~0x40;
        file = LoadFile(ASSET_D_2D_PLAYER_SELECT_KOOPA_BG_NCG_BIN);
        DecompressLZ16(file, (u32)G2::GetBG2CharPtr());
        Deallocate(file);
        file = LoadFile(ASSET_D_2D_PLAYER_SELECT_KOOPA_BG_NCL_BIN);
        GX::LoadBGPltt(file, 0, 0x1A0);
        GXS::LoadBGPltt(file, 0, 2);
        Deallocate(file);
        file = LoadFile(ASSET_D_2D_PLAYER_SELECT_KOOPA_BG_NSC_BIN);
        DecompressLZ16(file, (u32)G2::GetBG2ScrPtr());
        Deallocate(file);
        SetBg2Offset(0, 0);
        data_0209d45c |= 4;
        data_0209d45c &= ~1;
    } else {
        REG16(0x400000C) = (REG16(0x400000C) & ~3) | 1;
        REG16(0x400000C) = (REG16(0x400000C) & 0x43) | 0x1610;
        REG16(0x400000C) &= ~0x40;
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
        DecompressLZ16(file, (u32)G2::GetBG2CharPtr());
        Deallocate(file);
        file = LoadFile(ASSET_D_2D_SCENARIO_PLAYER_SELECT_COURSE_NUMBER_BG_NCL_BIN);
        GX::LoadBGPltt(file, 0, 0x1A0);
        Deallocate(file);
        file = LoadFile(ASSET_D_2D_SCENARIO_PLAYER_SELECT_COURSE_NUMBER_BG_NSC_BIN);
        DecompressLZ16(file, (u32)G2::GetBG2ScrPtr());
        Deallocate(file);
        SetBg2Offset(0, -4);
        data_0209d45c |= 4;
        GXS::LoadBGPltt(&data_ov003_020b1698, 0, 2);
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
    DecompressLZ16(file, 0x06400000);
    Deallocate(file);
    file = LoadFile(ASSET_D_2D_BTM_ICON_NCG_BIN);
    {
        char *a5 = (char *)0x06400000;
        a5 += 0x2000;
        DecompressLZ16(file, (u32)a5);
    }
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
    GX::LoadOBJPltt(file, 0, 0x120);
    Deallocate(file);
    file = LoadFile(ASSET_D_2D_BTM_ICON_NCL_BIN);
    GX::LoadOBJPltt(file, 0x120, 0xE0);
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
    DecompressLZ16(file, 0x06600000);
    Deallocate(file);
    file = LoadFile(ASSET_D_2D_SCENARIO_PLAYER_SELECT_NCL_BIN);
    GXS::LoadOBJPltt(file, 0, 0x200);
    Deallocate(file);
    data_02092128 = data_0209caa0[0x41];
    switch (data_0209caa0[0x41]) {
    case 0:
        file = LoadFile(ASSET_D_2D_PLAYER_SELECT_MARIO_8BIT_NCG_BIN);
        {
            char *a6 = (char *)0x06600000;
            a6 += 0x4000;
            DecompressLZ16(file, (u32)a6);
        }
        Deallocate(file);
        file = LoadFile(ASSET_D_2D_PLAYER_SELECT_MARIO_8BIT_NCL_BIN);
        GXS::BeginLoadOBJExtPltt();
        GXS::LoadOBJExtPltt(file, 0, 0x200);
        GXS::EndLoadOBJExtPltt();
        Deallocate(file);
        break;
    case 1:
        file = LoadFile(ASSET_D_2D_PLAYER_SELECT_LUIGI_8BIT_NCG_BIN);
        {
            char *a6 = (char *)0x06600000;
            a6 += 0x4000;
            DecompressLZ16(file, (u32)a6);
        }
        Deallocate(file);
        file = LoadFile(ASSET_D_2D_PLAYER_SELECT_LUIGI_8BIT_NCL_BIN);
        GXS::BeginLoadOBJExtPltt();
        GXS::LoadOBJExtPltt(file, 0, 0x200);
        GXS::EndLoadOBJExtPltt();
        Deallocate(file);
        break;
    case 2:
        file = LoadFile(ASSET_D_2D_PLAYER_SELECT_WARIO_8BIT_NCG_BIN);
        {
            char *a6 = (char *)0x06600000;
            a6 += 0x4000;
            DecompressLZ16(file, (u32)a6);
        }
        Deallocate(file);
        file = LoadFile(ASSET_D_2D_PLAYER_SELECT_WARIO_8BIT_NCL_BIN);
        GXS::BeginLoadOBJExtPltt();
        GXS::LoadOBJExtPltt(file, 0, 0x200);
        GXS::EndLoadOBJExtPltt();
        Deallocate(file);
        break;
    case 3:
        file = LoadFile(ASSET_D_2D_PLAYER_SELECT_YOSHI_8BIT_NCG_BIN);
        {
            char *a6 = (char *)0x06600000;
            a6 += 0x4000;
            DecompressLZ16(file, (u32)a6);
        }
        Deallocate(file);
        file = LoadFile(ASSET_D_2D_PLAYER_SELECT_YOSHI_8BIT_NCL_BIN);
        GXS::BeginLoadOBJExtPltt();
        GXS::LoadOBJExtPltt(file, 0, 0x200);
        GXS::EndLoadOBJExtPltt();
        Deallocate(file);
        break;
    }

    level = (u8)SublevelToLevel(data_02092110);
    if ((u32)level <= 0xE) {
        accB = 0;
        anyUnfilled = 0;
        bitPos = 0;
        totalStars = CountStarsCollectedInLevelToDisplay(level);
        if (IsStarCollected(level, 0) != 0) {
            totalStars = (u32)(u8)(totalStars - 1);
        }
        SELECTED_PLATE(this) = 0xFF;
        PLATE_MASK(this) = 0;
        loopI = 1;
        if ((u32)accB < totalStars) {
            sp8 = 1;
            spC = 1;
            varOne = 1;
            bitsPtr = (u8 *)this;
            bitsPtr += 0x131;
            do {
                if (IsStarCollected(level, loopI) != 0) {
                    PLATE_MASK(this) |= (u8)(varOne << bitPos);
                    accB += 1;
                } else {
                    if (SELECTED_PLATE(this) == 0xFF) {
                        SELECTED_PLATE(this) = bitPos;
                        *bitsPtr |= (u8)(sp8 << bitPos);
                    }
                    anyUnfilled = spC;
                }
                bitPos += 1;
                loopI += 1;
            } while ((u32)accB < totalStars);
        }
        PLATE_COUNT(this) = bitPos;
        if ((anyUnfilled == 0) && (totalStars != 7)) {
            PLATE_COUNT(this) += 1;
            if (SELECTED_PLATE(this) == 0xFF) {
                SELECTED_PLATE(this) = bitPos;
                PLATE_MASK(this) |= (u8)(1 << bitPos);
            }
        }
        if (SELECTED_PLATE(this) == 0xFF) {
            SELECTED_PLATE(this) = 0;
        }
        tmpU8 = PLATE_COUNT(this);
        if (tmpU8 & 1)
            colorVal = 0x80;
        else
            colorVal = 0x92;
        colorVal -= ((s32)tmpU8 >> 1) * 0x23;
        for (idx2 = 0; idx2 < (s32)PLATE_COUNT(this); idx2++) {
            PLATE_X(this, idx2) = colorVal;
            colorVal += 0x23;
        }
    }

    if (SublevelToLevel(data_02092110) <= 0x14) {
        Message::DisplayCourseNameForStarSelect((u32)(s16)SublevelToLevel(data_02092110));
        if (SublevelToLevel(data_02092110) <= 0xE) {
            Message::DisplayStarNameForStarSelect(SELECTED_PLATE(this));
        }
        SetBg3Offset(0, -4);
    } else {
        data_0209d45c &= ~8;
    }

    charIdx = 0;
    STRIP_MODE(this) = 0;
    do {
        if (SaveData::IsCharacterUnlocked(charIdx) != 0) {
            STRIP_MODE(this)++;
        }
        charIdx += 1;
    } while (charIdx < 4);

    if (SublevelToLevel(data_02092110) <= 0xE) {
        if ((STRIP_MODE(this) != 1) && (data_0209caa0[0x41] == 3)) {
            PANEL_Y(this) = 0x88;
            BOUNCE_DELAY(this) = 0;
        } else {
            PANEL_Y(this) = 0x60;
            BOUNCE_DELAY(this) = 0;
        }
        charCount = STRIP_MODE(this);
        charCountM1 = charCount - 1;
        tabR5 = data_ov003_020b16a8[charCountM1];
        tabSL = data_ov003_020b16a0[charCountM1];
        if ((u32)charCount > 1U) {
            varFp = 0;
            varR6 = 0;
            tabSB = data_ov003_020b16a4[charCountM1];
            idx8 = (charCount - 2) * 4;
            if (charCountM1 > 0) {
                tablePtr = &data_ov003_020b16c8[idx8];
                do {
                    if (SaveData::IsCharacterUnlocked(varR6) != 0) {
                        CHAR_X(this, varR6) = tabR5;
                        CHAR_Y(this, varR6) = tabSL;
                        CHAR_SPRITE(this, varR6) = tabSB;
                        varFp += 1;
                        tabR5 = (u8)(tabR5 + data_ov003_020b16e4[idx8]);
                        tabSL = (u8)(tabSL + *tablePtr);
                        tabSB = (u8)(tabSB + 1);
                        tablePtr += 1;
                        idx8 += 1;
                    }
                    varR6 += 1;
                } while (varFp < (s32)(STRIP_MODE(this) - 1));
            }
        }
    }

    PICKED_CHAR(this) = 0xFF;
    CURSOR_ON(this) = 0;
    CURSOR_MODE(this) = 0;
    SCALE_X(this) = 0x1000;
    SCALE_Y(this) = 0x1000;
    IDLE_ICON_SCALE(this) = 0x1000;
    SPIN_ANGLE(this) = 0;
    ANIM_MODE(this) = 0;
    PANEL_ANIM_STEP(this) = 0;
    SPIN_ANGLE(this) = 0;
    BOUNCE_DELAY(this) = 0x78;
    ANIM_FLIP(this) = 0;
    data_0209d454 = 0x10;
    REG32(0x4000000) = (REG32(0x4000000) & ~0x1F00) | (data_0209d45c << 8);
    REG32(0x4001000) = (REG32(0x4001000) & ~0x1F00) | 0x1000;
    LIFE_ICON(this) = data_0209caa0[0x41];
    data_0208ee44 = 1;
    return 1;
}
#pragma pop

// @symbol dScStarSel_c_classInit
/* Reconstructed source-style name: SM64DS proves dScStarSel_c through RTTI,
 * allocation size, vtable identity, and the STAR_SELECT registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: StarSelect_Spawn. */
extern "C" dScStarSel_c *dScStarSel_c_classInit(void) {
    return new dScStarSel_c;
}

extern "C" {
/* Inferred role: background palette entry; existing address label retained. */
u16 data_ov003_020b1698 = 32767;
/* The inverse lookup visits three entries; mode 4 bounds the cursor to 0..2. */
u8 sStarSelectCharacterOrder[3] = {1, 0, 2};
/* Inferred role: initial icon y position by unlocked count; existing address label retained. */
u8 data_ov003_020b16a0[4] = {0, 72, 72, 72};
/* Inferred role: initial icon sprite index by unlocked count; existing address label retained. */
u8 data_ov003_020b16a4[4] = {0, 0, 1, 0};
/* Inferred role: initial icon x position by unlocked count; existing address label retained. */
u8 data_ov003_020b16a8[4] = {0, 128, 92, 128};
/* Divisors for the hundreds, tens and units digits. */
u16 sStarSelectDecimalDivisors[3] = {100, 10, 1};
/* Inferred role: icon y position steps for two, three and four unlocked characters; existing address label retained. */
/* Four-slot strides are observed; unused slots do not prove original bounds. */
s8 data_ov003_020b16c8[12] = {0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, 0};
/* Inferred role: icon x position steps for two, three and four unlocked characters; existing address label retained. */
s16 data_ov003_020b16e4[12] = {0, 0, 0, 0, 72, 0, 0, 0, -68, 136, 0, 0};
BaseSpawnInfo<dScStarSel_c> g_profile_STAR_SELECT = {dScStarSel_c_classInit, 4, 7};
}
