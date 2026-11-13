//cpp
/* dCamera_c's destructors and the first three camera helpers.
 * arm9 .text 0x0200705c..0x020072c4.
 *
 *   ~dCamera_c     D1 then D0 (0x0200705c, 0x02007098)
 *   func_020070e8  orbit pos around the target player: ease the distance and
 *                  the two angles from the player to pos, then rebuild pos
 *   StarDanceBoundYaw  set mOrbitTargetYaw to ang once the yaw from mTargetPos
 *                  to pos is more than threshold past it
 *   CutsceneGrandStarFlyAppCvars  scripted state: rotate savedLookAt / savedPos by the target
 *                  player's facing and ease pos and lookAt onto them
 *   Vector3::~Vector3
 *                  the cartridge's copy (0x020072c0), emitted at the end of
 *                  the object for CutsceneGrandStarFlyAppCvars's Vector3 locals
 *
 * The emitted Vector3 destructor ends the object, so CutsceneGrandStarFlyMarioOffscreen starts
 * the next one, src/engine/view/d_camera.cpp (0x020072c4..0x0200e494).
 * This object is built with defer_codegen off and that one is not.
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
}

extern void Vec3fGetDistAndAngle(const Vector3 *from, const Vector3 *to, int *outDist,
                                 short *outVertAng, short *outHorzAng);
extern void Vec3fSetDistAndAngle(Vector3 *res, const Vector3 *trans, int mag, short ang,
                                 short angY);
extern int func_02007cec(void *a, const Vector3 *b, int c);
extern void ApproachVec3fAsymptotic(Vector3 *self, const Vector3 *src, int sx, int sy, int sz);
extern "C" {

extern void Math_Function_0203b0fc(int *value, int target, int step, int max);
extern int AngleDiff(int a, int b);
}

#ifdef _MSC_VER
/* Host definition of the one destructor MSVC emits. The flat D0 name is
 * port/hal/host_d0.cpp. Nothing in this arm reaches mwccarm. */
dCamera_c::~dCamera_c() {}
#else
#pragma defer_codegen off

dCamera_c::~dCamera_c() {}
#endif

void func_020070e8(dCamera_c *cam, int distTarget, int distStep, short vertTarget, short vertDiv,
                   short horzTarget, short horzDiv) {
    int dist;
    short vertAng;
    short horzAng;
    Vector3 *trans = &reinterpret_cast<dActor_c *>(cam->mTargetPlayer)->mPos;
    Vec3fGetDistAndAngle(trans, &cam->pos, &dist, &vertAng, &horzAng);
    Math_Function_0203b0fc(&dist, distTarget, distStep, 0x7fffffff);
    if (vertDiv != 0)
        ApproachAngle(&vertAng, vertTarget, vertDiv, 0x4000, 0);
    if (horzDiv != 0)
        ApproachAngle(&horzAng, horzTarget, horzDiv, 0x4000, 0);
    Vec3fSetDistAndAngle(&cam->pos, trans, dist, vertAng, horzAng);
}

/* Super Mario 64's star_dance_bound_yaw, called for the same stars. Where
   the N64 clamps the camera's yaw to within yawMax of absYaw, this aims
   the orbit at absYaw once the camera strays further than that. */
void StarDanceBoundYaw(dCamera_c *cam, short ang, int threshold) {
    if (AngleDiff(Vec3_HorzAngle(&cam->mTargetPos, &cam->pos), ang) > threshold)
        cam->mOrbitTargetYaw = ang;
}

/* Script op, Super Mario 64's cutscene_grand_star_fly_app_cvars: turn
   savedLookAt and savedPos by the target player's yaw, close pos in on the
   first at a rising speed, and ease lookAt toward the second. */
int CutsceneGrandStarFlyAppCvars(dCamera_c *cam) {
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
    Vec3fGetDistAndAngle(&va, &cam->pos, &dist, &ang, &angY);
    ApproachLinear(dist, 0, cam->mScriptParam);
    Vec3fSetDistAndAngle(&cam->pos, &va, dist, ang, angY);
    func_02007cec(&cam->pos, &va, 0x28);
    ApproachVec3fAsymptotic(&cam->lookAt, &vb, 0x800, 0xccc, 0x800);
    return 1;
}

/* The Vector3 destructor's body is the inline one in include/types.h. */
