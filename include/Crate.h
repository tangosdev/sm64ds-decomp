/* Derives from dBgActor_c. RTTI carries this class as `daObjBlockS_c`
 * (_ZTI13daObjBlockS_c / _ZTS13daObjBlockS_c, ov098 0x0213c4d4/0x0213c500) but
 * every mangled symbol in the ROM already spells it `Crate` (_ZTV5Crate,
 * _ZN5CrateD1Ev, Crate_Spawn, ...), so the C++ class keeps that name -- a real
 * class references its own _ZTV, and a name mismatch only fails at link.
 *
 * SIZE 0x608, the literal Crate_Spawn (src/Crate_Spawn.cpp) passes to
 * fBase_c::operator new. dBgActor_c ends 0x320; everything from there down is
 * this class's own, confirmed by _ZN5CrateD1Ev.cpp destroying
 * dCcAcPos_c x2, ShadowModel and dBgCh_Actr in reverse before
 * storing _ZTV10dBgActor_c (inlined) and chaining to dActor_c.
 *
 * 0x0d0..0x0d4 is dBgActor_c's own generic pad (include/dBgActor_c.h), not a
 * Crate field -- InitResources reaches it with a raw `this + 0xd0` write
 * instead of a named member, the same idiom dBgActor_c.h documents for
 * daObjRc_Guruguru_c's tail-padding field.
 *
 * Field names come from what the matched bodies do with each offset; the
 * per-offset citations are in notes/bgobject-provenance.md. Offsets with no
 * consumer in a matched body are still spelled unk_NNN. Slots 18
 * (OnYoshiTryEat), 19 (OnTurnIntoEgg), 21 (OnGroundPounded) and 31 (Kill) are
 * this class's own overrides -- see include/dActor_c.h / include/dBgActor_c.h
 * for the slot table. */
#ifndef CRATE_H
#define CRATE_H
#include "types.h"
#include "Model.h"
#include "dBgW_KcMbg.h"

/* Player is only ever pointed at from here, so a declaration is enough --
 * no definition is pulled in. The typedef keeps the member spelled the
 * same in C and in C++; the guard is common.h's idiom for the same job. */
#ifndef PLAYER_FWD_DECLARED
#define PLAYER_FWD_DECLARED
struct Player;
typedef struct Player Player;
#endif

#ifdef __cplusplus

#include "dBgActor_c.h"
#include "dBgCh_Actr.h"
#include "ShadowModel.h"
#include "dCcAcPos_c.h"

struct Crate : dBgActor_c {
    dBgCh_Actr mWithMeshClsn;   /* 0x320 */
    u8  pad_4dc[0xc];
    s32 mHomePosX;              /* 0x4e8 */
    s32 mHomePosY;              /* 0x4ec */
    s32 mHomePosZ;              /* 0x4f0 */
    u8  pad_4f4[0xc];
    s16 mHomeAngleX;            /* 0x500 */
    s16 mHomeAngleY;            /* 0x502 */
    s16 mHomeAngleZ;            /* 0x504 */
    u8  pad_506[0x2];
    ShadowModel mShadowModel;   /* 0x508 */
    u8  pad_530[0x30];
    s32 mState;                 /* 0x560 */
    dCcAcPos_c mdCcAcPos_c1;    /* 0x564 */
    dCcAcPos_c mdCcAcPos_c2;    /* 0x5a4 */
    /* The ROM loads this WORD and passes it to _ZN6Player9DropActorEv as that function's
       `this`, which is an object address, so the word is a Player *. Behavior drops the
       actor through it when the game pauses, so it is the player currently carrying the
       crate. It says nothing about the rest of the marker's span, which stays explicit
       padding. Was a u8 marker. */
    Player *mHoldingPlayer;     /* 0x5e4 */
    u8  pad_5e8[0xc];
    s32 mClsnYOffset;           /* 0x5f4 */
    u8  pad_5f8[0x4];
    u32 mParticleHandle1;       /* 0x5fc */
    u32 mParticleHandle2;       /* 0x600 */
    u8  pad_604[0x2];
    u8  mBreakTimer;            /* 0x606 */
    u8  mCoinsPaid;             /* 0x607 -- read/written by OnTurnIntoEgg (slot 19) */

