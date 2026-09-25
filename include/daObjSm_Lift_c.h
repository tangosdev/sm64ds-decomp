#ifndef DAOBJSM_LIFT_C_H
#define DAOBJSM_LIFT_C_H

#include "types.h"
#include "dBgActor_c.h"

/* daObjSm_Lift_c -- Cool, Cool Mountain's ski lift (SM_LIFT). It rides a path
 * and yaws when a mega mushroom hits it. Own fields start where dBgActor_c
 * ends.
 *
 * NAME: the cartridge spells this class daObjSm_Lift_c. The typeinfo word of
 * the vtable header, at ov018 0x021138c8 (address point - 4), is 0x02113888.
 * That __si_class_type_info record reads [0x0209a764
 * (_ZTVN3abi20__si_class_type_infoE), 0x02113894, 0x021089ec], and the string
 * at 0x02113894 is "14daObjSm_Lift_c". The tree previously called the class
 * SkiLift, a coined name present only on the vtable address.
 *
 * BASE: dBgActor_c, direct -- the record's third word is _ZTI10dBgActor_c at
 * ov002 0x021089ec. Two witnesses close on each other:
 *   daObjSm_Lift_c_classInit (historically SkiLift_Spawn): fBase_c::operator
 *     new(852 = 0x354), dBgActor_c::dBgActor_c(), stores _ZTV14daObjSm_Lift_c;
 *   ~daObjSm_Lift_c: stores its own vptr, then dBgActor_c's (inlined), destroys
 *     dBgActor_c's dBgW_KcMbg at 0x124 and Model at 0xd4, then ~dActor_c. This
 *     class adds no member with a destructor of its own.
 *
 * SIZE 0x354 is the factory's own literal, and the last field closes on it.
 *
 * THE VTABLE at ov018 0x021138cc has the same 32 slots as dBgActor_c. Only
 * slots 0, 3, 6, 9, 16, 17 and 27 differ, exactly the overrides declared here.
 */

struct daObjSm_Lift_c : dBgActor_c {
    dActor_c *unk_320;          /* 0x320 */
    s32 mBasePosX;              /* 0x324 */
    s32 mBasePosY;              /* 0x328 */
    s32 mBasePosZ;              /* 0x32c */
    u8 unk_330;                 /* 0x330 */
    u8 unk_331;                 /* 0x331 */
    u8 pad_332[2];              /* 0x332 */
    s32 mPathId;                /* 0x334 */
    s32 mNodeCount;             /* 0x338 */
    s32 mNodeIndex;             /* 0x33c */
    u8 pad_340[0xc];            /* 0x340 */
    s32 mNodeStep;              /* 0x34c */
    u32 unk_350;                /* 0x350 */

    /* OUT OF LINE, DECLARED FIRST: the key function. The TU defines it first
       under `#pragma defer_codegen off`, which emits D1 then D0 -- the
       cartridge's order -- then a D2 the cartridge has no home for. */
    virtual ~daObjSm_Lift_c();            /* slots 16 (D1), 17 (D0) */

    virtual s32   InitResources();         /* slot  0 */
    virtual s32   CleanupResources();      /* slot  3 */
    virtual s32   Behavior();              /* slot  6 */
    virtual s32   Render();                /* slot  9 */
    virtual void  OnHitByMegaChar(Player &player); /* slot 27 */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daObjSm_Lift_c_size_must_be_0x354[sizeof(daObjSm_Lift_c) == 0x354 ? 1 : -1];
#endif

#endif /* DAOBJSM_LIFT_C_H */
