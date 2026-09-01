//cpp
// @symbol _ZN16daObjC0_Switch_c15OnGroundPoundedER8dActor_c
#include "daObjC0_Switch_c.h"

/* daObjC0_Switch_c::OnGroundPounded -- vtable slot 21, ov012 0x0211123c.
 *
 * mPosY and mPressed are the same fields InitResources touches.
 * dActor_c::FindWithActorID walks every actor of a given actorID (0x22, this
 * class's own -- see include/fBase_c.h actorID). This one no-ops if already
 * pressed; otherwise it sinks, marks itself pressed, then walks every OTHER
 * actorID-0x22 pillar looking for one already pressed, and if it finds one,
 * sets the shared group flag in data_0209caa0[2] that InitResources reads. */
extern "C" {
extern int data_0209caa0[];
}

void daObjC0_Switch_c::OnGroundPounded(dActor_c &other)
{
    if (mPressed) return;
    mPosY -= 0x64000;
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    mPressed = 1;
    dActor_c *p = dActor_c::FindWithActorID(0x22, 0);
    if (p == 0) return;
check:
    if (p != (dActor_c *)this) {
        if (((daObjC0_Switch_c *)p)->mPressed) {
            data_0209caa0[2] |= 0x80000;
        }
        return;
    }
    p = dActor_c::FindWithActorID(0x22, p);
    if (p != 0) goto check;
}
