#ifndef DAOBJCTMECHA10_C_H
#define DAOBJCTMECHA10_C_H

#include "types.h"
#include "dBgW_KcMbg.h"

/* Derives from dBgActor_c: the destructor stores this class's vtable, then
 * dBgActor_c's -- inlined -- then destroys the dBgW_KcMbg at 0x124 and
 * the Model at 0xd4 before chaining to dActor_c. All three belong to dBgActor_c.
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

#ifdef __cplusplus

#include "dBgActor_c.h"

struct daObjCtMecha10_c : dBgActor_c {
    s16 mStepTimer;                   /* 0x31e -- DecIfAbove0_Short; reloaded from data_ov035_02111ef4[mRotationState][setting] */
    s16 mDirTimer;                    /* 0x320 -- second countdown; gates the direction re-roll under clock setting 2 */
    s16 mTargetAngleY;                /* 0x322 -- ApproachLinear target for mAngleY; stepped by mAngleYStep */
    s16 mAngleYStep;                  /* 0x324 -- +/- data_ov035_02111ef0[mRotationState] */
    s16 unk_326;                      /* 0x326 -- InitResources gives it the same table value as mAngleYStep; no matched reader */
    u8  pad_328[0x4];
    s32 mRotationState;               /* 0x32c */

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

#else

/* The C spelling of the same object, flat. Kept because the D0 file is a C
   translation unit that reads these fields, and D0 is compiler-generated so it
   can never be migrated. Same arrangement as include/ShadowModel.h. */
struct daObjCtMecha10_c {
    u8  pad_000[0xc];
    u16 actorID;            /* 0x00c */
    u8  pad_00e[0x80];
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x44];
    /* Model member, named by _ZN5ModelD1Ev at +0xd4 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    Model mModel;            /* 0x0d4 */
    /* dBgW_KcMbg member. The cartridge's own ~daObjCtMecha10_c calls
       _ZN10dBgW_KcMbgD1Ev at +0x124 (D0/D1), a relocation the ROM build checks;
       recovered by tools/dtor_members.py. D1 and not D2, so it is this type and not an
       inlined base. */
    dBgW_KcMbg mMovingMeshCollider;            /* 0x124 */
    u8  pad_2ec[0x32];
    s16 mStepTimer;         /* 0x31e */
    s16 mDirTimer;          /* 0x320 */
    s16 mTargetAngleY;      /* 0x322 */
    s16 mAngleYStep;        /* 0x324 */
    s16 unk_326;            /* 0x326 */
    u8  pad_328[0x4];
    s32 mRotationState;            /* 0x32c */
};

#endif /* __cplusplus */

#endif /* DAOBJCTMECHA10_C_H */
