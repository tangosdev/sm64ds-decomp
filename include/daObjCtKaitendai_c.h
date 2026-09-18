/* The cartridge's RTTI names this Tick Tock Clock rotating-platform class
 * daObjCtKaitendai_c. overlay_actors maps profiles 114/115 to
 * TTC_ROTATING_GEAR / TTC_ROTATING_TRIANGLE; the debug table strings are
 * CT_MECHA06 / CT_MECHA07. ov065 is TTC — this is the rotating bar pair,
 * not the conveyor (mecha04) or moving bar (mecha05).
 *
 * Derives from dBgActor_c directly: _ZTI/_ZTS18daObjCtKaitendai_c give the
 * class name, and both factories construct dBgActor_c then ShadowModel at
 * 0x324. The allocation literal is 0x37c. */
#ifndef DAOBJCTKAITENDAI_C_H
#define DAOBJCTKAITENDAI_C_H
#include "types.h"

#ifdef __cplusplus
#include "dBgActor_c.h"
#include "ShadowModel.h"

struct daObjCtKaitendai_c : dBgActor_c {
    /* InitResources sets 0 for actorID 0x72 (CT_MECHA06) and 1 for 0x73
     * (CT_MECHA07), then indexes the ov065 {model, collision, clps} table.
     * CleanupResources indexes the same row. mwccarm reuses dBgActor_c's
     * 0x31e tail padding. */
    u8  mVariant;                     /* 0x31e */
    u8  pad_31f[0x1];
    s32 mGroundY;                     /* 0x320 -- probe Y, then dBgCh_Gnd::clsnY on a hit */
    ShadowModel mShadowModel;         /* 0x324 */
    /* func_ov065_0211b40c copies mModel.mat4x3 here, then writes
     * (mGroundY + 0x32000) >> 3 into the translation Y before
     * DropShadowRadHeight. */
    Matrix4x3 mShadowMat;             /* 0x34c */

    int CleanupResources();
    int InitResources();
    int Behavior();
    int Render();

    /* Declared last and inline so class instantiation emits the retail D1/D0
     * pair in cartridge order without a separate leaf D2 body. */
    virtual ~daObjCtKaitendai_c() {}
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daObjCtKaitendai_c_size_must_be_0x37c[
    sizeof(daObjCtKaitendai_c) == 0x37c ? 1 : -1];
#endif

#endif /* __cplusplus */

#endif /* DAOBJCTKAITENDAI_C_H */
