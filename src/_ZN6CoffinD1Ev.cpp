//cpp
// @symbol _ZN6CoffinD1Ev
/* The class-body destructor is real C++. This otherwise-unused explicit call
 * forces mwccarm to emit its out-of-line D1 copy; objisolate keeps that symbol
 * and discards the forcing wrapper. */
#include "Coffin.h"

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D1 NAME, AND MSVC NEVER EMITS IT. ~Coffin() is
 * defined in the class body, and MSVC emits an inline destructor only for a
 * caller that odr-uses it. The port's slot 16 for this class and the class's
 * own D0 arm (src/_ZN6CoffinD0Ev.cpp) call the flat name, so this arm defines
 * it as the qualified, direct call that makes MSVC emit the body: the text
 * port/tools/dtorfwd.py generated for this name into
 * hal/dtor_forwarders_gen.cpp until run linkfull wave 27, and the same object
 * code. QUALIFIED on purpose: `self->~Coffin()` would be a virtual call
 * through the port's ROM-shaped table at slot 16, straight back into this
 * symbol. The inline chain MSVC emits stores an MSVC vftable
 * (??_7dBgActor_c@@6B@, measured) where the cartridge's D1 (0x02121fe4, ov071)
 * stores _ZTV6Coffin; port/tools/dtor_store_guard.py reads this arm and
 * re-proves from the ROM body on every build that nothing dispatches before
 * the base destructor restores a ROM-shaped table. Nothing here reaches
 * mwccarm: it builds the `#else` arm and emits the ROM bytes it always
 * emitted. */
extern "C" void _ZN6CoffinD1Ev(void *self)
{ ((Coffin *)self)->Coffin::~Coffin(); }
#else
void Coffin_EmitDestructor(Coffin *p)
{
    p->~Coffin();
}
#endif
