//cpp
// @symbol _ZN4Trap6RenderEv
/* Trap::Render -- vtable slot 9, ov010 0x021115a8, a real Trap::Render() method.
 * A spawner Trap draws nothing; a spawned one draws its own Model at +0x320
 * (Trap.h names it mDoorModel) through Model::Render (vtable slot 5). mwccarm still
 * dispatches that member call through the vtable -- it does not devirtualize the
 * embedded object -- so the ROM's indirect call is reproduced from the plain,
 * named form; no shadow-vtable struct is needed. */
#include "Trap.h"

int Trap::Render() {
    if (mIsSpawner == 0) {
        mDoorModel.Render(0);
    }
    return 1;
}
