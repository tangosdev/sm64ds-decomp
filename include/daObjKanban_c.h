#ifndef DAOBJKANBAN_C_H
#define DAOBJKANBAN_C_H

#include "types.h"
#include "dBgW_KcMbg.h"

/* Derives from dBgActor_c: the destructor stores this class's vtable, then
 * dBgActor_c's -- inlined -- then destroys the dBgW_KcMbg at 0x124 and
 * the dExtSimpleModel_c at 0xd4 before chaining to dActor_c. All three belong to dBgActor_c.
 * Everything this header used to restate below 0x31e was dActor_c's and
 * dBgActor_c's, and is inherited now.
 *
 * SIZE IS THE OBSERVED FIELD SPAN, rounded up. It guards this declaration; it
 * is not independent evidence about the ROM.
 *
 * SM64DS proves this class as daObjKanban_c through RTTI, allocation size
 * and vtable identity. The factory and profile spellings below are
 * reconstructed source-style names -- evidence-bounded proposals, not
 * recovered SM64DS symbols.
 *
 * daObjKanban_c_classInit at 0x0212f244 (historical alias WallSign_Spawn)
 * allocates 0x368 and installs this class's cartridge vtable. It backs the
 * KANBAN registry profile, whose descriptor at 0x021303dc is reconstructed
 * as g_profile_KANBAN.
 */

#include "dBgActor_c.h"
#include "dCcAcPos_c.h"

struct daObjKanban_c : dBgActor_c {
    u8  pad_31e[0x2];
    dCcAcPos_c mdCcAcPos_c;/* 0x320 */

    /* --- vtable --- */
    virtual ~daObjKanban_c();

    int CleanupResources();
    int InitResources();
    int Behavior();
    int Render();

    /* Tail fields, named from Behavior's own reads and writes. */
    daPly_c *mTalkingPlayer; /* 0x360 */
    u8 mState;              /* 0x364 */
    u8 pad_365;
    s16 mMessageID;         /* 0x366 */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daObjKanban_c_size_must_be_0x368[sizeof(daObjKanban_c) == 0x368 ? 1 : -1];
#endif

#endif /* DAOBJKANBAN_C_H */
