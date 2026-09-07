#ifndef POWERSTAR_H
#define POWERSTAR_H

/* RECONSTRUCTED NAMES USED IN THIS HEADER. SM64DS RTTI names the
 * implementation(s) below; the registry profile object and the factory
 * spelling are Tier B reconstructions -- evidence-bounded proposals, not
 * recovered SM64DS symbols. Exact original spellings are not preserved.
 *
 *   daStar_c -- daStar_c_classInit_STAR (was PowerStar_Spawn), g_profile_STAR (was PowerStar_SpawnInfo)
 */

#include "types.h"

/* Derives from dEnemyBase_c, and TWO INDEPENDENT WITNESSES agree on the layout:
 * the class's own destructor `_ZN9PowerStarD1Ev` destroys each member, and
 * `daStar_c_classInit_STAR` constructs the same types at the same offsets before
 * storing `_ZTV9PowerStar`. Everything this header used to restate below
 * 0x110 belongs to dEnemyBase_c and dActor_c and is inherited now.
 *
 * The members close on each other, which is what makes the layout a
 * reading rather than a guess:
 *
 *     0x110 dCcAcPos_c  0x40    -> 0x150
 *     0x150 dBgCh_Actr               0x1bc   -> 0x30c
 *     0x30c ModelAnim                  0x64    -> 0x370
 *     0x370 ModelAnim                  0x64    -> 0x3d4
 *     0x3d4 ShadowModel                0x28    -> 0x3fc
 *
 * SIZE IS THE ROM'S OWN: `daStar_c_classInit_STAR` calls
 * `fBase_c::operator new(1220)` -- 0x4c4 -- and stores this class's
 * vtable, so that literal IS this class's sizeof.
 */

#include "dEnemyBase_c.h"
#include "ModelAnim.h"
#include "dCcAcPos_c.h"
#include "ShadowModel.h"
#include "dBgCh_Actr.h"

struct PowerStar : dEnemyBase_c {
    dCcAcPos_c    mdCc_c;         /* 0x110 */
    dBgCh_Actr                 mWithMeshClsn;         /* 0x150 */
    ModelAnim                    mModelAnim1;           /* 0x30c */
    ModelAnim                    mModelAnim2;           /* 0x370 */
    ShadowModel                  mShadowModel;          /* 0x3d4 */
    u8  pad_3fc[0x40];
    s32                          unk_43c;               /* 0x43c */
    s32                          unk_440;               /* 0x440 */
    u8  pad_444[0x54];
    s8                           unk_498;               /* 0x498 */
    u8  pad_499[0x1];
    u8                           unk_49a;               /* 0x49a */
    u8  pad_49b[0x2];
    u8                           unk_49d;               /* 0x49d */
    u8  pad_49e[0x1];
    u8                           unk_49f;               /* 0x49f */
    u8  pad_4a0[0x2];
    u16 unk_4a2;              /* 0x4a2 */
    u8  pad_4a4[0x4];
    s32                          unk_4a8;               /* 0x4a8 */
    s32                          unk_4ac;               /* 0x4ac */
    s32                          unk_4b0;               /* 0x4b0 */
    u8  pad_4b4[0x10];

    /* --- vtable --- */
    virtual ~PowerStar();

    virtual s32   OnYoshiTryEat();         /* slot 18 */
    virtual int   OnTurnIntoEgg(Player &player); /* slot 19 */

    void AddStarMarker();
    int Behavior();
    int CleanupResources();
    s32 InitResources();
    int Render();
};

typedef char PowerStar_size_must_be_0x4c4[sizeof(PowerStar) == 0x4c4 ? 1 : -1];

#endif /* POWERSTAR_H */
