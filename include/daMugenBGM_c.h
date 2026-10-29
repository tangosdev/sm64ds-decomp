#ifndef DAMUGENBGM_C_H
#define DAMUGENBGM_C_H

/* RECONSTRUCTED NAMES USED IN THIS HEADER. SM64DS RTTI names the
 * implementation(s) below; the registry profile object and the factory
 * spelling are Tier B reconstructions -- evidence-bounded proposals, not
 * recovered SM64DS symbols. Exact original spellings are not preserved.
 *
 *   daMugenBGM_c -- daMugenBGM_c_classInit (was MugenBgm_Spawn), g_profile_MUGEN_BGM (was MugenBgm_SpawnInfo)
 */

#include "dActor_c.h"

struct Fog;

/* daMugenBGM_c_classInit allocates 0xd4 bytes, constructs dActor_c, and installs
 * _ZTV12daMugenBGM_c. D1 chains directly to dActor_c::~dActor_c, so the class has
 * no owned subobjects; its behavior uses only inherited actor state.
 */
struct daMugenBGM_c : dActor_c {
    u8 pad_0d0[0x4];

    virtual ~daMugenBGM_c() {}

    virtual s32 InitResources();
    virtual s32 CleanupResources();
    virtual s32 Behavior();
    virtual s32 Render();
    virtual void OnPendingDestroy();

    /* The stored Fog while the stored area is the current one, else null. */
    static Fog *GetActiveFog();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daMugenBGM_c_size_must_be_0xd4[sizeof(daMugenBGM_c) == 0xd4 ? 1 : -1];
#endif

#endif /* DAMUGENBGM_C_H */
