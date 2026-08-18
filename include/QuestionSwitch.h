/* class QuestionSwitch : dBgActor_c. Real C++ form.
 *
 * Base and size from the factory (src/QuestionSwitch_Spawn.c):
 * fBase_c::operator new(0x724), then dBgActor_c::dBgActor_c(), then stores
 * _ZTV14QuestionSwitch. No intermediate base: one non-base vtable store.
 *
 * dBgActor_c ends at 0x320. Two MovingMeshCollider members follow back to
 * back (0x324..0x4ec..0x6b4, each 0x1c8 -- confirmed by
 * src/QuestionSwitch_Spawn.c constructing both and src/_ZN14QuestionSwitchD1Ev.cpp
 * destroying both before chaining to dBgActor_c), then a ModelAnim at 0x6b4
 * (0x64 bytes, ends exactly at 0x718). Field names below keep what
 * src/_ZN14QuestionSwitch*.cpp already call them (unk_324, mMovingMeshCollider)
 * to avoid touching working call sites; only the TYPE changes, from a u8
 * marker to the real member. */
#ifndef QUESTIONSWITCH_H
#define QUESTIONSWITCH_H
#include "types.h"

#ifdef __cplusplus

#include "dBgActor_c.h"
#include "ModelAnim.h"

struct QuestionSwitch : dBgActor_c {
    s32 mActiveMeshCollider;            /* 0x320 -- points at whichever MMC below is live */
    MovingMeshCollider unk_324;         /* 0x324 */
    MovingMeshCollider mMovingMeshCollider;    /* 0x4ec */
    /* ModelAnim member, named by _ZN9ModelAnimD1Ev at +0x6b4 -- a relocation
       the ROM build checks. D1 and not D2, so it is this type and not an
       inlined base. */
    ModelAnim mModelAnim;               /* 0x6b4 */
    s8  unk_718;            /* 0x718 */
    u8  pad_719[0x1];
    u8  unk_71a;            /* 0x71a */
    u8  unk_71b;            /* 0x71b */
    u16 unk_71c;            /* 0x71c */
    u8  pad_71e[0x2];
    s32 mTalkingPlayer;            /* 0x720 */

    /* --- vtable --- */
    virtual ~QuestionSwitch();

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();

    /* Slot 21, dActor_c's combat-callback override (include/dActor_c.h).
       Attributed by the vtable: _ZTV14QuestionSwitch + 4*21 = 0x02108e5c + 0x54
       = 0x02108eb0, config/arm9/overlays/ov002/relocs.txt confirms
       0x02108eb0 -> 0x020b4fc4. Returns `void`, matching the slot 21
       correction the sibling Platform classes already forced. */
    void OnGroundPounded(dActor_c &other);  /* slot 21 */
};

typedef char QuestionSwitch_size_must_be_0x724[sizeof(QuestionSwitch) == 0x724 ? 1 : -1];

#else

#include "Model.h"

/* The C spelling of the same object, flat. Retained for any leftover C
   translation unit, same arrangement as include/ShadowModel.h. */
struct QuestionSwitch {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x26];
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x44];
    u8  mModel;            /* 0x0d4 */
    u8  pad_0d5[0x4f];
    u8  unk_124;            /* 0x124 */
    u8  pad_125[0x1fb];
    s32 mActiveMeshCollider;            /* 0x320 */
    u8  unk_324;            /* 0x324 */
    u8  pad_325[0x1c7];
    u8  mMovingMeshCollider;            /* 0x4ec */
    u8  pad_4ed[0x1c7];
    u8  mModelAnim[0x64];            /* 0x6b4 */
    s8  unk_718;            /* 0x718 */
    u8  pad_719[0x1];
    u8  unk_71a;            /* 0x71a */
    u8  unk_71b;            /* 0x71b */
    u16 unk_71c;            /* 0x71c */
    u8  pad_71e[0x2];
    s32 mTalkingPlayer;            /* 0x720 */
};

#endif /* __cplusplus */

#endif
