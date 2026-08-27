typedef long long s64;
typedef unsigned long long u64;
typedef struct { int x, y, z; } Vec3;
extern int _ZN4cstd4sqrtEy(u64 v);
extern int Vec3_HorzDist(const Vec3* a, const Vec3* b);
extern short Vec3_HorzAngle(const Vec3* a, const Vec3* b);

void func_ov002_020e947c(char* c, int* p, int n) {
    int dy = p[1] - *(int*)(c + 0x60);
    if (dy < 0)
        dy = -dy;
    {
        int s0 = _ZN4cstd4sqrtEy((u64)(s64)n);
        int s1 = _ZN4cstd4sqrtEy((u64)(s64)(n + dy));
        int s2 = _ZN4cstd4sqrtEy((u64)(s64)n);
        dy = (s0 * 0x32) / (s1 + s2);
    }
    {
        int left = -(n << 1);
        int den = dy * dy;
        n = 0x32 - dy;
        *(int*)(c + 0x9c) = left / den;
    }
    if (p[1] >= *(int*)(c + 0x60)) {
        int v = *(int*)(c + 0x9c);
        if (v < 0)
            v = -v;
        *(int*)(c + 0xa8) = n * v;
    } else {
        int v = *(int*)(c + 0x9c);
        if (v < 0)
            v = -v;
        *(int*)(c + 0xa8) = (dy + 1) * v;
    }
    *(int*)(c + 0xa0) = -0x32000;
    *(int*)(c + 0x98) = Vec3_HorzDist((const Vec3*)(c + 0x5c), (const Vec3*)p) / 50;
    *(short*)(c + 0x94) = Vec3_HorzAngle((const Vec3*)(c + 0x5c), (const Vec3*)p);
}
