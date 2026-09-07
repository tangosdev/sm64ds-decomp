//cpp
// @symbol _ZN12dScMgSlot1_c15OnHitByMegaCharEv
/* dScMgSlot1_c::OnHitByMegaChar -- slot 27, and a REAL OVERRIDE as of this
   commit: dScMgBase_c declares the slot now, so this stops being a new virtual
   that merely happened to land on index 27 and becomes the override the
   cartridge has.  include/dScMgSlot1_c.h's banner carries the arithmetic.

   THE PARAMETER IS GONE, and that is a measurement rather than a tidy-up.  This
   body opens `mov r4, r0` and then writes r0 and r1 with zero for
   SetSubBg1Offset; no second argument register is live on entry, and
   dScMgBase_c's own body reads none either.  The `Player &player` this file
   used to carry was copied from include/dActor_c.h's slot 27, a parallel
   hierarchy -- and dActor_c.h has now been wrong on every parameter list this
   campaign has measured.  Keeping it would have been worse than cosmetic:
   `(Player &)` and `()` are different functions, so this would have become a
   NEW slot at 28, pushed OnHitFromUnderneath to 29 and put _ZTV12dScMgSlot1_c
   back into DIFFERS -- with rombuild green the whole time.

   The return type is unchanged and still `void`.  dScMgBase_c's body measures
   it directly now (an early `popne {r4,lr}; bxne lr` with no r0 set), where
   before this file could only cite Stump::OnHitByMegaChar from the other
   branch.

   The forwarding call is written qualified, `dScMgBase_c::OnHitByMegaChar()`,
   which suppresses the virtual dispatch and emits the same direct `bl` the ROM
   has.  It was previously spelled as an `extern "C"` call to the base body's
   pre-rename name, which is the only spelling that was available while the
   base had not declared the member. */
#include "decl_common.h"
#include "dScMgSlot1_c.h"

void dScMgSlot1_c::OnHitByMegaChar()
{
    SetSubBg1Offset(0, 0);
    dScMgBase_c::OnHitByMegaChar();
}
