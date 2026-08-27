//cpp
// @symbol _ZN4Door6RenderEv
#include "Door.h"
// recovered name: Door::Render
/* recovered: renamed to Class_Method, vtable slot 9 */
/* Door::Render -- vtable slot 9, ov100 0x021454c8. Declared in include/Door.h
 * and defined here as a real Door::Render() method; the object is `this` and
 * every field is named.
 *
 * FOLDED ONTO include/Door.h. This file used to include the generated flat
 * placeholder include/daDoor_c.h and reach its own object through raw
 * `c + 0xNN` arithmetic; it now takes a Door* and names every field. Nothing
 * about the offsets changed -- the byte gate is unchanged either way -- but
 * three of them turn out to say something:
 *
 *   0x08c / 0x090   dActor_c::mAngleX / mAngleZ, which a Door does NOT use as
 *                   angles. Both are loaded `ldrsh` and then sign-extended
 *                   through byte width (`lsl #0x18` / `asr #0x18`) before
 *                   IsAreaShowing sees them, so what is stored is a signed
 *                   BYTE area id in a halfword slot -- one per side. A door
 *                   joins two areas and renders if either is on screen. Only
 *                   mAngleY (0x08e) is a real angle for this class; see
 *                   _ZN4Door13InitResourcesEv.c, which rotates the door's
 *                   position by it.
 *   0x0e8           mModel.data.transforms -- the door's own bone transform
 *                   array (ModelBase::data at +0x08, ModelComponents::
 *                   transforms at +0x0c, so 0xd4 + 0x14). daDoor_c.h called
 *                   this `unk_0e8` and it was reachable only as a raw offset.
 *
 * The two virtual calls are Model's, not shadow slots: include/Model.h names
 * vtable slot 4 `Virtual10(Matrix4x3&)` (offset 0x10) and slot 5
 * `Render(const Vector3*)` (offset 0x14), which is exactly what the ROM
 * dispatches -- `ldr r2,[r2,#0x10]` and `ldr r2,[r2,#0x14]`. include/Door.h
 * previously recorded these as "unidentified Model vtable slots"; they are
 * identified, so the local V1/V2 shadow structs this file carried are gone.
 *
 * So the body reads as what it is: draw the door, then hand the door's bone
 * transforms to the key model hanging off 0x138 and draw that too. */
extern "C" {
extern unsigned char IsAreaShowing(int idx);
}
int Door::Render() {
    if (IsAreaShowing((char)mAngleX) == 0) {
        if (IsAreaShowing((char)mAngleZ) == 0) goto done;
    }
    {
        /* &mModel is taken HERE and not at the top of the function: hoisting
           it costs a callee-saved register and a stack adjust the ROM does
           not have (`add r5,r4,#0xd4` before the area checks, plus `sub sp`).
           The ROM computes it lazily, so this does too. */
        ModelAnim *body = &mModel;
        Model *key;
        body->Render(0);
        key = mKeyModel;
        if (key == 0) goto done;
        key->Virtual10(*mModel.data.transforms);
        mKeyModel->Render(0);
    }
done:
    return 1;
}
