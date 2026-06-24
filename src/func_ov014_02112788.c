typedef int Fix12;
typedef short s16;
extern void Matrix4x3_FromRotationXYZExt(void *m, int x, int y, int z);
extern void _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void* self, void* sm, void* mtx, Fix12 a, Fix12 b, unsigned int g);

struct M48 { int w[12]; };
extern const struct M48 data_02082128;

void func_ov014_02112788(char* c) {
    struct M48 tmp;
    int i;
    int t;
    char* m;
    char* e;
    char* o;
    char* sm;
    Matrix4x3_FromRotationXYZExt(c+0x16c, *(s16*)(c+0x8c), *(s16*)(c+0x8e), *(s16*)(c+0x90));
    *(int*)(c+0x190) = *(int*)(c+0x5c) >> 3;
    *(int*)(c+0x194) = *(int*)(c+0x60) >> 3;
    *(int*)(c+0x198) = *(int*)(c+0x64) >> 3;
    t = *(int*)(c+0x60) - *(int*)(c+0x5f0);
    if (t <= 0x1000) t = 0x1000;
    _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        c, c+0x1b4, c+0x16c,
        0x15e000 - (int)(((long long)t * 0x180 + 0x800) >> 12),
        t + 0x28000,
        0xf);
    tmp = data_02082128;
    sm = c + 0x40c;
    i = 0;
    m = c + 0x1dc;
    e = c;
    o = c;
    for (; i < 7; i++) {
        *(struct M48*)(m+0x1c) = tmp;
        *(int*)(o+0x21c) = *(int*)(e+0x524) >> 3;
        *(int*)(o+0x220) = *(int*)(e+0x528) >> 3;
        *(int*)(o+0x224) = *(int*)(e+0x52c) >> 3;
        t = *(int*)(e+0x528) - *(int*)(c+0x5f0);
        if (t <= 0x1000) t = 0x1000;
        _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
            c, sm, m+0x1c,
            0x78000 - (int)(((long long)t * 0x180 + 0x800) >> 12),
            t + 0x28000,
            0xf);
        m += 0x50;
        e += 0xc;
        o += 0x50;
        sm += 0x28;
    }
}
