#ifndef DAWATER_RING_C_H
#define DAWATER_RING_C_H

#include "types.h"

/* Derives from dEnemyBase_c, and TWO INDEPENDENT WITNESSES agree on the layout:
 * the class's own destructor `_ZN14daWater_Ring_cD1Ev` destroys each member, and
 * `daWater_Ring_c_classInit` constructs the same types at the same offsets before
 * storing `_ZTV14daWater_Ring_c`. Everything this header used to restate below
 * 0x110 belongs to dEnemyBase_c and dActor_c and is inherited now.
 *
 * The members close on each other, which is what makes the layout a
 * reading rather than a guess:
 *
 *     0x110 dCcAcPos_c  0x40    -> 0x150
 *     0x150 dBgCh_Actr               0x1bc   -> 0x30c
 *     0x30c dExtSimpleModel_c                      0x50    -> 0x35c
 *     0x35c dExtAnmTexSRT_c         0x14    -> 0x370
 *
 * Typing them absorbed markers that were their insides:
 *   - unk_368 = dExtAnmTexSRT_c.speed
 *
 * SIZE IS THE ROM'S OWN: `daWater_Ring_c_classInit` calls
 * `fBase_c::operator new(912)` -- 0x390 -- and stores this class's
 * vtable, so that literal IS this class's sizeof.
 *
 * SM64DS RTTI names the implementation daWater_Ring_c. The reconstructed
 * factory daWater_Ring_c_classInit (historical alias
 * WaterRing_Spawn) constructs it for the WATER_RING
 * registry profile.
 */

#include "dEnemyBase_c.h"
#include "dExtSimpleModel_c.h"
#include "dCcAcPos_c.h"
#include "dExtAnmTexSRT_c.h"
#include "dBgCh_Actr.h"

struct daWater_Ring_c : dEnemyBase_c {
    /* One state is two member-function pointers: init runs once, when
       SetState installs the state, and execute runs every frame
       from Behavior. The tables (0x0211c944, 0x0211c954, 0x10 each) are
       filled in by the overlay's static initializer. */
    struct State {
        int (daWater_Ring_c::*init)();
        int (daWater_Ring_c::*execute)();
    };

    dCcAcPos_c mdCcAcPos_c;              /* 0x110 */
    dBgCh_Actr mWithMeshClsn;            /* 0x150 */
    dExtSimpleModel_c mModel;            /* 0x30c */
    dExtAnmTexSRT_c mTextureTransformer; /* 0x35c */
    const State *mState;                 /* 0x370 */
    /* Phase accumulators: each frame adds 0x1000 / 0x800, and the high bits
       index the sine table for the scale wobble / the spin. */
    s32 mWobblePhase; /* 0x374 */
    s32 mSpinPhase;   /* 0x378 */
    /* param1 & 0xff, clamped to 0..2. 1 is the heal ring. */
    s32 mRingType; /* 0x37c */
    u8 mOpacity;   /* 0x380 */
    u8 pad_381[0x3];
    /* Resting scale the wobble is added to; approaches 0x2000 while active. */
    s32 mScaleBase; /* 0x384 */
    /* The previous HorzAngleToCPlayer reading; the pass test compares a
       side bit of it against the current reading. */
    s16 mPrevPlayerAngle; /* 0x388 */
    u8 pad_38a[0x2];
    /* The actor that spawned this ring; stored by the spawner after
       dActor_c::Spawn (daWater_Hakidasi_c for type 2). */
    dActor_c *mSpawner; /* 0x38c */

    /* --- vtable --- */
    virtual ~daWater_Ring_c();

    int Behavior();
    int CleanupResources();
    int InitResources();
    void OnPendingDestroy();
    int Render();

    /* --- non-virtual --- */
    void CheckPlayerPass();           /* 0x02119afc: did the player swim through */
    int StatePassed();                /* 0x02119c60: grow, fade, destroy */
    int InitStatePassed();            /* 0x02119ce4: tell the spawner */
    int StateActive();                /* 0x02119d28: wobble, spin, wait */
    s32 InitStateActive();            /* 0x02119ea0 */
    int SetState(const State *state); /* 0x02119ecc: install a state, run its init */
    void UpdateModel();               /* 0x02119f1c: matrix and opacity */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daWater_Ring_c_size_must_be_0x390[sizeof(daWater_Ring_c) == 0x390 ? 1 : -1];
#endif

#endif /* DAWATER_RING_C_H */
