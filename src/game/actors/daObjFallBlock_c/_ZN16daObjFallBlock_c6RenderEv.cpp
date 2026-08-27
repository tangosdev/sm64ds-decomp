//cpp
// @symbol _ZN16daObjFallBlock_c6RenderEv
#include "daObjFallBlock_c.h"
/* daObjFallBlock_c::Render - the draft cast the Model sub-object at
   offset 0xd4 to a 5-entry vtable-shim struct and called its slot-5
   method by index. That sub-object is dBgActor_c's own named
   `Model mModel;` field, and slot 5 is Model::Render(const Vector3*)
   (Model.h), so this calls it by name with a null scale. */
s32 daObjFallBlock_c::Render()
{
    if (mSuppressed) return 1;
    if (mState == 3) return 1;
    mModel.Render(0);
    return 1;
}
