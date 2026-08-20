#ifndef DAPGDFDR_C_H
#define DAPGDFDR_C_H

#include "types.h"

#ifdef __cplusplus

#include "dBgActor_c.h"
#include "ModelAnim.h"
#include "TextureSequence.h"
#include "dCcAc_c.h"

/* No header existed under any name -- confirmed against build/rtti.json (RTTI
 * record ov027:0x021139e4, mangled `10daPgDfdr_c`) and against
 * include/decl_common.h, which carried only a bare `extern int
 * _ZTV10daPgDfdr_c[];` placeholder. The RTTI name is used exactly as spelled;
 * no pretty name is coined.
 *
 * BASE: dBgActor_c, direct. The RTTI edge (base_key ov002:0x021089ec, offset 0)
 * agrees with the destructor: func_ov027_021118c8 (D1, vtable slot 16) stores
 * exactly two vtables in sequence -- `data_ov027_02113a90` (its own) then
 * `_ZTV10dBgActor_c` -- one intermediate-free inlined base, same shape as
 * daDgr_c and BigBrickBlock.
 *
 * SIZE IS 0x3dc (988 decimal), THE LITERAL THE FACTORY PASSES TO operator new.
 * The factory, func_ov027_0211207c, is: `fBase_c::operator new(988)`,
 * `dBgActor_c::dBgActor_c()`, store `_ZTV10daPgDfdr_c`, then three member
 * constructors run in field order -- `ModelAnim::ModelAnim()` at +0x320,
 * `TextureSequence::TextureSequence()` at +0x384, `dCcAc_c::dCcAc_c()`
 * at +0x398. func_ov027_021118c8 (D1) is the same three in reverse --
 * dCcAc_c at +0x398, TextureSequence at +0x384, ModelAnim at +0x320
 * -- before it inlines dBgActor_c's own teardown. Constructor and destructor
 * witnesses agree exactly; there is no undestructed gap to close.
 *
 * 0x398 + sizeof(dCcAc_c) (0x34) = 0x3cc. The remaining 0x10 bytes,
 * 0x3cc..0x3dc, are POD -- nothing in the class destructs them -- and are read
 * across InitResources (func_ov027_02111eb4), the state-dispatch pair
 * func_ov027_02111d38/02111cfc, and OnGroundHit-family helpers
 * func_ov027_02111a28/02111b2c/02111c48/02111ca8:
 *
 *   0x3cc  void*  -- a pointer to a state-callback table; func_ov027_02111d70
 *                     writes `&data_ov027_02113ce4[idx]`, and the two callers
 *                     func_ov027_02111d38 / 02111cfc read *(p) and *(p+1) as
 *                     function pointers and call through `this`. Left as a
 *                     raw pointer -- the callee's exact function-pointer type
 *                     is not proven here.
 *   0x3d0  s32    -- set to 0 or 1 by func_ov027_02111ca8/02111b2c; a flag.
 *   0x3d4  s32    -- an index/lookup value into `data_ov027_02113a1c` and
 *                     friends, keyed by the byte at 0x3d9.
 *   0x3d8  u8     -- a countdown byte; func_ov027_02111ca8 sets it to 0x14
 *                     (20), func_ov027_02111c48 decrements it through
 *                     DecIfAbove0_Byte.
 *   0x3d9  u8     -- a table index, 0..9, reset to 0 above 9.
 *   0x3da  --     -- never dereferenced anywhere in the class; 2 bytes of
 *                     padding closing the class to 0x3dc.
 *
 * Field NAMES below are placeholders (unk_/pad_); types are proven from the
 * accesses above, not guessed.
 */

struct daPgDfdr_c : dBgActor_c {
    ModelAnim mModelAnim;                  /* 0x320 */
    TextureSequence mTextureSequence;      /* 0x384 */
    dCcAc_c mdCcAc_c; /* 0x398 */
    void *unk_3cc;
    s32   unk_3d0;
    s32   unk_3d4;
    u8    unk_3d8;
    u8    unk_3d9;
    u8    pad_3da[0x2];

