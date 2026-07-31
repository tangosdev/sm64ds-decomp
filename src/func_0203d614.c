// func_0203d614: length of the XY components of a vector

typedef int Fix12i;

typedef struct {
    Fix12i x;
    Fix12i y;
    Fix12i z;
} Vector3;

typedef unsigned long long u64;

extern Fix12i _ZN4cstd4sqrtEy(u64 val);

Fix12i func_0203d614(const Vector3* v) {
    long long sq = (long long)v->x * v->x;
    sq += (long long)v->y * v->y;
    return _ZN4cstd4sqrtEy((u64)sq);
}
