/* MgCamera: the 0xbc-byte camera the minigame scenes embed. The name is
 * COINED. The ROM has no typeinfo, vtable or out-of-line member for it,
 * and its routines are the extern "C" Camera_* functions at the head of ov006
 * (src/minigames/unit020bfec0.cpp). It is NOT dCamera_c, the stage camera.
 *
 * dScMgSingle3DBase_c holds one at 0x4660 and dScMgD3DBase_c holds two at
 * 0x466c. dScMgD3DBase_c's array strides by 0xbc, and 0x4660 + 0xbc is where
 * dScMgSingle3DBase_c::mParticle starts.
 *
 * Camera_UpdateMatrices fixes the layout. It hands mPos to G3i_LookAt_ as the
 * camera position and mLookAt as the point looked at, with mViewMtx as the
 * output. It looks mFovAngle up in the sine/cosine table for
 * G3i_PerspectiveW_ and passes it to dClipper::Func_020156DC as the fov
 * angle. These are the same pair dCamera_c keeps as lookAt and pos. */
#ifndef MGCAMERA_H
#define MGCAMERA_H
#include "types.h"
#include "nitro/fx/fx.h"

/* The members are the SDK's plain VecFx32 and MtxFx43, not the game's Vector3
   and Matrix4x3, which declare destructors (Matrix4x3 through its Vector3).
   A destructor would make dScMgD3DBase_c tear its two cameras down with
   __cxa_vec_cleanup, and the ROM's destructors do not. The constructor is
   real: the D3D factories step through both elements, which is what an array
   of a class with an empty inline constructor compiles to. */
struct MgCamera {
    MgCamera() {}

    /* G3i_LookAt_ writes it, CopyToViewMat loads it, and
       Camera_ProjectToScreen transforms a world point by it. */
    MtxFx43 mViewMtx; /* 0x00 */
    u8 pad_30[0x30];  /* 0x30 */
    /* G3i_PerspectiveW_ writes all sixteen words. */
    fx32 mProjMtx[4][4]; /* 0x60 */
    VecFx32 mLookAt;     /* 0xa0 */
    VecFx32 mPos;        /* 0xac */
    /* Loaded with ldrsh; >> 4 indexes the sine/cosine table. */
    s16 mFovAngle;  /* 0xb8 */
    u8 pad_ba[0x2]; /* 0xba */
};

#ifndef SM64DS_PLATFORM_PC
/* The offsets above rely on the SDK types' sizes. */
typedef char fx32_size_must_be_0x4[sizeof(fx32) == 0x4 ? 1 : -1];
typedef char VecFx32_size_must_be_0xc[sizeof(VecFx32) == 0xc ? 1 : -1];
typedef char MtxFx43_size_must_be_0x30[sizeof(MtxFx43) == 0x30 ? 1 : -1];
typedef char MgCamera_size_must_be_0xbc[sizeof(MgCamera) == 0xbc ? 1 : -1];
#endif

/* Camera_ProjectToScreen16, Camera_InitSlotsView and Camera_InitFlowerView
   are declared in decl_common.h with void * and char * cameras. mwcc rejects
   a second extern "C" declaration with a different parameter type as illegal
   overloading, so they are not declared here until decl_common.h drops its
   copies. */
extern "C" {
void Camera_ProjectToScreen(MgCamera *, void *, int *, int *);
}
#ifdef __cplusplus
void Camera_UpdateMatrices(MgCamera *);
void Camera_InitDealerView(MgCamera *);
void Camera_InitPresenterView(MgCamera *);
#else
extern void Camera_UpdateMatrices(MgCamera *);
extern void Camera_InitDealerView(MgCamera *);
extern void Camera_InitPresenterView(MgCamera *);
#endif

#endif
