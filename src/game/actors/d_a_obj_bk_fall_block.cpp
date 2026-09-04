//cpp
/* Production translation unit for ov015/daObjBk_Fall_Block_c, hand-curated.
 * 4 function(s), .text 0x02112cf4..0x02112dd0.
 *
 * ENROLLED AND CANONICAL. config/arm9/overlays/ov015/delinks.txt licenses that
 * whole run to this one path, so the ROM build compiles this file in place of
 * the four legacy one-function sources it replaces.
 *
 * Bob-omb Battlefield's falling block. It adds no state of its own to
 * daObjFallBlock_c (see include/daObjBk_Fall_Block_c.h) and overrides only the
 * two vtable slots the base leaves null -- both of which hand the actor and
 * this level's descriptor to the shared ov098 falling-block helpers. Its
 * already-promoted siblings are ov022/daObjFl_Fall_Block_c (Lethal Lava Land)
 * and ov045/daObjKm2_Fall_Block_c, and all three have the identical shape.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S. mwccarm 2004/b56
 * emits one .text section per function in the REVERSE of source order, so the
 * highest-address ROM function is written FIRST here. Do not reorder:
 * tools/rombuild.py refuses the object outright when the emitted order and the
 * ROM's disagree.
 *
 * Assembled from these legacy one-function sources (ROM address order):
 *   [0] 0x02112cf4  src/_ZN20daObjBk_Fall_Block_cD1Ev.cpp
 *   [1] 0x02112d44  src/_ZN20daObjBk_Fall_Block_cD0Ev.cpp
 *   [2] 0x02112da8  src/_ZN20daObjBk_Fall_Block_c16CleanupResourcesEv.cpp
 *   [3] 0x02112dbc  src/_ZN20daObjBk_Fall_Block_c13InitResourcesEv.cpp
 */

/* TUBUILD NOTE -- #pragma directive(s) were present in the legacy sources
 * of this TU. `#pragma long_calls` is POSITIONAL in mwccarm 2004/b56 and is
 * carried verbatim before its own member below, bracketed with `off` so it
 * cannot leak into later members (dropping it silently costs the pooled
 * cross-overlay tail-call -- a byte diff; see ShutterBob in ov014).
 * Any OTHER pragma is FILE-GLOBAL last-wins (opt_propagation,
 * optimize_for_size) and is still left out: carried into a merged TU it
 * would silently recompile every other member. Decide those by hand:
 *   _ZN20daObjBk_Fall_Block_c16CleanupResourcesEv: #pragma long_calls on   [carried below]
 *   _ZN20daObjBk_Fall_Block_c13InitResourcesEv: #pragma long_calls on   [carried below]
 */

/* Includes: union of the legacy files', first-seen in ROM-ascending
 * processing order. NOT verified for header ordering constraints (e.g. a
 * common.h-before-X rule) -- watch for new compile errors after this. */
#include "daObjBk_Fall_Block_c.h"

/* The three-word block at ov015 0x02114880 that ov098's shared setup and
   teardown helpers read: this level's model and collision file handles.
   Nothing in this TU dereferences it, so it stays an opaque descriptor rather
   than a guess at its members. */
struct ResourceDescriptor {
    void *entries[3];
};

extern "C" {
/* ov098's shared falling-block setup and teardown, still under placeholder
   names. Both take the actor and the per-level descriptor. */
int func_ov098_0213a2cc(daObjBk_Fall_Block_c *self, ResourceDescriptor *descriptor);
int func_ov098_0213a794(daObjBk_Fall_Block_c *self, ResourceDescriptor *descriptor);
extern ResourceDescriptor data_ov015_02114880;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN20daObjBk_Fall_Block_c13InitResourcesEv, 0x02112dbc, size 0x14 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN20daObjBk_Fall_Block_c13InitResourcesEv
/* Vtable slot 0. Delegates to the shared ov098 falling-block setup helper,
   handing it the Bob-omb Battlefield descriptor.

   Cross-overlay tail-call veneer. #pragma long_calls forces mwccarm to emit the
   pooled `ldr ip,[pc]; bx ip` indirect tail-call (a plain near `b` otherwise)
   that the ROM uses to reach another overlay. */
#pragma long_calls on  /* carried verbatim from the legacy file (positional) */
int daObjBk_Fall_Block_c::InitResources()
{
    return func_ov098_0213a794(this, &data_ov015_02114880);
}
#pragma long_calls off  /* close the bracket: positional, must not leak downward */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN20daObjBk_Fall_Block_c16CleanupResourcesEv, 0x02112da8, size 0x14 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN20daObjBk_Fall_Block_c16CleanupResourcesEv
/* Vtable slot 3, the teardown half of the InitResources delegation, over the
   same ov015 descriptor.

   Cross-overlay tail-call veneer. #pragma long_calls forces mwccarm to emit the
   pooled `ldr ip,[pc]; bx ip` indirect tail-call (a plain near `b` otherwise)
   that the ROM uses to reach another overlay. */
#pragma long_calls on  /* carried verbatim from the legacy file (positional) */
int daObjBk_Fall_Block_c::CleanupResources()
{
    return func_ov098_0213a2cc(this, &data_ov015_02114880);
}
#pragma long_calls off  /* close the bracket: positional, must not leak downward */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN20daObjBk_Fall_Block_cD0Ev, 0x02112d44, size 0x64      */
/* ROM ordinal 0 -- _ZN20daObjBk_Fall_Block_cD1Ev, 0x02112cf4, size 0x50      */
/* -------------------------------------------------------------------------- */
// @symbol _ZN20daObjBk_Fall_Block_cD1Ev
// @symbol _ZN20daObjBk_Fall_Block_cD0Ev
/* NOT WRITTEN HERE ON PURPOSE. The inline `~daObjBk_Fall_Block_c() {}` in the
   header is the whole source of both variants: from an inline body mwcc emits
   D1 and then D0 -- the cartridge's own order -- and no D2. Writing the body
   out of line here instead flips them to D0-before-D1 and the isolation step
   rejects the object.

   Their bodies are three vptr stores and the member destructions, every one a
   consequence of `daObjBk_Fall_Block_c : daObjFallBlock_c : dBgActor_c`: this
   class's vptr, then daObjFallBlock_c's and dBgActor_c's -- both inlined,
   because both destructors are defined in their class bodies -- then
   dBgActor_c's dBgW_KcMbg and Model, then dActor_c. This class adds no member
   with a destructor of its own, and D0's trailing deallocation is the inline
   `operator delete` it inherits, which is why nothing here names a heap. */
