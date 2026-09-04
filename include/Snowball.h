#ifndef SNOWBALL_H
#define SNOWBALL_H

#include "types.h"

/* Derives from dEnemyBase_c, on the evidence of its own destructor: `_ZN8SnowballD1Ev`
 * stores this vtable, destroys its members in reverse declaration order, then
 * calls `dEnemyBase_c::~dEnemyBase_c`. Everything this header used to restate below 0x110
 * belongs to that chain and is inherited now.
 *
 * The members close exactly on one another:
 *
 *     0x110 dCcAc_c         0x34   -> 0x144
 *     0x144 dBgCh_Actr               0x1bc  -> 0x300
 *     0x300 Model                      0x50   -> 0x350
 *     0x350 ShadowModel                0x28   -> 0x378
 *
 * Member NAMES are the ones this header already used -- a rebase should not
 * also rename things its callers spell.
 *
 * SIZE IS THE ROM'S OWN, not a rounded-up field span: `Snowball_Spawn` calls
 * `fBase_c::operator new(908)` -- 0x38c -- and stores `_ZTV8Snowball`,
 * so that literal IS this class's sizeof. The observed fields only span to
 * 0x388; the difference is trailing space no source reads.
 *
 * SM64DS proves this class as daSnowball_c through RTTI, allocation size and
 * vtable identity. The factory and profile spellings below are reconstructed
 * source-style names -- evidence-bounded proposals, not recovered SM64DS
 * symbols.
 *
 * daSnowball_c_classInit at 0x021264b4 (historical alias Snowball_Spawn)
 * allocates 0x38c and installs this class's cartridge vtable. It backs the
 * SNOWBALL registry profile, whose descriptor at 0x02128a98 is reconstructed
 * as g_profile_SNOWBALL.
 */

#include "dEnemyBase_c.h"
#include "Model.h"
#include "ModelAnim.h"
#include "dCcAc_c.h"
#include "dCcAcPos_c.h"
#include "ShadowModel.h"
#include "TextureTransformer.h"
#include "dBgCh_Actr.h"

struct Snowball : dEnemyBase_c {
    dCcAc_c           mdCcAc_c;   /* 0x110 */
    dBgCh_Actr                 mWithMeshClsn;         /* 0x144 */
    Model                        mModel;                /* 0x300 */
    ShadowModel                  mShadowModel;          /* 0x350 */
    s32                          unk_378;               /* 0x378 */
    s32                          unk_37c;               /* 0x37c */
    s32                          unk_380;               /* 0x380 */
    s32                          unk_384;               /* 0x384 */
    u8  pad_388[0x4];

    /* --- vtable --- */
    virtual ~Snowball();

    int Behavior();
    int InitResources();
    int Render();
    int CleanupResources();
    void OnPendingDestroy();
};

typedef char Snowball_size_must_be_0x38c[sizeof(Snowball) == 0x38c ? 1 : -1];

#endif /* SNOWBALL_H */
