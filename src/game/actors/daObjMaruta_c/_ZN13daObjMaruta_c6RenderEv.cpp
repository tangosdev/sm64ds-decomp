//cpp
// @symbol _ZN13daObjMaruta_c6RenderEv
#include "daObjMaruta_c.h"
/* daObjMaruta_c::Render - dispatches through the Model at 0xd4 (dBgActor_c's
   mModel, inherited). The shadow draft cast an offset-0xd4 sub-object to a
   5-entry vtable-shim struct and called its slot-5 method; that slot is
   Model::Render(const Vector3*) (see Model.h), so this calls it by name
   with a null scale, matching the ROM's literal-0 argument. */
s32 daObjMaruta_c::Render()
{
    mModel.Render(0);
    return 1;
}
