#ifndef DAOBJCTMECHA10_C_H
#define DAOBJCTMECHA10_C_H

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
 * NAME. The cartridge names this class itself. ov035's vtable storage begins at
 * 0x02112af8 with an offset-to-top of 0 and a typeinfo word of 0x02112a84; that
 * record is an __si_class_type_info whose name word points at 0x02112a90, where
 * the overlay reads the length-prefixed string "16daObjCtMecha10_c", and whose
 * base word points at _ZTI10dBgActor_c (ov002:0x021089ec). The tree spelled the
 * class RotatingCogSmall until that reading was applied; that name was coined
 * and is retired. Three registry profiles share the class -- g_profile_CT_MECHA10,
 * g_profile_CT_MECHA12L and g_profile_CT_MECHA12S -- and each of the three
 * reconstructed factories installs this same cartridge vtable. The factory and
 * profile spellings are reconstructed, not preserved original source names
 * (historical aliases RotatingCogSmall_Spawn and RotatingCogSmall_SpawnInfo).
 */

#include "dBgActor_c.h"

struct daObjCtMecha10_c : dBgActor_c {
    u16 mStepTimer; /* 0x31e -- DecIfAbove0_Short; reloaded from data_ov035_02111ef4[mRotationState][setting] */
    u16 mDirTimer; /* 0x320 -- second countdown; gates the direction re-roll under clock setting 2 */
    s16 mTargetAngleY; /* 0x322 -- ApproachLinear target for mAngleY; stepped by mAngleYStep */
    s16 mAngleYStep;   /* 0x324 -- +/- data_ov035_02111ef0[mRotationState] */
    s16 unk_326; /* 0x326 -- InitResources gives it the same table value as mAngleYStep; no matched reader */
    u8 pad_328[0x4];
    s32 mRotationState; /* 0x32c */

    /* --- vtable --- */
    /* Inline empty dtor: mwccarm emits D1 then D0 and no D2, which is the
       order the cartridge has -- D1 at 0x021111a0, D0 at 0x021111e4. Same
       form as include/daObjBk_Lift_c.h, the same dBgActor_c shape. */
    virtual ~daObjCtMecha10_c() {}

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daObjCtMecha10_c_size_must_be_0x330[sizeof(daObjCtMecha10_c) == 0x330 ? 1 : -1];
#endif

#endif /* DAOBJCTMECHA10_C_H */
