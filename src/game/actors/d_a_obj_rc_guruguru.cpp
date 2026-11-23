//cpp
/* Rainbow Ride armed rotating platform (ARMED_ROTATING_PLATFORM 132,
 * profile RC_GURUGURU) -- ov036/daObjRc_Guruguru_c.
 *
 * A dBgActor_c that adds a fixed yaw step to its own heading every frame
 * and drags its model and collision mesh round with it, until mFlags bit
 * 0x8 (off screen) is set and it parks itself with the mesh switched off.
 *
 * RTTI ov036:0x02113d90 names daObjRc_Guruguru_c; the debug table names
 * RC_GURUGURU. ov036 is mixed -- this is the spinning platform, not
 * kaitendai (ROTATING_PLATFORM_RR 129) / carpet / buranko / hane.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S. mwccarm 2004/b56
 * emits one .text section per function in the REVERSE of source order, so
 * the highest-address ROM function is written FIRST here. Do not reorder.
 *
 * Notes:
 * - data_ov036_02113d78 is a three-word overlay table (BMD 0x021140ac,
 *   KCL 0x021140b4, CLPS 0x02112ac8). Naming the three targets directly
 *   builds a different literal pool and moves every call site. The CLPS
 *   words at 0x02112aa8..0x02112ae8 are defined in this TU.
 * - g_profile_RC_GURUGURU lives outside this TU.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum d_a_obj_rc_guruguru_AssetId { ASSET_RC_GURUGURU_BMD = 0x06a4, ASSET_RC_GURUGURU_KCL = 0x06a5 };

#include "SpawnInfo.h"
#include "daObjRc_Guruguru_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
#include "CLPS.h"

/* The BMD's SharedFilePtr (0x021140ac), the KCL's (0x021140b4) and the
   CLPS block dBgW_KcMbg::SetFile is handed (0x02112ac8). */
struct RcGuruguruAssets {
    SharedFilePtr *modelFile;
    SharedFilePtr *clsnFile;
    CLPS_Block *clps;
};
extern RcGuruguruAssets data_ov036_02113d78;

/* dActor_c::mFlags bit 0x08 is written by the framework while the actor is
 * off screen. This int-returning predicate is what makes the ROM
 * materialize 0/1 with movne/moveq before testing it; a direct `if` on the
 * bit folds the two. */
static inline int IsOffScreen(dActor_c *actor) {
    return (actor->mFlags & 8) != 0;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* Reconstructed source-style name: SM64DS proves daObjRc_Guruguru_c through
   RTTI, allocation size, vtable identity, and the RC_GURUGURU registry
   profile; later EAD lineage supplies classInit. Exact original spelling is
   not preserved. Historical alias: ArmedRotatingPlatform_Spawn.

   Every instruction the cartridge has here falls out of the one `new`.
   800 = 0x320 is the class's own size, passed to fBase_c::operator new;
   the implicit constructor inlines dBgActor_c's C2 and the derived vptr
   store. Declaring a constructor of our own would emit a `bl` the factory
   does not have. */
daObjRc_Guruguru_c *daObjRc_Guruguru_c_classInit() {
    return new daObjRc_Guruguru_c();
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
s32 daObjRc_Guruguru_c::InitResources() {
    mModel.SetFile(dExtSimpleModel_c::LoadFile(*data_ov036_02113d78.modelFile), 1, -1);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();

    KCL_File *kcl = dBgW_Kc::LoadFile(*data_ov036_02113d78.clsnFile);
    mMeshCollider.SetFile(kcl, mClsnMat, 0x1000, mAngle.y, *data_ov036_02113d78.clps);

    /* The mesh follows the actor's position AND angles -- this platform turns,
       so the collision has to turn with it. */
    mMeshCollider.SetBeforeClsnCallback(&dBgW::UpdatePosAndAngs);

    /* The default spin is clockwise at 0x80 units of yaw a frame; a level
       author overrides both rate and direction through the spawn's mAngle.z. */
    mAngVelY = -0x80;
    if (mAngle.z != 0)
        mAngVelY = mAngle.z;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
s32 daObjRc_Guruguru_c::Behavior() {
    /* `+=`, not `mAngle.y = mAngle.y + ...`: the compound form CSEs the field
       address into a register and reuses it for the load and the store, which
       is what the ROM does here. */
    mAngle.y += mAngVelY;

    if (IsOffScreen(this)) {
        if (mMeshCollider.IsEnabled())
            mMeshCollider.Disable();
        return 1;
    }

    UpdateModelPosAndRotY();
    if (IsClsnInRange(0, 0))
        UpdateClsnPosAndRot();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
s32 daObjRc_Guruguru_c::Render() {
    mModel.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
s32 daObjRc_Guruguru_c::CleanupResources() {
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();

    data_ov036_02113d78.modelFile->Release();
    data_ov036_02113d78.clsnFile->Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* NOT WRITTEN HERE ON PURPOSE. The inline `~daObjRc_Guruguru_c() {}` in the
   header is the whole source of both variants: from an inline body mwcc emits
   D1 and then D0 -- the cartridge's own order -- and no D2, which is the order
   and the set the ROM carries. Written out of line here instead, mwcc emits D0
   ahead of D1 and rombuild refuses the object outright. */

/* File-scope resource handles: the ctor/dtor are the ROM's SharedFilePtr
 * veneer pairs (func_02017acc / func_02017ab4 for the model, func_02017b4c /
 * SharedFilePtr_Destruct_Clsn for the collision), spelled through
 * declared-only subclasses so the static initializer names the real entry
 * points. */
struct RcGuruguruModelFilePtr : SharedFilePtr {
    RcGuruguruModelFilePtr(u32 fileID);
    ~RcGuruguruModelFilePtr();
};
struct RcGuruguruCollisionFilePtr : SharedFilePtr {
    RcGuruguruCollisionFilePtr(u32 fileID);
    ~RcGuruguruCollisionFilePtr();
};

/* The retail initializer constructs the model first, then the collision
 * file. */
RcGuruguruModelFilePtr gRcGuruguruModelFile(ASSET_RC_GURUGURU_BMD);         /* model */
RcGuruguruCollisionFilePtr gRcGuruguruCollisionFile(ASSET_RC_GURUGURU_KCL); /* collision */

/* 0x02112aa8 is one CLPS entry. 0x02112ac8 is the two-entry block SetFile
   receives. The 0xffffffff words after each count are not entries; symbols.txt
   still names them inside the block. */
struct {
    u32 tag;
    u16 stride;
    u16 count;
    struct CLPS entry;
    u32 tail[4];
} data_ov036_02112aa8 = {
    0x53504c43, 8, 1, {0x01000fc0, 0x000000ff}, {0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff},
};
struct {
    u32 tag;
    u16 stride;
    u16 count;
    struct CLPS entries[2];
    u32 tail[2];
} data_ov036_02112ac8 = {
    0x53504c43,
    8,
    2,
    {{0x01000fc0, 0x000000ff}, {0x01000fc4, 0x000000ff}},
    {0xffffffff, 0xffffffff},
};

RcGuruguruAssets data_ov036_02113d78 = {
    &gRcGuruguruModelFile,
    &gRcGuruguruCollisionFile,
    reinterpret_cast<CLPS_Block *>(&data_ov036_02112ac8),
};

extern "C" SpawnInfo<daObjRc_Guruguru_c> g_profile_RC_GURUGURU = {daObjRc_Guruguru_c_classInit,
                                                                  132,
                                                                  262,
                                                                  0x00000002,
                                                                  0x00000000,
                                                                  0x00800000,
                                                                  0x02000000,
                                                                  0x00000000};
