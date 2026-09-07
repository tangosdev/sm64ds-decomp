#ifndef DAOBJWC_MIZU_C_H
#define DAOBJWC_MIZU_C_H

#include "types.h"
#include "dBgW_KcMbg.h"

/* Derives from dBgActor_c: the destructor stores this class's vtable, then
 * dBgActor_c's -- inlined -- then destroys the dBgW_KcMbg at 0x124 and
 * the Model at 0xd4 before chaining to dActor_c. All three belong to dBgActor_c.
 * Everything this header used to restate below 0x31e was dActor_c's and
 * dBgActor_c's, and is inherited now.
 *
 * SIZE IS THE OBSERVED FIELD SPAN, rounded up. It guards this declaration; it
 * is not independent evidence about the ROM.
 */

#ifdef __cplusplus

#include "dBgActor_c.h"
#include "TextureTransformer.h"

struct daObjWc_Mizu_c : dBgActor_c {
    u8  pad_31e[0x2];
    TextureTransformer mTextureTransformer;/* 0x320 */
    s32 mTargetPosY;                  /* 0x334 */
    u8 mUseSpawnPosY;                 /* 0x338 -- param1 & 1; when clear InitResources overrides mPosY from data_ov029_02112b2c[setting] */
    u8  pad_339[0x3];
    s32 mSoundID;                      /* 0x33c */
    u8 mTrueAreaID;                       /* 0x340 */
    u8  pad_341[0x3];
    s32 mWaterHeight;                      /* 0x344 */

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();

    /* --- vtable --- */
    /* MEASURED -- DECLARED LAST ON PURPOSE, after the non-virtual methods.
       Nothing DEFINES this destructor as a C++ member (D1 and D0 are carried in
       src/actors/d_a_obj_wc_mizu.cpp as `// @symbol` marked mangled bodies, for
       the emission-order reason that file's header gives), so the class's vtable
       and RTTI have no home. With the declaration LAST, mwccarm still emits them
       as vague linkage into the TU that defines the class's members, and
       tools/romdata_check.py word-compares that emitted copy against the
       cartridge; with the declaration FIRST it emits nothing at all and those
       three ROM records go unverified by any source. Measured both ways on this
       class: first -> 0 data symbols emitted, last -> 11 (6 VERIFIED, 5 PARTIAL).
       include/daObjWc_Obj04_c.h has the same shape for the same reason. */
    virtual ~daObjWc_Mizu_c();
};

typedef char daObjWc_Mizu_c_size_must_be_0x348[sizeof(daObjWc_Mizu_c) == 0x348 ? 1 : -1];

#else

/* The C spelling of the same object, flat. Same arrangement as
   include/ShadowModel.h.

   NOTHING COMPILES THIS BRANCH TODAY. It was kept for a D0 file that was said
   to be a C translation unit; it never was (src/_ZN9WDW_WaterD0Ev.cpp was
   //cpp), and the whole class was absorbed into src/actors/d_a_obj_wc_mizu.cpp,
   where D0 is a hand-written `// @symbol` body rather than anything
   compiler-generated. Every remaining includer of this header is //cpp, so the
   #if arm above is the one that builds. Left in place rather than deleted
   because the offsets below are measured and are the only flat record of this
   layout; delete it once nothing wants that record. */
struct daObjWc_Mizu_c {
    u8  pad_000[0x8];
    s32 mParam;            /* 0x008 */
    u8  pad_00c[0x54];
    s32 mPosY;            /* 0x060 */
    u8  pad_064[0x8];
    s32 mPrevPosY;            /* 0x06c */
    u8  pad_070[0x1e];
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x3c];
    s8  mAreaId;            /* 0x0cc */
    u8  pad_0cd[0x7];
    /* Model member, named by _ZN5ModelD1Ev at +0xd4 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. The marker's pad stopped
       short of the object, so the member also takes over unk_0dc (+0x8 = data), which the
       header declared separately inside it. */
    Model mModel;            /* 0x0d4 */
    /* dBgW_KcMbg member. The cartridge's own ~daObjWc_Mizu_c calls _ZN10dBgW_KcMbgD1Ev at
       +0x124 (D0/D1), a relocation the ROM build checks; recovered by
       tools/dtor_members.py. D1 and not D2, so it is this type and not an inlined base. */
    dBgW_KcMbg mMeshCollider;            /* 0x124 */
    u8  pad_2ec[0x34];
    TextureTransformer mTextureTransformer; /* 0x320 */
    s32 mTargetPosY;            /* 0x334 */
    u8  mUseSpawnPosY;            /* 0x338 */
    u8  pad_339[0x3];
    s32 mSoundID;            /* 0x33c */
    u8  mTrueAreaID;            /* 0x340 */
    u8  pad_341[0x3];
    s32 mWaterHeight;            /* 0x344 */
};

#endif /* __cplusplus */

#endif /* DAOBJWC_MIZU_C_H */
