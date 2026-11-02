#ifndef DAOBJCTMECHA08_C_H
#define DAOBJCTMECHA08_C_H

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
 * NAME. The cartridge names this class itself. ov065's vtable storage begins at
 * 0x0211d48c with an offset-to-top of 0 and a typeinfo word of 0x0211d434; that
 * record is an __si_class_type_info whose name word points at 0x0211d440, where
 * the overlay reads the length-prefixed string "16daObjCtMecha08_c", and whose
 * base word points at _ZTI10dBgActor_c (ov002:0x021089ec). The tree spelled the
 * class TtcRotatingGear until that reading was applied; that name was coined
 * and is retired. Two registry profiles share the class -- g_profile_CT_MECHA08A
 * and g_profile_CT_MECHA08B -- and both reconstructed factories install this
 * same cartridge vtable. The factory and profile spellings are reconstructed,
 * not preserved original source names (historical aliases TtcMovingCubeA_Spawn
 * and TtcMovingCubeB_Spawn). The unrelated alias TtcRotatingGear_Spawn names
 * daObjCtKaitendai_c's CT_MECHA06 factory, not this class.
 */

#include "dBgActor_c.h"

struct daObjCtMecha08_c : dBgActor_c {
    u8  pad_31e[0x2];
    s32 mHomePosX;                    /* 0x320 -- InitResources copies mPosX/Y/Z here */
    s32 mHomePosY;                    /* 0x324 -- Behavior clamps mPosY to [mHomePosY, mHomePosY + 0x14a000] */
    s32 mHomePosZ;                    /* 0x328 */
    u16 mMoveTimer;                   /* 0x32c -- DecIfAbove0_Short'ed; reloaded from the per-setting table on expiry */
    u8 mMoveDir;                      /* 0x32e */

    /* --- vtable --- */
    /* Inline empty dtor: mwccarm emits D1 then D0 and no D2, which is the
       order the cartridge has -- D1 at 0x0211b7f0, D0 at 0x0211b834. Same
       form as include/daObjCtMecha10_c.h, the same dBgActor_c shape. */
    virtual ~daObjCtMecha08_c() {}

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daObjCtMecha08_c_size_must_be_0x330[sizeof(daObjCtMecha08_c) == 0x330 ? 1 : -1];
#endif

#endif /* DAOBJCTMECHA08_C_H */
