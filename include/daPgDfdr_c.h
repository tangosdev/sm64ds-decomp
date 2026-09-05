#ifndef DAPGDFDR_C_H
#define DAPGDFDR_C_H

#include "types.h"
#include "dBgW_KcMbg.h"
#include "Model.h"

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
 * The factory, daPgDfdr_c_classInit, is: `fBase_c::operator new(988)`,
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
 *   0x3cc  mStateTable -- a pointer to a state-callback table; func_ov027_02111d70
 *                     writes `&data_ov027_02113ce4[idx]`, and the two callers
 *                     func_ov027_02111d38 / 02111cfc read *(p) and *(p+1) as
 *                     function pointers and call through `this`. Left as a
 *                     raw pointer -- the callee's exact function-pointer type
 *                     is not proven here.
 *   0x3d0  s32    -- set to 0 or 1 by func_ov027_02111ca8/02111b2c; a flag.
 *   0x3d4  s32    -- an index/lookup value into `data_ov027_02113a1c` and
 *                     friends, keyed by the byte at 0x3d9.
 *   0x3d8  mTimer -- a countdown byte; func_ov027_02111ca8 sets it to 0x14
 *                     (20), func_ov027_02111c48 decrements it through
 *                     DecIfAbove0_Byte.
 *   0x3d9  u8     -- a table index, 0..9, reset to 0 above 9.
 *   0x3da  --     -- never dereferenced anywhere in the class; 2 bytes of
 *                     padding closing the class to 0x3dc.
 *
 * Types are proven from the accesses above, not guessed. Only the two offsets
 * whose USE is unambiguous are named -- the state-callback table pointer and the
 * countdown byte; 0x3d0, 0x3d4 and 0x3d9 stay unk_ because "a flag", "an index"
 * and "a table index" do not say what they hold. See
 * notes/bgobject-provenance.md.
 *
 * SM64DS RTTI names the implementation daPgDfdr_c. The reconstructed factory
 * daPgDfdr_c_classInit (historical alias func_ov027_0211207c) installs this class's
 * cartridge vtable; the reconstructed profile global g_profile_PENGUIN_DEFENDER
 * (historical alias data_ov027_02113a00) is its registry descriptor.
 */

struct daPgDfdr_c : dBgActor_c {
    ModelAnim mModelAnim;                  /* 0x320 */
    TextureSequence mTextureSequence;      /* 0x384 */
    dCcAc_c mdCcAc_c;                      /* 0x398 */
    void *mStateTable;                     /* 0x3cc */
    s32   unk_3d0;                         /* 0x3d0 */
    s32   mDistanceLeft;                         /* 0x3d4 */
    u8    mTimer;                          /* 0x3d8 */
    u8    mStepIndex;                         /* 0x3d9 */
    u8    pad_3da[0x2];

    /* --- vtable, in ROM order. Do not reorder. ---
     * THE DESTRUCTOR IS DEFINED INLINE HERE, and that is load-bearing. It used
     * to be declared out of line and defined twice over, once in each of
     * src/_ZN10daPgDfdr_cD1Ev.cpp and src/_ZN10daPgDfdr_cD0Ev.cpp; the promotion
     * into one TU (src/actors/daPgDfdr_c.cpp) can carry only one definition, and
     * an out-of-line one makes mwccarm 2004/b56 emit D0 BEFORE D1 -- the reverse
     * of the cartridge's 0x021118c8 (D1) then 0x02111924 (D0) -- plus a third D2
     * body with no ROM home. Defined in the class body it yields the retail
     * D1/D0 pair in ROM order and no D2. The body stays empty: the compiler
     * emits the three member teardowns (reverse declaration order: dCcAc_c,
     * TextureSequence, ModelAnim), the inherited vtable store and dBgActor_c's
     * own two member teardowns by itself.
     *
     * Making it inline moves the KEY FUNCTION to InitResources, the first
     * DECLARED non-inline virtual below, so src/actors/daPgDfdr_c.cpp -- which
     * defines InitResources -- is still the TU that emits _ZTV/_ZTI/_ZTS. */
    virtual ~daPgDfdr_c() {}

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
    s32 mCamSpacePosX;                 /* 0x074 */
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
    s32 mHorzSpeed;                 /* 0x098 */
    s32 mVertAccel;                 /* 0x09c */
    s32 mTerminalVelocity;                 /* 0x0a0 */
    u8  pad_0a4[0x4];
    s32 mVertSpeed;                 /* 0x0a8 */
    u8  pad_0ac[0x4];
    s32 mFlags;                 /* 0x0b0 */
    s32 mClipOffsetY;                 /* 0x0b4 */
    s32 mClipRadius;                 /* 0x0b8 */
    u8  pad_0bc[0x18];
    /* Model member. The cartridge's own ~daPgDfdr_c calls _ZN5ModelD1Ev at +0x0d4
       (D0/D1), a relocation the ROM build checks; recovered by tools/dtor_members.py.
       D1 and not D2, so it is this type and not an inlined base. The marker's pad
       of the object, so the member also takes over unk_0f0 (+0x1c = mat4x3) and
       unk_114/unk_118/unk_11c (+0x40/+0x44/+0x48 = mat4x3.t.x/.y/.z), which the
       header declared separately inside it. */
    Model mModel;            /* 0x0d4 */
    /* dBgW_KcMbg member. The cartridge's own ~daPgDfdr_c calls _ZN10dBgW_KcMbgD1Ev at
       +0x124 (D0/D1), a relocation the ROM build checks; recovered by
       tools/dtor_members.py. D1 and not D2, so it is this type and not an inlined base. */
    dBgW_KcMbg mMeshCollider;            /* 0x124 */
    struct Matrix4x3 mClsnMat;   /* 0x2ec */
    u8  unk_31c;                 /* 0x31c */
    u8  unk_31d;                 /* 0x31d */
    /* daPgDfdr_c's own bytes, from the header comment above. */
    u8  mModelAnim[0x64];             /* 0x320 */
    u8  mTextureSequence[0x14];       /* 0x384 */
    u8  mdCcAc_c[0x34];    /* 0x398 */
    void *mStateTable;                    /* 0x3cc */
    s32   unk_3d0;                    /* 0x3d0 */
    s32   mDistanceLeft;                    /* 0x3d4 */
    u8    mTimer;                    /* 0x3d8 */
    u8    mStepIndex;                    /* 0x3d9 */
    u8    pad_3da[0x2];
};

#endif /* __cplusplus */

#endif
