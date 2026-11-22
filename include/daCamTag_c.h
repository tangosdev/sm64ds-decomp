#ifndef DACAMTAG_C_H
#define DACAMTAG_C_H

#include "dActor_c.h"

/* dCamera_c-tag position marker -- ov002/daCamTag_c.
 *
 * ROM RTTI at ov002 0x021084fc names `10daCamTag_c`; its one base is
 * _ZTI8dActor_c. overlay_actors.md: CAMERA_TAG(333). Ugly RTTI name is
 * final.
 *
 * SIZE 0xd4 from the factory's own allocation literal (212).
 * That is exactly sizeof(dActor_c): this class adds no field of its own
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

    /* Inline empty dtor: mwccarm emits D1 then D0, no D2. InitResources is
     * the first out-of-line virtual/key function, so this TU owns the
     * _ZTV/_ZTI/_ZTS group. */
    virtual ~daCamTag_c() {} /* slots 16 (D1), 17 (D0) */

    virtual s32 InitResources();     /* slot 0 */
    virtual s32 CleanupResources();  /* slot 3 */
    virtual s32 Behavior();          /* slot 6 */
    virtual s32 Render();            /* slot 9 */
    virtual void OnPendingDestroy(); /* slot 12 */
};

typedef char daCamTag_c_size_must_be_0xd4[sizeof(daCamTag_c) == 0xd4 ? 1 : -1];

#endif /* DACAMTAG_C_H */