    /* --- vtable, in ROM order. Do not reorder. ---
     * A leaf class (no RTTI children), so the destructor is declared OUT OF
     * LINE here, matching every other leaf under dBgActor_c (e.g.
     * include/BigBrickBlock.h). Defined as a real method in
     * src/_ZN10daPgDfdr_cD1Ev.cpp / src/_ZN10daPgDfdr_cD0Ev.cpp -- both empty
     * bodies; the compiler emits the three member teardowns (in reverse
     * declaration order: dCcAc_c, TextureSequence, ModelAnim), the
     * two inherited vtable stores and the two dBgActor_c member teardowns on
     * its own. */
    virtual ~daPgDfdr_c();

    /* --- overrides of inherited fBase_c slots. Each takes its base's index
     *     (see include/fBase_c.h for the full 32-slot table). --- */
    virtual s32 InitResources();       /* slot  0 */
    virtual s32 CleanupResources();    /* slot  3 */
    virtual s32 Behavior();            /* slot  6 */
    virtual s32 Render();              /* slot  9 */
    virtual void OnPendingDestroy();   /* slot 12 -- empty body in the ROM */

    /* slot 31 (Kill) is NOT overridden: the vtable word at data_ov027_02113a90
       + 0x7c is 0x020ee55c, identical to dBgActor_c's own slot 31 -- confirmed
       by reloc, not assumed. */
};

typedef char daPgDfdr_c_size_must_be_0x3dc[sizeof(daPgDfdr_c) == 0x3dc ? 1 : -1];

#else

/* The same object for a C translation unit, which has no base class to
   inherit dBgActor_c's fields from and so spells the whole layout flat.
   dBgActor_c's own C-side layout is duplicated from include/dBgActor_c.h;
   ModelAnim/TextureSequence/dCcAc_c are opaque byte blobs here
   because their own headers are C++-only. */
struct daPgDfdr_c {
    u8  pad_000[0x5c];
    s32 mPosX;                   /* 0x05c */
    s32 mPosY;                   /* 0x060 */
    s32 mPosZ;                   /* 0x064 */
    u8  pad_068[0xc];
    s32 unk_074;                 /* 0x074 */
    s32 mCamSpacePosY;           /* 0x078 */
    s32 mCamSpacePosZ;           /* 0x07c */
    s32 mScaleX;                 /* 0x080 */
    s32 mScaleY;                 /* 0x084 */
    s32 mScaleZ;                 /* 0x088 */
    s16 mAngleX;                 /* 0x08c */
    s16 mAngleY;                 /* 0x08e */
    u8  pad_090[0x4];
    s16 mPrevAngleY;             /* 0x094 */
    u8  pad_096[0x2];
    s32 unk_098;                 /* 0x098 */
    s32 unk_09c;                 /* 0x09c */
    s32 unk_0a0;                 /* 0x0a0 */
    u8  pad_0a4[0x4];
    s32 unk_0a8;                 /* 0x0a8 */
    u8  pad_0ac[0x4];
    s32 unk_0b0;                 /* 0x0b0 */
    s32 unk_0b4;                 /* 0x0b4 */
    s32 unk_0b8;                 /* 0x0b8 */
    u8  pad_0bc[0x18];
    u8  mModel;                  /* 0x0d4 */
    u8  pad_0d5[0x1b];
    u8  unk_0f0;                 /* 0x0f0 */
    u8  pad_0f1[0x23];
    s32 unk_114;                 /* 0x114 */
    s32 unk_118;                 /* 0x118 */
    s32 unk_11c;                 /* 0x11c */
    u8  pad_120[0x4];
    u8  mMeshCollider;           /* 0x124 */
    u8  pad_125[0x1c7];
    struct Matrix4x3 mClsnMat;   /* 0x2ec */
    u8  unk_31c;                 /* 0x31c */
    u8  unk_31d;                 /* 0x31d */
    /* daPgDfdr_c's own bytes, from the header comment above. */
    u8  mModelAnim[0x64];             /* 0x320 */
    u8  mTextureSequence[0x14];       /* 0x384 */
    u8  mdCcAc_c[0x34];    /* 0x398 */
    void *unk_3cc;                    /* 0x3cc */
    s32   unk_3d0;                    /* 0x3d0 */
    s32   unk_3d4;                    /* 0x3d4 */
    u8    unk_3d8;                    /* 0x3d8 */
    u8    unk_3d9;                    /* 0x3d9 */
    u8    pad_3da[0x2];
};

#endif /* __cplusplus */

#endif
