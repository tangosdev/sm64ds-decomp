//cpp
// @symbol _ZN11VolcanoRing6RenderEv
#include "VolcanoRing.h"

/* VolcanoRing::Render -- vtable slot 9, ov022 0x02111284.
 *
 * mModel is dBgActor_c's own field (include/dBgActor_c.h). Model::Render is
 * its own slot 5, `virtual void Render(const Vector3 *scale)`
 * (include/Model.h) -- called with a null scale, same as the pre-migration
 * recovery's `b->m(0)`. */
int VolcanoRing::Render()
{
    mModel.Render(0);
    return 1;
}
