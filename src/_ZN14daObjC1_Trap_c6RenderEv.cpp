//cpp
// @symbol _ZN14daObjC1_Trap_c6RenderEv
/* daObjC1_Trap_c::Render -- vtable slot 9, ov010 0x021115a8, a real daObjC1_Trap_c::Render() method.
 * A spawner daObjC1_Trap_c draws nothing; a spawned one draws its own Model at +0x320
 * (daObjC1_Trap_c.h names it mDoorModel) through Model::Render (vtable slot 5). mwccarm still
 * dispatches that member call through the vtable -- it does not devirtualize the
 * embedded object -- so the ROM's indirect call is reproduced from the plain,
 * named form; no shadow-vtable struct is needed. */
#include "daObjC1_Trap_c.h"

int daObjC1_Trap_c::Render() {
    if (mIsSpawner == 0) {
        mDoorModel.Render(0);
    }
    return 1;
}
