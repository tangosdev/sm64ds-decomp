//cpp
/* Production translation unit for ov045/daObjKm2_Ukishima_c, hand-curated.
 * 5 function(s), .text 0x02111b14..0x02111c30.
 *
 * ENROLLED AND CANONICAL. config/arm9/overlays/ov045/delinks.txt licenses that
 * whole run to this one path, so the ROM build compiles this file in place of
 * the four legacy class-method sources and the adjacent factory it replaces.
 *
 * The file stem follows the snake_case scheme in
 * notes/tu-naming-and-swallowers.md sec 1 (tools/tu_names.py):
 * daObjKm2_Ukishima_c -> d_a_obj_km2_ukishima.
 *
 * Bowser in the Fire Sea's floating floor -- one of the slabs that sinks under
 * your weight and bobs back up. It adds no state of its own to daObjUkiyuka_c
 * (see include/daObjKm2_Ukishima_c.h) and overrides only the two vtable slots
 * the abstract base leaves pure, both of which hand the actor and this level's
 * descriptor to the shared ov002 helpers.
 *
 * THE .text RUN, DERIVED BY ADDRESS. delinks.txt bounds it on both sides with a
 * different class: src/d_a_obj_km2_nobiru.c ends at 0x02111b14 and
 * src/game/actors/d_a_obj_km2_gura.cpp begins at 0x02111c30, and the five
 * members in between tile it with no gap -- 0x02111b14 + 0x50 + 0x64 + 0x14 +
 * 0x18 + 0x3c = 0x02111c30 exactly.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S. mwccarm 2004/b56
 * emits one .text section per function in the REVERSE of source order, so the
 * highest-address ROM function is written FIRST here. Do not reorder:
 * tools/rombuild.py refuses the object outright when the emitted order and the
 * ROM's disagree.
 *
 * It absorbs five legacy one-function sources, at ROM 0x02111b14 (D1),
 * 0x02111b64 (D0), 0x02111bc8 (CleanupResources), 0x02111bdc (InitResources)
 * and 0x02111bf4 (the factory). Their paths are recorded per function in
 * config/tu_manifest.d/ov045/daObjKm2_Ukishima_c.json, which is where a
 * reference to a deleted file belongs.
 *
 * THE FIFTH IS THE FACTORY, and build/tu_map.json does not report it. Its
 * factory rule matches names ending `_Spawn`, which labels zero functions in
 * this image, so every `_classInit` is orphaned from its class and the map's
 * count for this run is a floor of 4, not the 5 the addresses give.
 */

#include "daObjKm2_Ukishima_c.h"

extern "C" {
/* ov002's shared floating-floor setup and teardown, still under placeholder
   names. Read off the literal pools at 0x02111bec and 0x02111bd4 rather than
   taken from relocs.txt, because tools/match.py wildcards every relocated word
   and cannot decide the identity of a call target. */
int func_ov002_020b6584(void *self, void *descriptor, int actorID);
int func_ov002_020b6424(void *self, void *descriptor);

/* The three-word block at ov045 0x02112f08 that those two helpers read: this
   level's model and collision file handles. Retail places it between the class
   typeinfo and type-name records. Nothing in this TU dereferences it, so it
   stays an opaque descriptor rather than a guess at its members, and it is
   DECLARED rather than defined -- this entry claims .text only. */
extern int data_ov045_02112f08[];
}

/* Field names are coined; the ov045 spelling is the one
   src/game/actors/d_a_obj_km2_gura.cpp already uses for the neighbouring
   profile in the same .data band. */
struct UkishimaSpawnInfo {
    daObjKm2_Ukishima_c *(*classInit)();
    s16 profileIDAndExecuteOrder;
    s16 drawOrder;
    u32 actorFlags;
    Fix12i clipOffsetY;
    Fix12i clipRadius;
    Fix12i clipDistance;
    Fix12i farDistance;
};

