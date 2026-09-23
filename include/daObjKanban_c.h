#ifndef DAOBJKANBAN_C_H
#define DAOBJKANBAN_C_H

#include "types.h"
#include "dBgW_KcMbg.h"

/* Derives from dBgActor_c: the destructor stores this class's vtable, then
 * dBgActor_c's -- inlined -- then destroys the dBgW_KcMbg at 0x124 and
 * the Model at 0xd4 before chaining to dActor_c. All three belong to dBgActor_c.
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

#ifdef __cplusplus

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
    Player *mTalkingPlayer; /* 0x360 */
    u8 mState;              /* 0x364 */
    u8 pad_365;
    s16 mMessageID;         /* 0x366 */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daObjKanban_c_size_must_be_0x368[sizeof(daObjKanban_c) == 0x368 ? 1 : -1];
#endif

#else

/* The C spelling of the same object, flat. Kept because the D0 file is a C
   translation unit that reads these fields, and D0 is compiler-generated so it
   can never be migrated. Same arrangement as include/ShadowModel.h. */
struct daObjKanban_c {
    u8  pad_000[0xd4];
    /* Model member, named by _ZN5ModelD1Ev at +0xd4 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    Model mModel;            /* 0x0d4 */
    /* dBgW_KcMbg member. The cartridge's own ~daObjKanban_c calls _ZN10dBgW_KcMbgD1Ev at
       +0x124 (D0/D1), a relocation the ROM build checks; recovered by
       tools/dtor_members.py. D1 and not D2, so it is this type and not an inlined base. */
    dBgW_KcMbg mMovingMeshCollider;            /* 0x124 */
    u8  pad_2ec[0x34];
    u8  mdCcAcPos_c;            /* 0x320 */
};

#endif /* __cplusplus */

#endif /* DAOBJKANBAN_C_H */
