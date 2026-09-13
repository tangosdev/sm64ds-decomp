#ifndef DACHROOM_C_H
#define DACHROOM_C_H

#include "dActor_c.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

/* An area-transition trigger box -- ov002/daChRoom_c.
 *
 * ROM RTTI at ov002 0x021085b8 names `10daChRoom_c`; its one base is
 * _ZTI8dActor_c. overlay_actors.md: VIRTUAL_DOOR(348). Ugly RTTI name is
 * final.
 *
 * SIZE 0xd4 from the factory's `mov r0, #0xd4` into fBase_c::operator new.
 * dActor_c ends at 0xd0; pad_0d0 is unused padding to that size, not a live
 * field. Every value this class uses is inherited actor state: InitResources
 * writes mScaleX/mScaleY/mAngleY, Behavior reads mPosX/mAngleY/mScaleX/
 * mScaleY/mAngleX/mAngleZ. mScaleX/mScaleY are the trigger box half-width
 * and height; mAngleX/mAngleZ are the two area ids.
 *
 * Vtable matches _ZTV8dActor_c except slots 0, 3, 6, 9, 12, 16, 17.
 *
 * daChRoom_c_classInit is reconstructed (RTTI daChRoom_c, CH_ROOM registry).
 * Retail does not store that spelling.
 */
struct daChRoom_c : dActor_c {
    u8 pad_0d0[0x4];

    /* Inline empty dtor: mwccarm emits D1 then D0, no D2. */
    virtual ~daChRoom_c() {}            /* slots 16 (D1), 17 (D0) */

    virtual s32 InitResources();        /* slot 0  -- 0x020b0938 */
    virtual s32 CleanupResources();     /* slot 3  -- 0x020b0854 */
    virtual s32 Behavior();             /* slot 6  -- 0x020b0868 */
    virtual s32 Render();               /* slot 9  -- 0x020b0860 */
    virtual void OnPendingDestroy();    /* slot 12 -- 0x020b085c */

    /* Leaf operator new until #2570 puts the same allocator on fBase_c.
       Parameter is size_t (unsigned long on this compiler). `return new`
       relocates to `_Znwm` without this. */
    static void *operator new(unsigned long size) {
        return _ZN7fBase_cnwEj((unsigned)size);
    }
};

typedef char daChRoom_c_size_must_be_0xd4[
    sizeof(daChRoom_c) == 0xd4 ? 1 : -1];

#endif /* DACHROOM_C_H */
