//cpp
// @symbol _ZN14daObjFl_Ring_c6RenderEv
#include "daObjFl_Ring_c.h"

/* daObjFl_Ring_c::Render -- vtable slot 9, ov022 0x02111284.
 *
 * mModel is dBgActor_c's own field (include/dBgActor_c.h). Model::Render is
 * its own slot 5, `virtual void Render(const Vector3 *scale)`
 * (include/Model.h) -- called with a null scale, same as the pre-migration
 * recovery's `b->m(0)`. */
int daObjFl_Ring_c::Render()
{
    mModel.Render(0);
    return 1;
}
