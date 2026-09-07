/* BigMovingIceBlock, reconstructed from its factory, destructor pair, vtable
 * and four matched methods.
 *
 * BASE: dBgActor_c, direct. The cartridge's __si_class_type_info record at
 * ov056 0x02113320 points at _ZTI10dBgActor_c with subobject offset zero. Its
 * ROM name is `18daObjEwmIceBlock_c`, while the configured function symbols
 * use the readable compatibility spelling `BigMovingIceBlock`. A compiler
 * object may therefore emit `_ZTI17BigMovingIceBlock`; that metadata is a
 * discard-only passenger of the independently isolated functions, not a
 * claim that the readable spelling owns the cartridge RTTI.
 *
 * SIZE 0x330 is the literal passed by daObjEwmIceBlock_c_classInit to
 * fBase_c::operator new. dBgActor_c occupies 0x000..0x31f. The factory
 * constructs a PathPtr at 0x320, and the matched behavior reads the two
 * trailing words at 0x328 and 0x32c.
 *
 * SM64DS RTTI names the implementation daObjEwmIceBlock_c. The reconstructed factory
 * daObjEwmIceBlock_c_classInit (historical alias BigMovingIceBlock_Spawn)
 * installs this class's cartridge vtable; the reconstructed profile
 * global g_profile_EWM_ICE_BLOCK (historical alias BigMovingIceBlock_SpawnInfo)
 * is its registry descriptor.
 */
#ifndef BIGMOVINGICEBLOCK_H
#define BIGMOVINGICEBLOCK_H

#include "types.h"

#ifdef __cplusplus

#include "dBgActor_c.h"
#include "PathPtr.h"

struct BigMovingIceBlock : dBgActor_c {
    PathPtr mPath;          /* 0x320 */
    s32 mPathNodeIdx;       /* 0x328 */
    s32 mPathDir;           /* 0x32c */

    /* Inline is load-bearing: explicit use from the destructor sources makes
     * mwccarm emit the ROM's D1/D0 pair without a homeless D2. */
    virtual ~BigMovingIceBlock() {}

    /* Overrides of fBase_c's slots 0, 3, 6 and 9. */
    virtual s32 InitResources();
    virtual s32 CleanupResources();
    virtual s32 Behavior();
    virtual s32 Render();
};

#else

/* Flat compatibility view for C translation units. */
struct BigMovingIceBlock {
    u8 pad_000[0x320];
    u8 mPath[0x8];          /* 0x320 */
    s32 mPathNodeIdx;       /* 0x328 */
    s32 mPathDir;           /* 0x32c */
};

#endif

typedef char BigMovingIceBlock_size_must_be_0x330[
    sizeof(struct BigMovingIceBlock) == 0x330 ? 1 : -1];

#endif /* BIGMOVINGICEBLOCK_H */
