//cpp
// @symbol _ZN9ModelBase12ApplyOpacityEj
#include "ModelBase.h"
extern "C" void func_020461b4(ModelComponents *data, u32 a);

/* The base itself ends at 0x8; this addresses the derived class's components
   view at +0x8 blindly. That is only meaningful where the components are
   EMBEDDED there (Model) -- on the pointer-holding siblings this slot is a
   ModelComponents*, so the original can only ever have called this on a
   Model-shaped object. The cast preserves the ROM's this+8. */
void ModelBase::ApplyOpacity(u32 a)
{
    func_020461b4((ModelComponents *)((char *)this + 8), a);
}