    virtual ~Crate();

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();

    /* --- vtable, own overrides --- */
    virtual int  OnYoshiTryEat();                      /* slot 18 */
    virtual int  OnTurnIntoEgg(Player &player);         /* slot 19 */
    virtual void OnGroundPounded(dActor_c &other);      /* slot 21 */
    virtual void Kill();                                /* slot 31 */
};

typedef char Crate_size_must_be_0x608[sizeof(Crate) == 0x608 ? 1 : -1];

#else

/* The same object for a C translation unit, flat. Kept for parity with the
 * rest of the family (include/BigBrickBlock.h, include/daObjFallBlock_c.h)
 * even though no remaining C translation unit needs it. */
struct Crate {
    u8  pad_000[0xc];
    u16 mActorID;               /* 0x00c */
    u8  pad_00e[0x4e];
    s32 mPosX;                  /* 0x05c */
    s32 mPosY;                  /* 0x060 */
    s32 mPosZ;                  /* 0x064 */
    u8  pad_068[0x24];
    s16 mAngleX;                /* 0x08c */
    s16 mAngleY;                /* 0x08e */
    s16 mAngleZ;                /* 0x090 */
    u8  pad_092[0xa];
    s32 mVertAccel;             /* 0x09c */
    s32 mTerminalVelocity;      /* 0x0a0 */
    u8  pad_0a4[0xc];
    u32 mFlags;                 /* 0x0b0 */
    u8  pad_0b4[0x1c];
    s32 mEatingPlayer;          /* 0x0d0 */
    /* Model member. The cartridge's own ~Crate calls _ZN5ModelD1Ev at +0x0d4 (D0/D1), a
       relocation the ROM build checks; recovered by tools/dtor_members.py. D1 and not
       D2, so it is this type and not an inlined base. */
    Model mModel;               /* 0x0d4 */
    /* dBgW_KcMbg member. The cartridge's own ~Crate calls _ZN10dBgW_KcMbgD1Ev at +0x124
       (D0/D1), a relocation the ROM build checks; recovered by tools/dtor_members.py.
       D1 and not D2, so it is this type and not an inlined base. */
    dBgW_KcMbg mMeshCollider;   /* 0x124 */
    u8  pad_2ec[0x34];
    u8  mWithMeshClsn;          /* 0x320 */
    u8  pad_321[0x1c7];
    s32 mHomePosX;              /* 0x4e8 */
    s32 mHomePosY;              /* 0x4ec */
    s32 mHomePosZ;              /* 0x4f0 */
    u8  pad_4f4[0xc];
    s16 mHomeAngleX;            /* 0x500 */
    s16 mHomeAngleY;            /* 0x502 */
    s16 mHomeAngleZ;            /* 0x504 */
    u8  pad_506[0x2];
    u8  mShadowModel;           /* 0x508 */
    u8  pad_509[0x57];
    s32 mState;                 /* 0x560 */
    u8  mdCcAcPos_c1;           /* 0x564 */
    u8  pad_565[0x33];
    s32 mdCcAcPos_c1_posX;      /* 0x598 */
    s32 mdCcAcPos_c1_posY;      /* 0x59c */
    s32 mdCcAcPos_c1_posZ;      /* 0x5a0 */
    u8  mdCcAcPos_c2;           /* 0x5a4 */
    u8  pad_5a5[0x33];
    s32 mdCcAcPos_c2_posX;      /* 0x5d8 */
    s32 mdCcAcPos_c2_posY;      /* 0x5dc */
    s32 mdCcAcPos_c2_posZ;      /* 0x5e0 */
    Player *mHoldingPlayer;     /* 0x5e4 */
    u8  pad_5e8[0xc];
    s32 mClsnYOffset;           /* 0x5f4 */
    u8  pad_5f8[0x4];
    u32 mParticleHandle1;       /* 0x5fc */
    u32 mParticleHandle2;       /* 0x600 */
    u8  pad_604[0x2];
    u8  mBreakTimer;            /* 0x606 */
    u8  mCoinsPaid;             /* 0x607 */
};

#endif /* __cplusplus */

#endif
