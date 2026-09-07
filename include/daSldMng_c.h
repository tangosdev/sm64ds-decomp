#ifndef DASLDMNG_C_H
#define DASLDMNG_C_H

#include "dActor_c.h"

/* THE CLASS NAME IS THE CARTRIDGE'S OWN. The decomp used to call this class
 * `IceSlideManager`, a coined name. MEASURED in
 * extracted/overlays/overlay_0019.bin (ov019 base 0x021111a0, the `.text
 * start:` on line 1 of config/arm9/overlays/ov019/delinks.txt): the vtable
 * object's preamble at 0x021133c4 is [offset-to-top 0, 0x0211338c], and
 * _ZTI10daSldMng_c at 0x0211338c reads [0x0209a764, 0x02113398, 0x0208e390] --
 * _ZTVN3abi20__si_class_type_infoE (config/arm9/symbols.txt), a typeinfo name
 * at 0x02113398 whose bytes are the string "10daSldMng_c", and _ZTI8dActor_c.
 *
 * So the __si_class_type_info record points directly at dActor_c at offset
 * zero, and the vtable at 0x021133cc has exactly the same 31 slots as that
 * base. Only InitResources (slot 0), Behavior (slot 6), and the D1/D0
 * destructor pair (slots 16/17) are overridden.
 *
 * The factory's literal allocation size pins the class at 0xd8. dActor_c ends
 * at 0xd0; the intervening word has no observed meaning and remains unnamed.
 * InitResources seeds mKillTimer with 120, and Behavior counts it down before
 * killing the actor. Behavior's two-way switch and byte increment pin mState.
 *
 * SM64DS RTTI names the implementation daSldMng_c. The reconstructed factory
 * daSldMng_c_classInit (historical alias daSldMng_c_Spawn) installs this class's
 * cartridge vtable for the SLIDER_MANAGER registry profile.
 */
struct daSldMng_c : dActor_c {
    u32 unk_0d0;          /* 0x0d0 */
    u16 mKillTimer;       /* 0x0d4 */
    u8 mState;            /* 0x0d6 */
    u8 pad_0d7;           /* 0x0d7 */

    /* Inline is load-bearing. Written out-of-line in the TU, mwccarm emits D0
     * BEFORE D1 -- the reverse of the cartridge's order, which objisolate
     * refuses for the whole translation unit -- plus a third D2 with no ROM
     * home. Defined here it emits the retail D1/D0 pair in ROM order and no D2.
     * The two forcing scaffolds this replaces (daSldMng_c_EmitDestructor and
     * daSldMng_c_EmitDeletingDestructor, in the deleted per-function sources)
     * are no longer needed: src/actors/d_a_sld_mng.cpp defines InitResources,
     * so it is the key-function TU and materializes the pair on its own. */
    virtual ~daSldMng_c() {}

    /* THE KEY FUNCTION IS InitResources -- the first DECLARED non-inline
     * virtual, not the first slot. Whichever TU defines it emits this class's
     * _ZTV/_ZTI/_ZTS group; that is src/actors/d_a_sld_mng.cpp, which licenses
     * all three as deadstrip-data so romdata_check word-compares them against
     * the cartridge. */
    virtual int InitResources(); /* slot 0 */
    virtual int Behavior();      /* slot 6 */
};

typedef char daSldMng_c_size_must_be_0xd8[
    sizeof(daSldMng_c) == 0xd8 ? 1 : -1];

#endif
