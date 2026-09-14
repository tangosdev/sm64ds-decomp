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
 * deslop
 * Leftover: dBgW_KcMbg::SetFile stays mangled -- InitResources passes
 *   Fix12<int> by value (wall 6az); a method call homes the argument and
 *   size-DIFFs.
 * Leftover: dBgActor_c::IsClsnInRange stays mangled -- Behavior's two
 *   by-value Fix12<int> (wall 6az); the header method form is refused
 *   (include/dBgActor_c.h).
 * Leftover: func_020393d4 stores dBgW::UpdatePosAndAngs on mMeshCollider;
 *   dBgW.h has no setter. Naming the store belongs with dBgW in arm9.
 * Leftover: data_ov036_02113d78 is a three-word overlay table (BMD
 *   0x021140ac, KCL 0x021140b4, CLPS 0x02112ac8). Naming the three
 *   targets directly builds a different literal pool and moves every
 *   call site. This TU claims .text only.
 * Leftover: g_profile_RC_GURUGURU lives outside this TU (S14).
 */

#include "daObjRc_Guruguru_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"

extern "C" {
/* [0] the BMD's SharedFilePtr (0x021140ac), [1] the KCL's (0x021140b4),
   [2] the CLPS block dBgW_KcMbg::SetFile is handed (0x02112ac8). */
extern void *data_ov036_02113d78[];

void func_020393d4(void *bgw, void *fn);

bool _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *self, void *kcl, void *mtx, int scale, short angleY, void *clps);
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol daObjRc_Guruguru_c_classInit
/* Reconstructed source-style name: SM64DS proves daObjRc_Guruguru_c through
   RTTI, allocation size, vtable identity, and the RC_GURUGURU registry
   profile; later EAD lineage supplies classInit. Exact original spelling is
   not preserved. Historical alias: ArmedRotatingPlatform_Spawn.

   Every instruction the cartridge has here falls out of the one `new`.
   800 = 0x320 is the class's own size, into the header's leaf operator new;
   the implicit constructor inlines dBgActor_c's C2 and the derived vptr
   store. Declaring a constructor of our own would emit a `bl` the factory
   does not have. */
extern "C" daObjRc_Guruguru_c *daObjRc_Guruguru_c_classInit()
{
    return new daObjRc_Guruguru_c();
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN18daObjRc_Guruguru_c13InitResourcesEv
/* dBgW_KcMbg::SetFile takes Fix12<int> by value. An ordinary member call
 * triggers mwccarm's by-value-class parameter homing and changes the ROM ABI,
 * so this one call deliberately retains the measured register-level view. */
s32 daObjRc_Guruguru_c::InitResources()
{
    mModel.SetFile((BMD_File *)Model::LoadFile(
        *(SharedFilePtr *)data_ov036_02113d78[0]), 1, -1);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();

    void *kcl = dBgW_Kc::LoadFile(*(SharedFilePtr *)data_ov036_02113d78[1]);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, kcl, &mClsnMat, 0x1000, mAngleY,
        data_ov036_02113d78[2]);

    /* The mesh follows the actor's position AND angles -- this platform turns,
       so the collision has to turn with it. */
    func_020393d4(&mMeshCollider, (void *)&dBgW::UpdatePosAndAngs);

    /* The default spin is clockwise at 0x80 units of yaw a frame; a level
       author overrides both rate and direction through the spawn's mAngleZ. */
    mAngVelY = -0x80;
    if (mAngleZ != 0)
        mAngVelY = mAngleZ;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN18daObjRc_Guruguru_c8BehaviorEv
/* IsClsnInRange takes two Fix12<int> by value; see InitResources for why that
 * one call keeps the measured register-level spelling. */
s32 daObjRc_Guruguru_c::Behavior()
{
    /* `+=`, not `mAngleY = mAngleY + ...`: the compound form CSEs the field
       address into a register and reuses it for the load and the store, which
       is what the ROM does here. */
    mAngleY += mAngVelY;

    /* The temporary is load-bearing: the ROM materialises the predicate with
       movne/moveq and then tests it, where a direct `if` folds the two.
       Bit 0x8 is the framework off-screen flag (include/dActor_c.h). */
    int flagged = (int)((mFlags & 8) != 0);
    if (flagged != 0) {
        if (mMeshCollider.IsEnabled())
            mMeshCollider.Disable();
        return 1;
    }

    UpdateModelPosAndRotY();
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0))
        UpdateClsnPosAndRot();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN18daObjRc_Guruguru_c6RenderEv
s32 daObjRc_Guruguru_c::Render()
{
    mModel.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN18daObjRc_Guruguru_c16CleanupResourcesEv
s32 daObjRc_Guruguru_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();

    ((SharedFilePtr *)data_ov036_02113d78[0])->Release();
    ((SharedFilePtr *)data_ov036_02113d78[1])->Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN18daObjRc_Guruguru_cD1Ev
// @symbol _ZN18daObjRc_Guruguru_cD0Ev
/* NOT WRITTEN HERE ON PURPOSE. The inline `~daObjRc_Guruguru_c() {}` in the
   header is the whole source of both variants: from an inline body mwcc emits
   D1 and then D0 -- the cartridge's own order -- and no D2, which is the order
   and the set the ROM carries. Written out of line here instead, mwcc emits D0
   ahead of D1 and rombuild refuses the object outright. */
