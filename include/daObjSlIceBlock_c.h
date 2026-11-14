#ifndef DAOBJSLICEBLOCK_C_H
#define DAOBJSLICEBLOCK_C_H

#include "types.h"
#include "dBgW_KcMbg.h"

/* Derives from dBgActor_c: the destructor stores this class's vtable, then
 * dBgActor_c's -- inlined -- then destroys the dBgW_KcMbg at 0x124 and
 * the dExtSimpleModel_c at 0xd4 before chaining to dActor_c. All three belong to dBgActor_c.
 * Everything this header used to restate below 0x31e was dActor_c's and
 * dBgActor_c's, and is inherited now.
 *
 * SIZE IS THE OBSERVED FIELD SPAN, rounded up. It guards this declaration; it
 * is not independent evidence about the ROM.
 *
 * SM64DS RTTI names the implementation daObjSlIceBlock_c. The reconstructed factory
 * daObjSlIceBlock_c_classInit_SL_ICEBLOCK_SHOT (historical alias SlidingIce_Spawn) installs this class's
 * cartridge vtable; the reconstructed profile global g_profile_SL_ICEBLOCK_SHOT
 * (historical alias SlidingIce_SpawnInfo) is its registry descriptor.
 */

#include "dBgActor_c.h"

struct daObjSlIceBlock_c : dBgActor_c {
    unsigned short mDelayTimer; /* 0x31e -- DecIfAbove0_Short; both variants of Behavior do nothing until it reaches 0 */
    unsigned char mNumToBigIce; /* 0x320 */
    u8 pad_321[0x3];
    s32 mMinPosY; /* 0x324 */
    s32 mSoundID; /* 0x328 */

    /* --- vtable --- */
    virtual ~daObjSlIceBlock_c();

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();

    virtual void OnHitByMegaChar(daPly_c &player); /* slot 27 */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daObjSlIceBlock_c_size_must_be_0x32c[sizeof(daObjSlIceBlock_c) == 0x32c ? 1 : -1];
#endif

#endif /* DAOBJSLICEBLOCK_C_H */
