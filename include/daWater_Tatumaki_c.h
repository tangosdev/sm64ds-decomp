#ifndef DAWATER_TATUMAKI_C_H
#define DAWATER_TATUMAKI_C_H

#include "types.h"

/* Derives from dEnemyBase_c, on the evidence of its own destructor: `_ZN18daWater_Tatumaki_cD1Ev`
 * stores this vtable, destroys its members in reverse declaration order, then
 * calls `dEnemyBase_c::~dEnemyBase_c`. Everything this header used to restate below 0x110
 * belongs to that chain and is inherited now.
 *
 * The members close exactly on one another:
 *
 *     0x114 dExtAnmModel_c                  0x64   -> 0x178
 *     0x178 dExtAnmTexSRT_c         0x14   -> 0x18c
 *
 * Typing them absorbed these markers, which were a member's insides:
 *   - 0x184 unk_184      = mTextureTransformer.speed (dExtFrameCtrl_c +0x0c)
 *
 * Member NAMES are the ones this header already used -- a rebase should not
 * also rename things its callers spell.
 *
 * SIZE IS THE OBSERVED FIELD SPAN, rounded up. It guards this declaration; it
 * is not independent evidence about the ROM.
 *
 * SM64DS RTTI names the implementation daWater_Tatumaki_c. The reconstructed factory
 * daWater_Tatumaki_c_classInit (historical alias Whirlpool_Spawn) installs this class's
 * cartridge vtable; the reconstructed profile global g_profile_WATER_TATUMAKI
 * (historical alias Whirlpool_SpawnInfo) is its registry descriptor.
 */

#include "dEnemyBase_c.h"
#include "dExtSimpleModel_c.h"
#include "dExtAnmModel_c.h"
#include "dCcAc_c.h"
#include "dCcAcPos_c.h"
#include "dExtShadowModel_c.h"
#include "dExtAnmTexSRT_c.h"
#include "dBgCh_Actr.h"

struct daWater_Tatumaki_c : dEnemyBase_c {
    /* One state of the whirlpool: `enter` runs once when the state is
       installed (SetState), `execute` every frame from Behavior.
       The two records live in ov026 .bss (0x02113f2c, 0x02113f3c); the
       static initializer 0x02112c94 copies each pair of member pointers in
       from .data 0x02113cec..0x02113d04. State, enter and execute are coined
       names. */
    struct State {
        int (daWater_Tatumaki_c::*enter)();
        int (daWater_Tatumaki_c::*execute)();
    };

    const State *mState;                 /* 0x110 */
    dExtAnmModel_c mAnmModel;            /* 0x114 */
    dExtAnmTexSRT_c mTextureTransformer; /* 0x178 */
    /* Set once the player has been dragged under the whirlpool's centre and
       KillPlayer has run, so it runs once. */
    s32 mPlayerKilled; /* 0x18c */
    s32 unk_190;       /* 0x190 -- only ever zeroed */
    /* The captured player orbits mCenter: mPullRadius out along mPullAngle,
       at height mPullHeight, and mPullPos chases that point. */
    s32 mPullHeight;  /* 0x194 */
    s32 mPullRadius;  /* 0x198 */
    Vector3 mPullPos; /* 0x19c */
    /* The spawn position lowered by 0x64 -- the bottom of the funnel. */
    Vector3 mCenter; /* 0x1a8 */
    s16 mPullAngle;  /* 0x1b4 */
    s16 mPullSpin;   /* 0x1b6 -- added to mPullAngle each frame */
    u32 mParticleID; /* 0x1b8 -- Particle::System::New handle, effect 0x139 */

    /* --- vtable --- */
    virtual ~daWater_Tatumaki_c();

    /* --- non-virtual --- */
    int StateDrag();             /* ov026 0x02111b24 */
    int InitStateDrag();         /* ov026 0x02111cb4 */
    int StateSpin();             /* ov026 0x02111d4c */
    int InitStateSpin();         /* ov026 0x02111ed8 */
    int SetState(const State *); /* ov026 0x02111ee0 */
    void UpdateModelMatrix();    /* ov026 0x02111f30 */

    int Behavior();
    int InitResources();
    int Render();
    int CleanupResources();
    void OnPendingDestroy();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daWater_Tatumaki_c_size_must_be_0x1bc[sizeof(daWater_Tatumaki_c) == 0x1bc ? 1 : -1];
#endif

#endif /* DAWATER_TATUMAKI_C_H */
