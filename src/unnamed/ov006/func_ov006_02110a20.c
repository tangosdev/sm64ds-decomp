//cpp
typedef struct { int x; int y; } V2;
typedef struct { V2 d; V2 v; V2 e; } W;

extern "C" void func_ov006_0211470c(int *a, int *b);
extern "C" void func_ov006_02115598(void *c, int *src, int v2, int v3, int v5);
extern "C" void func_ov006_02114fd0(char *p);
namespace Sound { void PlayBank2_2D(unsigned int); }

inline unsigned char *GetObj(char *g, int i)
{
    return i >= 13 ? 0 : *(unsigned char **)(g + 0x4688 + i * 4);
}

#pragma opt_strength_reduction off
extern "C" void func_ov006_02110a20(int *c)
{
    int i;
    W s;

    c[4] = c[2];
    c[5] = c[3];
    for (i = 0; i < *(int *)((char *)c[1] + 0x4668); i++) {
        if (GetObj((char *)c[1], i)[0x30] == 0)
            continue;
        if (GetObj((char *)c[1], i)[0x120] == 1)
            continue;
        func_ov006_0211470c((int *)&s.v, (int *)GetObj((char *)c[1], i));
        s.d = s.v;
        s.d.x = s.d.x - c[2];
        s.d.y = s.d.y - c[3];
        if (s.d.x < -0x8000)
            continue;
        if (s.d.x >= 0x8000)
            continue;
        if (s.d.y < -0x40000)
            continue;
        if (s.d.y >= -0x38000)
            continue;
        s.e.x = c[2];
        s.e.y = -0x40000;
        func_ov006_02115598((void *)c[1], (int *)&s.e, 0xbb8, 0, 1);
        func_ov006_02114fd0((char *)c[1]);
        GetObj((char *)c[1], i)[0x120] = 1;
        Sound::PlayBank2_2D(0x16);
    }
}
