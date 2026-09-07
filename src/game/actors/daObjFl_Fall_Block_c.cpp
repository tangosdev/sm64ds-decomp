//cpp
/* Production translation unit for ov022/daObjFl_Fall_Block_c, hand-curated.
 * 4 function(s), .text 0x02112380..0x0211245c.
 *
 * ENROLLED AND CANONICAL. config/arm9/overlays/ov022/delinks.txt licenses that
 * whole run to this one path, so the ROM build compiles this file in place of
 * the four legacy one-function sources it replaces.
 *
 * The Lethal Lava Land falling block. It adds no state of its own to
 * daObjFallBlock_c (see include/daObjFl_Fall_Block_c.h) and overrides only the
 * two vtable slots the base leaves null -- both of which hand the actor and
 * this level's descriptor to the shared ov098 falling-block helpers.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S. mwccarm 2004/b56
 * emits one .text section per function in the REVERSE of source order, so the
 * highest-address ROM function is written FIRST here. Do not reorder:
 * tools/rombuild.py refuses the object outright when the emitted order and the
 * ROM's disagree.
 *
 * Assembled from these legacy one-function sources (ROM address order):
 *   [0] 0x02112380  src/_ZN20daObjFl_Fall_Block_cD1Ev.cpp
 *   [1] 0x021123d0  src/_ZN20daObjFl_Fall_Block_cD0Ev.cpp
 *   [2] 0x02112434  src/_ZN20daObjFl_Fall_Block_c16CleanupResourcesEv.cpp
 *   [3] 0x02112448  src/_ZN20daObjFl_Fall_Block_c13InitResourcesEv.cpp
 */

#include "daObjFl_Fall_Block_c.h"

/* The three-word block at ov022 0x0211427c that ov098's shared setup and
   teardown helpers read: this level's model and collision file handles.
   Nothing in this TU dereferences it, so it stays an opaque descriptor rather
   than a guess at its members. */
struct ResourceDescriptor {
    void *entries[3];
};

extern "C" {
/* ov098's shared falling-block setup and teardown, still under placeholder
   names. Both take the actor and the per-level descriptor. */
int func_ov098_0213a2cc(daObjFl_Fall_Block_c *self, ResourceDescriptor *descriptor);
int func_ov098_0213a794(daObjFl_Fall_Block_c *self, ResourceDescriptor *descriptor);
extern ResourceDescriptor data_ov022_0211427c;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN20daObjFl_Fall_Block_c13InitResourcesEv, 0x02112448, 0x14 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN20daObjFl_Fall_Block_c13InitResourcesEv
/* Vtable slot 0. Delegates to the shared ov098 falling-block setup helper,
   handing it the Lethal Lava Land descriptor. */
int daObjFl_Fall_Block_c::InitResources()
{
    return func_ov098_0213a794(this, &data_ov022_0211427c);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN20daObjFl_Fall_Block_c16CleanupResourcesEv, 0x02112434, 0x14 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN20daObjFl_Fall_Block_c16CleanupResourcesEv
/* Vtable slot 3, the teardown half of the InitResources delegation, over the
   same ov022 descriptor. */
int daObjFl_Fall_Block_c::CleanupResources()
{
    return func_ov098_0213a2cc(this, &data_ov022_0211427c);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN20daObjFl_Fall_Block_cD0Ev, 0x021123d0, 0x64            */
/* ROM ordinal 0 -- _ZN20daObjFl_Fall_Block_cD1Ev, 0x02112380, 0x50            */
/* -------------------------------------------------------------------------- */
// @symbol _ZN20daObjFl_Fall_Block_cD1Ev
// @symbol _ZN20daObjFl_Fall_Block_cD0Ev
/* NOT WRITTEN HERE ON PURPOSE. The inline `~daObjFl_Fall_Block_c() {}` in the
   header is the whole source of both variants: from an inline body mwcc emits
   D1 and then D0 -- the cartridge's own order -- and no D2. Writing the body
   out of line here instead flips them to D0-before-D1 and the isolation step
   rejects the object.
 
   Their bodies are three vptr stores and the member destructions, every one a
   consequence of `daObjFl_Fall_Block_c : daObjFallBlock_c : dBgActor_c`: this
   class's vptr, then daObjFallBlock_c's and dBgActor_c's -- both inlined,
   because both destructors are defined in their class bodies -- then
   dBgActor_c's dBgW_KcMbg and Model, then dActor_c. This class adds no member
   with a destructor of its own, and D0's trailing deallocation is the inline
   `operator delete` it inherits, which is why nothing here names a heap. */
