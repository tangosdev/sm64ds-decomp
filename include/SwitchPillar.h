#ifndef SWITCHPILLAR_H
#define SWITCHPILLAR_H

#include "types.h"
#include "dBgActor_c.h"

/* TWO WITNESSES, and they close on each other:
 *
 *   SwitchPillar_Spawn  fBase_c::operator new(800 = 0x320), dBgActor_c::dBgActor_c(), stores _ZTV12SwitchPillar,
 *                 then the members below in this order.
 *   ~SwitchPillar   the same members destroyed in reverse, then ~dBgActor_c.
 *
 * SIZE 0x320 is the factory's own literal, and the last member closes exactly on it.
 *
 * THE VTABLE was diffed slot by slot against _ZTV10dBgActor_c. Only the slots declared
 * below differ; every other slot holds the base's own word and is inherited, so it
 * is deliberately not redeclared here.
 *
 * mPressed sits at 0x31e, in dBgActor_c's TAIL PADDING (same placement rationale as
 * ArmedRotatingPlatform's mAngVelY): InitResources and OnGroundPounded both guard on
 * and set this byte -- OnGroundPounded no-ops if already set, then walks every other
 * actorID-0x22 instance and, on finding one already pressed, sets a shared flag in
 * data_0209caa0[2]. It is this switch pillar's own "pressed" state.
 */
struct SwitchPillar : dBgActor_c {
    u8  mPressed;            /* 0x31e */

    virtual ~SwitchPillar() {}            /* slots 16 (D1), 17 (D0) */

    virtual s32   InitResources();         /* slot  0 */
    virtual s32   CleanupResources();      /* slot  3 */
    virtual s32   Behavior();              /* slot  6 */
    virtual s32   Render();                /* slot  9 */
    virtual void  OnGroundPounded(dActor_c &other); /* slot 21 */
};

typedef char SwitchPillar_size_must_be_0x320[sizeof(SwitchPillar) == 0x320 ? 1 : -1];

#endif /* SWITCHPILLAR_H */
