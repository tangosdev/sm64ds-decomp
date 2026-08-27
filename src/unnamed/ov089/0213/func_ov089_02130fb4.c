extern unsigned int _ZN4cstd4sqrtEy(unsigned long long v);
typedef struct { int x, y, z; } Vec3;
extern int Vec3_HorzDist(const Vec3* a, const Vec3* b);
extern short Vec3_HorzAngle(const Vec3* a, const Vec3* b);

void func_ov089_02130fb4(char* c, int* p, int n) {
    int dy = p[1] - *(int*)(c + 0x60);
    if (dy < 0)
        dy = -dy;
    {
        int s0 = (int)_ZN4cstd4sqrtEy((unsigned long long)(long long)n);
        int s1 = (int)_ZN4cstd4sqrtEy((unsigned long long)(long long)(n + dy));
        int s2 = (int)_ZN4cstd4sqrtEy((unsigned long long)(long long)n);
        dy = (s0 * 0x1e) / (s1 + s2); /* reuse dy as r7 */
    }
    {
        int left = -(n << 1);
        int den = dy * dy;
        n = 0x1e - dy; /* reuse n as inv (target overwrites r4) */
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
    *(int*)(c + 0x98) = Vec3_HorzDist((const Vec3*)(c + 0x5c), (const Vec3*)p) / 30;
    *(short*)(c + 0x94) = Vec3_HorzAngle((const Vec3*)(c + 0x5c), (const Vec3*)p);
}
