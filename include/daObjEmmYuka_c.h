#ifndef DAOBJEMMYUKA_C_H
#define DAOBJEMMYUKA_C_H

#include "types.h"

/* daObjEmmYuka_c -- Tiny-Huge Island's square path lift. Actor 175 EMM_YUKA
 * (ov052, SQUARE_PATH_LIFT). The low byte of param1 selects the path;
 * Behavior walks mNodeIndex along it, reversing at an open end.
 *
 * NAME: the cartridge spells this class daObjEmmYuka_c. The typeinfo word of
 * the vtable header, at ov052 0x021125ec (address point - 4), is 0x021125ac.
 * That __si_class_type_info record reads [0x0209a764
 * (_ZTVN3abi20__si_class_type_infoE), 0x021125b8, 0x021089ec], and the string
 * at 0x021125b8 is "14daObjEmmYuka_c". The tree previously called the class
 * SquarePathLift, a coined name present only on the vtable address.
 *
 * BASE: dBgActor_c, direct -- the record's third word is _ZTI10dBgActor_c at
 * ov002 0x021089ec. The destructor stores this class's vtable, then
 * dBgActor_c's -- inlined -- then destroys the dBgW_KcMbg at 0x124 and the
 * Model at 0xd4 before chaining to dActor_c. Those members belong to
 * dBgActor_c and are inherited.
 *
 * MEMBERS: the PathPtr at 0x320 is named by the factory calling
 * _ZN7PathPtrC1Ev at +0x320; InitResources hands it param1 & 0xff through
 * PathPtr::FromID.
 *
 * SIZE 0x330 is the literal daObjEmmYuka_c_classInit (historical alias
 * SquarePathLift_Spawn) passes to fBase_c::operator new. The reconstructed
 * profile global g_profile_EMM_YUKA (historical alias
 * SquarePathLift_SpawnInfo) is its registry descriptor.
 *
 * THE VTABLE at ov052 0x021125f0 has the same 32 slots as dBgActor_c. Only
 * slots 0, 3, 6, 9, 16 and 17 differ, exactly the overrides declared here.
 */

#ifdef __cplusplus

#include "dBgActor_c.h"
#include "PathPtr.h"

struct daObjEmmYuka_c : dBgActor_c {
    u8 pad_31e[2];
    PathPtr mPath;       /* 0x320 */
    s32 mNodeIndex;      /* 0x328 */
    s32 mPathDir;        /* 0x32c -- +1 or -1 */

    /* OUT OF LINE, DECLARED FIRST: the key function. The TU defines it first
       under `#pragma defer_codegen off`, which emits D1 then D0 -- the
       cartridge's order -- then a D2 the cartridge has no home for. */
    virtual ~daObjEmmYuka_c();          /* slots 16, 17 */

    virtual int InitResources();        /* slot  0 */
    virtual int CleanupResources();     /* slot  3 */
    virtual int Behavior();             /* slot  6 */
    virtual int Render();               /* slot  9 */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daObjEmmYuka_c_size_must_be_0x330[sizeof(daObjEmmYuka_c) == 0x330 ? 1 : -1];
#endif

#endif /* __cplusplus */

#endif /* DAOBJEMMYUKA_C_H */
