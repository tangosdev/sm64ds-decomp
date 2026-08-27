//cpp
// @symbol _ZN10dFdDummy_c11AdvanceFadeEv
/* recovered: real C++ method
 *
 * Vtable slot 2, arm9 0x0201721c. The whole body is a tail call into the base
 * chain's own AdvanceInterp at 0x020175e8 -- three words, `ldr ip,[pc]; bx ip;
 * .word'. `this' rides through in r0 untouched, which is exactly what the
 * inherited implementation wants, so the dummy variant adds nothing here.
 *
 * The base implementation is still a mangled free function, so it is reached
 * through decl_Fader.h's extern "C" declaration rather than as Fader::.
 */
#include "dFdDummy_c.h"
#include "decl_Fader.h"

void dFdDummy_c::AdvanceFade()
{
    _ZN5Fader13AdvanceInterpEv();
}
