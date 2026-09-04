#ifndef KEY_H
#define KEY_H

#include "types.h"

/* Derives from dEnemyBase_c, and TWO INDEPENDENT WITNESSES agree on the layout:
 * the class's own destructor `_ZN3KeyD1Ev` destroys each member, and
 * `Key_Spawn` constructs the same types at the same offsets before
 * storing `_ZTV3Key`. Everything this header used to restate below
 * 0x110 belongs to dEnemyBase_c and dActor_c and is inherited now.
 *
 * The members close on each other, which is what makes the layout a
 * reading rather than a guess:
 *
 *     0x114 ModelAnim                  0x64    -> 0x178
 *     0x178 Model                      0x50    -> 0x1c8
 *     0x1c8 ShadowModel                0x28    -> 0x1f0
 *     0x220 dCcAcPos_c  0x40    -> 0x260
 *     0x260 dBgCh_Actr               0x1bc   -> 0x41c
 *
 * Typing them absorbed markers that were their insides:
 *   - unk_124 = ModelAnim.data.bones
 *   - unk_128 = ModelAnim.data.transforms
 *   - mAnimation = the Animation base subobject
 *   - unk_174 = ModelAnim.file
 *
 * THE FIRST MEMBER IS AT 0x114, NOT 0x110: Key keeps four bytes of its own
 * between dEnemyBase_c's end and the ModelAnim. Every other class in this batch
 * starts its members flush against dEnemyBase_c.
 *
 * SIZE IS THE ROM'S OWN: `Key_Spawn` calls
 * `fBase_c::operator new(1136)` -- 0x470 -- and stores this class's
 * vtable, so that literal IS this class's sizeof.
 *
 * SM64DS proves this class as daObjKey_c through RTTI, allocation size and
 * vtable identity. The factory and profile spellings below are reconstructed
 * source-style names -- evidence-bounded proposals, not recovered SM64DS
 * symbols.
 *
 * daObjKey_c_classInit_OBJ_KEY at 0x02132828 (historical alias Key_Spawn)
 * allocates 0x470 and installs this class's cartridge vtable. It backs the
 * OBJ_KEY registry profile, whose descriptor at 0x02132b68 is reconstructed
 * as g_profile_OBJ_KEY.
 */

#include "dEnemyBase_c.h"
#include "Model.h"
#include "ModelAnim.h"
#include "dCcAcPos_c.h"
#include "ShadowModel.h"
#include "dBgCh_Actr.h"

struct Key : dEnemyBase_c {
    s32                          unk_110;               /* 0x110 */
    ModelAnim                    mModelAnim;            /* 0x114 */
    Model                        mModel;                /* 0x178 */
    ShadowModel                  mShadowModel;          /* 0x1c8 */
    u8  pad_1f0[0x30];
    dCcAcPos_c    mdCcAcPos_c; /* 0x220 */
    dBgCh_Actr                 mWithMeshClsn;         /* 0x260 */
    s32                          unk_41c;               /* 0x41c */
    s32                          unk_420;               /* 0x420 */
    s32                          unk_424;               /* 0x424 */
    u8  pad_428[0x18];
    s16                          mSpinSpeed;            /* 0x440 */
    u8                           unk_442;               /* 0x442 */
    u8                           unk_443;               /* 0x443 */
    s32                          mState;                /* 0x444 */
    s32                          unk_448;               /* 0x448 */
    u8  pad_44c[0x18];
    s32                          unk_464;               /* 0x464 */
    s32                          unk_468;               /* 0x468 */
    s32                          unk_46c;               /* 0x46c */

    /* --- vtable --- */
    virtual ~Key();

    virtual s32   OnYoshiTryEat();         /* slot 18 */
    virtual int   OnTurnIntoEgg(Player &player); /* slot 19 */

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
};

typedef char Key_size_must_be_0x470[sizeof(Key) == 0x470 ? 1 : -1];

#endif /* KEY_H */
