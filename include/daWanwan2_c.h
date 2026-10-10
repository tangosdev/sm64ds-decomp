#ifndef DAWANWAN2_C_H
#define DAWANWAN2_C_H

#include "types.h"
#include "dEnemyBase_c.h"
#include "Model.h"
#include "ModelAnim.h"
#include "dCcAcPos_c.h"
#include "dExtShadowModel_c.h"
#include "dBgCh_Actr.h"

/* The chain has six links; Render draws only five of the six link models. */
enum { daWanwan2_NUM_LINKS = 6 };

/* daWanwan2_c in the ROM's RTTI. Derives from dEnemyBase_c, and the destructor is an
 * unusually strong witness because five of the nine members are ARRAYS: __cxa_vec_cleanup
 * takes a count and a stride, so it names not just the type at an offset but how many
 * and how far apart. The members with destructors sit at 0x370..0x78c with no overlap
 * (the plain fields at 0x668..0x6d8 lie between the single dExtShadowModel_c and the link
 * arrays):
 *
 *     0x370  Model       x6   stride 0x50
 *     0x550  dExtShadowModel_c x6   stride 0x28
 *     0x640  dExtShadowModel_c  1
 *     0x6d8  Vector3     x6   stride 0x0c
 *     0x720  Vector3     x6   stride 0x0c
 *     0x768  Vector3s    x6   stride 0x06   -> ends 0x78c
 *
 * daWanwan2_c_classInit constructs the same five arrays through __cxa_vec_ctor, which takes the
 * same counts and strides, and allocates 0x7a4. The 0x18 after the last array is six
 * plain s32, mLinkMinY, which the chain step reads and writes (a plain int array has no
 * constructor or destructor to witness it).
 *
 * SM64DS RTTI names the implementation daWanwan2_c: _ZTI11daWanwan2_c at
 * 0x02148014, _ZTS11daWanwan2_c at 0x02148020, and the vtable
 * _ZTV11daWanwan2_c at 0x02148054 that the factory and destructor store.
 * The reconstructed factory daWanwan2_c_classInit (historical alias
 * UnchainedChomp_Spawn) constructs it for the WANWAN2
 * registry profile.
 */
struct daWanwan2_c : dEnemyBase_c {
    dCcAcPos_c mdCcAcPos_c;  /* 0x110 */
    dBgCh_Actr        mWithMeshClsn;      /* 0x150 */
    ModelAnim           mModelAnim;         /* 0x30c -- the chomp's body; its mat4x3 is the body matrix */
    Model               mModels[6];                           /* 0x370 -- one per chain link; mat4x3 is that link's matrix */
    dExtShadowModel_c         mShadowModels[6];                     /* 0x550 -- the links' drop shadows */
    dExtShadowModel_c         mShadowModel;       /* 0x640 -- the body's drop shadow */
    /* The current state: a pair of pointers-to-member (enter, execute) that
       func_ov100_02143b18 stores and calls and Behavior calls every frame --
       see ChompState in src/actors/daWanwan2_c.cpp. */
    void               *mStatePair;         /* 0x668 */
    u8  pad_66c[0x34];
    /* Counts how many times func_ov100_021437d4 has stepped the chain; the
       links bob on a sine of it. Neither InitResources nor the factory
       sets it, so it starts at 0 (fBase_c::operator new clears the
       allocation). */
    s32 mChainTimer;                        /* 0x6a0 */
    /* The heading to the path node the chomp is walking to: Behavior stores it
       (Vec3_HorzAngle) on each frame it is not halted and eases mPrevAngleY
       toward it, and InitResources seeds it with the placed angle. */
    s16 mTargetAngle;                       /* 0x6a4 */
    /* Set to 90 by func_ov100_0214344c on a hit. While Behavior's count-down
       leaves it nonzero (89 frames) the chomp is halted: no horizontal speed,
       and the state's execute member (walk animation, chain step, contact
       check) is not run. */
    u16 mHaltTimer;                         /* 0x6a6 */
    /* Counted down in Behavior, and the two sounds there play only while it is
       zero. Nothing in daWanwan2_c.cpp sets it. */
    u16 unk_6a8;                            /* 0x6a8 */
    u8  pad_6aa[0x2];
    s32 mPathID;                            /* 0x6ac -- param1's low byte (0xff reads as 0) */
    s32 mNumPathNodes;                      /* 0x6b0 -- PathPtr::NumNodes() of that path */
    s32 mPathNodeIndex;                     /* 0x6b4 -- the node it is walking to; starts at 1 */
    /* param1 bits 8..11. The only reader ORs 0x30 into it and hands it to the
       STAR and the STARBASE that func_ov100_021435e8 spawns in game mode 1. */
    s32 mSpawnParam;                        /* 0x6b8 */
    u8  pad_6bc[0xc];
    /* Latched by func_ov100_021435e8 once the actor it spawned at the last link
       is gone or has reached its state 5; after that it does nothing. */
    u8  mChainEndDone;                      /* 0x6c8 */
    u8  unk_6c9;                            /* 0x6c9 -- InitResources sets 0x1f; not read in daWanwan2_c.cpp */
    /* Frames until Behavior next spawns a COIN (in game mode 1); InitResources
       and Behavior set it to 200. */
    u16 mCoinTimer;                         /* 0x6ca */
    s32 unk_6cc;                            /* 0x6cc -- InitResources sets 3; not read in daWanwan2_c.cpp */
    /* uniqueID of the actor func_ov100_021435e8 spawned at the last link
       (0 = none: not spawned yet, or finished). */
    s32 mChainEndActorID;                   /* 0x6d0 */
    s32 unk_6d4;                            /* 0x6d4 -- zeroed by InitResources; not read in daWanwan2_c.cpp */
    /* The six chain links, by func_ov100_021437d4: the link positions, then each
       link's velocity: its movement in the latest chain step scaled by
       3000/4096 (for link 0, the movement from the anchor 250 units behind the
       chomp, not from its old position). Link 5 is the last; func_ov100_021435e8
       keeps its actor there. */
    Vector3             mLinkPos[6];                          /* 0x6d8 */
    Vector3             mLinkVel[6];                          /* 0x720 */
    Vector3s            mUnk_768[6];                          /* 0x768 -- constructed and destroyed, never read in this class's source */
    /* A lower bound on the Y that func_ov100_021437d4 drifts link i toward (the
       link, placed 50 units from the one before it, can still end below it).
       Each is rewritten every step as the chomp's Y minus 200 units, or the
       link's own Y when that bound would be more than 200 units above it.
       InitResources does not set them, so the first step clamps against 0. */
    s32                 mLinkMinY[6];                         /* 0x78c */

    virtual ~daWanwan2_c();

    virtual s32   OnAimedAtWithEgg();      /* slot 29 */

    /* methods */
    int Behavior();
    int CleanupResources();
    int InitResources();
    void OnPendingDestroy();
    int Render();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daWanwan2_c_size_must_be_0x7a4[sizeof(daWanwan2_c) == 0x7a4 ? 1 : -1];
#endif

#endif /* DAWANWAN2_C_H */
