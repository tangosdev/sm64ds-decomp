#ifndef ICESLIDEMANAGER_H
#define ICESLIDEMANAGER_H

#include "dActor_c.h"

/* The cartridge RTTI names this class daSldMng_c. IceSlideManager is the
 * readable compatibility spelling already carried by all four configured
 * virtual symbols. Its __si_class_type_info record points directly at
 * dActor_c at offset zero, and its vtable has exactly the same 31 slots as
 * that base. Only InitResources (slot 0), Behavior (slot 6), and the D1/D0
 * destructor pair (slots 16/17) are overridden.
 *
 * The factory's literal allocation size pins the class at 0xd8. dActor_c ends
 * at 0xd0; the intervening word has no observed meaning and remains unnamed.
 * InitResources seeds mKillTimer with 120, and Behavior counts it down before
 * killing the actor. Behavior's two-way switch and byte increment pin mState.
 */
struct IceSlideManager : dActor_c {
    u32 unk_0d0;          /* 0x0d0 */
    u16 mKillTimer;       /* 0x0d4 */
    u8 mState;            /* 0x0d6 */
    u8 pad_0d7;           /* 0x0d7 */

    /* Inline is load-bearing: explicit use from the destructor sources makes
     * mwccarm emit D1/D0 in cartridge order without a homeless D2. */
    virtual ~IceSlideManager() {}

    virtual int InitResources(); /* slot 0 */
    virtual int Behavior();      /* slot 6 */
};

typedef char IceSlideManager_size_must_be_0xd8[
    sizeof(IceSlideManager) == 0xd8 ? 1 : -1];

#endif
