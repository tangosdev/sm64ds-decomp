#ifndef DACAMTAG_C_H
#define DACAMTAG_C_H

#include "dActor_c.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

/* Camera-tag position marker -- ov002/daCamTag_c.
 *
 * ROM RTTI at ov002 0x021084fc names `10daCamTag_c`; its one base is
 * _ZTI8dActor_c. overlay_actors.md: CAMERA_TAG(333). Ugly RTTI name is
 * final.
 *
 * SIZE 0xd4 from the factory's own allocation literal (212).
 * dActor_c ends at 0xd0; pad_0d0 is unused padding to that size, not a live
 * field -- no enrolled body touches those bytes
 * (notes/actor-leaf-provenance.md), and all five overrides below are stubs
 * that never read `this`. daBar_c next door is a different class (0x108,
 * own vtable, own dCcAc_c); do not re-merge them.
 *
 * Vtable at ov002 0x0210853c matches _ZTV8dActor_c except slots 0, 3, 6,
 * 9, 12, 16, 17.
 *
 * daCamTag_c_classInit is reconstructed (RTTI daCamTag_c, CAMERA_TAG
 * registry). Retail does not store that spelling. Historical alias
 * daCamTag_c_Spawn.
 */
struct daCamTag_c : dActor_c {
    u8 pad_0d0[0x4];

    /* Inline empty dtor: mwccarm emits D1 then D0, no D2. InitResources is
     * the first out-of-line virtual/key function, so this TU owns the
     * _ZTV/_ZTI/_ZTS group. */
    virtual ~daCamTag_c() {}            /* slots 16 (D1), 17 (D0) */

    virtual s32 InitResources();        /* slot 0 */
    virtual s32 CleanupResources();     /* slot 3 */
    virtual s32 Behavior();             /* slot 6 */
    virtual s32 Render();               /* slot 9 */
    virtual void OnPendingDestroy();    /* slot 12 */

    /* Leaf operator new until #2570 puts the same allocator on fBase_c.
       Parameter is size_t (unsigned long on this compiler). `return new`
       relocates to `_Znwm` without this. */
    static void *operator new(unsigned long size) {
        return _ZN7fBase_cnwEj((unsigned)size);
    }
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daCamTag_c_size_must_be_0xd4[
    sizeof(daCamTag_c) == 0xd4 ? 1 : -1];
#endif

#endif /* DACAMTAG_C_H */
