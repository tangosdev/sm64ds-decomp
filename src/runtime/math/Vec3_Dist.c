// Vec3_Dist: 3D distance between two Vector3 points

typedef int Fix12i;

typedef struct {
    Fix12i y;
    Fix12i x;
    Fix12i z;
} Vector3;

typedef unsigned long long u64;

extern Fix12i _ZN4cstd4sqrtEy(u64 val);

Fix12i Vec3_Dist(const Vector3* a, const Vector3* b) {
    Fix12i dy = a->y - b->y;
    Fix12i ax = a->x;
    long long sq = (long long)dy * dy;
    Fix12i bx = b->x;
    Fix12i az = a->z;
    ax -= bx;
    Fix12i bz = b->z;
    sq += (long long)ax * ax;
    ax = az - bz;
    sq += (long long)ax * ax;
    return _ZN4cstd4sqrtEy((u64)sq);
}
