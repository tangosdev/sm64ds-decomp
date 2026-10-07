//cpp
/* dScStarSel_c -- the star-select screen (ov003), the scene between the
 * level select and the level itself. It shows a row of star plates for the
 * picked course, a character strip along the bottom, and a cursor the D-pad
 * walks across both. ROM RTTI name dScStarSel_c, base dScene_c; the layout
 * evidence is in include/dScStarSel_c.h.
 *
 * ELEVEN of the class's fifteen functions are here: the destructor (slots
 * 16/17), the seven free helpers Render and Behavior drive, the empty
 * OnPendingDestroy (slot 12) and Render (slot 9). .text
 * 0x020addfc..0x020af038, 4668 bytes, one object, byte-identical to retail
 * through the real link.
 *
 * Source order IS the ROM's: `#pragma defer_codegen off` below makes
 * mwccarm emit .text in source order instead of reverse. Do not reorder.
 *
 * THE DESTRUCTOR. The cartridge places D1 (0x020addfc) BELOW D0
 * (0x020ade54). Under deferred codegen an out-of-line destructor emits D2,
 * D0, D1 -- the wrong order -- which is why the destructor pair used to
 * stay in its own files. With `#pragma defer_codegen off` and the file in
 * ascending order, the same out-of-line definition emits D1, D0, D2: the
 * cartridge's order, both byte-identical, written first so they land
 * first. The extra D2 is unreferenced and is dropped at link, as the
 * retail link did. Because the destructor is the first declared
 * non-inline virtual in dScStarSel_c.h, it is the key function, so this
 * file also emits the vtable and the typeinfo chain; the cartridge's
 * copies of those are outside this range and stay canonical.
 *
 * #pragma opt_strength_reduction off is FILE-GLOBAL and last-wins in
 * mwccarm 2004/b56, not positional, so it applies to every function here
 * and there is no way to scope it to one. It is not decoration: of the
 * four cells of {strength reduction, loop invariants} x {on, off}, this is
 * the only one in which all of these functions reproduce. Leaving both at
 * their defaults, or turning loop invariants off as well, costs Render 0x10
 * bytes of frame. Bracketing it around a single member does nothing --
 * measured; the last directive in the file wins for the whole file. (Those
 * measurements predate `defer_codegen off`; here the directive sits above
 * every function, so it covers all eleven in either regime.)
 *
 * common.h MUST precede dScStarSel_c.h. With the order reversed, Render
 * builds 0x18 bytes long. That is the only include-order constraint found.
 *
 * Known limits:
 * - PARTIAL FOLD, and exactly where the line falls. The class occupies one
 *   contiguous linker run, 0x020addfc..0x020b0580, fifteen functions.
 *   Behavior (slot 6, 0x020af038..0x020af86c) does not match. It is a
 *   long-standing near-miss and it did not close when folded in here; the
 *   residual is register allocation only, no schedule change. It sits in
 *   the MIDDLE of the run, and a delinks entry carries exactly one .text
 *   range, so a licensed range cannot skip it. That splits the run in two
 *   and this file is the larger side: everything from the destructor up to
 *   Behavior. Still in their own files: Behavior, CleanupResources,
 *   InitResources and the factory dScStarSel_c_classInit.
 * - The seven func_ov003_* helpers are written free, and those names are
 *   address-derived analysis labels, not recovered spellings: the image
 *   preserves no linker symbol table and RTTI supplies class identities,
 *   not function names (notes/tu-promotion-conventions.md section 1,
 *   notes/symbol-name-provenance.md). What IS measured is that every
 *   inbound relocation to each of them comes from inside this class's own
 *   run and no other module reaches any of them -- evidence to narrow
 *   ownership on later, not proof the originals were free functions.
 * - Where two legacy shards declared the same object with different
 *   shapes, one spelling had to win for the merged file, and the winner is
 *   the one the surviving call sites already hold: data_02092110 and
 *   data_02092128 as scalars, data_020a0e58 as u16[], data_020a0e5a as
 *   u16[][2] (the stride-carrying form -- the flat u16[i*2] spelling costs
 *   func_ov003_020ae358 seventeen words), OAM::TIMES as a scalar, and
 *   func_ov003_020ae1a4's first parameter as char*. Every one of these is
 *   the same address arithmetic written differently; none changes a byte.
 * - OAM::Render and the G3i/G2/GX entry points stay mangled with local
 *   shadow parameter types. The ROM names carry by-value class parameters
 *   that mwccarm passes differently at the call site, so declaring the
 *   true types breaks the byte match (notes/mwccarm-codegen.md 6az).
 *   Carried from the legacy sources unchanged.
 * - The scalar layout past dScStarSel_c's Model pair has not been recovered
 *   field-by-field: include/dScStarSel_c.h holds it as opaque bytes. The
 *   accessor macros below name each offset by what this file does with it;
 *   they are file-local views, not header members. Offsets that are only
 *   copied or compared without a visible meaning (0x118, 0x119, the
 *   ANIM_MODE 1 phase byte 0x137 and the level-number limit 0xf) are left as numbers.
 */

