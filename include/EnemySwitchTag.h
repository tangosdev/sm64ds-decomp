/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class EnemySwitchTag: 5 matched functions, 7 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen.
 *
 * Real inheritance (was a flat, non-inheriting struct re-declaring dActor_c's
 * own 0xd0-byte span as opaque padding): build/rtti_vtables.json keys this
 * class as "daESwitch_c" (11 chars) vs the decomp name "EnemySwitchTag" (14
 * chars) -- a length mismatch, not data-verifiable. parent: dActor_c,
 * parent_slots: 31, own overrides at slots 0/3/6 (InitResources/
 * CleanupResources/Behavior) plus the destructor pair at 16/17 -- no Render
 * or OnPendingDestroy override.
 */
#ifndef ENEMYSWITCHTAG_H
#define ENEMYSWITCHTAG_H
#include "types.h"
#include "dActor_c.h"
#include "dCcAc_c.h"

struct EnemySwitchTag : dActor_c {
    u8  pad_0d0[0x4];
    /* dCcAc_c member. The cartridge's own ~EnemySwitchTag calls _ZN7dCcAc_cD1Ev at
       +0x0d4 (D0/D1), a relocation the ROM build checks; recovered by
       tools/dtor_members.py. D1 and not D2, so it is this type and not an inlined base. */
    dCcAc_c mdCcAc_c;            /* 0x0d4 */
    u16 unk_108;            /* 0x108 */
    u16 unk_10a;            /* 0x10a */
    u8  unk_10c;            /* 0x10c */
    u8  mEventID;            /* 0x10d */

    virtual ~EnemySwitchTag();

    virtual s32 InitResources();
    virtual s32 CleanupResources();
    virtual s32 Behavior();
};

typedef char EnemySwitchTag_size_must_be_0x110[sizeof(EnemySwitchTag) == 0x110 ? 1 : -1];

#endif
