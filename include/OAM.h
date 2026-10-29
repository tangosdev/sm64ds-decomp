/* Hand-edited, against evidence. This file used to carry the
 * "AUTO-GENERATED ... by tools/gen_header.py" banner, which was never true --
 * see notes/runbook-type-reconstruction.md section 2.
 *
 * class OAM: 7 matched functions, 5 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen.
 *
 * OAM is NOT polymorphic -- no RTTI record, no vtable, no ctor or dtor -- so the
 * key-function rule (runbook section 7) does not apply and its methods migrate
 * freely. They are STATIC: the ROM bodies take no `this` and drive the hardware
 * OAM buffers through file globals.
 *
 * Render's int overloads are declared below with no parameter names: a named
 * parameter renumbers @NNN in every includer. A by-value Fix12<int> still must
 * not be declared here -- mwccarm homes r0-r3 to the stack for it.
 *
 * OamAttr now lives in include/OamAttr.h -- eleven files that each carried their own
 * copy of the 4 x u16 layout share it, and the `OamAttri` misspelling is retired. See
 * that header for the field evidence and for why it is deliberately dependency-free.
 *
 * THIS HEADER STILL FORWARD-DECLARES IT RATHER THAN INCLUDING IT, and that is load
 * bearing, not leftover. Two files need bit-level access to the same eight bytes and
 * keep their own local decompositions:
 *
 *   _ZN3OAM6Render*P9Matrix2x2                bitfields (yb, objMode, mode, mosaic,
 *                                             shape, xc, aff, size / tile, prio, pal)
 *   _ZN3OAM6Render*5Fix12IiES3_ii             `u32 a01; u16 a2; u16 a3;`
 *
 * The first of those includes THIS header, so if OAM.h pulled in OamAttr.h it would
 * collide with that file's own definition. A pointer parameter needs only the forward
 * declaration, so every method below can be declared without forcing a view on anyone;
 * the 68 files that merely pass an OamAttr* around never need the definition at all. */
#ifndef OAM_H
#define OAM_H
#include "types.h"
struct OamAttr;   /* fwd only -- see the note above */
struct Matrix2x2; /* fwd only -- a pointer parameter needs no more, and the two
                     files that pass one keep their own 4 x s32 definition */
struct OamTmp;    /* fwd only -- EnableSubOAM's caller passes one and the body
                     does not read it */

struct OAM {
    u8  pad_000[0xa8];
    s32 unk_0a8;            /* 0x0a8 */
    u8  pad_0ac[0x45b4];
    u8  unk_4660;           /* 0x4660 */
    u8  pad_4661[0x23];
    u8  unk_4684;           /* 0x4684 */
    u8  pad_4685[0xb];
    u8  unk_4690;           /* 0x4690 */
    u8  pad_4691[0x23];
    s32 unk_46b4;           /* 0x46b4 */
#ifdef __cplusplus
    /* Static -- no `this`; see the header note. Parameter types are read off the
       mangled name, not chosen: `Eii` is (int, int), `Ev` is (). */
    static u8 GetObjWidth(int shape, int sizeBits);
    static u8 GetObjHeight(int shape, int sizeBits);
    static u32 EnableSubOAM(OamTmp *);
    static void Flush();
    static void Load();
    static void Reset();
    /* Returns the affine-slot index, or -1 when the table is full. `count' is the
       live entry count and is advanced by 4 -- one OAM entry per matrix
       coefficient, which is also why the search strides by 4. */
    static int LoadAffineParams(OamAttr* attr, int* count, Matrix2x2* mtx);
    static void Render(bool draw, OamAttr* obj, int x, int y, int palette, int priority, Matrix2x2* mtx);
    /* 10-arg form. Scales are ints (0x1000), not Fix12. Unnamed: a named parameter renumbers @NNN in every includer. */
    static void Render(bool, OamAttr *, int, int, int, int, int, int, int, int);
    /* 8-arg form. Scale and rotation are ints, not Fix12. Unnamed: a named parameter renumbers @NNN in every includer. */
    static OamAttr *Render(bool, OamAttr *, int, int, int, int, int, int);
    static void RenderSub(OamAttr* data, s32 x, s32 y);
    static void RenderSub(OamAttr* data, s32 x, s32 y, s32 palette, s32 priority);

    /* The sprite tables. These are ROM data with no definition in src/; each
       name is its symbols.txt row (_ZN3OAM<name>E). A single sprite
       is an OamAttr whose ADDRESS the callers pass to Render; a table is an
       array of OamAttr pointers that the callers index by digit, character or
       mode. A data member's type is not part of its mangled name. */
    static OamAttr *LIFE_ICONS[];
    static OamAttr *NUMBERS[];
    static OamAttr *VS_YELLOW_NUMBERS[];
    static OamAttr *CONTROLLER_MODE_TEXTS[];
    static OamAttr *MM_PLAYER_ICONS[];
    static OamAttr *MM_VS_PLAYER_ICONS[];
    static OamAttr *MM_VS_PLAYER_ICONS_S[];
    static OamAttr *MM_STAR_MARKERS[];
    static OamAttr *MM_STAR_KEYS[];
    static OamAttr TIMES;
    static OamAttr COIN;
    static OamAttr POWER_STAR;
    static OamAttr SMALL_STAR_EMPTY;
    static OamAttr TINY_STAR;
    static OamAttr BOUNCING_ARROW;
    static OamAttr ARROW_POINTING_LEFT;
    static OamAttr ARROW_POINTING_RIGHT;
    static OamAttr SMALL_ARROW_POINTING_LEFT;
    static OamAttr SMALL_ARROW_POINTING_RIGHT;
    static OamAttr PAUSE;
    static OamAttr VS_PAUSE;
    static OamAttr VS_NEW_STAR_APPEARED;
    static OamAttr VS_TIME;
    static OamAttr VS_TIME_UP;
    static OamAttr TIME;
    static OamAttr MINUTES;
    static OamAttr MM_SMALL_STAR;
    static OamAttr MM_ARROW;
    static OamAttr MM_RED_COIN;
    static OamAttr MM_SPIKE_BOMB;
    /* The camera-control buttons, top screen and (S_) sub screen. */
    static OamAttr CAM_BUTTON_L;
    static OamAttr CAM_BUTTON_L_PRESSED;
    static OamAttr CAM_BUTTON_R;
    static OamAttr CAM_BUTTON_R_PRESSED;
    static OamAttr S_CAM_BUTTON_L;
    static OamAttr S_CAM_BUTTON_L_PRESSED;
    static OamAttr S_CAM_BUTTON_R;
    static OamAttr S_CAM_BUTTON_R_PRESSED;
    static OamAttr CAM_ZOOM_BUTTON;
    static OamAttr CAM_ZOOM_BUTTON_PRESSED;
    static OamAttr S_CAM_ZOOM_BUTTON;
    static OamAttr S_CAM_ZOOM_BUTTON_PRESSED;
#endif
};

#endif
