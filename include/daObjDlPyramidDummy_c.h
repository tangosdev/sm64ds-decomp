#ifndef DAOBJDLPYRAMIDDUMMY_C_H
#define DAOBJDLPYRAMIDDUMMY_C_H

#include "dActor_c.h"
#include "dCcAc_c.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

/* daObjDlPyramidDummy_c is the cartridge's RTTI name for this class
 * (_ZTS21daObjDlPyramidDummy_c; the project called it PyramidTag before the
 * rename). The ROM records a single dActor_c base at offset zero; its vtable is
 * at 0x02113844.
 *
 * The factory allocates 0x10c bytes, constructs dActor_c, then constructs the
 * dCcAc_c at 0x0d4. Both destructor variants destroy that member before
 * chaining to dActor_c, independently proving its ownership and the extent.
 * The remaining word is the unique ID of the daObjDlPyramid_c (the pyramid top) this tag reports to.
 *
 * The 31-slot ROM vtable has the same extent as dActor_c and overrides only
 * slots 0, 6, 16, and 17. The original code TU is shared with daObjDlPyramid_c and
 * four daObjDlPyramid_c-only helpers: src/game/actors/d_a_obj_dl_pyramid.cpp. */
struct daObjDlPyramidDummy_c : dActor_c {
    u8       pad_0d0[0x4];
    dCcAc_c  mCylinder;       /* 0x0d4 -- the tag's collision cylinder */
    u32      mPyramidTopID;   /* 0x0108 -- daObjDlPyramid_c unique ID */

    virtual ~daObjDlPyramidDummy_c();
    virtual s32 InitResources();  /* slot 0 */
    virtual s32 Behavior();       /* slot 6 */

    /* Leaf adapter until fBase_c::operator new(size_t) lands.
       `return new daObjDlPyramidDummy_c` then routes through the retail
       allocator, which is what the cartridge's factory at 0x0211183c calls.
       Without this the spelling resolves to the global _Znwm, which the ROM
       does not carry. */
    static void *operator new(size_t size) {
        return _ZN7fBase_cnwEj((unsigned)size);
    }
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daObjDlPyramidDummy_c_size_must_be_0x10c[
    sizeof(daObjDlPyramidDummy_c) == 0x10c ? 1 : -1];
#endif

#endif