#pragma defer_codegen off
#pragma opt_strength_reduction off
#include "common.h"
#include "dScStarSel_c.h"
#include "decl_common.h"
#include "OAM.h"
#include "SaveData.h"

/* Raw this-relative accessors, and the cosine table lookup used by Render. */
#define FB(p, o) (*(u8 *)((u8 *)(p) + (o)))
#define FH(p, o) (*(s16 *)((u8 *)(p) + (o)))
#define FW(p, o) (*(s32 *)((u8 *)(p) + (o)))
#define COS(a) data_02082214[((u16)(a) >> 4) * 2 + 1]

/* Named views onto the storage between the Model pair and the end of the
 * object. Fixed-point values are 20.12 (0x1000 = 1.0), as the OAM::Render
 * scale arguments and the 0x1000 steps below show. */
#define SCALE_X(p)        FW(p, 0x50)   /* scale of the panel sprite at (0x80, PANEL_Y) */
#define SCALE_Y(p)        FW(p, 0x54)
#define ICON_SCALE_X(p)   FW(p, 0x58)   /* scale of the icon under CHAR_CURSOR */
#define ICON_SCALE_Y(p)   FW(p, 0x5c)
#define IDLE_ICON_SCALE(p) FW(p, 0x60)  /* scale of the other character icons */
#define REPEAT_TIMER(p)   (*(u16 *)((u8 *)(p) + 0x106))   /* D-pad auto-repeat */
#define BOTTOM_Y_OFFSET(p) FH(p, 0x10a) /* 8.8 offset of the bottom sprite's y */
#define SPIN_ANGLE(p)     FH(p, 0x110)
#define PULSE_ANGLE(p)    FH(p, 0x112)
#define PLATE_COUNT(p)    FB(p, 0x114)  /* star plates drawn by Render */
#define SELECTED_PLATE(p) FB(p, 0x115)
#define LIFE_ICON(p)      FB(p, 0x116)  /* index into OAM::LIFE_ICONS */
#define PLATE_X(p, i)     FB((u8 *)(p) + (i), 0x11a)
#define DIGIT(p, i)       (*(s8 *)((u8 *)(p) + (i) + 0x121))  /* 3 digits, -1 = blank */
#define CHAR_X(p, i)      FB((u8 *)(p) + (i), 0x124)          /* per character */
#define CHAR_Y(p, i)      FB((u8 *)(p) + (i), 0x128)
#define PANEL_Y(p)        FB(p, 0x12b)
#define CHAR_SPRITE(p, i) FB((u8 *)(p) + (i), 0x12c)
#define STRIP_MODE(p)     FB(p, 0x130)  /* strip slots + 1; 4 = fixed order from data_ov003_020b169c */
#define PLATE_MASK(p)     FB(p, 0x131)  /* bit i: extra sprite behind plate i */
#define PICKED_CHAR(p)    FB(p, 0x132)
#define CURSOR_MODE(p)    FB(p, 0x133)  /* 0 plates, 1 character strip, else fixed */
#define CHAR_CURSOR(p)    FB(p, 0x134)  /* slot within the character strip */
#define CURSOR_ON(p)      FB(p, 0x135)
#define ANIM_MODE(p)      FB(p, 0x139)  /* 1 or 2 selects the animation below */
#define ANIM_FLIP(p)      FB(p, 0x13a)

