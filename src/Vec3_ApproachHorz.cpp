//cpp
/* Move a point toward another, in 3D and on the horizontal plane.
 *
 * leftovers:
 * - Vec3, Vector3, V3 and Vec3_Fix12 are the same three ints. Vec3_Sub,
 *   LenVec3 and SubVec3 take Vec3; Vec3_HorzLen takes Vector3; Vec3_Equal
 *   takes V3; Vec3_MulScalar takes Vec3_Fix12. One struct makes those
 *   calls disagree with the declarations they already have.
 * - The divide stays _ZN4cstd4fdivEii. decl_common.h spells cstd_fdiv, which
 *   is a different linker symbol.
 */
struct Vec3 { int x, y, z; };
struct Vector3 { int x, y, z; };
struct V3 { int x, y, z; };
typedef struct { int x, y, z; } Vec3_Fix12;

extern "C" {
extern void Vec3_Sub(struct Vec3* out, struct Vec3* a, struct Vec3* b);
extern int Vec3_HorzLen(const struct Vector3* v);
extern int LenVec3(struct Vec3* v);
extern int _ZN4cstd4fdivEii(int a, int b);
extern void Vec3_MulScalar(Vec3_Fix12* out, const Vec3_Fix12* in, int scale);
extern void SubVec3(struct Vec3* a, struct Vec3* b, struct Vec3* c);
extern int Vec3_Equal(struct V3* a, struct V3* b);

int _Z14ApproachLinearR7Vector3RKS_5Fix12IiE(struct Vector3& dst, const struct Vector3& target, int step)
{
    struct Vector3 diff;
    struct Vector3 scaled;
    if (step != 0) {
        Vec3_Sub((struct Vec3*)&diff, (struct Vec3*)&dst, (struct Vec3*)&target);
        int len = LenVec3((struct Vec3*)&diff);
        if (len == 0 || len <= step) {
            dst = target;
            return 1;
        }
        Vec3_MulScalar((Vec3_Fix12*)&scaled, (const Vec3_Fix12*)&diff, _ZN4cstd4fdivEii(step, len));
        SubVec3((struct Vec3*)&dst, (struct Vec3*)&scaled, (struct Vec3*)&dst);
    } else if (Vec3_Equal((struct V3*)&dst, (struct V3*)&target)) {
        return 1;
    }
    return 0;
}

int Vec3_ApproachHorz(struct Vec3* out, struct Vec3* a, int maxStep)
{
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
            int s = _ZN4cstd4fdivEii(maxStep, len);
            Vec3_MulScalar((Vec3_Fix12*)&scaled, (const Vec3_Fix12*)&diff, s);
            SubVec3(out, &scaled, out);
        }
    } else {
        if (len == 0) return 1;
    }
    return 0;
}
}
