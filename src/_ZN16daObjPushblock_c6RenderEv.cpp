//cpp
// @symbol _ZN16daObjPushblock_c6RenderEv
/* daObjPushblock_c::Render -- vtable slot 9, ov002 0x020b8dac. Attributed by the
 * vtable: _ZTV16daObjPushblock_c carries this address at slot 9, where
 * _ZTV10dBgActor_c carries fBase_c's generic 0x02043af0.
 *
 * Draws the block's own Model sub-object at +0xd4 (dBgActor_c::mModel, inherited)
 * through Model::Render (vtable slot 5). mwccarm dispatches the embedded-member
 * call through the vtable rather than devirtualizing it, so the ROM's indirect
 * call is reproduced from the plain named form. */
#include "daObjPushblock_c.h"

int daObjPushblock_c::Render() {
    mModel.Render(0);
    return 1;
}
