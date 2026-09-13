#ifndef DAMCFLAG_C_H
#define DAMCFLAG_C_H
#include "types.h"
#include "dActor_c.h"
#include "ModelAnim.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

/* FLAG (342) -- ov009/daMcFlag_c, castle grounds flag.
 *
 * ov009 is mixed (DOCK_POLE / CASTLE_WATER / METAL_NET / FLAG / BIRD).
 * RTTI ov009:0x02113b60 names daMcFlag_c; the debug table names MC_FLAG.
 * This is the flag, not metalnet and not castle water.
 *
 * SIZE 0x138, the literal daMcFlag_c_classInit passes to operator new.
 * Factory constructs ModelAnim at 0xd4; D1 tears it down before ~dActor_c.
 * dActor_c ends at 0xd0; pad_0d0 is the 4-byte gap before mModelAnim.
 * 0xd4 + sizeof(ModelAnim) 0x64 = 0x138.
 *
 * The 31-slot ROM vtable differs from dActor_c only in slots 0, 3, 6, 9, 16
 * and 17. All other actor virtuals are inherited and must not be redeclared.
 */
struct daMcFlag_c : dActor_c {
    u8        pad_0d0[0x4];
    ModelAnim mModelAnim;       /* 0x0d4 */

    /* INLINE IS LOAD-BEARING. Out of line, mwccarm emits D0 before D1
       (cartridge is 0x02112078 D1 then 0x021120a8 D0) plus a D2 with no
       ROM home. Empty body: mModelAnim teardown, the vptr store and
       dActor_c's teardown are synthesised. Key function is InitResources,
       the first declared non-inline virtual. */
    virtual ~daMcFlag_c() {}                       /* slots 16 (D1), 17 (D0) */

    virtual s32 InitResources();             /* slot  0 */
    virtual s32 CleanupResources();          /* slot  3 */
    virtual s32 Behavior();                  /* slot  6 */
    virtual s32 Render();                    /* slot  9 */

    /* Until #2570 merges, a leaf `unsigned long` new forwards the retail
       `fBase_c::operator new(unsigned int)`. `unsigned int` here mangles
       differently and the factory misses. */
    static void *operator new(unsigned long size) {
        return _ZN7fBase_cnwEj((unsigned)size);
    }
};

typedef char daMcFlag_c_size_must_be_0x138[sizeof(daMcFlag_c) == 0x138 ? 1 : -1];

#endif
