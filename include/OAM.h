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
 * The three Render overloads taking a by-value Fix12<int> are NOT declared here and
 * must not be: mwccarm homes r0-r3 to the stack for a by-value class parameter
 * (+0x14) on all 25 sweep versions at every optimisation level. Those stay
 * extern "C" free functions -- runbook section 7.
 *
 * OamAttr is FORWARD-DECLARED ONLY, deliberately. The tree holds three mutually
 * incompatible spellings of that 8-byte type, and an attempt to centralise it in
 * include/OamAttr.h broke the ROM build:
 *
 *   _ZN3OAM9RenderSub*                4 x u16, unnamed roles
 *   _ZN3OAM16LoadAffineParams*        `unsigned short a, b, c, param` -- names the
 *                                     4th word, which is the affine parameter
 *   _ZN3OAM6Render*P9Matrix2x2        two u32 words decomposed into bitfields
 *                                     (yb, objMode, mode, mosaic, shape, tile...)
 *
 * All three agree on the size and disagree on everything else, and each is the view
 * its own function needs. Reconciling them is real work -- one type, one set of
 * field names, every user re-verified -- and it is not a language-mode migration.
 * A pointer parameter needs only the forward declaration, so the methods below can
 * be declared without picking a winner. */
#ifndef OAM_H
#define OAM_H
#include "types.h"
struct OamAttr;   /* fwd only -- see the note above */

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
    static u32 EnableSubOAM();
    static void Flush();
    static void RenderSub(OamAttr* data, s32 x, s32 y);
    static void RenderSub(OamAttr* data, s32 x, s32 y, s32 palette, s32 priority);
#endif
};

#endif
