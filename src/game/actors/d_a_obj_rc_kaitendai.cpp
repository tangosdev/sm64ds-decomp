//cpp
/* Production translation unit for ov036/daObjRc_Kaitendai_c, hand-curated.
 * 4 function(s), .text 0x02111444..0x02111544.
 *
 * Rainbow Ride's spinning disc. It adds no state of its own to daObjKaitendai_c
 * (see include/daObjRc_Kaitendai_c.h) and overrides only the two vtable slots
 * the base leaves null -- both of which hand the actor and this level's
 * descriptor to daObjKaitendai_c's shared ov002 helpers. Its already-promoted
 * sibling ov015/daObjBk_Ukisima_c has the same four-function shape; the one
 * difference is that this one picks between two start angles on param1.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S. mwccarm 2004/b56
 * emits one .text section per function in the REVERSE of source order, so the
 * highest-address ROM function is written FIRST here. Do not reorder:
 * tools/rombuild.py refuses the object outright when the emitted order and the
 * ROM's disagree.
 *
 * Assembled from these legacy one-function sources (ROM address order):
 *   [0] 0x02111444  src/_ZN19daObjRc_Kaitendai_cD1Ev.cpp
 *   [1] 0x02111494  src/_ZN19daObjRc_Kaitendai_cD0Ev.cpp
 *   [2] 0x021114f8  src/_ZN19daObjRc_Kaitendai_c16CleanupResourcesEv.cpp
 *   [3] 0x0211150c  src/_ZN19daObjRc_Kaitendai_c13InitResourcesEv.cpp
 */

/* TUBUILD NOTE -- #pragma directive(s) were present in the legacy sources of
 * this TU. `#pragma long_calls` is POSITIONAL in mwccarm 2004/b56 and is
 * carried verbatim before its own member below, bracketed with `off` so it
 * cannot leak into later members (dropping it silently costs the pooled
 * cross-overlay tail-call -- a byte diff). Only CleanupResources carried one;
 * InitResources deliberately carries none, exactly as its legacy file had none.
 *   _ZN19daObjRc_Kaitendai_c16CleanupResourcesEv: #pragma long_calls on   [carried below]
 */

/* Includes: union of the legacy files', first-seen in ROM-ascending
 * processing order. */
#include "daObjRc_Kaitendai_c.h"

extern "C" {
/* daObjKaitendai_c's shared rotating-platform setup and teardown, still under
   placeholder names. data_ov036_02113b2c is this level's file-table pointer;
   the two shorts are the two start angles this disc chooses between. */
extern int func_ov002_020b676c(void *self, void *arg, short val);
extern int func_ov002_020b66a8(void *self, void *data);
extern short data_ov036_02113b18;
extern short data_ov036_02113b1c;
extern void *data_ov036_02113b2c;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN19daObjRc_Kaitendai_c13InitResourcesEv, 0x0211150c, size 0x38 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN19daObjRc_Kaitendai_c13InitResourcesEv
/* Vtable slot 0. Delegates to daObjKaitendai_c's shared ov002 setup helper.
   Unlike its ov015 sibling, the angle argument is picked between two
   per-instance statics on param1 (fBase_c's own field, include/fBase_c.h) --
   param1 == 1 selects the alternate angle. No `#pragma long_calls` here: the
   legacy file carried none. */
int daObjRc_Kaitendai_c::InitResources()
{
    short v = data_ov036_02113b18;
    if ((param1 & 0xff) == 1) {
        v = data_ov036_02113b1c;
    }
    return func_ov002_020b676c(this, &data_ov036_02113b2c, v);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN19daObjRc_Kaitendai_c16CleanupResourcesEv, 0x021114f8, size 0x14 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN19daObjRc_Kaitendai_c16CleanupResourcesEv
/* Vtable slot 3, the teardown half of the InitResources delegation, over the
   same ov036 file table.

   Cross-overlay tail-call veneer. #pragma long_calls forces mwccarm to emit the
   pooled `ldr ip,[pc]; bx ip` indirect tail-call (a plain near `b` otherwise)
   that the ROM uses to reach another overlay. */
#pragma long_calls on  /* carried verbatim from the legacy file (positional) */
int daObjRc_Kaitendai_c::CleanupResources()
{
    return func_ov002_020b66a8(this, &data_ov036_02113b2c);
}
#pragma long_calls off  /* close the bracket: positional, must not leak downward */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN19daObjRc_Kaitendai_cD0Ev, 0x02111494, size 0x64      */
/* ROM ordinal 0 -- _ZN19daObjRc_Kaitendai_cD1Ev, 0x02111444, size 0x50      */
/* -------------------------------------------------------------------------- */
// @symbol _ZN19daObjRc_Kaitendai_cD1Ev
// @symbol _ZN19daObjRc_Kaitendai_cD0Ev
/* NOT WRITTEN HERE ON PURPOSE. The inline `~daObjRc_Kaitendai_c() {}` in the
   header is the whole source of both variants: from an inline body mwcc emits
   D1 and then D0 -- the cartridge's own order -- and no D2. Writing the body
   out of line here instead flips them to D0-before-D1 and the isolation step
   rejects the object.

   Their bodies are three vptr stores and the member destructions, every one a
   consequence of `daObjRc_Kaitendai_c : daObjKaitendai_c : dBgActor_c`: this
   class's vptr, then daObjKaitendai_c's and dBgActor_c's -- both inlined,
   because both destructors are defined in their class bodies -- then
   dBgActor_c's dBgW_KcMbg and Model, then dActor_c. This class adds no member
   with a destructor of its own, and D0's trailing deallocation is the inline
   `operator delete` it inherits, which is why nothing here names a heap. */
