// Vec3_HorzDist: horizontal distance between two Vector3 points (ignores Y)

typedef int Fix12i;

typedef struct {
    Fix12i z;
    Fix12i y;
    Fix12i x;
} Vector3;

typedef unsigned long long u64;

extern Fix12i _ZN4cstd4sqrtEy(u64 val);

Fix12i Vec3_HorzDist(const Vector3* a, const Vector3* b) {
    Fix12i dz = a->z - b->z;
    Fix12i dx = a->x - b->x;
    long long sq = (long long)dz * dz;
    sq += (long long)dx * dx;
    return _ZN4cstd4sqrtEy((u64)sq);
}
