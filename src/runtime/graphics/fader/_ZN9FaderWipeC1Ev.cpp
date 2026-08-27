//cpp
// @symbol _ZN9FaderWipeC1Ev
/* recovered: real C++ constructor -- the compiler emits the whole body
 *
 * C1 is the COMPLETE-object constructor at 0x02017480, and the ROM's clearest
 * single statement of this chain's shape. Its body is empty; everything in the
 * emitted function is a consequence of `struct FaderWipe : FaderColor` plus the
 * `Model model` member:
 *
 *   str  _ZTV5Fader             Fader's (abstract) sub-object ctor, inlined
 *   str  _ZTV15FaderBrightness  FaderBrightness's, inlined
 *   currInterp = 0x1000, speed = 0        ... its two field stores
 *   str  _ZTV10FaderColor       FaderColor's, inlined
 *   color = 0                             ... its one field store
 *   str  _ZTV9FaderWipe         this class's own vptr
 *   bl   _ZN5ModelC1Ev(this+0x10)         the member, constructed last
 *
 * The three inlined base constructors are inline in their headers precisely so
 * they inline here; Model's is DECLARED and never defined, which is what keeps
 * it an out-of-line `bl` instead of being inlined too.
 */
#include "FaderWipe.h"

FaderWipe::FaderWipe()
{
}
