//cpp
#pragma defer_codegen off
/**
 * daObjFl_Koma_D_c -- a rotating lava-flow platform (ROM profile FL_KOMA_D).
 *
 * Four functions form this production text slice at ov022
 * 0x021115a8..0x02111688. The class data group at 0x02113d98
 * (RTTI, type name, profile, vtable) is what names this class: the tree called
 * it RotatingPlatformLll until the rename in this branch.
 *
 * With defer_codegen off, mwccarm 2004/b56 emits this source in ROM order,
 * including the out-of-line D1-before-D0 destructor group. This setting and
 * the ascending method order are measured together on this translation unit.
 * The adjacent classInit factory and the class/profile/resource data remain
 * outside this four-function text promotion and need an owned continuation.
 *
 * CleanupResources keeps its #pragma long_calls bracket: it tail-calls into
 * another overlay, and the pragma is what makes mwccarm emit the pooled
 * `ldr ip,[pc]; bx ip` indirect form the ROM uses instead of a near branch.
 */

#include "daObjFl_Koma_D_c.h"

/* This descriptor matches the shared helpers in daObjKaitendai_c.cpp. */
struct SharedFilePtr;

struct CLPS_Block;

struct ResourceDescriptor {
    SharedFilePtr *model;
    SharedFilePtr *collision;
    CLPS_Block *clps;
};

typedef char ResourceDescriptor_size_must_be_0x0c[
    sizeof(ResourceDescriptor) == 0x0c ? 1 : -1];

extern "C" {
extern int func_ov002_020b66a8(daObjKaitendai_c *self, ResourceDescriptor *data);
extern int func_ov002_020b676c(daObjKaitendai_c *self, ResourceDescriptor *a,
short arg2);
extern ResourceDescriptor data_ov022_02113da4;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN16daObjFl_Koma_D_cD0Ev, 0x021115f8, size 0x64 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjFl_Koma_D_cD0Ev
/* One native destructor definition emits D1 (0x021115a8, size 0x50) and
 * D0 (0x021115f8, size 0x64), both owned by this four-function text slice.
 * D0 adds the inherited actor-heap operator delete after destruction.
 * The compiler-only D2 has no standalone ROM home and retains its explicit
 * manifest deadstrip policy. The single marker identifies the shared native
 * body for both ROM destructor variants in the lifecycle coverage audit. */
daObjFl_Koma_D_c::~daObjFl_Koma_D_c()
{
}


/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN16daObjFl_Koma_D_c16CleanupResourcesEv, 0x0211165c, size 0x14 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjFl_Koma_D_c16CleanupResourcesEv
// Cross-overlay tail-call veneer. #pragma long_calls forces mwccarm to emit the pooled
// `ldr ip,[pc]; bx ip` indirect tail-call (a plain near `b` otherwise) that the ROM uses
// to reach another overlay. func_ov002_020b66a8 is daObjKaitendai_c's shared cleanup
// helper, out of this task's scope, kept under its existing name.
#pragma long_calls on  /* carried verbatim from the legacy file (positional) */
int daObjFl_Koma_D_c::CleanupResources()
{
    /* The legacy shard declared this `extern int data_ov022_02113da4[]`, so the
     * bare name decayed to a pointer. The merged TU keeps the other member's
     * spelling -- the real ResourceDescriptor object -- so take its address
     * explicitly. Same pointer value, same codegen. */
    return func_ov002_020b66a8(this, &data_ov022_02113da4);
}
#pragma long_calls off  /* close the bracket: positional, must not leak downward */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN16daObjFl_Koma_D_c13InitResourcesEv, 0x02111670, size 0x18 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjFl_Koma_D_c13InitResourcesEv
/* daObjFl_Koma_D_c::InitResources -- vtable slot 0, ov022 0x02111670.
 *
 * A tail call into daObjKaitendai_c's shared cross-overlay helper
 * func_ov002_020b676c(this, filetable, angle) -- out of this task's scope,
 * kept under its existing name, same idiom as
 * src/game/actors/d_a_obj_bk_ukisima.cpp (ov015). Return the helper's
 * resource status: the ROM tail call already leaves that result in r0.
 * The explicit return is relocation-verified under mwccarm 2004/b56.
 *
 * The helper takes the BASE, daObjKaitendai_c, not this leaf: this class's
 * __si_class_type_info in ov022 records exactly one base, daObjKaitendai_c at
 * ov002 0x021091ac, at subobject offset 0, and the four other leaves reach the
 * same helper through the same base. */
/* The second argument, which this file used to call `Arg`: the three-word
 * model/collision/CLPS descriptor, spelled the way
 * src/actors/daObjKaitendai_c.cpp -- the file that DEFINES
 * func_ov002_020b676c -- spells it. */
int daObjFl_Koma_D_c::InitResources()
{
    return func_ov002_020b676c(this, &data_ov022_02113da4, 0x100);
}

#ifdef _MSC_VER
/* The host uses flat ROM destructor names. Keep the renamed canonical entry
 * and the legacy entry introduced by PR #3096 at the same ABI boundary.
 * Qualified destruction calls this class directly before its actor-heap delete.
 * Neither compatibility entry is compiled into the cartridge object. */
extern "C" daObjFl_Koma_D_c *_ZN16daObjFl_Koma_D_cD0Ev(daObjFl_Koma_D_c *thiz)
{
    thiz->daObjFl_Koma_D_c::~daObjFl_Koma_D_c();
    daObjFl_Koma_D_c::operator delete(thiz);
    return thiz;
}

extern "C" daObjFl_Koma_D_c *_ZN19RotatingPlatformLllD0Ev(daObjFl_Koma_D_c *thiz)
{
    return _ZN16daObjFl_Koma_D_cD0Ev(thiz);
}
#endif
