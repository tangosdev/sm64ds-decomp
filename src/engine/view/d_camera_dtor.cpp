//cpp
/* dCamera_c's destructors and the first three camera helpers.
 * arm9 .text 0x0200705c..0x020072c4.
 *
 *   ~dCamera_c     D1 then D0 (0x0200705c, 0x02007098)
 *   func_020070e8  orbit pos around the target player: ease the distance and
 *                  the two angles from the player to pos, then rebuild pos
 *   func_020071a8  set mOrbitTargetYaw to ang once the yaw from mTargetPos
 *                  to pos is more than threshold past it
 *   func_020071e0  scripted state: rotate savedLookAt / savedPos by the target
 *                  player's facing and ease pos and lookAt onto them
 *   Vector3::~Vector3
 *                  the cartridge's copy (0x020072c0), emitted at the end of
 *                  the object for func_020071e0's Vector3 locals
 *
 * The emitted Vector3 destructor ends the object, so func_020072c4 starts
 * the next one. Not part of src/engine/view/d_camera.cpp
 * (0x020073a0..0x0200e494): func_020072c4 lies between, and this object
 * is built with defer_codegen off while that one is not.
 *
 * defer_codegen off emits the functions in source order: D1, D0, a homeless
 * D2, then the helpers. Retail _ZTV9dCamera_c stays at 0x02086f84; this TU's
 * copy is discarded. The helpers keep their ROM names under C linkage;
 * the other camera states call them by those names.
 */
#include "types.h"
#include "dCamera_c.h"
#include "dActor_c.h"
#include "math/Approach.h"
#include "math/CalcLib.h"

extern "C" {
/* local extern: these camera vector helpers have no header outside the
   decl_common.h catch-all, which is C. */
extern void Vec3_RotateYAndTranslate(Vector3 *out, const Vector3 *trans, short ang,
                                     const Vector3 *add);
extern void func_02007c9c(const Vector3 *from, const Vector3 *to, int *outDist, short *outVertAng,
                          short *outHorzAng);
extern void func_02007c14(Vector3 *res, const Vector3 *trans, int mag, short ang, short angY);
extern int func_02007cec(void *a, const Vector3 *b, int c);
extern void func_02007d0c(Vector3 *self, const Vector3 *src, int sx, int sy, int sz);
extern void Math_Function_0203b0fc(int *value, int target, int step, int max);
/* local extern: AngleDiff is defined int; func_020071a8 compares its
   result without sign-extending it, so it is declared short here. */
extern short AngleDiff(int a, int b);
}

#ifdef _MSC_VER
/* The host needs the ROM's flat D0 name. MSVC folds the Itanium variants
 * into the one ~dCamera_c() it emits, so the deleting destructor is spelled
 * out here: the D1 body, called qualified, then the class-specific delete.
 * Nothing in this arm reaches mwccarm. */
dCamera_c::~dCamera_c() {}

extern "C" dCamera_c *_ZN9dCamera_cD0Ev(dCamera_c *thiz) {
    thiz->dCamera_c::~dCamera_c();
    dCamera_c::operator delete(thiz);
    return thiz;
}
#else
#pragma defer_codegen off

// @symbol _ZN9dCamera_cD1Ev
// @symbol _ZN9dCamera_cD0Ev
dCamera_c::~dCamera_c() {}
#endif

extern "C" {

// @symbol func_020070e8
void func_020070e8(dCamera_c *cam, int distTarget, int distStep, short vertTarget, short vertDiv,
                   short horzTarget, short horzDiv) {
    int dist;
    short vertAng;
    short horzAng;
    Vector3 *trans = &reinterpret_cast<dActor_c *>(cam->mTargetPlayer)->mPos;
    func_02007c9c(trans, &cam->pos, &dist, &vertAng, &horzAng);
    Math_Function_0203b0fc(&dist, distTarget, distStep, 0x7fffffff);
    if (vertDiv != 0)
        ApproachAngle(&vertAng, vertTarget, vertDiv, 0x4000, 0);
    if (horzDiv != 0)
        ApproachAngle(&horzAng, horzTarget, horzDiv, 0x4000, 0);
    func_02007c14(&cam->pos, trans, dist, vertAng, horzAng);
}

// @symbol func_020071a8
void func_020071a8(dCamera_c *cam, short ang, int threshold) {
    short diff = AngleDiff(Vec3_HorzAngle(&cam->mTargetPos, &cam->pos), ang);
    if (diff > threshold)
        cam->mOrbitTargetYaw = ang;
}

// @symbol func_020071e0
int func_020071e0(dCamera_c *cam) {
    Vector3 va, vb;
    int dist;
    short ang, angY;
    /* mScriptParam is a speed here; func_020080b0 reads it as a height. */
    ApproachLinear(cam->mScriptParam, 0x5a000, 0x2800);
    {
        dActor_c *o = reinterpret_cast<dActor_c *>(cam->mTargetPlayer);
        short a = (short)(o->mAngleY + 0x8000);
        const Vector3 *trans = &o->mPos;
        Vec3_RotateYAndTranslate(&va, trans, a, &cam->savedLookAt);
        Vec3_RotateYAndTranslate(&vb, trans, a, &cam->savedPos);
    }
    func_02007c9c(&va, &cam->pos, &dist, &ang, &angY);
    ApproachLinear(dist, 0, cam->mScriptParam);
    func_02007c14(&cam->pos, &va, dist, ang, angY);
    func_02007cec(&cam->pos, &va, 0x28);
    func_02007d0c(&cam->lookAt, &vb, 0x800, 0xccc, 0x800);
    return 1;
}
}

/* The Vector3 destructor's body is the inline one in include/types.h. */
// @symbol _ZN7Vector3D1Ev
