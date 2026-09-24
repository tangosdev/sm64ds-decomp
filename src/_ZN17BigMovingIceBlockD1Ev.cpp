//cpp
// @symbol _ZN17BigMovingIceBlockD1Ev
/* Real compiler-spelled complete destructor. BigMovingIceBlock's destructor
 * is inline in the class so its D1/D0 order and lack of a homeless D2 match
 * the original TU. This uncalled helper forces mwcc to materialize D1;
 * objisolate keeps the enrolled destructor and discards the helper. */
#include "BigMovingIceBlock.h"

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D1 NAME, AND MSVC NEVER EMITS IT.
 * ~BigMovingIceBlock() is defined in the class body, and MSVC emits an inline
 * destructor only for a caller that odr-uses it. The port's slot 16 for this
 * class and the class's own D0 arm (src/_ZN17BigMovingIceBlockD0Ev.cpp) call
 * the flat name, so this arm defines it as the qualified, direct call that
 * makes MSVC emit the body: the text port/tools/dtorfwd.py generated for this
 * name into hal/dtor_forwarders_gen.cpp until run linkfull wave 27, and the
 * same object code. QUALIFIED on purpose: `self->~BigMovingIceBlock()` would
 * be a virtual call through the port's ROM-shaped table at slot 16, straight
 * back into this symbol. The inline chain MSVC emits stores an MSVC vftable
 * (??_7dBgActor_c@@6B@, measured) where the cartridge's D1 (0x021111a0, ov056)
 * stores _ZTV17BigMovingIceBlock; port/tools/dtor_store_guard.py reads this
 * arm and re-proves from the ROM body on every build that nothing dispatches
 * before the base destructor restores a ROM-shaped table. Nothing here reaches
 * mwccarm: it builds the `#else` arm and emits the ROM bytes it always
 * emitted. */
extern "C" void _ZN17BigMovingIceBlockD1Ev(void *self)
{ ((BigMovingIceBlock *)self)->BigMovingIceBlock::~BigMovingIceBlock(); }
#else
void BigMovingIceBlock_EmitDestructor(BigMovingIceBlock *p)
{
    p->~BigMovingIceBlock();
}
#endif
