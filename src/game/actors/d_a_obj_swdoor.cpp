//cpp
/* daObjSwdoor_c -- ov002, the shared base of the switch-operated shutters
 * (RTTI and vtable evidence in include/daObjSwdoor_c.h).
 *
 * Slots 0 (InitResources), 3 (CleanupResources) and 6 (Behavior) are pure
 * virtual here; Render (slot 9) is the one real slot this class fills. The two
 * leaves, daObjBSwdoor_c (ov014) and daObjCvShutter_c (ov021), fill the null
 * slots by calling InitResourcesFrom, CleanupResourcesFrom and UpdateShutter
 * with their own file tables.
 *
 * The class's four byte fields (mTurnDir and mState in dBgActor_c's tail
 * padding, then mTimer and mEventBit) are set by InitResourcesFrom and run
 * by UpdateShutter; the leaves' own methods never name them.
 *
 * Notes:
 * - func_020393d4 is a 4-byte store into the dBgW at +0x18; dBgW.h has no
 *   setter.
 * - Abstract class: no factory and no g_profile row.
 */

#pragma defer_codegen off

#include "daObjSwdoor_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
#include "Sound.h"
#include "Event.h"
#include "math/CalcLib.h"

struct CLPS_Block;

struct ResourceDescriptor {
    SharedFilePtr *model;
    SharedFilePtr *collision;
    CLPS_Block *clps;
};
typedef char ResourceDescriptor_size_must_be_0x0c[sizeof(ResourceDescriptor) == 0x0c ? 1 : -1];

extern "C" {
void func_020393d4(dBgW_KcMbg *clsn, void *fn);
}

// @symbol _ZN13daObjSwdoor_cD0Ev
// @symbol _ZN13daObjSwdoor_cD1Ev
/* ROM ordinals 0 and 1 -- ov002 0x020bab0c (D0, 0x58) and 0x020bab64 (D1,
 * 0x44). No source here: both destructor variants come from the ONE
 * inline body in include/daObjSwdoor_c.h, which the class's descendants need
 * visible to inline its vptr store.
 *
 * The two calls below are never executed. Under `#pragma defer_codegen off`
 * the compiler emits ordinary functions at parse time in source order, so the
 * delete-expression pulls the deleting variant out of line first and the
 * explicit destructor call pulls the complete-object variant out second --
 * the cartridge's D0-then-D1 order, which no deferred form reaches without a
 * D2 the image does not contain. A delete-expression for D0 because
 * dBgActor_c declares Kill, a key function reachable from this class. */

#ifdef _MSC_VER
/* MSVC needs this flat D0 entry. Call the actual class-body destructor
 * qualified so dispatch is direct, then use the class-specific deallocator.
 * The inline body includes member/base teardown; no separate flat D1 provider
 * is supplied by this branch. The mwccarm definition below is unchanged. */
extern "C" daObjSwdoor_c *_ZN13daObjSwdoor_cD0Ev(daObjSwdoor_c *thiz) {
    thiz->daObjSwdoor_c::~daObjSwdoor_c(); /* direct member/base teardown */
    daObjSwdoor_c::operator delete(thiz);  /* the class-specific delete D0 ends with */
    return thiz;
}
#else
/* Not called. Forces the out-of-line copy of the deleting destructor. */
void daObjSwdoor_c_EmitDeletingDestructor(daObjSwdoor_c *p) {
    delete p;
}
#endif

/* Not called. Forces the out-of-line copy of the inline destructor. */
void daObjSwdoor_c_EmitDestructor(daObjSwdoor_c *p) {
    p->~daObjSwdoor_c();
}

/* 0x020baba8, size 0x48.
 * Teardown both leaves call: disable the collider if enabled, release the
 * descriptor's two files. */
// @symbol _ZN13daObjSwdoor_c20CleanupResourcesFromEP18ResourceDescriptor
s32 daObjSwdoor_c::CleanupResourcesFrom(ResourceDescriptor *descriptor) {
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    descriptor->model->Release();
    descriptor->collision->Release();
    return 1;
}

/* Vtable slot 9, ov002 0x020babf0. Key function. */
// @symbol _ZN13daObjSwdoor_c6RenderEv
s32 daObjSwdoor_c::Render() {
    mModel.Render(0);
    return 1;
}

/* 0x020bac18, size 0xf8.
 * Per-frame shutter the leaves install in the null Behavior slot. mState
 * runs 0, 1, 2:
 *   0  wait for the event bit and for the timer to run out, then arm a
 *      0x40-frame timer, clear the 0x1 bit of mFlags and play sound 0x3f;
 *   1  turn mAngleY by 0x100 a frame (direction from mTurnDir) until the timer
 *      runs out, then set the 0x1 bit of mFlags again;
 *   2  done. */
// @symbol _ZN13daObjSwdoor_c13UpdateShutterEv
s32 daObjSwdoor_c::UpdateShutter() {
    switch (mState) {
    case 0:
        if (Event::GetBit(mEventBit) != 0) {
            if (DecIfAbove0_Byte(&mTimer) == 0) {
                mTimer = 0x40;
                mState++;
                mFlags &= ~1;
                Sound::PlayBank3(0x3f, mCamSpacePos);
            }
        }
        break;
    case 1:
        if (DecIfAbove0_Byte(&mTimer) != 0) {
            if (mTurnDir != 0) {
                mAngleY += 0x100;
            } else {
                mAngleY -= 0x100;
            }
        } else {
            mState++;
            mFlags |= 1;
        }
        break;
    case 2:
        break;
    }

    UpdateModelPosAndRotY();
    return 1;
}

/* 0x020bad10, size 0xc0.
 * Resource setup both leaves call. The descriptor's model file goes to
 * dExtSimpleModel_c::LoadFile, its collision file to dBgW_Kc::LoadFile, and its CLPS block
 * into SetFile; then the stock UpdatePosAndAngs hook and Enable. The four
 * byte fields are set last: mTurnDir from bit 0 of param1, mEventBit from
 * bits 1..5, mState 0 and the start-up mTimer 5. */
// @symbol _ZN13daObjSwdoor_c17InitResourcesFromEP18ResourceDescriptor
s32 daObjSwdoor_c::InitResourcesFrom(ResourceDescriptor *descriptor) {
    mModel.SetFile((BMD_File *)dExtSimpleModel_c::LoadFile(*descriptor->model), 1, -1);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    mMeshCollider.SetFile((KCL_File *)dBgW_Kc::LoadFile(*descriptor->collision), mClsnMat, 0x1000,
                          mAngleY, *descriptor->clps);
    func_020393d4(&mMeshCollider, (void *)&dBgW::UpdatePosAndAngs);
    mMeshCollider.Enable(this);
    mTurnDir = param1 & 1;
    mEventBit = (param1 >> 1) & 0x1f;
    mState = 0;
    mTimer = 5;
    return 1;
}
