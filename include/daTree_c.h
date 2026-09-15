/* TREE (actor 286): the course trees. dActor_c base, proven by the ROM's
 * own _ZTI8daTree_c; SIZE 0x264 is the factory's allocation literal.
 *
 * The destructor is inline ON PURPOSE. Out of line, mwccarm emits D0
 * before D1 -- the reverse of the cartridge's order -- plus a third D2
 * body with no ROM home. Inline, the compiler emits the retail D1/D0
 * pair in ROM order and no D2.
 */
#ifndef DATREE_C_H
#define DATREE_C_H
#include "types.h"
#include "dActor_c.h"
#include "Model.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

struct daTree_c : dActor_c {
    u8 pad_0d0[0x4];
    /* One Model per spawn-param variant (bits 4-6, clamped to 4); the
     * factory constructs all five, InitResources loads only its own. */
    Model mModel[5]; /* 0x0d4 */

    virtual ~daTree_c() {}
    virtual s32 InitResources();     /* slot  0 -- key function */
    virtual s32 CleanupResources();  /* slot  3 */
    virtual s32 Behavior();          /* slot  6 */
    virtual s32 Render();            /* slot  9 */
    virtual void OnPendingDestroy(); /* slot 12 -- empty body in the ROM */

    /* Leaf adapter until fBase_c::operator new(unsigned long) lands (#2570).
       `return new daTree_c()` then routes through the retail allocator. */
    static void *operator new(unsigned long size)
    {
        return _ZN7fBase_cnwEj((unsigned)size);
    }
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daTree_c_size_must_be_0x264[sizeof(daTree_c) == 0x264 ? 1 : -1];
#endif

#endif
