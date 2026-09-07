#ifndef ICEBLOCK_H
#define ICEBLOCK_H

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
#include "dCcAc_c.h"

struct Player;

struct IceBlock : dBgActor_c {
    u8  pad_31e[0x2];
    dCcAc_c mdCcAc_c;/* 0x320 */
    u8  mMeltTimer;                   /* 0x354 -- 0x1e on the hit; drives mScale down through cstd::fdiv and destroys the block at 0 */
    u8  pad_355[0x3];
    s32 mScale;                       /* 0x358 */
    u32 mParticleHandle1;             /* 0x35c -- recycled Particle::System::New handle, effect 0x77 */
    u32 mParticleHandle2;             /* 0x360 -- the same, effect 0x78 */
    u8  mContainedActor;              /* 0x364 -- read as an actor pointer; +0xc compared against actorID 0xb2 */

    /* --- vtable --- */
    virtual ~IceBlock();

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
    virtual void OnHitByMegaChar(Player &player); /* slot 27 */
    /* THE VTABLE SAYS SO. _ZTV8IceBlock is ov081 0x02128cc4 and the word at
       +0x7c relocates to ov081 0x02127cf4, while _ZTV10dBgActor_c carries
       _ZN10dBgActor_c4KillEv at the same slot -- so this is this class's own
       override of dBgActor_c's Kill, not a new virtual. Slot 30 (+0x78) is still
       the main-module 0x02010124 both tables share, which is what makes 31 the
       first slot where they differ. An override adds no field and no slot; the
       0x368 assertion below is unchanged. */
    virtual void Kill();                            /* slot 31 */
};

typedef char IceBlock_size_must_be_0x368[sizeof(IceBlock) == 0x368 ? 1 : -1];

#else

/* The C spelling of the same object, flat. Kept because the D0 file is a C
   translation unit that reads these fields, and D0 is compiler-generated so it
   can never be migrated. Same arrangement as include/ShadowModel.h. */
struct IceBlock {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x26];
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x44];
    /* Model member, named by the class's own destructor calling
       Model's D1 at +0x0d4 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN8IceBlockD1Ev.c] */
    Model mModel;            /* 0x0d4 */
    /* dBgW_KcMbg member. The cartridge's own ~IceBlock calls _ZN10dBgW_KcMbgD1Ev at
       +0x124 (D0/D1), a relocation the ROM build checks; recovered by
       tools/dtor_members.py. D1 and not D2, so it is this type and not an inlined base. */
    dBgW_KcMbg mMeshCollider;            /* 0x124 */
    u8  pad_2ec[0x34];
    u8  mdCcAc_c;            /* 0x320 */
    u8  pad_321[0x1f];
    u32 mdCcAc_c_hitFlags;  /* 0x340 -- 0x320 + 0x20, dCc_c::hitFlags */
    u32 mdCcAc_c_otherOwner; /* 0x344 -- 0x320 + 0x24 */
    u8  pad_348[0xc];
    u8  mMeltTimer;            /* 0x354 */
    u8  pad_355[0x3];
    s32 mScale;            /* 0x358 */
    u32 mParticleHandle1;            /* 0x35c */
    u32 mParticleHandle2;            /* 0x360 */
    u8  mContainedActor;            /* 0x364 */
};

#endif /* __cplusplus */

#endif /* ICEBLOCK_H */
