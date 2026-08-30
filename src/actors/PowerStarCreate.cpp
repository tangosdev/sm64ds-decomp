//cpp
/* Production translation unit, manually curated.
 * ov018/PowerStarCreate  (4 function(s))
 *
 * ENROLLED AND CANONICAL. The module's delinks.txt licenses the .text range
 * below to this path, so the ROM build compiles this file in place of the
 * legacy one-function sources listed at the foot of this comment.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function, in the REVERSE of source order, so
 * the highest-address ROM function is written FIRST here. Do not reorder;
 * see notes/tu-reconstruction-pilot-report.md sec 3 for the one documented
 * exception (a destructor's D0/D1/D2 group has compiler-chosen order).
 *
 * Assembled from these legacy one-function sources (ROM address order):
 *   [0] 0x021126d4  src/_ZN15PowerStarCreateD1Ev.cpp
 *   [1] 0x021126f8  src/_ZN15PowerStarCreateD0Ev.c
 *   [2] 0x02112730  src/_ZN15PowerStarCreate8BehaviorEv.cpp
 *   [3] 0x0211278c  src/PowerStarCreate_Spawn.c
 *
 * Naming boundary: these configured text symbols use PowerStarCreate, while
 * the adjacent retail RTTI spells daSCre_c (_ZTS8daSCre_c/_ZTI8daSCre_c).
 * Until that project-wide symbol migration is resolved, this TU claims only
 * the four verified text contributions; its generated RTTI/vtable is evidence,
 * not licensed retail data.
 */

#include "PowerStarCreate.h"

extern "C" {
extern int _ZTV15PowerStarCreate[];
extern void *_ZN7fBase_cnwEj(unsigned);
extern void _ZN8dActor_cC2Ev(void *);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- PowerStarCreate_Spawn, 0x0211278c, size 0x30 */
/* -------------------------------------------------------------------------- */
extern "C" PowerStarCreate *PowerStarCreate_Spawn(void)
{
    PowerStarCreate *p = (PowerStarCreate *)_ZN7fBase_cnwEj(sizeof(PowerStarCreate));
    if (p) {
        _ZN8dActor_cC2Ev(p);
        *(int *)p = (int)&_ZTV15PowerStarCreate[2];
    }
    return p;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN15PowerStarCreate8BehaviorEv, 0x02112730, size 0x5c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15PowerStarCreate8BehaviorEv
// recovered name: PowerStarCreate::Behavior
/* PowerStarCreate::Behavior - vtable slot 6, overriding fBase_c::Behavior().
 * Migrated to a real member: every field this touches (param1 at 0x008,
 * mPosX/mPosY/mPosZ at 0x05c, mAreaId at 0x0cc) is dActor_c's own, already
 * recovered in include/dActor_c.h -- PowerStarCreate itself adds no typed
 * fields (include/PowerStarCreate.h: only pad_0d0[0x4]). dActor_c::Spawn
 * and dActor_c::DistToCPlayer/fBase_c::MarkForDestruction are real
 * (non-virtual) members there. */
s32 PowerStarCreate::Behavior()
{
    if (DistToCPlayer() < 0x64000) {
        Spawn(0xb2, (param1 & 0xf) | 0x40, *(Vector3 *)&mPosX, 0, mAreaId, -1);
    }
    MarkForDestruction();
    return 1;
}

/* ROM ordinals 0/1 -- the compiler emits D1 and D0 from this definition. */
/* recovered: real C++ destructor -- the compiler emits the whole body.
 * Vtable slot 16: one vtable store, the members in reverse, then ~dActor_c. */
