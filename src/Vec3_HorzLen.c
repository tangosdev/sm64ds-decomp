typedef int Fix12;
struct Vector3 { Fix12 x, y, z; };
extern Fix12 _ZN4cstd4sqrtEy(unsigned long long x);
Fix12 Vec3_HorzLen(const struct Vector3 *v) {
    return _ZN4cstd4sqrtEy((unsigned long long)v->x * v->x + (unsigned long long)v->z * v->z);
}
