#ifndef DAOBJC0_SWITCH_C_H
#define DAOBJC0_SWITCH_C_H

#include "types.h"
#include "dBgActor_c.h"

/* TWO WITNESSES, and they close on each other:
 *
 *   daObjC0_Switch_c_classInit  fBase_c::operator new(800 = 0x320), dBgActor_c::dBgActor_c(), stores _ZTV16daObjC0_Switch_c,
 *                 then the members below in this order.
 *   ~daObjC0_Switch_c   the same members destroyed in reverse, then ~dBgActor_c.
 *
 * SIZE 0x320 is the factory's own literal, and the last member closes exactly on it.
 *
 * THE VTABLE was diffed slot by slot against _ZTV10dBgActor_c. Only the slots declared
 * below differ; every other slot holds the base's own word and is inherited, so it
 * is deliberately not redeclared here.
 *
 * mPressed sits at 0x31e, in dBgActor_c's TAIL PADDING (same placement rationale as
 * daObjRc_Guruguru_c's mAngVelY): InitResources and OnGroundPounded both guard on
 * and set this byte -- OnGroundPounded no-ops if already set, then walks every other
 * actorID-0x22 instance and, on finding one already pressed, sets a shared flag in
 * data_0209caa0[2]. It is this switch pillar's own "pressed" state.
 *
 * SM64DS RTTI names the implementation daObjC0_Switch_c. The reconstructed factory
 * daObjC0_Switch_c_classInit (historical alias daObjC0_Switch_c_Spawn) installs this class's
 * cartridge vtable for the C0_SWITCH registry profile.
 */
struct daObjC0_Switch_c : dBgActor_c {
    u8  mPressed;            /* 0x31e */

    /* Defined inline on purpose. InitResources is the first out-of-line
     * virtual/key function, so together with this inline destructor mwcc owns the
     * retail D1/D0 pair in the cartridge's own order and the complete class
     * RTTI/vtable group, without retaining a D2 base-object body that has no ROM
     * home. Written out-of-line, mwcc emits D0 before D1 and objisolate refuses
     * the translation unit. The body is empty in the ROM too: D1 at 0x021111a0
     * stores the vtable and tail-calls ~dBgActor_c, with no member teardown. */
    virtual ~daObjC0_Switch_c() {}          /* slots 16 (D1), 17 (D0) */

    virtual s32   InitResources();         /* slot  0 */
    virtual s32   CleanupResources();      /* slot  3 */
    virtual s32   Behavior();              /* slot  6 */
    virtual s32   Render();                /* slot  9 */
    virtual void  OnGroundPounded(dActor_c &other); /* slot 21 */
};

typedef char daObjC0_Switch_c_size_must_be_0x320[sizeof(daObjC0_Switch_c) == 0x320 ? 1 : -1];

#endif /* DAOBJC0_SWITCH_C_H */
