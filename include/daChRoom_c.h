#ifndef DACHROOM_C_H
#define DACHROOM_C_H

#include "dActor_c.h"

/* An area-transition trigger box -- ov002/daChRoom_c.
 *
 * ROM RTTI at ov002 0x021085b8 names `10daChRoom_c`; its one base is
 * _ZTI8dActor_c. overlay_actors.md: VIRTUAL_DOOR(348). Ugly RTTI name is
 * final.
 *
 * SIZE 0xd4 from the factory's `mov r0, #0xd4` into fBase_c::operator new.
 * That is exactly sizeof(dActor_c): this class adds no field of its own.
 * Every value this class uses is inherited actor state: InitResources
 * writes mScale.x/mScale.y/mAngle.y, Behavior reads mPos.x/mAngle.y/mScale.x/
 * mScale.y/mAngle.x/mAngle.z. mScale.x/mScale.y are the trigger box half-width
 * and height; mAngle.x/mAngle.z are the two area ids.
 *
 * Vtable matches _ZTV8dActor_c except slots 0, 3, 6, 9, 12, 16, 17.
 *
 * daChRoom_c_classInit is reconstructed (RTTI daChRoom_c, CH_ROOM registry).
 * Retail does not store that spelling.
 */
struct daChRoom_c : dActor_c {

    /* Inline empty dtor: mwccarm emits D1 then D0, no D2. */
    virtual ~daChRoom_c() {} /* slots 16 (D1), 17 (D0) */

    virtual s32 InitResources();     /* slot 0  -- 0x020b0938 */
    virtual s32 CleanupResources();  /* slot 3  -- 0x020b0854 */
    virtual s32 Behavior();          /* slot 6  -- 0x020b0868 */
    virtual s32 Render();            /* slot 9  -- 0x020b0860 */
    virtual void OnPendingDestroy(); /* slot 12 -- 0x020b085c */
};

typedef char daChRoom_c_size_must_be_0xd4[sizeof(daChRoom_c) == 0xd4 ? 1 : -1];

#endif /* DACHROOM_C_H */
