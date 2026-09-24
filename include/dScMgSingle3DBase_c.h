/* Base class for the "single 3D minigame" family -- 13 direct RTTI children
 * (card, cup, memory x2, mahjong-carlo x2, roulette, slot3, sound, BSC,
 * snowball, flower, 3DEsp). Adds a Particle::SysTracker at 0x471c. See
 * notes/minigame-provenance.md for the field evidence. */
#ifndef DSCMGSINGLE3DBASE_C_H
#define DSCMGSINGLE3DBASE_C_H
#include "dScMgBase_c.h"

#include "Particle__SysTracker.h"

namespace Particle {
/* Called by dScMgSingle3DBase_c::BeforeRender. */
void RenderAll();
}

struct dScMgSingle3DBase_c : dScMgBase_c {
    /* Declared first, so it takes slots 16 (D1) and 17 (D0) -- but it is NOT
       this class's key function.  An inline destructor is emitted in every
       TU that needs it, so it anchors nothing; the key function is
       AfterInitResources, the first DECLARED non-inline virtual, which is
       why the vtable and typeinfo land in src/minigames/d_s_mg_single3_d_base.cpp
       (see that class's rows in config/tu_manifest.d/ov006/).
       MUST STAY DEFINED INLINE -- all 13 children inline this body, and
       _ZN19dScMgSingle3DBase_cD2Ev exists nowhere in the ROM, so an
       out-of-line definition leaves every child with an undefined external.
       MEASURED on dScMgMemory_c; do not move the body out. */
    virtual ~dScMgSingle3DBase_c() {}

    /* --- re-overrides of dScMgBase_c's virtuals, in _ZTV order.
           Slots 26 and 33 are ALSO re-overrides, not new virtuals: this class
           was previously annotated as introducing them, but dScMgBase_c's own
           vtable already carries bodies at both (ov004:0x020b04e0 for slot 26,
           ov004:0x020b265c for slot 33).  Slot 26 is declared below now that
           dScMgBase_c has named it, and its ov006 body is a real member
           definition in src/minigames/d_s_mg_single3_d_base.cpp rather than the
           mangled free function it had to be while the base was silent.
           Slot 33 is declared below now as well, and its ov006 body is a
           real member definition in the same file, where it byte-verified for
           three commits as the mangled free function func_ov006_0210a708.  It
           was the LAST vtable slot in that unit still spelled that way; the
           one function left there under a func_<module>_<address> name,
           0x0210a534, is in no vtable at all.
           Declaring dScMgBase_c's remaining two (34-35) is what lets
           this class emit its full 36-slot vtable; today it emits a byte-exact
           34-slot prefix. --- */
    virtual void AfterInitResources(u32 vfSuccess); /* slot  2 */
    virtual void AfterCleanupResources(u32 vfSuccess); /* slot  5 */
    virtual int  BeforeBehavior();                  /* slot  7 */
    virtual int  BeforeRender();                    /* slot 10 */
    virtual int  OnHitByCannonBlastedChar();      /* slot 26 */
    virtual void Virtual84();                          /* slot 33 */

    /* DO NOT restyle the seven `offset 0xNN` comments below into this
     * tree's usual hex-comment form: check_header_offsets.py's DATA_SIZE
     * precompute cannot parse the namespaced mSysTracker line and stops at
     * the last field it CAN parse, which silently undercounts every derived
     * class's field offsets. MEASURED -- see notes/minigame-provenance.md. */
    /* 0x4660..0x471c IS ONE MINIGAME CAMERA, 0xbc bytes, and three readings
     * agree. src/Camera_UpdateMatrices.c -- the ov006 routine both
     * dScMgRoulette_c::Render and dScMg3DEsp_c::Render call as
     * Camera_UpdateMatrices(this + 0x4660) -- carries the layout outright:
     * view matrix at +0x00, projection at +0x60, `eye` at +0xa0, `target` at
     * +0xac, `angle` at +0xb8, and it computes the view direction as
     * eye - target. Those three land on 0x4700 / 0x470c / 0x4718 here. And
     * 0x4660 + 0xbc = 0x471c, which is exactly where mSysTracker starts.
     * The head of the object stays a pad: nothing in this family reads a
     * matrix element individually. */
    u8  pad_4660[0xa0];
    s32 mCameraEyeX; /* offset 0x4700 */
    s32 mCameraEyeY; /* offset 0x4704 */
    s32 mCameraEyeZ; /* offset 0x4708 */
    s32 mCameraTargetX; /* offset 0x470c */
    s32 mCameraTargetY; /* offset 0x4710 */
    s32 mCameraTargetZ; /* offset 0x4714 */
    s16 mCameraAngle; /* offset 0x4718 */
    u8  pad_471a[0x2];
    Particle::SysTracker mSysTracker; /* 0x471c */
};

/* A floor, not a claim the object ends here: 0x471c + sizeof(SysTracker).
   See notes/minigame-provenance.md. */
#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dScMgSingle3DBase_c_size_must_be_0x4f38[sizeof(dScMgSingle3DBase_c) == 0x4f38 ? 1 : -1];
#endif

#endif
