/* Base class for the single-camera 3D minigames. Its 13 direct RTTI
 * children are Cup, Memory, Memory2, Slot3, Sound, Snowball, Card, MCarlo,
 * MCarlo2, Roulette, BSC, 3DEsp and Flower. Adds one MgCamera at 0x4660 and
 * a dPa_c at 0x471c. See notes/minigame-provenance.md for
 * the field evidence. */
#ifndef DSCMGSINGLE3DBASE_C_H
#define DSCMGSINGLE3DBASE_C_H
#include "dScMgBase_c.h"

#include "MgCamera.h"
#include "dPa_c.h"

namespace Particle {
/* Called by dScMgSingle3DBase_c::BeforeRender. */
void RenderAll();
}

struct dScMgSingle3DBase_c : dScMgBase_c {
    /* Overrides the destructor slots 16 (D1) and 17 (D0). Must stay inline:
       all 13 children inline this teardown, and the ROM has no
       _ZN19dScMgSingle3DBase_cD2Ev, so an out-of-line body leaves each child
       an undefined external (measured on dScMgMemory_c). Being inline, it is
       not the key function; AfterInitResources is, which places the vtable
       and typeinfo in src/minigames/d_s_mg_single3d_base.cpp. */
    virtual ~dScMgSingle3DBase_c() {}

    /* Overrides of dScMgBase_c virtuals, in vtable order; dScMgBase_c has
       its own bodies at all six slots, 26 and 33 included. This class adds
       no virtual, so its vtable is dScMgBase_c's 36 slots, and the compiler
       emits all 36, matching the ROM's table at ov006:0x0213e448. */
    virtual void AfterInitResources(u32 vfSuccess); /* slot  2 */
    virtual void AfterCleanupResources(u32 vfSuccess); /* slot  5 */
    virtual int  BeforeBehavior();                  /* slot  7 */
    virtual int  BeforeRender();                    /* slot 10 */
    virtual int  GetDisplayMode();      /* slot 26 */
    virtual void InitEngines();                          /* slot 33 */

    /* DO NOT restyle the `offset 0xNN` comment below into this
     * tree's usual hex-comment form: check_header_offsets.py's DATA_SIZE
     * precompute cannot parse the mParticle line and stops at
     * the last field it CAN parse, which silently undercounts every derived
     * class's field offsets. MEASURED -- see notes/minigame-provenance.md. */
    /* 0x4660..0x471c is one MgCamera: 0x4660 + 0xbc = 0x471c, where
     * mParticle starts. The scenes pass &mCamera to the Camera_* routines. */
    MgCamera mCamera;                 /* offset 0x4660 */
    dPa_c mParticle;                  /* 0x471c */
};

/* A floor, not a claim the object ends here: 0x471c + sizeof(dPa_c).
   See notes/minigame-provenance.md. */
#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dScMgSingle3DBase_c_size_must_be_0x4f38[sizeof(dScMgSingle3DBase_c) == 0x4f38 ? 1 : -1];
#endif

#endif
