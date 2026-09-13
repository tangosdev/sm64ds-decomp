/* The cartridge's RTTI names this Tick Tock Clock pendulum class
 * daObjCtMecha03_c. overlay_actors maps profile 110 to CT_MECHA03; the debug
 * table string is that same ID. ov065 is TTC — this is the pendulum, not the
 * conveyor (mecha04) or moving bar (mecha05).
 *
 * Derives from dBgActor_c directly: _ZTI/_ZTS16daObjCtMecha03_c give the
 * class name, and the factory constructs dBgActor_c then ShadowModel at 0x330.
 * The allocation literal is 0x388. */
#ifndef DAOBJCTMECHA03_C_H
#define DAOBJCTMECHA03_C_H
#include "types.h"

#ifdef __cplusplus
extern "C" void *_ZN7fBase_cnwEj(unsigned size);
#include "dBgActor_c.h"
#include "ShadowModel.h"

struct daObjCtMecha03_c : dBgActor_c {
    /* A pendulum, in the four fields Behavior integrates: mSwingDir is the sign
     * of the restoring acceleration and flips whenever it agrees in sign with
     * mSwingAngle; mSwingAccel is its magnitude; mSwingSpeed accumulates
     * mSwingAccel * mSwingDir; mSwingAngle accumulates mSwingSpeed and is
     * copied into dActor_c::mAngleZ. mwccarm reuses dBgActor_c's 0x31e tail
     * padding for mSwingDir. */
    s16 mSwingDir;                    /* 0x31e -- 1 at InitResources */
    s16 mSwingAccel;                  /* 0x320 -- data_ov065_0211c0b0[setting], re-rolled to 0xd/0x2a under setting 2 */
    s16 mSwingAngle;                  /* 0x322 -- 0x1964 at InitResources */
    s16 mSwingSpeed;                  /* 0x324 */
    s16 mSoundTimer;                  /* 0x326 -- DecIfAbove0_Short; at 0 plays bank-3 sound 0x38, reloaded as mPauseTimer + 0xf */
    s16 mPauseTimer;                  /* 0x328 -- DecIfAbove0_Short; gates the whole swing update */
    s32 mGroundY;                     /* 0x32c -- probe Y, then dBgCh_Gnd::clsnY on a hit */
    ShadowModel mShadowModel;         /* 0x330 */
    /* func_ov065_02119fe8 builds this from mAngleY, then writes X / ground Y / Z
     * into its translation before passing it to DropShadowScaleXYZ. */
    Matrix4x3 mShadowMat;             /* 0x358 */

    int CleanupResources();
    int InitResources();
    int Behavior();
    int Render();

    static void *operator new(unsigned long size) {
        return _ZN7fBase_cnwEj((unsigned)size);
    }

    /* Declared last and inline so class instantiation emits the retail D1/D0
     * pair in cartridge order without a separate leaf D2 body. */
    virtual ~daObjCtMecha03_c() {}
};

typedef char daObjCtMecha03_c_size_must_be_0x388[sizeof(daObjCtMecha03_c) == 0x388 ? 1 : -1];

#endif /* __cplusplus */

#endif /* DAOBJCTMECHA03_C_H */