extern "C" {
extern unsigned char data_ov003_020b169c[];
extern unsigned short data_ov003_020b16ac[];
extern void func_02012790(int a);
extern int func_ov003_020adec0(char *scene, unsigned int chr);
extern void func_ov003_020ae1a4(char *scene, int value);
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
void _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(int, void *, int, int, int, int, int, int, int, int);
void _ZN3G3i13PerspectiveW_E5Fix12IiES1_S1_S1_S1_S1_bP9Matrix4x3(int, int, int, int, int, int, int, void *);
void _ZN3G3i7LookAt_EPK7Vector3S2_S2_bP9Matrix4x3(const void *, const void *, const void *, int, Matrix4x3 *);
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
extern void *_ZN3OAM7NUMBERSE[];
extern void *_ZN3OAM10LIFE_ICONSE[];
extern void *_ZN3OAM4COINE[];
extern void *_ZN3OAM5TIMESE;
extern void *_ZN3OAM10POWER_STARE;
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
// Model pair (__cxa_vec_cleanup) and the base destructors is all
// compiler-generated.
dScStarSel_c::~dScStarSel_c()
{
}

// @symbol _ZN12dScStarSel_cD0Ev
// ROM 0x020ade54, size 0x6c; vtable slot 17. The deleting destructor has no
// source of its own: mwccarm emits it from the definition above, directly
// after D1, ending with dScene_c's inline operator delete.

extern "C" {  /* .c-derived members: C linkage for the whole block */

// @symbol func_ov003_020adec0
// ROM 0x020adec0, size 0x90. The inverse of func_ov003_020adf50: maps a
// character index to its strip slot, or 0 when it has none.
int func_ov003_020adec0(char *c, unsigned int chr)
{
    unsigned char mode = STRIP_MODE(c);
    if (mode == 4) {
        unsigned char *p = data_ov003_020b169c;
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

// @symbol func_ov003_020adf50
// ROM 0x020adf50, size 0x78. Maps the strip slot CHAR_CURSOR to a character
// index: through data_ov003_020b169c when STRIP_MODE is 4, otherwise the
// CHAR_CURSOR-th unlocked character when STRIP_MODE is 2 or more; else 0.
int func_ov003_020adf50(char *c)
{
    unsigned char mode = STRIP_MODE(c);
    if (mode == 4) {
        return data_ov003_020b169c[CHAR_CURSOR(c)];
    }
    if (mode >= 2) {
        int slot = 0;
        int chr = 0;
        for (; chr < 3; chr++) {
            if (SaveData::IsCharacterUnlocked((unsigned int)chr) != 0) {
                if (slot == CHAR_CURSOR(c)) {
                    return chr;
                }
                slot++;
            }
        }
    }
    return 0;
}

// @symbol func_ov003_020adfc8
// ROM 0x020adfc8, size 0xe8. Draws the coin record of the current level
// (SaveData::GetCoinRecord) as three digits drawn right to left, then the
// TIMES and COIN sprites.
void func_ov003_020adfc8(char *scene)
{
    int x = 0xb8;
    int lvl = SublevelToLevel(data_02092110);
    int coin = SaveData::GetCoinRecord(lvl);
    func_ov003_020ae1a4(scene, coin);
    int i;
    for (i = 2; i >= 0; i--) {
        signed char d = DIGIT(scene, i);
        if (d >= 0) {
            OAM::Render(0, (OamAttr *)_ZN3OAM7NUMBERSE[d], x, 0x4c, 8, -1, 0);
            x -= 9;
        }
    }
    OAM::Render(0, (OamAttr *)&_ZN3OAM5TIMESE, x, 0x54, -1, -1, 0);
    OAM::Render(0, (OamAttr *)_ZN3OAM4COINE, x - 0x10, 0x4c, -1, -1, 0);
}

// @symbol func_ov003_020ae0b0
// ROM 0x020ae0b0, size 0xf4. Draws the power-star count: NumStars() as three
// digits, drawn right to left, then the TIMES and POWER_STAR sprites.
void func_ov003_020ae0b0(char *scene)
{
    int x;
    int y;
    if (SublevelToLevel(data_02092110) >= 0xf) {
        y = 0xa0;
        x = 0xb8;
    } else {
        y = 0xac;
        x = 0xf4;
    }
    func_ov003_020ae1a4(scene, NumStars());
    {
        int i = 2;
        do {
            signed char d = DIGIT(scene, i);
            if (d >= 0) {
                OAM::Render(0, (OamAttr *)_ZN3OAM7NUMBERSE[d], x, y, 8, -1, 0);
                x -= 9;
            }
            i--;
        } while (i >= 0);
    }
    OAM::Render(0, (OamAttr *)&_ZN3OAM5TIMESE, x, y + 8, -1, -1, 0);
    OAM::Render(0, (OamAttr *)&_ZN3OAM10POWER_STARE, x - 0x10, y + 8, -1, -1, 0);
}

// @symbol func_ov003_020ae1a4
// ROM 0x020ae1a4, size 0x94. Splits value into three decimal digits in
// DIGIT(scene, 0..2), dividing by data_ov003_020b16ac[i] in turn; leading
// zeros are stored as -1 (blank) except the last digit.
void func_ov003_020ae1a4(char *scene, int value)
{
    int found = 0;
    int one = 1;
    int blank = -1;
    int i;
    for (i = 0; i < 3; i++) {
        unsigned short place = data_ov003_020b16ac[i];
        int digit = value / place;
        if (digit == 0 && found == 0 && i != 2) {
            DIGIT(scene, i) = (char)blank;
        } else {
            found = one;
            DIGIT(scene, i) = (char)digit;
        }
        value = (unsigned short)(value % place);
    }
}

// @symbol func_ov003_020ae238
// ROM 0x020ae238, size 0x120. Draws the life counter: the LIFE_ICONS entry for
// LIFE_ICON, the TIMES sprite, and the three digits of data_0209f2f4[0].
void func_ov003_020ae238(char *scene)
{
    int x;
    int y;
    if (SublevelToLevel(data_02092110) >= 0xf) {
        y = 0xa0;
        x = 0x50;
    } else {
        x = 0x10;
        y = 0xac;
    }
    _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, _ZN3OAM10LIFE_ICONSE[LIFE_ICON(scene)], x, y + 8, -1, -1, 0x1000, 0x1000, 0, -1);
    OAM::Render(0, (OamAttr *)&_ZN3OAM5TIMESE, x + 0x10, y + 8, -1, -1, 0);
    func_ov003_020ae1a4(scene, (unsigned short)data_0209f2f4[0]);
    {
        int i = 0;
        x += 0x18;
        do {
            signed char d = DIGIT(scene, i);
            if (d >= 0) {
                OAM::Render(0, (OamAttr *)_ZN3OAM7NUMBERSE[d], x, y, 8, -1, 0);
                x += 9;
            }
            i++;
        } while (i < 3);
    }
}

// @symbol func_ov003_020ae358
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
// With no record active (label sect2) the d-pad half of the control word
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
void func_ov003_020ae358(char *c)
{
    int idx = gActivePlayerSlot;
    int valid = 0;
    int i;
    if (gTouchHeld[idx][0] != 0) {
        valid = gTouchEdge[idx][0] != 0;
    }
    if (valid == 0) {
        goto sect2;
    }
    if ((((unsigned char)(gTouchX[idx][0] - 0x58)) < 0x50) && (((unsigned char)((gTouchY[idx][0] - PANEL_Y(c)) + 0x28)) < 0x50)) {
        CURSOR_MODE(c) = 2;
        PICKED_CHAR(c) = 3;
        FB(c, 0x118) = (unsigned char)(data_0208ee44 * 6);
        ANIM_MODE(c) = 1;
        FB(c, 0x119) = 0x10;
        func_02012790(data_0209caa0[0x41] + 0x3c);
        return;
    }
    if (STRIP_MODE(c) <= 1) {
        return;
    }
    if (data_0209caa0[0x41] != 3) {
        return;
    }
    for (i = 0; i < 3; i++) {
        if (SaveData::IsCharacterUnlocked(i) != 0) {
            int ri = gActivePlayerSlot;
            if (((unsigned short)((gTouchHeld[gActivePlayerSlot][2] - CHAR_X(c, i)) + 0x18)) < 0x30) {
                if (((unsigned short)((gTouchHeld[ri][3] - CHAR_Y(c, i)) + 0x18)) < 0x2b) {
                    CURSOR_MODE(c) = 1;
                    CHAR_CURSOR(c) = (unsigned char)func_ov003_020adec0(c, i);
                    data_02092128 = (unsigned char)i;
                    data_02092114 = (unsigned char)i;
                    PICKED_CHAR(c) = (unsigned char)i;
                    FB(c, 0x118) = (unsigned char)(data_0208ee44 * 3);
                    ANIM_MODE(c) = 2;
                    FB(c, 0x119) = 0x10;
                    func_02012790(data_0209caa0[0x41] + 0x3c);
                    return;
                }
            }
        }
    }

    return;
sect2:
    if (data_0209caa0[0x42] == 0) {
        unsigned short ctrl = data_020a0e58[0];
        if ((ctrl & 0x30) != 0) {
            unsigned short timer = REPEAT_TIMER(c);
            unsigned char next;
            if (timer != 0) {
                REPEAT_TIMER(c) -= 1;
                return;
            }
            if (CURSOR_ON(c) == 0) {
                return;
            }
            if (CURSOR_MODE(c) != 1) {
                return;
            }
            if (STRIP_MODE(c) < 3) {
                return;
            }
            next = CHAR_CURSOR(c);
            if (ctrl & 0x20) {
                if (((data_020a0e58[1] & 0x20) != 0) || (timer == 0)) {
                    REPEAT_TIMER(c) = (data_020a0e5a[idx][0] & 0x20) ? (0x10) : (8);
                    if (CHAR_CURSOR(c) != 0) {
                        next = next - 1;
                    }
                }
            } else if (ctrl & 0x10) {
                if (((data_020a0e58[1] & 0x10) != 0) || (timer == 0)) {
                    REPEAT_TIMER(c) = (data_020a0e5a[idx][0] & 0x10) ? (0x10) : (8);
                    if (CHAR_CURSOR(c) != (STRIP_MODE(c) - 2)) {
                        next = next + 1;
                    }
                }
            }
            if (next == CHAR_CURSOR(c)) {
                return;
            }
            CHAR_CURSOR(c) = next;
            func_02012790(0x12e);
            return;
        }
    }

    REPEAT_TIMER(c) = 0;
}

}

// @symbol _ZN12dScStarSel_c16OnPendingDestroyEv
// ROM 0x020ae6f0, size 0x4; vtable slot 12. Empty override.
void dScStarSel_c::OnPendingDestroy()
{
}

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
s32 dScStarSel_c::Render()
{
    s32 x;
    s32 i;
    s32 idx;
    Model *plate;
    s32 pressY;
    Matrix4x3 mtx;

    func_ov003_020ae238((char *)this);
    func_ov003_020ae0b0((char *)this);

    if (SublevelToLevel(data_02092110) < 0xf) {
        u8 levelNum;
        s32 numX;
        func_ov003_020adfc8((char *)this);
        levelNum = SublevelToLevel(data_02092110) + 1;
        if (levelNum <= 0xf) {
            if (levelNum >= 10) {
                OAM::Render(0, (OamAttr *)_ZN3OAM7NUMBERSE[levelNum / 10], numX = 0x77, 0x7a, 8, -1, 0);
                numX += 9;
            } else {
                numX = 0x7c;
            }
            OAM::Render(0, (OamAttr *)_ZN3OAM7NUMBERSE[levelNum % 10], numX, 0x7a, 8, -1, 0);
        }
    }

    if (SublevelToLevel(data_02092110) <= 0xe) {
        _ZN3G3i13PerspectiveW_E5Fix12IiES1_S1_S1_S1_S1_bP9Matrix4x3(data_02082214[0x16], data_02082214[0x17], 0x1555, 0x1000, 0x1388000, 0x1000, 1, 0);
        _ZN3G3i7LookAt_EPK7Vector3S2_S2_bP9Matrix4x3(&data_ov003_020b132c, &data_ov003_020b1338, &data_ov003_020b1344, 1, &mtx);
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
                OAM::RenderSub((OamAttr *)&data_ov001_020abd78, PLATE_X(this, SELECTED_PLATE(this)), 6);
            } else if (CURSOR_MODE(this) == 1) {
                u8 *sel = (u8 *)this + func_ov003_020adf50((char *)this);
                OAM::RenderSub((OamAttr *)&data_ov001_020abd80, FB(sel, 0x124) - 0x24, FB(sel, 0x128) - 8);
            } else {
                OAM::RenderSub((OamAttr *)&data_ov001_020abd80, 0x50, PANEL_Y(this) + 8);
            }
        }

        if (ANIM_MODE(this) != 0) {
            if (ANIM_MODE(this) == 1) {
                switch (FB(this, 0x137)) {
                case 0:
                    SCALE_X(this) += 0x400;
                    if (SCALE_X(this) >= 0x1800) {
                        SCALE_X(this) = 0x1800;
                        FB(this, 0x137) = 1;
                    }
                    break;
                case 1:
                    SCALE_X(this) -= 0x400;
                    if (SCALE_X(this) <= 0x1000) {
                        SCALE_X(this) = 0x1000;
                        FB(this, 0x137) = 2;
                    }
                    break;
                case 2:
                    SCALE_X(this) -= 0x80;
                    if (FB(this, 0x119) == 0) {
                        FB(this, 0x137) = 3;
                    }
                    break;
                case 3:
                    SCALE_X(this) += 0x100;
                    SPIN_ANGLE(this) += 0x1000;
                    break;
                }
                SCALE_Y(this) = SCALE_X(this);
            } else if (ANIM_MODE(this) == 2) {
                ICON_SCALE_X(this) = (0x1000 - COS((s16)(PULSE_ANGLE(this) - 0x4000))) * 10 + 0x1000;
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
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(1, &data_ov001_020abb34, 0x80, PANEL_Y(this), -1, -1, SCALE_X(this), SCALE_Y(this), (u16)SPIN_ANGLE(this), -1);
            break;
        case 1:
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(1, &data_ov001_020abb54, 0x80, PANEL_Y(this), -1, -1, SCALE_X(this), SCALE_Y(this), (u16)SPIN_ANGLE(this), -1);
            break;
        case 2:
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(1, &data_ov001_020abb74, 0x80, PANEL_Y(this), -1, -1, SCALE_X(this), SCALE_Y(this), (u16)SPIN_ANGLE(this), -1);
            break;
        case 3:
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(1, &data_ov001_020abb94, 0x80, PANEL_Y(this), -1, -1, SCALE_X(this), SCALE_Y(this), (u16)SPIN_ANGLE(this), -1);
            for (i = 0; i < 3; i++) {
                pressY = 0;
                if (PICKED_CHAR(this) == i && FB(this, 0x118) != 0) {
                    pressY = 3;
                }
                if (SaveData::IsCharacterUnlocked(i) != 0) {
                    if (ANIM_MODE(this) == 2) {
                        if (i == func_ov003_020adf50((char *)this)) {
                            if (ANIM_FLIP(this) == 0) {
                                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(1, data_ov001_020abcb4[i + 3], CHAR_X(this, i), CHAR_Y(this, i), -1, -1, ICON_SCALE_X(this), ICON_SCALE_Y(this), 0, -1);
                                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(1, data_ov001_020abcb4[CHAR_SPRITE(this, i)], CHAR_X(this, i), CHAR_Y(this, i), -1, -1, ICON_SCALE_X(this), ICON_SCALE_Y(this), 0, -1);
                            } else {
                                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(1, data_ov001_020abcb4[CHAR_SPRITE(this, i ^ ((i != 0) ? 3 : 0))], CHAR_X(this, i), CHAR_Y(this, i), -1, -1, ICON_SCALE_X(this), ICON_SCALE_Y(this), 0, -1);
                            }
                        } else if (IDLE_ICON_SCALE(this) != 0) {
                            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(1, data_ov001_020abcb4[i + 3], CHAR_X(this, i), CHAR_Y(this, i), -1, -1, IDLE_ICON_SCALE(this), IDLE_ICON_SCALE(this), 0, -1);
                            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(1, data_ov001_020abcb4[CHAR_SPRITE(this, i)], CHAR_X(this, i), CHAR_Y(this, i), -1, -1, IDLE_ICON_SCALE(this), IDLE_ICON_SCALE(this), 0, -1);
                        }
                    } else {
                        OAM::RenderSub((OamAttr *)data_ov001_020abcb4[i + 3], CHAR_X(this, i), pressY + CHAR_Y(this, i));
                        OAM::RenderSub((OamAttr *)data_ov001_020abcb4[CHAR_SPRITE(this, i)], CHAR_X(this, i), pressY + CHAR_Y(this, i));
                    }
                }
            }
            break;
        }

        if (STRIP_MODE(this) <= 1 || data_0209caa0[0x41] != 3) {
            if (SublevelToLevel(data_02092110) <= 0xe) {
                OAM::RenderSub((OamAttr *)&data_ov001_020abbb4, 0x80, (BOTTOM_Y_OFFSET(this) >> 8) + 0xa0);
            } else {
                OAM::RenderSub((OamAttr *)&data_ov001_020abbf4, 0x80, (BOTTOM_Y_OFFSET(this) >> 8) + 0xa0);
            }
        }
    }
    return 1;
}
