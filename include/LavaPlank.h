#ifndef LAVAPLANK_H
#define LAVAPLANK_H

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

struct LavaPlank : dBgActor_c {
    u8  pad_31e[0x2];
    s32 mOriginalPosY;                      /* 0x320 */
    s16 mPhaseAngle;                  /* 0x324 -- seeded from mAngleX, += 0x400 per Behavior; (>>4) indexes the sine table */

    /* --- vtable --- */
    virtual ~LavaPlank();

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
};

typedef char LavaPlank_size_must_be_0x328[sizeof(LavaPlank) == 0x328 ? 1 : -1];

#else

/* The C spelling of the same object, flat. Kept because the D0 file is a C
   translation unit that reads these fields, and D0 is compiler-generated so it
   can never be migrated. Same arrangement as include/ShadowModel.h. */
struct LavaPlank {
    u8  pad_000[0x60];
    s32 mPosY;            /* 0x060 */
    u8  pad_064[0x28];
    s16 mAngleX;            /* 0x08c */
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x44];
    /* Model member, named by _ZN5ModelD1Ev at +0xd4 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    Model mModel;            /* 0x0d4 */
    /* dBgW_KcMbg member. The cartridge's own ~LavaPlank calls _ZN10dBgW_KcMbgD1Ev at
       +0x124 (D0/D1), a relocation the ROM build checks; recovered by
       tools/dtor_members.py. D1 and not D2, so it is this type and not an inlined base. */
    dBgW_KcMbg mMeshCollider;            /* 0x124 */
    u8  pad_2ec[0x34];
    s32 mOriginalPosY;            /* 0x320 */
    s16 mPhaseAngle;            /* 0x324 */
};

#endif /* __cplusplus */

#endif /* LAVAPLANK_H */
