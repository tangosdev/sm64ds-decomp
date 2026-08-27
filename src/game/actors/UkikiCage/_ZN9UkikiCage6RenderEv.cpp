//cpp
// @symbol _ZN9UkikiCage6RenderEv
/* UkikiCage::Render -- vtable slot 9, ov030 0x02111350, a real UkikiCage::Render()
 * method: refresh the model and collision transforms (both inherited dBgActor_c
 * methods), then draw the Model sub-object at +0xd4 (dBgActor_c::mModel) through
 * Model::Render (vtable slot 5). The member call keeps its indirect vtable
 * dispatch -- mwccarm does not devirtualize the embedded object -- so no
 * shadow-vtable struct or extern-C shim is needed. */
#include "UkikiCage.h"

int UkikiCage::Render() {
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    mModel.Render(0);
    return 1;
}
