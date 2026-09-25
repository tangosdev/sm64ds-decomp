#ifndef DAOBJKEY_C_H
#define DAOBJKEY_C_H

#include "types.h"

/* daObjKey_c derives from dEnemyBase_c: _ZTI10daObjKey_c at 0x02132b4c is an
 * __si_class_type_info naming _ZTI12dEnemyBase_c. The destructor
 * _ZN10daObjKey_cD1Ev destroys each typed member below, and
 * daObjKey_c_classInit_OBJ_KEY (0x02132828, historical alias Key_Spawn)
 * constructs the same types at the same offsets:
 *
 *     0x114 ModelAnim     0x64    -> 0x178
 *     0x178 Model         0x50    -> 0x1c8
 *     0x1c8 ShadowModel   0x28    -> 0x1f0
 *     0x220 dCcAcPos_c    0x40    -> 0x260
 *     0x260 dBgCh_Actr    0x1bc   -> 0x41c
 *
 * The first own field is at 0x110, a pointer to the Player collecting the key.
 * The factory calls fBase_c::operator new(0x470), so sizeof is 0x470. It backs
 * the OBJ_KEY registry profile, whose descriptor at 0x02132b68 is
 * reconstructed as g_profile_OBJ_KEY; the factory and profile spellings are
 * reconstructed names, not recovered symbols.
 */

#include "dEnemyBase_c.h"
#include "Model.h"
#include "ModelAnim.h"
#include "dCcAcPos_c.h"
#include "ShadowModel.h"
#include "dBgCh_Actr.h"

struct daObjKey_c : dEnemyBase_c {
    /* No member name here is a recovered symbol; "coined" marks names given
       from the field's uses in this class's code. */
    Player      *mPlayer;           /* 0x110 coined: the Player collecting the key */
    ModelAnim   mModelAnim;         /* 0x114 */
    Model       mModel;             /* 0x178 */
    ShadowModel mShadowModel;       /* 0x1c8 */
    Matrix4x3   mShadowMatrix;      /* 0x1f0 coined: DropShadowRadHeight matrix */
    dCcAcPos_c  mdCcAcPos_c;        /* 0x220 */
    dBgCh_Actr  mWithMeshClsn;      /* 0x260 */
    Vector3     mHomePos;           /* 0x41c coined: mPos at InitResources */
    u8          pad_428[0xc];
    Vector3     mCamLookAt;         /* 0x434 coined: eased camera target */
    s16         mSpinSpeed;         /* 0x440 */
    u8          mStep;              /* 0x442 coined: step within the state */
    u8          mBounceCount;       /* 0x443 coined */
    s32         mState;             /* 0x444 key kind, param1 & 7; indexes the state table */
    s32         mAnimID;            /* 0x448 coined: collect animation, 0 = none */
    Vector3     mSavedCamLookAt;    /* 0x44c coined: camera restored on landing */
    Vector3     mSavedCamPos;       /* 0x458 coined */
    u32         mParticleID[3];     /* 0x464 coined: Particle::System::New handles */

    /* --- vtable --- */
    virtual ~daObjKey_c();

    virtual s32   OnYoshiTryEat();         /* slot 18 */
    virtual void  OnTurnIntoEgg(Player &player); /* slot 19 */

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();

    /* Coined names. StateDrop is reached only through the state table. */
    void StateDrop();                   /* 0x02131b18 */
    void UpdateModelTransform();        /* 0x02131f54 */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daObjKey_c_size_must_be_0x470[sizeof(daObjKey_c) == 0x470 ? 1 : -1];
#endif

#endif /* DAOBJKEY_C_H */
