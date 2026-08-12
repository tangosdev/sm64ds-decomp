#ifndef SKEETER_H
#define SKEETER_H

#include "types.h"

/* Derives from Enemy, and TWO INDEPENDENT WITNESSES agree on the layout:
 * the class's own destructor `_ZN7SkeeterD1Ev` destroys each member, and
 * `Skeeter_Spawn` constructs the same types at the same offsets before
 * storing `_ZTV7Skeeter`. Everything this header used to restate below
 * 0x110 belongs to Enemy and Actor and is inherited now.
 *
 * The members close on each other, which is what makes the layout a
 * reading rather than a guess:
 *
 *     0x110 MovingCylinderClsnWithPos  0x40    -> 0x150
 *     0x150 WithMeshClsn               0x1bc   -> 0x30c
 *     0x30c ModelAnim                  0x64    -> 0x370
 *
 * SIZE IS THE ROM'S OWN: `Skeeter_Spawn` calls
 * `ActorBase::operator new(944)` -- 0x3b0 -- and stores this class's
 * vtable, so that literal IS this class's sizeof.
 */

#include "Enemy.h"
#include "ModelAnim.h"
#include "MovingCylinderClsnWithPos.h"
#include "WithMeshClsn.h"

struct Skeeter : Enemy {
    MovingCylinderClsnWithPos    mMovingCylinderClsnWithPos; /* 0x110 */
    WithMeshClsn                 mWithMeshClsn;         /* 0x150 */
    ModelAnim                    mModelAnim;            /* 0x30c */
    void*                        mState;                /* 0x370 */
    s32                          unk_374;               /* 0x374 */
    s32                          unk_378;               /* 0x378 */
    s32                          unk_37c;               /* 0x37c */
    u8  pad_380[0x14];
    u16                          unk_394;               /* 0x394 */
    u16                          unk_396;               /* 0x396 */
    u16                          unk_398;               /* 0x398 */
    u16                          unk_39a;               /* 0x39a */
    u8                           unk_39c;               /* 0x39c */
    u8  pad_39d[0x4];
    u8                           unk_3a1;               /* 0x3a1 */
    u8  pad_3a2[0x2];
    s32                          unk_3a4;               /* 0x3a4 */
    s32                          unk_3a8;               /* 0x3a8 */
    s32                          unk_3ac;               /* 0x3ac */

    /* --- vtable --- */
    virtual ~Skeeter();

    virtual s32   OnYoshiTryEat();         /* slot 18 */
    virtual s32   OnAimedAtWithEgg();      /* slot 29 */

    int Behavior();
    int CleanupResources();
    int InitResources();
    void OnPendingDestroy();
    int Render();
};

typedef char Skeeter_size_must_be_0x3b0[sizeof(Skeeter) == 0x3b0 ? 1 : -1];

#endif /* SKEETER_H */
