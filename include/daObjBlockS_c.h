/* Derives from dBgActor_c. The vtable at ov098 0x0213c534 points at
 * _ZTI13daObjBlockS_c (0x0213c4d4), whose name string at 0x0213c500 is
 * "13daObjBlockS_c". The tree used to spell the class Crate.
 *
 * SIZE 0x608, the literal daObjBlockS_c_classInit (src/game/actors/d_a_obj_block_s.cpp) passes to
 * fBase_c::operator new. dBgActor_c ends 0x320; everything from there down is
 * this class's own, confirmed by the destructor (D1) destroying
 * dCcAcPos_c x2, dExtShadowModel_c and dBgCh_Actr in reverse before
 * storing _ZTV10dBgActor_c (inlined) and chaining to dActor_c.
 *
 * 0x0d0..0x0d4 is dBgActor_c's own generic pad (include/dBgActor_c.h), not a
 * daObjBlockS_c field -- the .cpp reaches it by raw offset as `mEatingPlayer`,
 * the name the other actor headers (include/dEnemyBase_c.h, include/daCoin_c.h)
 * give the word at 0x0d0, the same idiom dBgActor_c.h documents for
 * daObjRc_Guruguru_c's tail-padding field.
 *
 * Field names come from what the matched bodies do with each offset; the
 * per-offset citations for the older members are in notes/bgobject-provenance.md
 * and the members added later are justified in the comments beside them. Offsets with no
 * consumer in a matched body are still spelled unk_NNN. Slots 18
 * (OnYoshiTryEat), 19 (OnTurnIntoEgg), 21 (OnGroundPounded) and 31 (Kill) are
 * this class's own overrides -- see include/dActor_c.h / include/dBgActor_c.h
 * for the slot table. */
#ifndef DAOBJBLOCKS_C_H
#define DAOBJBLOCKS_C_H
#include "types.h"
#include "dExtSimpleModel_c.h"
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
#include "dExtShadowModel_c.h"
#include "dCcAcPos_c.h"

struct daObjBlockS_c : dBgActor_c {
    dBgCh_Actr mWithMeshClsn;   /* 0x320 */
    /* Face normal of the floor under the crate, fix12 (1.0 == 0x1000): the
       target of SurfaceInfo::CopyNormalTo in the slide step, read back as
       three words. The same shape Player keeps at 0x554. */
    s32 mFloorNormalX;          /* 0x4dc */
    s32 mFloorNormalY;          /* 0x4e0 */
    s32 mFloorNormalZ;          /* 0x4e4 */
    s32 mHomePosX;              /* 0x4e8 */
    s32 mHomePosY;              /* 0x4ec */
    s32 mHomePosZ;              /* 0x4f0 */
    /* Offset of the crate from the carrying player's hand, fix12: eased toward
       a per-case target row (data_ov098_0213bf60) and handed to
       dActor_c::UpdateCarry as its Vector3. Zeroed when nothing carries it. */
    s32 mCarryOffsetX;          /* 0x4f4 */
    s32 mCarryOffsetY;          /* 0x4f8 */
    s32 mCarryOffsetZ;          /* 0x4fc */
    s16 mHomeAngleX;            /* 0x500 */
    s16 mHomeAngleY;            /* 0x502 */
    s16 mHomeAngleZ;            /* 0x504 */
    u8  pad_506[0x2];
    dExtShadowModel_c mShadowModel;   /* 0x508 */
    /* Built from the Y rotation alone with its translation row set to the
       crate's position (>> 3) and handed to dActor_c::DropShadowScaleXYZ with
       mShadowModel. */
    Matrix4x3 mShadowMtx;       /* 0x530 */
    s32 mState;                 /* 0x560 -- a State */
    dCcAcPos_c mdCcAcPos_c1;    /* 0x564 */
    dCcAcPos_c mdCcAcPos_c2;    /* 0x5a4 */
    /* The ROM loads this WORD and passes it to _ZN6Player9DropActorEv as that function's
       `this`, which is an object address, so the word is a Player *. Behavior drops the
       actor through it when data_0209b454 and mFlags both have bit 0x4000000 set
       (daObjKey_c names that mFlags bit a camera takeover), so it is the player
       currently carrying the crate. It says nothing about the rest of the marker's span, which stays explicit
       padding. Was a u8 marker. */
    Player *mHoldingPlayer;     /* 0x5e4 */
    /* The actor that last let go of the crate: copied from mHoldingPlayer when
       it is thrown and from the 0x0d0 word when it is spat out. The thrown
       state's hit test will not hurt it. */
    dActor_c *mPrevHolder;      /* 0x5e8 */
    /* Y of the ground under the crate: dBgCh_Gnd::DetectClsn is run at a point
       10 units below mPosY, and this holds its clsnY, or that probe point's own
       Y when nothing is found. mPosY minus this is the shadow's drop height. */
    s32 mGroundY;               /* 0x5ec */
    /* Depth, fix12, that mClsnYOffset is easing toward. Set from the floor's
       surface type 6..9 (30 / 45 / 60 / 100 units; include/Player.h calls
       those the quicksand tiers) and zeroed on entering every state
       except STATE_IDLE. */
    s32 mClsnYOffsetTarget;     /* 0x5f0 */
    s32 mClsnYOffset;           /* 0x5f4 -- how far the model, collider and cylinders sit below mPosY */
    /* Handle threaded through Sound::PlayLong in the slide step, which plays
       it together with the sliding-dust particle. */
    u32 mSlideSoundHandle;      /* 0x5f8 */
    u32 mParticleHandle1;       /* 0x5fc */
    u32 mParticleHandle2;       /* 0x600 */
    u8  mBounceCount;           /* 0x604 -- bounces left in STATE_BOUNCING; scales the launch speeds */
    u8  mWallCooldown;          /* 0x605 -- set to 3 when func_ov002_020ef228 returns nonzero; the wall test returns early until DecIfAbove0_Byte has counted it to 0 */
    u8  mBreakTimer;            /* 0x606 */
    u8  mCoinsPaid;             /* 0x607 -- read/written by OnTurnIntoEgg (slot 19) */

