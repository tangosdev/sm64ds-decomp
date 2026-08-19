#ifndef BULLET_H
#define BULLET_H

#include "types.h"

/* Derives from dEnemyBase_c, on the evidence of its own destructor: `_ZN6BulletD1Ev`
 * stores this vtable, destroys its members in reverse declaration order, then
 * calls `dEnemyBase_c::~dEnemyBase_c`. Everything this header used to restate below 0x110
 * belongs to that chain and is inherited now.
 *
 * The members close exactly on one another:
 *
 *     0x110 dCcAc_c         0x34   -> 0x144
 *     0x144 dBgCh_Actr               0x1bc  -> 0x300
 *     0x300 Model                      0x50   -> 0x350
 *
 * Member NAMES are the ones this header already used -- a rebase should not
 * also rename things its callers spell.
 *
 * SIZE IS THE OBSERVED FIELD SPAN, rounded up. It guards this declaration; it
 * is not independent evidence about the ROM.
 */

#include "dEnemyBase_c.h"
#include "Model.h"
#include "ModelAnim.h"
#include "dCcAc_c.h"
#include "dCcAcPos_c.h"
#include "ShadowModel.h"
#include "TextureTransformer.h"
#include "dBgCh_Actr.h"

struct Bullet : dEnemyBase_c {
    /* What mCurrentState points at. Behavior calls the handler at +0x08
       through it; only that handler is evidenced. The field was reachable only
       through the `struct dActor_c { char pad[0x350]; Holder* h; }` stand-in that
       file used to carry, so the generated header never had it. */
    struct State {
        u8  pad_00[0x8];
        void (Bullet::*mMain)();      /* 0x08 */
    };

    dCcAc_c           mdCcAc_c;   /* 0x110 */
    dBgCh_Actr                 mWithMeshClsn;         /* 0x144 */
    Model                        mModel;                /* 0x300 */
    State                       *mCurrentState;         /* 0x350 */
    u8  pad_354[0x4];
    s32                          unk_358;               /* 0x358 */

    /* --- vtable --- */
    virtual ~Bullet();

    int Behavior();
    int CleanupResources();
    int InitResources();
    void OnPendingDestroy();
    int Render();
};

typedef char Bullet_size_must_be_0x35c[sizeof(Bullet) == 0x35c ? 1 : -1];

#endif /* BULLET_H */
