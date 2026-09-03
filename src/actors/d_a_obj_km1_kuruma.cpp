//cpp
/* Production translation unit for ov043/daObjKm1_Kuruma_c, hand-curated.
 * 4 function(s), .text 0x02111518..0x021115f4.
 *
 * ENROLLED AND CANONICAL. config/arm9/overlays/ov043/delinks.txt licenses that
 * whole run to this one path, so the ROM build compiles this file in place of
 * the four legacy one-function sources it replaces.
 *
 * The Bob-omb Battlefield rickshaw platform -- the cart that rides on the axle
 * (daObjKm1_Kurumajiku_c, the same overlay's other half). It adds no state of
 * its own to daObjKuruma_c (see include/daObjKm1_Kuruma_c.h) and overrides
 * only the two vtable slots the base leaves null, both of which hand the actor
 * and this level's descriptor to the shared ov002 helpers.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S. mwccarm 2004/b56
 * emits one .text section per function in the REVERSE of source order, so the
 * highest-address ROM function is written FIRST here. Do not reorder:
 * tools/rombuild.py refuses the object outright when the emitted order and the
 * ROM's disagree.
 *
 * Assembled from these legacy one-function sources (ROM address order):
 *   [0] 0x02111518  src/_ZN17daObjKm1_Kuruma_cD1Ev.cpp
 *   [1] 0x02111568  src/_ZN17daObjKm1_Kuruma_cD0Ev.cpp
 *   [2] 0x021115cc  src/_ZN17daObjKm1_Kuruma_c16CleanupResourcesEv.cpp
 *   [3] 0x021115e0  src/_ZN17daObjKm1_Kuruma_c13InitResourcesEv.cpp
 */

/* Includes: union of the legacy files', first-seen in ROM-ascending
 * processing order. NOT verified for header ordering constraints (e.g. a
 * common.h-before-X rule) -- watch for new compile errors after this. */
#include "daObjKm1_Kuruma_c.h"

/* The three-word block at ov043 0x02112418 that ov002's shared setup and
   teardown helpers read: this level's model and collision file handles.
   Nothing in this TU dereferences it, so it stays an opaque descriptor rather
   than a guess at its members. */
struct ResourceDescriptor {
    void *entries[3];
};

extern "C" {
/* ov002's shared rickshaw setup and teardown, still under placeholder names.
   Both take the actor and the per-level descriptor. */
int func_ov002_020b68b0(daObjKm1_Kuruma_c *self, ResourceDescriptor *descriptor);
int func_ov002_020b6958(daObjKm1_Kuruma_c *self, ResourceDescriptor *descriptor);
extern ResourceDescriptor data_ov043_02112418;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN17daObjKm1_Kuruma_c13InitResourcesEv, 0x021115e0, size 0x14 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN17daObjKm1_Kuruma_c13InitResourcesEv
/* Vtable slot 0. Delegates to the shared ov002 setup helper, handing it the
   Bob-omb Battlefield descriptor. */
int daObjKm1_Kuruma_c::InitResources()
{
    return func_ov002_020b6958(this, &data_ov043_02112418);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN17daObjKm1_Kuruma_c16CleanupResourcesEv, 0x021115cc, size 0x14 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN17daObjKm1_Kuruma_c16CleanupResourcesEv
/* Vtable slot 3, the teardown half of the InitResources delegation, over the
   same ov043 descriptor. */
int daObjKm1_Kuruma_c::CleanupResources()
{
    return func_ov002_020b68b0(this, &data_ov043_02112418);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN17daObjKm1_Kuruma_cD0Ev, 0x02111568, size 0x64        */
/* ROM ordinal 0 -- _ZN17daObjKm1_Kuruma_cD1Ev, 0x02111518, size 0x50        */
/* -------------------------------------------------------------------------- */
// @symbol _ZN17daObjKm1_Kuruma_cD1Ev
// @symbol _ZN17daObjKm1_Kuruma_cD0Ev
/* NOT WRITTEN HERE ON PURPOSE. The inline `~daObjKm1_Kuruma_c() {}` in the
   header is the whole source of both variants: from an inline body mwcc emits
   D1 and then D0 -- the cartridge's own order -- and no D2. Writing the body
   out of line here instead flips them to D0-before-D1 and the isolation step
   rejects the object.

   Their bodies are three vptr stores and the member destructions, every one a
   consequence of `daObjKm1_Kuruma_c : daObjKuruma_c : dBgActor_c`: this
   class's vptr, then daObjKuruma_c's and dBgActor_c's -- both inlined,
   because both destructors are defined in their class bodies -- then
   dBgActor_c's dBgW_KcMbg and Model, then dActor_c. This class adds no member
   with a destructor of its own, and D0's trailing deallocation is the inline
   `operator delete` it inherits, which is why nothing here names a heap. */
