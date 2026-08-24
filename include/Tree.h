/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class Tree: 5 matched functions, 4 evidenced fields.
 * Offsets/widths are observed, not guessed. Field NAMES are placeholders -
 * renaming cannot change codegen.
 *
 * Real inheritance (was a flat, non-inheriting struct re-declaring
 * fBase_c's mParam and dActor_c's mPosX/mPosY/mPosZ inline): build/
 * rtti_vtables.json keys this class as "daTree_c" (8 chars) vs the decomp
 * name "Tree" (4 chars) -- a length mismatch, not data-verifiable. parent:
 * dActor_c, parent_slots: 31, own overrides at slots 0/3/6/9/12
 * (InitResources/CleanupResources/Behavior/Render/OnPendingDestroy) plus
 * the destructor pair at 16/17.
 *
 * InitResources/Behavior/D0/D1 stay hand-written extern "C" free functions
 * with the class's own mangled names (matching daObjPathLift_c/#1719's
 * precedent) -- their bodies never reference a named Tree/dActor_c member,
 * only raw self+offset arithmetic and a heap-allocated auxiliary
 * dCcPos_c-list unrelated to this class's own storage, so there is no
 * conversion benefit. The vtable slots for them are still correct: the
 * mangled symbol name is identical whether the definition is a real
 * Tree::Method() or an extern "C" free function of the same name.
 *
 * Per the Itanium ABI the key function is the class's FIRST DECLARED
 * virtual (InitResources, slot 0), not just any virtual defined
 * out-of-line in this TU -- confirmed empirically: the compiled object has
 * zero .data sections and imports _ZTV4Tree as UNDEFINED, even though
 * CleanupResources/Render/OnPendingDestroy are real Tree:: methods here.
 * Because InitResources's real body never took class-member form, this TU
 * does NOT become the key-function TU and emits no vtable/RTTI of its own,
 * nor references any inherited base RTTI -- unlike every prior pilot in
 * this series.
 */
#ifndef TREE_H
#define TREE_H
#include "types.h"
#include "dActor_c.h"
#include "Model.h"

struct Tree : dActor_c {
    u8  pad_0d0[0x4];
    /* Model[5] array, named by _ZN5ModelD1Ev/_ZN5ModelC1Ev at +0xd4 via
       __destroy_arr/func_020733a8 (a relocation the ROM build checks) --
       five Models, one per compass-ish direction slot indexed by
       InitResources/Render. Fills the class to its exact trailing size,
       0xd0 (dActor_c) + 0x4 (pad) + 5*0x50 (Model) = 0x264. */
    Model mModel[5];                  /* 0x0d4 */

    virtual ~Tree();
    virtual s32 InitResources();       /* slot  0 -- stays extern "C", see above */
    virtual s32 CleanupResources();    /* slot  3 */
    virtual s32 Behavior();            /* slot  6 -- stays extern "C", see above */
    virtual s32 Render();              /* slot  9 */
    virtual void OnPendingDestroy();   /* slot 12 -- empty body in the ROM */
};

typedef char Tree_size_must_be_0x264[sizeof(Tree) == 0x264 ? 1 : -1];

#endif
