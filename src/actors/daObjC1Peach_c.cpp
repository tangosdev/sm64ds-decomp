//cpp
/**
 * daObjC1Peach_c.cpp
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
 * data_ov010_02112d64 is the model SharedFilePtr this Init LoadFile's.
 * g_profile_C1_PEACH and the ov010 sinit stay outside this .text run.
 *
 * deslop
 * Leftover: cstd::fdiv is the shared coined fixed-point division used
 *   across TUs; naming belongs at a shared home, not this leaf.
 */

#include "daObjC1Peach_c.h"
#include "SharedFilePtr.h"

namespace cstd { int fdiv(int a, int b); }

extern "C" {
extern void Matrix4x3_FromRotationY(Matrix4x3 *m, s32 angle);
/* decl plurality is int[] (the ov010 sinit). SharedFilePtr has no fields. */
extern int data_ov010_02112d64[];
}

/* Reconstructed source-style name. SM64DS proves daObjC1Peach_c through
 * RTTI, the 0x128 allocation, the vtable, and profile C1_PEACH; later EAD
 * lineage supplies classInit. Historical alias: PeachPainting_Spawn.
 *
 * fBase_c::operator new(unsigned long) keeps the allocation on
 * _ZN7fBase_cnwEj. The emitted constructor stores the vtable address point. */
// @symbol daObjC1Peach_c_classInit
extern "C" daObjC1Peach_c *daObjC1Peach_c_classInit()
{
    return new daObjC1Peach_c();
}

// @symbol _ZN14daObjC1Peach_c13InitResourcesEv
int daObjC1Peach_c::InitResources()
{
    mModel.SetFile((BMD_File *)Model::LoadFile(*(SharedFilePtr *)data_ov010_02112d64), 1, -1);
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
        int opacity = (int)(((long long)fraction * 0xff + 0x800) >> 12);
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
    ((SharedFilePtr *)data_ov010_02112d64)->Release();
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
