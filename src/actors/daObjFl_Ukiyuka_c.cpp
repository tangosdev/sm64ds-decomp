//cpp
/* Recovered translation unit -- ov022/daObjFl_Ukiyuka_c, Lethal Lava Land's
 * floating floor, both sizes of it.
 *
 * .text span 0x02111f6c..0x021120b8, 4 functions, ROM ordinals 0..3, exactly
 * the contiguous linker run build/tu_map.json places here. All 4 are assembled
 * below and config/tu_manifest.d/ov022/daObjFl_Ukiyuka_c.json names every one,
 * so the run has no hole and the 4 one-function files under src/ that used to
 * own these bytes are deleted by this change.
 *
 * IDENTITY IS THE CARTRIDGE'S. ov022 holds _ZTS17daObjFl_Ukiyuka_c at
 * 0x021140c0; no FloatingFloorLllSmall RTTI record exists anywhere -- that was
 * a coined alias for this same class. The 32-slot table at 0x0211412c carries
 * this class's own _ZTI (0x021140b4) at its typeinfo word and overrides
 * InitResources (slot 0), CleanupResources (slot 3) and the destructor pair
 * (slots 16, 17). The two factories the tree gives separate names,
 * daObjFl_Ukiyuka_c_classInit_FL_UKIYUKA_L and daObjFl_Ukiyuka_c_classInit_FL_UKIYUKA,
 * build ONE class: both pass 816 = 0x330 and both store this vtable.
 *
 * FUNCTION ORDER IS THE ROM'S OWN, under `#pragma defer_codegen off`.
 * Deferred code generation stays off for this TU so mwccarm emits one .text
 * section per function in SOURCE order: the lowest-address ROM function
 * (_ZN17daObjFl_Ukiyuka_cD1Ev, 0x02111f6c) is written FIRST and the highest
 * (_ZN17daObjFl_Ukiyuka_c13InitResourcesEv, 0x02112040) LAST. Do not reorder.
 * The verifier reports all 4 sections in the expected ROM-ascending order.
 *
 * THE DESTRUCTOR IS ONE DEFINITION AND TWO SECTIONS (plus a homeless D2).
 * A single `daObjFl_Ukiyuka_c::~daObjFl_Ukiyuka_c()` emits D1 and D0, and
 * with deferred code generation off they land in the cartridge's order, D1
 * first at 0x02111f6c and D0 at 0x02111fbc.
 * compiler order, which is exactly the cartridge's (0x02111f6c D1, 0x02111fbc
 * D0). The homeless D2 has no ROM symbol and no inbound relocation once the
 * leaf D1/D0 pair is retained, so the manifest licenses it as deadstrip
 * compiler-only output. It stores THREE vptrs -- its own, daObjUkiyuka_c's
 * (inlined), then dBgActor_c's -- proving the two-level chain; a one-level
 * chain emits two.
 *
 * KEY-FUNCTION SIDE EFFECT: none claimed. ~daObjFl_Ukiyuka_c is the first
 * non-inline virtual and this TU defines it, but the promotion is text-only:
 * the manifest's compiler_only_output externalizes _ZTV/_ZTI/_ZTS to their
 * canonical public addresses (deadstrip-data) and claims no data or BSS range.
 *
 * WHAT THE PROMOTION CARRIES OVER, and nothing else was changed:
 *   1. InitResources' missing return is faithful: the header declares `int`,
 *      the body sets no return value and neither does the ROM. Adding an
 *      explicit `return` emits a real instruction and breaks the match.
 *   2. The ResourceDescriptor/CLPS table spelling matches the definition site
 *      in src/actors/daObjUkiyuka_c.cpp, which owns the shared helper.
 *   3. The destructor is one definition with two stacked markers -- writing
 *      them as two definitions is an `object redefined` error.
 *
 * The 4 legacy one-function sources this folds are named by the manifest's
 * `legacy_source` fields (history); they are not repeated here.
 */

/* Includes: union of the legacy files', first-seen in ROM-ascending
 * processing order. NOT verified for header ordering constraints (e.g. a
 * common.h-before-X rule) -- watch for new compile errors after this. */
#include "daObjFl_Ukiyuka_c.h"

/* Local shadow declarations carried from the legacy files verbatim.
 * NOT reconciled against real project headers -- check include/*.h for
 * each of these before compiling; a real header should usually win. */
/* shadow struct 'SharedFilePtr' */
struct SharedFilePtr;

/* shadow struct 'CLPS_Block' */
struct CLPS_Block;

