/* class daObjHatenaSwitch_c : dBgActor_c. Real C++ form.
 *
 * Base and size from the factory (src/QuestionSwitch_Spawn.c):
 * fBase_c::operator new(0x724), then dBgActor_c::dBgActor_c(), then stores
 * _ZTV19daObjHatenaSwitch_c. No intermediate base: one non-base vtable store.
 *
 * dBgActor_c ends at 0x320. Two dBgW_KcMbg members follow back to
 * back (0x324..0x4ec..0x6b4, each 0x1c8 -- confirmed by
 * src/QuestionSwitch_Spawn.c constructing both and src/_ZN19daObjHatenaSwitch_cD1Ev.cpp
 * destroying both before chaining to dBgActor_c), then a ModelAnim at 0x6b4
 * (0x64 bytes, ends exactly at 0x718). Field names below keep what
 * the legacy per-function sources already call them (mStaticMeshCollider, mMovingMeshCollider)
 * to avoid touching working call sites; only the TYPE changes, from a u8
 * marker to the real member. */
#ifndef QUESTIONSWITCH_H
#define QUESTIONSWITCH_H
#include "types.h"
#include "Model.h"
#include "dBgW_KcMbg.h"

#ifdef __cplusplus

#include "dBgActor_c.h"
#include "ModelAnim.h"

struct daObjHatenaSwitch_c : dBgActor_c {
    dBgW_KcMbg *mActiveMeshCollider;     /* 0x320 -- whichever collider below is live */
    /* The switch owns two colliders. mActiveMeshCollider points at this one
       until the switch fires, at whichever one InitResources picked from the
       saved bit, and at mMovingMeshCollider afterwards. */
    dBgW_KcMbg mStaticMeshCollider;         /* 0x324 */
    dBgW_KcMbg mMovingMeshCollider;    /* 0x4ec */
    /* ModelAnim member, named by _ZN9ModelAnimD1Ev at +0x6b4 -- a relocation
       the ROM build checks. D1 and not D2, so it is this type and not an
       inlined base. */
    ModelAnim mModelAnim;               /* 0x6b4 */
    s8  mPressTimer;        /* 0x718 -- counts down while mPressedThisFrame is set; at 0 the switch fires */
    u8  pad_719[0x1];
    u8  mPressedThisFrame;  /* 0x71a -- cleared by the last statement of every Behavior */
    u8  mTalking;           /* 0x71b -- gates the whole Player::StartTalk/ShowMessage/EndTalk block */
    u16 mSoundDelay;        /* 0x71c -- 0x4b at talk start, run down by DecIfAbove0_Short */
    u8  pad_71e[0x2];
    Player *mTalkingPlayer;        /* 0x720 */

    /* --- vtable --- */
    virtual ~daObjHatenaSwitch_c();

    virtual int Behavior();
    virtual int CleanupResources();
    virtual int InitResources();
    virtual int Render();

    /* Names for the five formerly address-only helpers are descriptive
       inferences. Ownership is stronger than the names: every direct call is
       inside 0x020b4ed8..0x020b5734, and AfterClsnCallback is installed only
       into this object's static moving-mesh collider. */
    int UpdateClsnState();
    void UpdateClsnTransform();
    void UpdateModelTransform();
    void HandleClsn(dActor_c &other);
    static void AfterClsnCallback(dBgW *collider, dActor_c *owner,
                                  dActor_c *other);

    /* Slot 21, dActor_c's combat-callback override (include/dActor_c.h).
       Attributed by the vtable: _ZTV19daObjHatenaSwitch_c + 4*21 = 0x02108e5c + 0x54
       = 0x02108eb0, config/arm9/overlays/ov002/relocs.txt confirms
       0x02108eb0 -> 0x020b4fc4. Returns `void`, matching the slot 21
       correction the sibling Platform classes already forced. */
    void OnGroundPounded(dActor_c &other);  /* slot 21 */
};

typedef char daObjHatenaSwitch_c_size_must_be_0x724[sizeof(daObjHatenaSwitch_c) == 0x724 ? 1 : -1];

#else

#include "Model.h"

/* The C spelling of the same object, flat. Retained for any leftover C
   translation unit, same arrangement as include/ShadowModel.h. */
struct daObjHatenaSwitch_c {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x26];
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x44];
    /* Model member. The cartridge's own ~daObjHatenaSwitch_c calls _ZN5ModelD1Ev at +0x0d4
       (D0/D1), a relocation the ROM build checks; recovered by tools/dtor_members.py.
       D1 and not D2, so it is this type and not an inlined base. */
    Model mModel;            /* 0x0d4 */
    /* dBgW_KcMbg member. The cartridge's own ~daObjHatenaSwitch_c calls _ZN10dBgW_KcMbgD1Ev
       at +0x124 (D0/D1), a relocation the ROM build checks; recovered by
       tools/dtor_members.py. D1 and not D2, so it is this type and not an inlined base. */
    dBgW_KcMbg mMeshCollider;            /* 0x124 */
    u8  pad_2ec[0x34];
    s32 mActiveMeshCollider;            /* 0x320 */
    u8  mStaticMeshCollider;            /* 0x324 */
    u8  pad_325[0x1c7];
    u8  mMovingMeshCollider;            /* 0x4ec */
    u8  pad_4ed[0x1c7];
    u8  mModelAnim[0x64];            /* 0x6b4 */
    s8  mPressTimer;        /* 0x718 -- counts down while mPressedThisFrame is set; at 0 the switch fires */
    u8  pad_719[0x1];
    u8  mPressedThisFrame;  /* 0x71a -- cleared by the last statement of every Behavior */
    u8  mTalking;           /* 0x71b -- gates the whole Player::StartTalk/ShowMessage/EndTalk block */
    u16 mSoundDelay;        /* 0x71c -- 0x4b at talk start, run down by DecIfAbove0_Short */
    u8  pad_71e[0x2];
    s32 mTalkingPlayer;            /* 0x720 */
};

#endif /* __cplusplus */

#endif
