/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class HealingHeart: 5 matched functions, 9 evidenced fields.
 * Offsets/widths are observed, not guessed. Field NAMES are placeholders --
 * renaming cannot change codegen.
 *
 * Real inheritance (was a flat, non-inheriting struct with dActor_c's own
 * 0x074..0x0d0 fields re-declared inline): build/rtti_vtables.json keys this
 * class as "daObjHeart_c" (the ROM's own coined RTTI name, same
 * same-length-different-spelling shape as WingFeather/daFeather_c --
 * config/arm9/overlays/ov002/symbols.txt is the only reliable check, a
 * length-only heuristic would wrongly call this data-verifiable). parent:
 * dActor_c, parent_slots: 31, own overrides at slots 0/3/6/9 (InitResources/
 * CleanupResources/Behavior/Render) plus the destructor pair at 16/17 -- no
 * OnPendingDestroy override, same shape as WingFeather. */
#ifndef HEALINGHEART_H
#define HEALINGHEART_H
#include "types.h"
#include "dActor_c.h"
#include "ModelAnim.h"
#include "dCcAc_c.h"

struct HealingHeart : dActor_c {
    u8  pad_0d0[0x4];
    /* ModelAnim member, named by _ZN9ModelAnimD1Ev at +0xd4 -- a relocation the ROM build
       checks. D1 and not D2, so it is this type and not an inlined base. The marker's pad
       stopped short of the object, so the member also takes over mAnimation (+0x50 = the
       Animation base), unk_130 (+0x5c = speed), which the header declared separately
       inside it. */
    ModelAnim mModelAnim;            /* 0x0d4 */
    /* dCcAc_c member. The cartridge's own ~HealingHeart calls _ZN7dCcAc_cD1Ev at +0x138
       (D0/D1), a relocation the ROM build checks; recovered by tools/dtor_members.py.
       D1 and not D2, so it is this type and not an inlined base. The marker's pad
       stopped short of the object, so the member also takes over unk_15c
       (+0x24 = dCc_c::otherOwner), which the header declared separately inside it. */
    dCcAc_c mdCcAc_c;            /* 0x138 */
    s32 unk_16c;            /* 0x16c */
    u8  mHealTimer;            /* 0x170 */
    u8  unk_171;            /* 0x171 */

    virtual ~HealingHeart();
    virtual s32 InitResources();       /* slot  0 */
    virtual s32 CleanupResources();    /* slot  3 */
    virtual s32 Behavior();            /* slot  6 */
    virtual s32 Render();              /* slot  9 */
};

typedef char HealingHeart_size_must_be_0x174[sizeof(HealingHeart) == 0x174 ? 1 : -1];

#endif
