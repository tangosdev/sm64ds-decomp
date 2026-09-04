#ifndef MUGENBGM_H
#define MUGENBGM_H

/* RECONSTRUCTED NAMES USED IN THIS HEADER. SM64DS RTTI names the
 * implementation(s) below; the registry profile object and the factory
 * spelling are Tier B reconstructions -- evidence-bounded proposals, not
 * recovered SM64DS symbols. Exact original spellings are not preserved.
 *
 *   daMugenBGM_c -- daMugenBGM_c_classInit (was MugenBgm_Spawn), g_profile_MUGEN_BGM (was MugenBgm_SpawnInfo)
 */

#include "dActor_c.h"

/* daMugenBGM_c_classInit allocates 0xd4 bytes, constructs dActor_c, and installs
 * _ZTV8MugenBgm. D1 chains directly to dActor_c::~dActor_c, so the class has
 * no owned subobjects; its behavior uses only inherited actor state.
 */
struct MugenBgm : dActor_c {
    u8 pad_0d0[0x4];

    virtual ~MugenBgm();

    virtual s32 InitResources();
    virtual s32 CleanupResources();
    virtual s32 Behavior();
    virtual s32 Render();
    virtual void OnPendingDestroy();
};

typedef char MugenBgm_size_must_be_0xd4[
    sizeof(MugenBgm) == 0xd4 ? 1 : -1];

#endif /* MUGENBGM_H */
