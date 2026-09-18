#ifndef DAOBJHMBSKT_C_H
#define DAOBJHMBSKT_C_H

#include "types.h"
#include "dBgActor_c.h"
#include "dBgCh_Actr.h"

struct dActor_c;

/* Tall Tall Mountain's ukiki cage (profile HM_BASKET).
 * ROM RTTI daObjHmBskt_c; direct base dBgActor_c.
 *
 * SIZE IS 0x4e0: daObjHmBskt_c_classInit (historical alias UkikiCage_Spawn)
 * passes the literal 1248 to fBase_c::operator new -- not merely the observed
 * field span, though here the two agree: dBgActor_c ends at 0x320 and this
 * class adds exactly one dBgCh_Actr (0x1bc) plus one trailing pointer,
 * landing on 0x4e0 with no padding.
 *
 * Everything below 0x320 belongs to dActor_c and dBgActor_c and is inherited,
 * which is why the destructor stores this class's vtable, then dBgActor_c's --
 * inlined, because dBgActor_c defines its destructor in its class body -- then
 * destroys the dBgW_KcMbg at 0x124 and the Model at 0xd4 before chaining to
 * dActor_c.
 *
 * The class translation unit is src/game/actors/d_a_obj_hm_bskt.cpp. It is
 * this class's key-function TU -- the destructor below is inline, so the first
 * non-inline virtual declared, InitResources, carries the vague-linkage class
 * data -- which is what makes mwccarm emit _ZTV13daObjHmBskt_c and the
 * matching _ZTI/_ZTS pair under the ROM's own name.
 */
struct daObjHmBskt_c : dBgActor_c {
    dBgCh_Actr mWithMeshClsn;       /* 0x320 */
    dActor_c *mStarActor;           /* 0x4dc -- Spawn(0xb2, ...) result */

    /* --- vtable --- */
    /* MEASURED -- INLINE ON PURPOSE. Out of line, mwccarm emits the
       synthesized D0 ahead of the written D1 and a homeless D2; the cartridge
       has D1 at 0x021111a0 and D0 at 0x021111ec and no D2. */
    virtual ~daObjHmBskt_c() {}

    virtual s32 InitResources();     /* slot 0 */
    virtual s32 CleanupResources();  /* slot 3 */
    virtual s32 Behavior();          /* slot 6 */
    virtual s32 Render();            /* slot 9 */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daObjHmBskt_c_size_must_be_0x4e0[
    sizeof(daObjHmBskt_c) == 0x4e0 ? 1 : -1];
#endif

#endif /* DAOBJHMBSKT_C_H */
