//cpp
// @symbol _ZN7fBase_c13InitResourcesEv
/* fBase_c::InitResources() at 0x02043c80 -- vtable slot 0. Base does nothing
 * and returns VS_FAIL (1); leaf classes override slot 0 to load models, anims
 * and collision.
 *
 * Deliberately defined WITHOUT including fBase_c.h, i.e. not as a real
 * `s32 fBase_c::InitResources()`. InitResources is the first virtual declared
 * in the class, which makes it the key function: CW 1.2 emits the vtable into
 * whichever TU defines it. The vtable is already supplied as ROM data by the
 * module's gap object, so a real method definition here makes the ROM link fail
 * with
 *
 *     mwldarm.exe: Multiply-defined: "virtual table for fBase_c"
 *
 * include/Fader.h avoids this only by accident -- its first declared virtual is
 * the destructor, and the destructor TUs are C files that never see the class,
 * so no TU is ever the key function's definition. fBase_c cannot copy that,
 * because its destructor is pinned to slots 16/17 and cannot be declared first.
 *
 * Keeping this one function out of the class is what leaves the other 24 free to
 * be real methods.
 */
extern "C" int _ZN7fBase_c13InitResourcesEv(void)
{
    return 1; /* VS_FAIL */
}
