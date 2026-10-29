//cpp
/**
 * d_a_obj_c1_peach.cpp
 * Object - Peach's painting
 *
 * Castle painting (profile C1_PEACH). RTTI ov010:0x02112c24 names
 * daObjC1Peach_c, a direct dActor_c subclass of size 0x128.
 *
 * Ordinary functions are written in reverse ROM order: mwccarm emits one
 * .text section per function, reversed from source order. InitResources is
 * the out-of-line key function. The inline destructor in daObjC1Peach_c.h
 * makes that key function emit retail's D1 then D0, with no D2.
 *
 *   _ZN14daObjC1Peach_cD1Ev                    0x02111e10
 *   _ZN14daObjC1Peach_cD0Ev                    0x02111e40
 *   _ZN14daObjC1Peach_c20UpdateModelTransformEv 0x02111e84
 *   _ZN14daObjC1Peach_c16CleanupResourcesEv    0x02111ec4
 *   _ZN14daObjC1Peach_c6RenderEv               0x02111ee8
 *   _ZN14daObjC1Peach_c8BehaviorEv             0x02111f28
 *   _ZN14daObjC1Peach_c13InitResourcesEv       0x02111fc0
 *   daObjC1Peach_c_classInit                  0x02112004
 *
 * gC1PeachModelFile is the model SharedFilePtr this Init LoadFile's,
 * defined at the end of this file (constructed as file ID 1454).
 * g_profile_C1_PEACH stays outside this .text run.
 *
 * deslop
 * Leftover: cstd::fdiv is the shared coined fixed-point division used
 *   across TUs; naming belongs at a shared home, not this leaf.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum daObjC1Peach_c_AssetId {
    ASSET_C1_PEACH_BMD = 0x05ae
};

#include "daObjC1Peach_c.h"
#include "SharedFilePtr.h"
#include "nitro/fx/fx.h"

namespace cstd {
int fdiv(int a, int b);
}

/* The cartridge's ctor/dtor imports are the functions the wrapper's aliases
 * name. */
struct C1PeachModelFilePtr : SharedFilePtr {
    C1PeachModelFilePtr(u32 fileID);
    ~C1PeachModelFilePtr();
};

extern "C" {
extern void Matrix4x3_FromRotationY(Matrix4x3 *m, s32 angle);
/* Defined at the end of this file so its constructor does not enter .text. */
extern C1PeachModelFilePtr gC1PeachModelFile;
}

/* Reconstructed source-style name. SM64DS proves daObjC1Peach_c through
 * RTTI, the 0x128 allocation, the vtable, and profile C1_PEACH; later EAD
 * lineage supplies classInit. Historical alias: PeachPainting_Spawn.
 *
 * fBase_c::operator new(unsigned long) keeps the allocation on
 * _ZN7fBase_cnwEm. The emitted constructor stores the vtable address point. */
// @symbol daObjC1Peach_c_classInit
extern "C" daObjC1Peach_c *daObjC1Peach_c_classInit()
{
    return new daObjC1Peach_c();
}

// @symbol _ZN14daObjC1Peach_c13InitResourcesEv
int daObjC1Peach_c::InitResources()
{
    mModel.SetFile((BMD_File *)dExtSimpleModel_c::LoadFile(gC1PeachModelFile), 1, -1);
    mOpacity = 0xff;
    UpdateModelTransform();
    return 1;
}

// @symbol _ZN14daObjC1Peach_c8BehaviorEv
int daObjC1Peach_c::Behavior()
{
    int distance = DistToCPlayer();
    if (distance >= 0xe10000) {
        mOpacity = 0xff;
    } else if (distance <= 0xbf4000) {
        mOpacity = 0;
    } else {
        int fraction = cstd::fdiv(distance - 0xbf4000, 0x21c000);
        int opacity = FX_MUL(fraction, 0xff);
        mOpacity = (u8)(opacity >> 3);
    }

    mModel.ApplyOpacity(mOpacity, 1);
    return 1;
}

// @symbol _ZN14daObjC1Peach_c6RenderEv
int daObjC1Peach_c::Render()
{
    if (mOpacity == 0)
        return 1;

    mModel.Render(0);
    return 1;
}

// @symbol _ZN14daObjC1Peach_c16CleanupResourcesEv
int daObjC1Peach_c::CleanupResources()
{
    gC1PeachModelFile.Release();
    return 1;
}

// @symbol _ZN14daObjC1Peach_c20UpdateModelTransformEv
void daObjC1Peach_c::UpdateModelTransform()
{
    Matrix4x3_FromRotationY(&mModel.mat4x3, mAngleY);
    mModel.mat4x3.t.x = mPosX >> 3;
    mModel.mat4x3.t.y = mPosY >> 3;
    mModel.mat4x3.t.z = mPosZ >> 3;
}

/* The ROM's initializer constructs the model handle with file ID 1454 and
 * registers a destructor node for it. mwcc emits
 * __sinit_d_a_obj_c1_peach.cpp from this definition. */
C1PeachModelFilePtr gC1PeachModelFile(ASSET_C1_PEACH_BMD);
