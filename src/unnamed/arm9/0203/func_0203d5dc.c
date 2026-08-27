// func_0203d5dc: 2D (XY plane) distance between two Vector3 points

typedef int Fix12i;

typedef struct {
    Fix12i y;
    Fix12i x;
    Fix12i z;
} Vector3;

typedef unsigned long long u64;

extern Fix12i _ZN4cstd4sqrtEy(u64 val);

Fix12i func_0203d5dc(const Vector3* a, const Vector3* b) {
    Fix12i dy = a->y - b->y;
    Fix12i dx = a->x - b->x;
    long long sq = (long long)dy * dy;
    sq += (long long)dx * dx;
    return _ZN4cstd4sqrtEy((u64)sq);
}
