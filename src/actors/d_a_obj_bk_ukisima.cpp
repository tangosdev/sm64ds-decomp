//cpp
/* Production translation unit for ov015/daObjBk_Ukisima_c, hand-curated.
 * 4 function(s), .text 0x02112bd0..0x02112cb8.
 *
 * Bob-omb Battlefield's spinning disc. It adds no state of its own to
 * daObjKaitendai_c (see include/daObjBk_Ukisima_c.h) and overrides only the two
 * vtable slots the base leaves null -- both of which hand the actor and this
 * level's descriptor to daObjKaitendai_c's shared ov002 helpers. Its immediate
 * ROM neighbour ov015/daObjBk_Fall_Block_c (0x02112cf4, promoted) has the same
 * four-function shape, as do ov022/daObjFl_Fall_Block_c and
 * ov045/daObjKm2_Fall_Block_c.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S. mwccarm 2004/b56
 * emits one .text section per function in the REVERSE of source order, so the
 * highest-address ROM function is written FIRST here. Do not reorder:
 * tools/rombuild.py refuses the object outright when the emitted order and the
 * ROM's disagree.
 *
 * Assembled from these legacy one-function sources (ROM address order):
 *   [0] 0x02112bd0  src/_ZN17daObjBk_Ukisima_cD1Ev.cpp
 *   [1] 0x02112c20  src/_ZN17daObjBk_Ukisima_cD0Ev.cpp
 *   [2] 0x02112c84  src/_ZN17daObjBk_Ukisima_c16CleanupResourcesEv.cpp
 *   [3] 0x02112c98  src/_ZN17daObjBk_Ukisima_c13InitResourcesEv.cpp
 */

/* TUBUILD NOTE -- #pragma directive(s) were present in the legacy sources of
 * this TU. `#pragma long_calls` is POSITIONAL in mwccarm 2004/b56 and is
 * carried verbatim before its own member below, bracketed with `off` so it
 * cannot leak into later members (dropping it silently costs the pooled
 * cross-overlay tail-call -- a byte diff). Only CleanupResources carried one;
 * InitResources deliberately carries none, exactly as its legacy file had none.
 *   _ZN17daObjBk_Ukisima_c16CleanupResourcesEv: #pragma long_calls on   [carried below]
 */

/* Includes: union of the legacy files', first-seen in ROM-ascending
 * processing order. */
#include "daObjBk_Ukisima_c.h"

/* The three-word block at ov015 0x021147a4 that daObjKaitendai_c's shared ov002
   setup and teardown helpers read: this level's model and collision file
   handles. Nothing in this TU dereferences it, so it stays an opaque descriptor
   rather than a guess at its members. data_ov015_02114794 is this class's own
   starting-angle static, passed alongside it. */
struct ResourceDescriptor {
    void *entries[3];
};

extern "C" {
/* daObjKaitendai_c's shared rotating-platform setup and teardown, still under
   placeholder names. */
int func_ov002_020b676c(daObjBk_Ukisima_c *self, ResourceDescriptor *descriptor, short angle);
int func_ov002_020b66a8(daObjBk_Ukisima_c *self, ResourceDescriptor *descriptor);
extern ResourceDescriptor data_ov015_021147a4;
extern short data_ov015_02114794;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN17daObjBk_Ukisima_c13InitResourcesEv, 0x02112c98, size 0x20 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN17daObjBk_Ukisima_c13InitResourcesEv
/* Vtable slot 0. Delegates to daObjKaitendai_c's shared ov002 setup helper,
   handing it the Bob-omb Battlefield descriptor and this disc's start angle.
   No `#pragma long_calls` here: the legacy file carried none, and the ROM's
   call site is the plain near form. */
int daObjBk_Ukisima_c::InitResources()
{
    return func_ov002_020b676c(this, &data_ov015_021147a4, data_ov015_02114794);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN17daObjBk_Ukisima_c16CleanupResourcesEv, 0x02112c84, size 0x14 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN17daObjBk_Ukisima_c16CleanupResourcesEv
/* Vtable slot 3, the teardown half of the InitResources delegation, over the
   same ov015 descriptor.

   Cross-overlay tail-call veneer. #pragma long_calls forces mwccarm to emit the
   pooled `ldr ip,[pc]; bx ip` indirect tail-call (a plain near `b` otherwise)
   that the ROM uses to reach another overlay. */
#pragma long_calls on  /* carried verbatim from the legacy file (positional) */
int daObjBk_Ukisima_c::CleanupResources()
{
    return func_ov002_020b66a8(this, &data_ov015_021147a4);
}
#pragma long_calls off  /* close the bracket: positional, must not leak downward */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN17daObjBk_Ukisima_cD0Ev, 0x02112c20, size 0x64        */
/* ROM ordinal 0 -- _ZN17daObjBk_Ukisima_cD1Ev, 0x02112bd0, size 0x50        */
/* -------------------------------------------------------------------------- */
// @symbol _ZN17daObjBk_Ukisima_cD1Ev
// @symbol _ZN17daObjBk_Ukisima_cD0Ev
/* NOT WRITTEN HERE ON PURPOSE. The inline `~daObjBk_Ukisima_c() {}` in the
   header is the whole source of both variants: from an inline body mwcc emits
   D1 and then D0 -- the cartridge's own order -- and no D2. Writing the body
   out of line here instead flips them to D0-before-D1 and the isolation step
   rejects the object.

   Their bodies are three vptr stores and the member destructions, every one a
   consequence of `daObjBk_Ukisima_c : daObjKaitendai_c : dBgActor_c`: this
   class's vptr, then daObjKaitendai_c's and dBgActor_c's -- both inlined,
   because both destructors are defined in their class bodies -- then
   dBgActor_c's dBgW_KcMbg and Model, then dActor_c. This class adds no member
   with a destructor of its own, and D0's trailing deallocation is the inline
   `operator delete` it inherits, which is why nothing here names a heap. */
