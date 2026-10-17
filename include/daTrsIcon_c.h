#ifndef DATRSICON_C_H
#define DATRSICON_C_H

#include "dActor_c.h"

/* ICON_TERESA, actor 0xd3 (g_profile_ICON_TERESA at 0x0211e7c4). The ROM RTTI
 * names this class daTrsIcon_c; BigBooIcon is its historical descriptive
 * spawn-table name. The factory and TU now use the daTrsIcon_c identity.
 * A Big Boo spawns it
 * (daTrs_c::mSpawnedActorID) and moves it along with itself; it tracks the
 * star the Big Boo releases.
 *
 * The factory allocates 0xd8 bytes and constructs dActor_c. Its 0xd0-byte base
 * leaves the two InitResources fields at 0xd4 and 0xd5, with alignment padding
 * on either side. The vtable and RTTI independently confirm the inheritance.
 *
 * SM64DS RTTI names the implementation daTrsIcon_c. The reconstructed
 * factory daTrsIcon_c_classInit (historical alias
 * BigBooIcon_Spawn) constructs it for the ICON_TERESA
 * registry profile.
 */
struct daTrsIcon_c : dActor_c {
    u8 pad_0d0[4];        /* 0x0d0 */
    /* dActor_c::TrackStar result. */
    s8 mTrackStarID;      /* 0x0d4 */
    /* (param1 >> 8) & 0xf. */
    u8 mStarID;           /* 0x0d5 */
    u8 pad_0d6[2];        /* 0x0d6 */

    /* --- vtable --- */
    virtual ~daTrsIcon_c();
    virtual int InitResources();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daTrsIcon_c_size_must_be_0xd8[
    sizeof(struct daTrsIcon_c) == 0xd8 ? 1 : -1];
#endif

extern "C" daTrsIcon_c *daTrsIcon_c_classInit();

#endif /* DATRSICON_C_H */
