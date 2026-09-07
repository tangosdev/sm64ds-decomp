#ifndef DA1UP_C_H
#define DA1UP_C_H

/* THE CLASS NAME IS THE CARTRIDGE'S. ov002 holds the length-prefixed Itanium
 * type-name string `7da1up_c\0` at 0x02108370, and _ZTI7da1up_c at 0x0210837c
 * is the matching __si_class_type_info whose +8 word reaches _ZTI12dEnemyBase_c
 * at 0x021081c0. The project used to call this class OneUpMushroom; that word
 * is absent from every executable image in every encoding tested, so it was a
 * coined name and is gone.
 *
 * WHAT IS STILL RECONSTRUCTED, and it is only the spellings around the class:
 * the two factories da1up_c_classInit_ONEUPKINOKO / _SCALEUP_KINOKO and the two
 * registry descriptors g_profile_ONEUPKINOKO / _SCALEUP_KINOKO. ONEUPKINOKO and
 * SCALEUP_KINOKO themselves are NOT coined -- they are ROM-attested ASCII in
 * arm9's actor debug-name table at 0x020901e8 and 0x02090658, profile ids 276
 * and 277 -- but the `classInit` and `g_profile_` affixes are later EAD lineage,
 * not SM64DS symbols. Every FIELD NAME below is a project invention too: the
 * ROM proves each offset, width and member TYPE and none of the words.
 */

#include "types.h"

/* Derives from dEnemyBase_c, and TWO INDEPENDENT WITNESSES agree on the layout: the
 * class's own destructor `_ZN7da1up_cD1Ev` destroys each member, and
 * `da1up_c_classInit_ONEUPKINOKO` constructs the same types at the same offsets before
 * storing `_ZTV7da1up_c`. Everything this header used to restate below 0x110
 * belongs to dEnemyBase_c and dActor_c and is inherited now.
 *
 * The members close on each other, which is what makes the layout a reading
 * rather than a guess:
 *
 *     0x110 dCcAc_c         0x34    -> 0x144
 *     0x144 dBgCh_Actr               0x1bc   -> 0x300
 *     0x300 Model                      0x50    -> 0x350
 *     0x350 ShadowModel                0x28    -> 0x378
 *
 * SIZE IS THE ROM'S OWN: `da1up_c_classInit_ONEUPKINOKO` calls `fBase_c::operator new(920)`
 * -- 0x398 -- and stores this class's vtable, so that literal IS this
 * class's sizeof.
 */

#include "dEnemyBase_c.h"
#include "Model.h"
#include "dCcAc_c.h"
#include "ShadowModel.h"
#include "dBgCh_Actr.h"

struct da1up_c : dEnemyBase_c {
    dCcAc_c           mdCcAc_c;   /* 0x110 */
    dBgCh_Actr                 mWithMeshClsn;         /* 0x144 */
    Model                        mModel;                /* 0x300 */
    ShadowModel                  mShadowModel;          /* 0x350 */
    s32                          unk_378;               /* 0x378 */
    s32                          unk_37c;               /* 0x37c */
    s32                          unk_380;               /* 0x380 */
    s32                          mMushroomType;         /* 0x384 */
    s32                          unk_388;               /* 0x388 */
    u8  pad_38c[0x2];
    u8                           unk_38e;               /* 0x38e */
    u8                           unk_38f;               /* 0x38f */
    s32                          unk_390;               /* 0x390 */
    s32                          unk_394;               /* 0x394 */

    /* --- vtable ---
       Nine own overrides, and _ZTV7da1up_c at 0x021083c8 is what says which:
       its 31 slots relocate to this run at slot 0 (0x020b01c0), 3 (0x020affe8),
       6 (0x020b00e8), 9 (0x020b0070), 12 (0x020b006c), 16/17 (the destructor
       pair) and 18/19. Five of these were declared NON-virtual here until this
       change -- Behavior, CleanupResources, InitResources, OnPendingDestroy and
       Render -- which the cartridge's own table contradicts, and which silently
       let dActor_c's inherited addresses stand in five slots of the vtable this
       class's key-function TU now emits.

       The destructor stays OUT OF LINE and declared FIRST, so it is this
       class's key function: the ROM puts D1 at 0x020aee40 below D0 at
       0x020aee88 with no D2, and out-of-line plus `#pragma defer_codegen off`
       is the form that reproduces that order. */
    virtual ~da1up_c();

    virtual s32   InitResources();               /* slot  0 */
    virtual s32   CleanupResources();            /* slot  3 */
    virtual s32   Behavior();                    /* slot  6 */
    virtual s32   Render();                      /* slot  9 */
    virtual void  OnPendingDestroy();            /* slot 12 */
    virtual s32   OnYoshiTryEat();               /* slot 18 */
    virtual int   OnTurnIntoEgg(Player &player); /* slot 19 */
};

typedef char da1up_c_size_must_be_0x398[sizeof(da1up_c) == 0x398 ? 1 : -1];

#endif /* DA1UP_C_H */
