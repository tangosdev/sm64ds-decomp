#ifndef DAOBJCTMECHA03_C_H
#define DAOBJCTMECHA03_C_H

#include "types.h"

/* Derives from dBgActor_c directly: RTTI _ZTI16daObjCtMecha03_c /
 * _ZTS16daObjCtMecha03_c (config/arm9/overlays/ov065/symbols.txt) give the
 * class name; the un-migrated D1 (func_ov065_02119f3c) stores this class's
 * own vtable, then dBgActor_c's -- inlined, because dBgActor_c's destructor
 * is defined in its class body -- then destroys the dBgW_KcMbg at
 * 0x124 and the Model at 0xd4 before chaining to dActor_c. All three belong
 * to dBgActor_c and are inherited now; nothing below 0x31e restates them.
 *
 * 0x08e/0x090 are NOT this class's own storage: dActor_c.h already names
 * them mAngleY/mAngleZ, and dBgActor_c ends its own data at 0x31e (sizeof
 * rounds to 0x320) -- see the header comment there. The original
 * AUTO-GENERATED flat placeholder called them unk_08e/unk_090 because it
 * had no base class to inherit them from; the real class uses the
 * inherited names instead.
 *
 * SIZE IS THE ROM'S OWN LITERAL: daObjCtMecha03_c_classInit (historical
 * placeholder func_ov065_0211a45c) calls fBase_c::operator new(0x388), read
 * off the retail instruction. The field span now reaches it exactly: the
 * per-frame shadow helper at ov065:0x02119fe8 writes 0x32c and builds a
 * Matrix4x3 in place at 0x358, so what an earlier reading of this header
 * called a 6-byte pad and a 0x30-byte tail pad is real storage.
 */

#ifdef __cplusplus

#include "dBgActor_c.h"
#include "ShadowModel.h"
#include "math/Matrix.h"

struct daObjCtMecha03_c : dBgActor_c {
    /* A pendulum, in the four fields the ROM's own Behavior integrates:
       mSwingDir is the sign of the restoring acceleration and flips whenever
       it agrees in sign with mSwingAngle; mSwingAccel is its magnitude;
       mSwingSpeed accumulates mSwingAccel * mSwingDir; mSwingAngle accumulates
       mSwingSpeed and is copied straight into dActor_c::mAngleZ. */
    s16 mSwingDir;                    /* 0x31e -- 1 at InitResources */
    s16 mSwingAccel;                  /* 0x320 -- data_ov065_0211c0b0[setting], re-rolled to 0xd/0x2a under setting 2 */
    s16 mSwingAngle;                  /* 0x322 -- 0x1964 at InitResources */
    s16 mSwingSpeed;                  /* 0x324 */
    s16 mSoundTimer;                  /* 0x326 -- DecIfAbove0_Short; at 0 plays bank-3 sound 0x38, reloaded as mPauseTimer + 0xf */
    s16 mPauseTimer;                  /* 0x328 -- DecIfAbove0_Short; gates the whole swing update */
    u8  pad_32a[0x2];
    /* The drop shadow's ground height, and the matrix it is projected with.
       Both are written by the per-frame shadow helper at ov065:0x02119fe8:
       it casts a dBgCh_Gnd ray 0xc8000 below a point 0x320000 under the
       platform's own rotated origin, parks the hit height here, builds a
       Y-rotation matrix from mAngleY at 0x358 and writes the shadow position
       into its translation row before calling dActor_c::DropShadowScaleXYZ.
       The AUTO-GENERATED placeholder called 0x32c part of a 6-byte pad and
       the matrix a 0x30-byte tail pad; both are real storage. */
    s32 mShadowGroundY;               /* 0x32c */
    ShadowModel mShadowModel;         /* 0x330 */
    Matrix4x3 mShadowMat;             /* 0x358, to the ROM's 0x388 */

    /* --- vtable, in ROM order. Do not reorder. ---
     * Inline, and declared FIRST. This TU defines every virtual the class
     * has, so it emits the vtable and RTTI; out of line, mwccarm emits the
     * D2/D1/D0 triple in D0-before-D1 order, but retail puts D1 (0x02119f3c)
     * ABOVE D0 (0x02119f88) and has no D2 at all, and objisolate then refuses
     * the whole TU for emitting out of ROM address order. The body is the two
     * vptr stores, the ShadowModel member's own destructor, and the
     * dBgActor_c teardown -- all of it compiler-emitted. */
    virtual ~daObjCtMecha03_c() {}     /* slots 16 (D1), 17 (D0) */

    virtual s32 InitResources();       /* slot 0 */
    virtual s32 CleanupResources();    /* slot 3 */
    virtual s32 Behavior();            /* slot 6 */
    virtual s32 Render();              /* slot 9 */
    /* slot 31 (Kill) is NOT overridden: the vtable word at the 0x0211d0ec
       address point + 0x7c (0x0211d168) is 0x020ee55c, identical to
       dBgActor_c's own slot 31 -- confirmed by reloc, not assumed. */
};

typedef char daObjCtMecha03_c_size_must_be_0x388[sizeof(daObjCtMecha03_c) == 0x388 ? 1 : -1];

#endif /* __cplusplus */

#endif /* DAOBJCTMECHA03_C_H */
