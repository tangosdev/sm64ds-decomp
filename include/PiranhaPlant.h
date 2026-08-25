#ifndef PIRANHAPLANT_H
#define PIRANHAPLANT_H

#include "types.h"

/* Derives from dEnemyBase_c, on the evidence of its own destructor: `_ZN12PiranhaPlantD1Ev`
 * stores this vtable, destroys six members, then calls `dEnemyBase_c::~dEnemyBase_c`.
 * Everything this header used to restate below 0x110 belongs to that chain and
 * is inherited now.
 *
 * The members close exactly on one another, and dEnemyBase_c's own 0x110 closes
 * exactly on the first:
 *
 *     0x110 ModelAnim                  0x64   -> 0x174
 *     0x174 Model                      0x50   -> 0x1c4
 *     0x1c4 dBgCh_Actr               0x1bc  -> 0x380
 *     0x380 dCcAc_c         0x34   -> 0x3b4
 *     0x3b4 dCcAc_c         0x34   -> 0x3e8
 *     0x3e8 dCcAcPos_c  0x40   -> 0x428
 *
 * Typing them absorbed these markers, which were a member's insides:
 *   - 0x160 mAnimation   = the Animation base of mModelAnim
 *   - 0x170 unk_170      = mModelAnim.file (+0x60)
 *
 * SIZE IS THE OBSERVED FIELD SPAN, rounded up. It guards this declaration; it
 * is not independent evidence about the ROM.
 */

#include "dEnemyBase_c.h"
#include "ModelAnim.h"
#include "Model.h"
#include "dCcAc_c.h"
#include "dCcAcPos_c.h"
#include "dBgCh_Actr.h"

struct PiranhaPlant : dEnemyBase_c {
    ModelAnim                    mModelAnim;            /* 0x110 */
    Model                        mModel;                /* 0x174 */
    dBgCh_Actr                 mWithMeshClsn;         /* 0x1c4 */
    dCcAc_c           mdCcAc_c1;  /* 0x380 */
    dCcAc_c           mdCcAc_c2;  /* 0x3b4 */
    dCcAcPos_c    mdCcAcPos_c; /* 0x3e8 */
    Vector3                      mPipeScale;            /* 0x428 */
    Vector3                      mFirePos;              /* 0x434 */
    Vector3                      mSpawnPos;             /* 0x440 */
    Vector3                      mHomePos;              /* 0x44c */
    s32                          mState;                /* 0x458 */
    u8                           mClsnEnabled;          /* 0x45c */
    u8                           unk_45d;               /* 0x45d */
    u8  pad_45e[0x2];
    s32                          unk_460;               /* 0x460 */
    s32                          unk_464;               /* 0x464 */
    s16                          mInitAngleY;           /* 0x468 */
    u8  pad_46a[0x2];
    s32                          unk_46c;               /* 0x46c */
    s32                          mParticleHandle;       /* 0x470 */
    s32                          unk_474;               /* 0x474 */
    s32                          unk_478;               /* 0x478 */

    /* --- vtable --- */
    virtual ~PiranhaPlant();

    virtual s32   OnAimedAtWithEgg();      /* slot 29 */

    int Behavior();
    int CleanupResources();
    int InitResources();
    void OnPendingDestroy();
    int Render();
};

typedef char PiranhaPlant_size_must_be_0x47c[sizeof(PiranhaPlant) == 0x47c ? 1 : -1];

#endif /* PIRANHAPLANT_H */
