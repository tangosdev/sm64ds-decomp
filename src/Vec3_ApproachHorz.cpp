//cpp
/* Move a point toward another, in 3D and on the horizontal plane.
 *
 * leftovers:
 * - Vec3 and Vector3 are the same three ints. VEC_Mag, Vec3_Sub, VEC_Subtract,
 *   Vec3_HorzLen, Vec3_Equal and Vec3_MulScalar take Vector3; the local POD
 *   Vector3 keeps those calls on their declared interface without adding
 *   types.h Vector3 lifetimes. Vec3_ApproachHorz's own parameters use the
 *   local POD Vector3 spelling too. That matches the public Vector3
 *   declaration the actors call through, without adding a Vector3 lifetime
 *   here.
 * - This local Vector3 is the tree's last header shadow, and it is kept on
 *   purpose. It is not known whether EAD wrote these locals as NitroSDK's
 *   VecFx32: VEC_Mag and VEC_Subtract are SDK functions, but Vec3_Sub and the
 *   other game helpers take the game type. Issue #5207 tracks that question.
 */
#include "math/Approach.h"
#include "nitro/fx/fx_cp.h"

/* Not types.h's: its inline ~Vector3 adds a _ZN7Vector3D1Ev copy and renumbers the registration nodes (measured). */
struct Vector3 { int x, y, z; };

extern "C" {
extern void Vec3_Sub(struct Vector3 *out, const struct Vector3 *a, const struct Vector3 *b);
extern int Vec3_HorzLen(const struct Vector3* v);
extern int VEC_Mag(const struct Vector3 *v);
extern void Vec3_MulScalar(struct Vector3 *out, const struct Vector3 *in, int scale);
extern void VEC_Subtract(Vector3 *a, Vector3 *b, Vector3 *c);
extern int Vec3_Equal(const struct Vector3 *a, const struct Vector3 *b);
}

int ApproachLinear(Vector3 &dst, const Vector3 &target, int step) {
    struct Vector3 diff;
    struct Vector3 scaled;
    if (step != 0) {
        Vec3_Sub(&diff, &dst, &target);
        int len = VEC_Mag(&diff);
        if (len == 0 || len <= step) {
            dst = target;
            return 1;
        }
        Vec3_MulScalar(&scaled, &diff, FX_Div(step, len));
        VEC_Subtract(&dst, &scaled, &dst);
    } else if (Vec3_Equal(&dst, &target)) {
        return 1;
    }
    return 0;
}

extern "C" int Vec3_ApproachHorz(struct Vector3 *out, struct Vector3 *a, int maxStep) {
    struct Vector3 diff;
    struct Vector3 scaled;
    int len;
    Vec3_Sub(&diff, out, a);
    diff.y = 0;
    len = Vec3_HorzLen(&diff);
    if (maxStep != 0) {
        if (len == 0 || len <= maxStep) {
            out->x = a->x;
            out->z = a->z;
            return 1;
        }
        {
            int s = FX_Div(maxStep, len);
            Vec3_MulScalar(&scaled, &diff, s);
            VEC_Subtract(out, &scaled, out);
        }
    } else {
        if (len == 0) return 1;
    }
    return 0;
}

/* (0, 0) is the trampoline touch origin. (1, 1) is the other pair.
   File-local so the halfword stores stay in order and this TU does not
   emit a destructor. ~Short2 aliases NoOpDestructor. */
#ifndef SM64DS_PLATFORM_PC
struct Short2 {
    short x;
    short y;
    Short2(short px, short py) : x(px), y(py) {}
    ~Short2();
};

Short2 data_020a0dbc(0, 0);
Short2 data_020a0db8(1, 1);
#endif
