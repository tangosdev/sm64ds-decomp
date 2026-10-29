//cpp
/* Move a point toward another, in 3D and on the horizontal plane.
 *
 * leftovers:
 * - Vec3, Vector3 and Vec3_Fix12 are the same three ints. Vec3_Sub,
 *   LenVec3 and SubVec3 take Vec3; Vec3_HorzLen and Vec3_Equal take
 *   Vector3; Vec3_MulScalar takes Vec3_Fix12. One struct makes those
 *   calls disagree with the declarations they already have.
 * - Vector3 stays a local POD twin rather than include/types.h's: that one
 *   declares a ctor and dtor under C++, which turns `dst = target` below from
 *   a block move into memberwise stores and loses the match.
 */
#include "math/Approach.h"

struct Vec3 { int x, y, z; };
struct Vector3 { int x, y, z; };
typedef struct { int x, y, z; } Vec3_Fix12;

namespace cstd {
int fdiv(int, int);
}

extern "C" {
extern void Vec3_Sub(struct Vec3* out, struct Vec3* a, struct Vec3* b);
extern int Vec3_HorzLen(const struct Vector3* v);
extern int LenVec3(struct Vec3* v);
extern void Vec3_MulScalar(Vec3_Fix12* out, const Vec3_Fix12* in, int scale);
extern void SubVec3(struct Vec3* a, struct Vec3* b, struct Vec3* c);
extern int Vec3_Equal(const struct Vector3 *a, const struct Vector3 *b);
}

// @symbol _Z14ApproachLinearR7Vector3RKS_i
int ApproachLinear(Vector3 &dst, const Vector3 &target, int step) {
    struct Vector3 diff;
    struct Vector3 scaled;
    if (step != 0) {
        Vec3_Sub((struct Vec3*)&diff, (struct Vec3*)&dst, (struct Vec3*)&target);
        int len = LenVec3((struct Vec3*)&diff);
        if (len == 0 || len <= step) {
            dst = target;
            return 1;
        }
        Vec3_MulScalar((Vec3_Fix12 *)&scaled, (const Vec3_Fix12 *)&diff, cstd::fdiv(step, len));
        SubVec3((struct Vec3*)&dst, (struct Vec3*)&scaled, (struct Vec3*)&dst);
    } else if (Vec3_Equal(&dst, &target)) {
        return 1;
    }
    return 0;
}

// @symbol Vec3_ApproachHorz
extern "C" int Vec3_ApproachHorz(struct Vec3 *out, struct Vec3 *a, int maxStep) {
    struct Vec3 diff;
    struct Vec3 scaled;
    int len;
    Vec3_Sub(&diff, out, a);
    diff.y = 0;
    len = Vec3_HorzLen((const struct Vector3*)&diff);
    if (maxStep != 0) {
        if (len == 0 || len <= maxStep) {
            out->x = a->x;
            out->z = a->z;
            return 1;
        }
        {
            int s = cstd::fdiv(maxStep, len);
            Vec3_MulScalar((Vec3_Fix12*)&scaled, (const Vec3_Fix12*)&diff, s);
            SubVec3(out, &scaled, out);
        }
    } else {
        if (len == 0) return 1;
    }
    return 0;
}
