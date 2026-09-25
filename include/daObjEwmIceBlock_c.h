/* daObjEwmIceBlock_c: the big ice block that moves along a path (registry
 * profile EWM_ICE_BLOCK). Reconstructed from its factory, destructor pair,
 * vtable and four matched methods.
 *
 * BASE: dBgActor_c, direct. The cartridge's __si_class_type_info record at
 * ov056 0x02113320 points at _ZTS18daObjEwmIceBlock_c (0x0211332c) and at
 * _ZTI10dBgActor_c with subobject offset zero. The vtable at 0x02113368
 * carries that _ZTI at its address point minus 4 (ov056 relocs.txt
 * `from:0x02113364 to:0x02113320`), so the three records are one triple. The
 * tree previously called the class BigMovingIceBlock (coined).
 *
 * SIZE 0x330 is the literal passed by daObjEwmIceBlock_c_classInit to
 * fBase_c::operator new. dBgActor_c occupies 0x000..0x31f. The factory
 * constructs a PathPtr at 0x320, and the matched behavior reads the two
 * trailing words at 0x328 and 0x32c.
 *
 * The reconstructed factory daObjEwmIceBlock_c_classInit (historical alias
 * BigMovingIceBlock_Spawn) installs this class's cartridge vtable; the
 * reconstructed profile global g_profile_EWM_ICE_BLOCK (historical alias
 * BigMovingIceBlock_SpawnInfo) is its registry descriptor.
 */
#ifndef DAOBJEWMICEBLOCK_C_H
#define DAOBJEWMICEBLOCK_C_H

#include "types.h"

#ifdef __cplusplus

#include "dBgActor_c.h"
#include "PathPtr.h"

struct daObjEwmIceBlock_c : dBgActor_c {
    PathPtr mPath;          /* 0x320 */
    s32 mPathNodeIdx;       /* 0x328 */
    s32 mPathDir;           /* 0x32c */

    /* Out of line in daObjEwmIceBlock_c.cpp: the key function, so that TU
     * emits _ZTV/_ZTI/_ZTS18daObjEwmIceBlock_c. */
    virtual ~daObjEwmIceBlock_c();

    /* Overrides of fBase_c's slots 0, 3, 6 and 9. */
    virtual s32 InitResources();
    virtual s32 CleanupResources();
    virtual s32 Behavior();
    virtual s32 Render();
};

#ifndef SM64DS_PLATFORM_PC
typedef char daObjEwmIceBlock_c_size_must_be_0x330[
    sizeof(daObjEwmIceBlock_c) == 0x330 ? 1 : -1];
#endif

#endif /* __cplusplus */

#endif /* DAOBJEWMICEBLOCK_C_H */
