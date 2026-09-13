#ifndef DASLDMNG_C_H
#define DASLDMNG_C_H

#include "dActor_c.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

/* Ice slide manager. ROM debug string is SLIDER_MANAGER (356);
 * overlay_actors.md's project name is ICE_SLIDE_MANAGER. The sibling
 * RACING_PENGUIN (259) is a different class and is not this TU.
 *
 * THE CLASS NAME IS THE CARTRIDGE'S OWN. Measured in
 * extracted/overlays/overlay_0019.bin (ov019 base 0x021111a0): the vtable
 * object's preamble at 0x021133c4 is [offset-to-top 0, 0x0211338c], and
 * _ZTI10daSldMng_c at 0x0211338c reads [0x0209a764, 0x02113398, 0x0208e390]
 * -- _ZTVN3abi20__si_class_type_infoE, a typeinfo name at 0x02113398 whose
 * bytes are the string "10daSldMng_c", and _ZTI8dActor_c. Direct base is
 * dActor_c at offset zero. The vtable at 0x021133cc has the same 31 slots as
 * that base; only InitResources (slot 0), Behavior (slot 6), and the D1/D0
 * pair (slots 16/17) are overridden.
 *
 * SIZE 0xd8 is daSldMng_c_classInit's own allocation literal. dActor_c ends
 * at 0xd0; the intervening word has no observed meaning. InitResources seeds
 * mKillTimer with 120, and Behavior counts it down before killing the actor.
 * Behavior's two-way switch and byte increment pin mState.
 *
 * SM64DS RTTI names the implementation daSldMng_c. The reconstructed factory
 * daSldMng_c_classInit (historical alias daSldMng_c_Spawn) installs this
 * class's cartridge vtable for the SLIDER_MANAGER registry profile.
 */
struct daSldMng_c : dActor_c {
    u32 unk_0d0;          /* 0x0d0 */
    u16 mKillTimer;       /* 0x0d4 */
    u8 mState;            /* 0x0d6 */
    u8 pad_0d7;           /* 0x0d7 -- never dereferenced */

    /* THE KEY FUNCTION IS InitResources -- the first DECLARED non-inline
     * virtual, not the first slot. Whichever TU defines it emits this class's
     * _ZTV/_ZTI/_ZTS group; that is src/game/actors/d_a_sld_mng.cpp. */
    virtual int InitResources(); /* slot 0 */
    virtual int Behavior();      /* slot 6 */

    static void *operator new(unsigned long size) {
        return _ZN7fBase_cnwEj((unsigned)size);
    }

    /* Declared last and inline so class instantiation emits the retail D1/D0
       pair in that order, with no separate D2 body. */
    virtual ~daSldMng_c() {}
};

typedef char daSldMng_c_size_must_be_0xd8[
    sizeof(daSldMng_c) == 0xd8 ? 1 : -1];

#endif
