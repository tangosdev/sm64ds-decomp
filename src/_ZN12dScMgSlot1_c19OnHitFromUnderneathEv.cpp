//cpp
// @symbol _ZN12dScMgSlot1_c19OnHitFromUnderneathEv
/* dScMgSlot1_c::OnHitFromUnderneath -- slot 28, and a REAL OVERRIDE as of this
   commit: dScMgBase_c declares the slot now, so this stops being a new virtual
   that merely happened to land on index 28 and becomes the override the
   cartridge has.  It was the last virtual in this class that mwcc numbered for
   itself; include/dScMgSlot1_c.h's banner carries the arithmetic that retires
   with it.

   THE PARAMETER IS GONE, and unlike slot 27's it comes off on the base's
   evidence alone.  dScMgBase_c's body at ov004:0x020af04c writes r1
   (`add r1, r4, #0x4000`) before it ever reads it and touches no other
   argument register, so nothing in the family consumes a second argument.
   This body cannot corroborate that the way the slot-27 override did: it calls
   the base as its very first act, so a second argument would ride through r1
   untouched and leave no trace.  The `dActor_c &other` this file used to carry
   was copied from include/dActor_c.h's slot 28, a parallel hierarchy, and
   dActor_c.h has been wrong on every parameter list this campaign has
   measured.  What is NOT invisible is DISAGREEMENT: `(dActor_c &)` and `()`
   are different functions, so keeping it would have made this a new slot at 29
   and put _ZTV12dScMgSlot1_c back into DIFFERS -- with rombuild green the
   whole time.

   THE RETURN TYPE IS NOT MEASURED, and slot 28 is the first in this campaign
   where that is true.  Neither body sets r0 deliberately: the base falls out of
   a virtual call it has just compared against zero, and this one falls out of
   SetSubBg1Offset.  Nor does any caller settle it -- ov004 and ov006 hold
   exactly three `ldr rN,[rM,#0x70]; blx rN` dispatch sites and all three
   discard the result, written out site by site in dScMgBase_c.h.  (An earlier
   draft of this comment said no caller loads vtable+0x70 at all.  That was
   reasoning from an absence never scanned for, and it was wrong: a bare `ldr`
   at +0x70 is an ordinary field read, and only the load/`blx` PAIR is a
   dispatch.)
     So the slot is unpinned, and it is spelled `void` -- the only type both
   definitions can honour.  `int` is include/dActor_c.h's hint from a parallel
   hierarchy, and declaring it would leave this body falling off the end of a
   non-void function, which is undefined behaviour that byte-matches only by
   accident of what SetSubBg1Offset happens to leave in r0.  `void` compiles to
   the same bytes.

   The forwarding call is written qualified, dScMgBase_c::OnHitFromUnderneath(),
   which suppresses the virtual dispatch and emits the same direct `bl` the ROM
   has -- straight to ov004:0x020af04c, not through ov006's veneer.  It was
   previously spelled as an extern "C" call to the base body's pre-rename name,
   which is the only spelling that was available while the base had not
   declared the member. */
#include "decl_common.h"
#include "dScMgSlot1_c.h"

void dScMgSlot1_c::OnHitFromUnderneath()
{
    dScMgBase_c::OnHitFromUnderneath();
    SetSubBg1Offset(0x100, 0);
}