/* shadow struct 'ResourceDescriptor' */
struct ResourceDescriptor {
    SharedFilePtr *model;
    SharedFilePtr *collision;
    CLPS_Block *clps;
};

/* shadow typedef 'char' */
typedef char ResourceDescriptor_size_must_be_0x0c[
    sizeof(ResourceDescriptor) == 0x0c ? 1 : -1];

extern "C" {
int func_ov002_020b6424(daObjUkiyuka_c *t, ResourceDescriptor *f);
extern char data_ov022_021140d4[];
void func_020393a4(int *p, int v);
int func_ov002_020b6584(void *self, void *p, int x);
}

#pragma defer_codegen off

/* -------------------------------------------------------------------------- */
/* ROM ordinals 0/1 -- _ZN17daObjFl_Ukiyuka_cD1Ev 0x02111f6c, _ZN17daObjFl_Ukiyuka_cD0Ev 0x02111fbc. */
/* ONE definition, two emitted sections.                                          */
/* -------------------------------------------------------------------------- */
// @symbol _ZN17daObjFl_Ukiyuka_cD1Ev
// @symbol _ZN17daObjFl_Ukiyuka_cD0Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * THREE vtable stores, and the middle one is the finding. `struct
 * daObjFl_Ukiyuka_c : daObjUkiyuka_c : dBgActor_c` emits its own vptr, then
 * daObjUkiyuka_c's -- inlined, because that destructor is defined in its
 * class body -- then dBgActor_c's, then dBgActor_c's dBgW_KcMbg and Model,
 * then dActor_c. Nothing in the chain adds a member with a destructor, so the
 * body is empty. D0 additionally returns the object to the actor heap through
 * the inline operator delete, which is why nothing below mentions a heap.
 */
daObjFl_Ukiyuka_c::~daObjFl_Ukiyuka_c()
{
}
/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN17daObjFl_Ukiyuka_c16CleanupResourcesEv, 0x02112020, size 0x20 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN17daObjFl_Ukiyuka_c16CleanupResourcesEv
/* recovered: named members + real C++ method */
/* daObjFl_Ukiyuka_c::CleanupResources() -- indexes the ov022 parameter
 * table with mVariant, the one field this class adds. */
/* One row of the 0xc-stride ov022 table: the model/collision/CLPS descriptor
 * the shared helper reads out of its second argument, spelled the way
 * src/actors/daObjUkiyuka_c.cpp -- the file that DEFINES func_ov002_020b6424
 * -- spells it. */
/* The helper takes the BASE, daObjUkiyuka_c, not this leaf: this class's
 * __si_class_type_info in ov022 records exactly one base, daObjUkiyuka_c at
 * ov002 0x02109104, at subobject offset 0, and daObjKm2_Ukishima_c (ov045)
 * reaches the same helper through the same base. */
int daObjFl_Ukiyuka_c::CleanupResources()
{
    return func_ov002_020b6424(
        this, (ResourceDescriptor *)(data_ov022_021140d4 + mVariant * 0xc));
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN17daObjFl_Ukiyuka_c13InitResourcesEv, 0x02112040, size 0x78 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN17daObjFl_Ukiyuka_c13InitResourcesEv
/* recovered: named members + real C++ method */
/* daObjFl_Ukiyuka_c::InitResources() -- the two spawn IDs 0x4b/0x4c select
 * the variant and its float period; mVariant then indexes the same ov022
 * parameter table CleanupResources uses. */
/* NO VALUE IS RETURNED, AND THAT IS FAITHFUL. The header declares `int`; this
 * body sets no return value and the ROM does not either. Adding an explicit
 * `return <v>;` emits a real instruction and breaks the match (measured on
 * daObjPushblock_c::OnPushed, 2026-08-22). The legacy .c declared this `void` and never
 * included the header, so nothing checked the disagreement until it became a
 * real method. Harmless for the ROM build; a host-port caller that reads this
 * result gets garbage. Fix upstream in the header's return type if it is wrong --
 * never with a `return` here. */
int daObjFl_Ukiyuka_c::InitResources()
{
    switch (*(unsigned short *)((char *)this + 0xc)) {
    case 0x4b:
        mVariant = 0;
        func_020393a4((int *)((char *)this + 0x124), 0x650000);
        break;
    case 0x4c:
        mVariant = 1;
        func_020393a4((int *)((char *)this + 0x124), 0x1000000);
        break;
    }
    unsigned char idx = mVariant;
    func_ov002_020b6584(this, data_ov022_021140d4 + idx * 0xc, 0x1051);
}
