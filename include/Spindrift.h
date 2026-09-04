#ifndef SPINDRIFT_H
#define SPINDRIFT_H

#include "types.h"

/* Derives from dEnemyBase_c, and TWO INDEPENDENT WITNESSES agree on the layout: the
 * class's own destructor `_ZN9SpindriftD1Ev` destroys each member, and
 * `Spindrift_Spawn` constructs the same types at the same offsets before
 * storing `_ZTV9Spindrift`. Everything this header used to restate below 0x110
 * belongs to dEnemyBase_c and dActor_c and is inherited now.
 *
 * The members close on each other, which is what makes the layout a reading
 * rather than a guess:
 *
 *     0x110 ModelAnim                  0x64    -> 0x174
 *     0x174 ShadowModel                0x28    -> 0x19c
 *     0x19c dCcAc_c         0x34    -> 0x1d0
 *     0x1d0 dBgCh_Actr               0x1bc   -> 0x38c
 *
 * SIZE IS THE ROM'S OWN: `Spindrift_Spawn` calls `fBase_c::operator new(924)`
 * -- 0x39c -- and stores this class's vtable, so that literal IS this
 * class's sizeof.
 *
 * The ROM's RTTI names this class daHuwa_c.
 *
 * SM64DS proves this class as daHuwa_c through RTTI, allocation size and
 * vtable identity. The factory and profile spellings below are reconstructed
 * source-style names -- evidence-bounded proposals, not recovered SM64DS
 * symbols.
 *
 * daHuwa_c_classInit at 0x02124040 (historical alias Spindrift_Spawn)
 * allocates 0x39c and installs this class's cartridge vtable. It backs the
 * HUWAHUWA registry profile, whose descriptor at 0x02128858 is reconstructed
 * as g_profile_HUWAHUWA.
 */

#include "dEnemyBase_c.h"
#include "ModelAnim.h"
#include "dCcAc_c.h"
#include "ShadowModel.h"
#include "dBgCh_Actr.h"

struct Spindrift : dEnemyBase_c {
    ModelAnim                    mModelAnim;            /* 0x110 */
    ShadowModel                  mShadowModel;          /* 0x174 */
    dCcAc_c           mdCcAc_c;   /* 0x19c */
    dBgCh_Actr                 mWithMeshClsn;         /* 0x1d0 */
    u8  pad_38c[0xc];
    s16                          unk_398;               /* 0x398 */
    u8                           unk_39a;               /* 0x39a */
    u8  pad_39b[0x1];

    /* --- vtable --- */
    virtual ~Spindrift();

    virtual s32   OnYoshiTryEat();         /* slot 18 */
    virtual int   OnTurnIntoEgg(Player &player); /* slot 19 */
    virtual s32   OnAimedAtWithEgg();      /* slot 29 */

    /* --- non-virtual --- */
    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
};

typedef char Spindrift_size_must_be_0x39c[sizeof(Spindrift) == 0x39c ? 1 : -1];

#endif /* SPINDRIFT_H */