    virtual ~daObjBlockS_c();

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();

    /* --- vtable, own overrides --- */
    virtual int  OnYoshiTryEat();                      /* slot 18 */
    virtual void OnTurnIntoEgg(Player &player);         /* slot 19 */
    virtual void OnGroundPounded(dActor_c &other);      /* slot 21 */
    virtual void Kill();                                /* slot 31 */

    /* The address is the name. The first argument is this crate. */
    void func_ov098_0213814c();
    void func_ov098_021381e8();
    void func_ov098_02138238();
    void func_ov098_02138318();
    void func_ov098_02138344();
    void func_ov098_02138484();
    void func_ov098_021384fc();
    void func_ov098_021385e0();
    void func_ov098_02138734();
    void func_ov098_02138818();
    void func_ov098_021388bc();
    void func_ov098_021389cc();
    void func_ov098_021389f8();
    void func_ov098_02138b18();
    void func_ov098_02138b70();
    int func_ov098_02138bb8();
    int func_ov098_02138bfc();
    void func_ov098_02138ce0();
    void func_ov098_02138e08();
    void func_ov098_02138e6c();
    void func_ov098_021390ec();
    int func_ov098_02139228();
    void func_ov098_021396a4();
    void func_ov098_021397c8();
    void func_ov098_02139850();

    /* The values of mState: each indexes one {enter, update} pair in the table
       at data_ov098_0213c878 (see src/game/actors/d_a_obj_block_s.cpp). The names
       describe what the handlers do; the ROM stores none. */
    enum State {
        STATE_IDLE      = 0,    /* sitting where it was placed or landed */
        STATE_CARRIED   = 1,    /* held by mHoldingPlayer */
        STATE_THROWN    = 2,    /* released by a player, flying */
        STATE_SPIT_OUT  = 3,    /* put in front of the player at 0x0d0 and launched */
        STATE_BOUNCING  = 4,    /* tumbling, with mBounceCount bounces left */
        STATE_IN_MOUTH  = 5,    /* entered when the 0x20000 / 0x40000 flags are set; may follow the player at 0x0d0 */
        STATE_BROKEN    = 6     /* invisible; waits to respawn at its home */
    };
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daObjBlockS_c_size_must_be_0x608[sizeof(daObjBlockS_c) == 0x608 ? 1 : -1];
#endif

#else

/* The same object for a C translation unit, flat. Kept for parity with the
 * rest of the family (include/daObjBlockL_c.h, include/daObjFallBlock_c.h)
 * even though no remaining C translation unit needs it. */
struct daObjBlockS_c {
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
    /* dExtSimpleModel_c member. The cartridge's own ~daObjBlockS_c calls _ZN17dExtSimpleModel_cD1Ev at +0x0d4 (D0/D1), a
       relocation the ROM build checks; recovered by tools/dtor_members.py. D1 and not
       D2, so it is this type and not an inlined base. */
    dExtSimpleModel_c mModel; /* 0x0d4 */
    /* dBgW_KcMbg member. The cartridge's own ~daObjBlockS_c calls _ZN10dBgW_KcMbgD1Ev at +0x124
       (D0/D1), a relocation the ROM build checks; recovered by tools/dtor_members.py.
       D1 and not D2, so it is this type and not an inlined base. */
    dBgW_KcMbg mMeshCollider;   /* 0x124 */
    u8  pad_2ec[0x34];
    u8  mWithMeshClsn;          /* 0x320 */
    u8  pad_321[0x1bb];
    s32 mFloorNormalX;          /* 0x4dc */
    s32 mFloorNormalY;          /* 0x4e0 */
    s32 mFloorNormalZ;          /* 0x4e4 */
    s32 mHomePosX;              /* 0x4e8 */
    s32 mHomePosY;              /* 0x4ec */
    s32 mHomePosZ;              /* 0x4f0 */
    s32 mCarryOffsetX;          /* 0x4f4 */
    s32 mCarryOffsetY;          /* 0x4f8 */
    s32 mCarryOffsetZ;          /* 0x4fc */
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
    void *mPrevHolder;          /* 0x5e8 */
    s32 mGroundY;               /* 0x5ec */
    s32 mClsnYOffsetTarget;     /* 0x5f0 */
    s32 mClsnYOffset;           /* 0x5f4 */
    u32 mSlideSoundHandle;      /* 0x5f8 */
    u32 mParticleHandle1;       /* 0x5fc */
    u32 mParticleHandle2;       /* 0x600 */
    u8  mBounceCount;           /* 0x604 */
    u8  mWallCooldown;          /* 0x605 */
    u8  mBreakTimer;            /* 0x606 */
    u8  mCoinsPaid;             /* 0x607 */
};

#endif /* __cplusplus */

#endif