typedef char UkishimaSpawnInfo_size_must_be_0x1c[
    sizeof(UkishimaSpawnInfo) == 0x1c ? 1 : -1];

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- daObjKm2_Ukishima_c_classInit, 0x02111bf4, size 0x3c      */
/* -------------------------------------------------------------------------- */
// @symbol daObjKm2_Ukishima_c_classInit
/* Reconstructed source-style name: SM64DS proves the RTTI class, the
   KM2_UKISHIMA registry ID, the descriptor/factory relationship and the object
   shape; later EAD lineage supplies the spelling prior. Exact original SM64DS
   symbols are not preserved. Historical project aliases: FloatingFloorBfs_Spawn
   and FloatingFloorBfs_SpawnInfo.

   Every instruction the cartridge has here falls out of the one `new`.
   0x02111bf8 loads 0x32c -- the class's own size -- into the header's inline
   operator new; 0x02111c08 calls dBgActor_c's C2 and the two stores at
   0x02111c14 and 0x02111c18 lay down daObjUkiyuka_c's vptr and then this
   class's, both to offset 0, because both intervening constructors are
   implicit and inline. The null check is the one `new` itself emits.

   The inline `operator new` in the header is what keeps the allocation on the
   cartridge's fBase_c::operator new: spelled as a plain `new` without it, the
   call relocates to the unavailable global `_Znwm` instead, which is why the
   ov045 sibling src/game/actors/d_a_obj_km2_fall_block.cpp kept a C allocator
   veneer. */
extern "C" daObjKm2_Ukishima_c *daObjKm2_Ukishima_c_classInit()
{
    return new daObjKm2_Ukishima_c();
}

/* The profile ID and descriptor relationship survive in the cartridge; the
   local descriptor type name is reconstructed from that runtime role. Every
   word below is read out of ov045 0x02112f2c: the factory pointer, the packed
   0x00d80091 halves, and the two clip literals. */
extern "C" UkishimaSpawnInfo g_profile_KM2_UKISHIMA = {
    daObjKm2_Ukishima_c_classInit,
    0x0091,
    0x00d8,
    2,
    0x00060000,
    0x01000000,
    0x02000000,
    0
};

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN19daObjKm2_Ukishima_c13InitResourcesEv, 0x02111bdc,    */
/* size 0x18                                                                  */
/* -------------------------------------------------------------------------- */
// @symbol _ZN19daObjKm2_Ukishima_c13InitResourcesEv
/* Vtable slot 0, one of the two the abstract base leaves pure. A tail call
   into daObjUkiyuka_c's shared cross-overlay file-load helper with this
   level's descriptor and the actor id 0xf50, which 0x02111be4 loads as a bare
   `mov r2, #0xf50`. */
s32 daObjKm2_Ukishima_c::InitResources()
{
    return func_ov002_020b6584(this, data_ov045_02112f08, 0xf50);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN19daObjKm2_Ukishima_c16CleanupResourcesEv, 0x02111bc8, */
/* size 0x14                                                                  */
/* -------------------------------------------------------------------------- */
// @symbol _ZN19daObjKm2_Ukishima_c16CleanupResourcesEv
/* Vtable slot 3, the teardown half of the InitResources delegation over the
   same ov045 descriptor, and THE KEY FUNCTION of this class: the destructor is
   inline in the header, so the first declared non-inline virtual is what makes
   this TU emit the _ZTV/_ZTI/_ZTS group. */
s32 daObjKm2_Ukishima_c::CleanupResources()
{
    return func_ov002_020b6424(this, data_ov045_02112f08);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN19daObjKm2_Ukishima_cD0Ev, 0x02111b64, size 0x64       */
/* ROM ordinal 0 -- _ZN19daObjKm2_Ukishima_cD1Ev, 0x02111b14, size 0x50       */
/* -------------------------------------------------------------------------- */
// @symbol _ZN19daObjKm2_Ukishima_cD1Ev
// @symbol _ZN19daObjKm2_Ukishima_cD0Ev
/* NOT WRITTEN HERE ON PURPOSE. The inline `~daObjKm2_Ukishima_c() {}` in the
   header is the whole source of both variants: from an inline body mwcc emits
   D1 and then D0 -- the cartridge's own order -- and no leaf D2. Writing the
   body out of line here instead flips them to D0-before-D1 and the isolation
   step rejects the object.

   Their bodies are three vptr stores and the member destructions, every one a
   consequence of `daObjKm2_Ukishima_c : daObjUkiyuka_c : dBgActor_c`: this
   class's vptr at 0x02112f50, then daObjUkiyuka_c's at 0x0210912c and
   dBgActor_c's at 0x0210ae38 -- both inlined, because both destructors are
   defined in their class bodies -- then dBgActor_c's dBgW_KcMbg at +0x124 and
   Model at +0xd4, then dActor_c. This class adds no member with a destructor
   of its own.

   D0's trailing deallocation is the inline `operator delete` it inherits:
   0x02111ba4 loads GAME_HEAP_PTR out of arm9 0x020a0eac and 0x02111ba8 calls
   Memory::Deallocate, which is what that inline body expands to -- so nothing
   here names a heap. */
