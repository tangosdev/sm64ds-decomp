//cpp
// @symbol _ZN11dScMgBase_c19OnHitFromUnderneathEv
// recovered name: dScMgBase_c_OnHitFromUnderneath
/* recovered: renamed to Class_Method, RTTI class fields named, declarations from a shared header */
#include "decl_common.h"
#include "dScMgBase_c.h"
/* dScMgBase_c::OnHitFromUnderneath - recovered from vtable slot identity.

   THE 27-METHOD SHADOW SCAFFOLD IS GONE, and declaring slot 26 is what let it
   go.  This file used to carry a local `struct Base` of twenty-six placeholder
   virtuals plus `struct Obj : Base` with a `char pad[0x4627]`, existing for one
   reason: so that `self->Init()` would compile to a load of vtable+0x68.  Slot
   26 is OnHitByCannonBlastedChar and dScMgBase_c declares it now, so the class
   itself does that job and the call can be written for what it is.  The two
   raw offsets go with the scaffold -- 0x4628 is mMenuOpen and 0xf4 is
   mTouchOptions, both named fields on the real struct.

   Reading it back: a hit from underneath closes the three-item overlay menu
   that OnHitByMegaChar opens, resets the polymorphic touch-icon set, and
   re-enables the 3D engines only if the class's own OnHitByCannonBlastedChar
   says so.

   Still on the Door route -- an `extern "C"` definition of the mangled name
   rather than a `dScMgBase_c::` member -- for the reason the rest of this
   family is: a real out-of-line member definition here risks moving the key
   function and emitting _ZTV11dScMgBase_c from this TU.  Including the header
   and calling through it costs nothing; defining a member would. */

extern "C" void func_02012e1c(char *c);
extern "C" void Enable3dEngines();

int dScMgBase_c::OnHitFromUnderneath()
{
    void *c = (void *)this;

    struct dScMgBase_c *self = (struct dScMgBase_c *)(void *)c;
    func_02012e1c((char *)self);
    self->mMenuOpen = 0;
    func_ov004_020b91fc((char *)&self->mTouchOptions);
    int r = self->OnHitByCannonBlastedChar();
    if (r == 0) return;
    Enable3dEngines();
}
