//cpp
// @symbol func_ov006_020e9e00
// recovered name: dScMg3DEsp_c_Behavior
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* dScMg3DEsp_c::Behavior - recovered from vtable slot identity */
class C;
typedef void (C::*PMF)();

extern "C" PMF data_ov006_02141f2c[];
extern "C" void func_ov006_020e8a44(C *self);
extern "C" void _ZN9Animation7AdvanceEv(void *anim);

extern "C" int func_ov006_020e9e00(C *self)
{
    char *f = (char *)self;
    int idx = *(int *)(f + 0x553c);
    (self->*data_ov006_02141f2c[idx])();
    func_ov006_020e8a44(self);
    _ZN9Animation7AdvanceEv(f + 0x51f4);
    func_ov006_020e7be8(f + 0x4fd8);
    return 1;
}
