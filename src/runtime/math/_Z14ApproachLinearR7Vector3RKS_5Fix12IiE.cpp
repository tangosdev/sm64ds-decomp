//cpp
typedef int Fix12;
struct Vector3 { int x, y, z; };

extern "C" void Vec3_Sub(Vector3* out, const Vector3* a, const Vector3* b);
extern "C" int LenVec3(const Vector3* v);
extern "C" int _ZN4cstd4fdivEii(int a, int b);
extern "C" void Vec3_MulScalar(Vector3* out, const Vector3* in, int scale);
extern "C" void SubVec3(Vector3* a, const Vector3* b, const Vector3* c);
extern "C" int Vec3_Equal(const Vector3* a, const Vector3* b);

extern "C" int _Z14ApproachLinearR7Vector3RKS_5Fix12IiE(Vector3& dst, const Vector3& target, Fix12 step)
{
    Vector3 diff;
    Vector3 scaled;
    if (step != 0) {
        Vec3_Sub(&diff, &dst, &target);
        int len = LenVec3(&diff);
        if (len == 0 || len <= step) {
            dst = target;
            return 1;
        }
        Vec3_MulScalar(&scaled, &diff, _ZN4cstd4fdivEii(step, len));
        SubVec3(&dst, &scaled, &dst);
    } else if (Vec3_Equal(&dst, &target)) {
        return 1;
    }
    return 0;
}
