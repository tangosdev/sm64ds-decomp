#ifndef ONEUPMUSHROOM_H
#define ONEUPMUSHROOM_H

/* RECONSTRUCTED NAMES USED IN THIS HEADER. SM64DS RTTI names the
 * implementation(s) below; the registry profile object and the factory
 * spelling are Tier B reconstructions -- evidence-bounded proposals, not
 * recovered SM64DS symbols. Exact original spellings are not preserved.
 *
 *   da1up_c -- da1up_c_classInit_ONEUPKINOKO (was OneUpMushroom_Spawn), g_profile_ONEUPKINOKO (was OneUpMushroom_SpawnInfo)
 */

#include "types.h"

/* Derives from dEnemyBase_c, and TWO INDEPENDENT WITNESSES agree on the layout: the
 * class's own destructor `_ZN13OneUpMushroomD1Ev` destroys each member, and
 * `da1up_c_classInit_ONEUPKINOKO` constructs the same types at the same offsets before
 * storing `_ZTV13OneUpMushroom`. Everything this header used to restate below 0x110
 * belongs to dEnemyBase_c and dActor_c and is inherited now.
 *
 * The members close on each other, which is what makes the layout a reading
 * rather than a guess:
 *
 *     0x110 dCcAc_c         0x34    -> 0x144
 *     0x144 dBgCh_Actr               0x1bc   -> 0x300
 *     0x300 Model                      0x50    -> 0x350
 *     0x350 ShadowModel                0x28    -> 0x378
 *
 * SIZE IS THE ROM'S OWN: `da1up_c_classInit_ONEUPKINOKO` calls `fBase_c::operator new(920)`
 * -- 0x398 -- and stores this class's vtable, so that literal IS this
 * class's sizeof.
 *
 * The ROM's RTTI names this class da1up_c.
 */

#include "dEnemyBase_c.h"
#include "Model.h"
#include "dCcAc_c.h"
#include "ShadowModel.h"
#include "dBgCh_Actr.h"

struct OneUpMushroom : dEnemyBase_c {
    dCcAc_c           mdCcAc_c;   /* 0x110 */
    dBgCh_Actr                 mWithMeshClsn;         /* 0x144 */
    Model                        mModel;                /* 0x300 */
    ShadowModel                  mShadowModel;          /* 0x350 */
    s32                          unk_378;               /* 0x378 */
    s32                          unk_37c;               /* 0x37c */
    s32                          unk_380;               /* 0x380 */
    s32                          mMushroomType;         /* 0x384 */
    s32                          unk_388;               /* 0x388 */
    u8  pad_38c[0x2];
    u8                           unk_38e;               /* 0x38e */
    u8                           unk_38f;               /* 0x38f */
    s32                          unk_390;               /* 0x390 */
    s32                          unk_394;               /* 0x394 */

    /* --- vtable --- */
    virtual ~OneUpMushroom();

    virtual s32   OnYoshiTryEat();         /* slot 18 */
    virtual int   OnTurnIntoEgg(Player &player); /* slot 19 */

    /* --- non-virtual --- */
    int Behavior();
    int CleanupResources();
    int InitResources();
    void OnPendingDestroy();
    int Render();
};

typedef char OneUpMushroom_size_must_be_0x398[sizeof(OneUpMushroom) == 0x398 ? 1 : -1];

#endif /* ONEUPMUSHROOM_H */
