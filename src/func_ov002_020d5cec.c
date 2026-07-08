//cpp
struct Vector3 { int x, y, z; };
extern "C" {
extern int _ZN6Player7IsStateERNS_5StateE(void* c, void* s);
extern void _ZN6Player7SetAnimEji5Fix12IiEj(void* c, unsigned int anim, int a, int b, unsigned int d);
extern void _ZN11RaycastLineC1Ev(void* self);
extern void _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(void* self, void* a, void* b, void* act);
extern int _ZN11RaycastLine10DetectClsnEv(void* self);
extern void _ZN11RaycastLineD1Ev(void* self);
extern int data_ov002_02110064[];
extern short data_02082214[];
}

extern "C" int func_ov002_020d5cec(char* c)
{
    Vector3 v1;
    Vector3 v2;
    char rl[0x78];
    char* d0;
    short* m;
    char* p;
    int y1;
    int shifted;

    if (_ZN6Player7IsStateERNS_5StateE(c, data_ov002_02110064) == 0) return 0;
    _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x56, 0x40000000, 0x1000, 0);

    d0 = *(char**)(c + 0xd0);
    m = (short*)(int)(((long long)(int)(d0 + 0x8c)) & 0xFFFFFFFFFFFFFFFFLL);
    *(short*)(c + 0x92) = m[0];
    *(short*)(c + 0x94) = m[1];
    *(short*)(c + 0x96) = m[2];
    *(short*)(c + 0x8e) = *(short*)(c + 0x94);
    *(int*)(c + 0x98) = 0x18000;
    *(int*)(c + 0xa8) = 0x18000;

    p = (char*)(int)(((long long)(int)(*(char**)(c + 0xd0) + 0x5c)) & 0xFFFFFFFFFFFFFFFFLL);
    v1.x = *(int*)p;
    y1 = *(int*)(p + 4);
    v1.y = y1;
    v1.z = *(int*)(p + 8);
    v1.y = y1 + 0x32000;

    shifted = *(unsigned short*)(c + 0x8e) >> 4;
    v2.x = v1.x + (int)(((long long)data_02082214[shifted * 2] * 0x50000 + 0x800) >> 12);
    v2.y = v1.y;
    v2.z = v1.z + (int)(((long long)data_02082214[shifted * 2 + 1] * 0x50000 + 0x800) >> 12);

    _ZN11RaycastLineC1Ev(rl);
    _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(rl, &v1, &v2, c);
    if (_ZN11RaycastLine10DetectClsnEv(rl) != 0) {
        *(int*)(c + 0x5c) = v1.x;
        *(int*)(c + 0x60) = v1.y;
        *(int*)(c + 0x64) = v1.z;
        *(int*)(c + 0x98) = 0x8000;
    } else {
        *(int*)(c + 0x5c) = v2.x;
        *(int*)(c + 0x60) = v2.y;
        *(int*)(c + 0x64) = v2.z;
    }
    *(unsigned char*)(c + 0x6f5) = 0x1f;
    *(int*)(int)(((long long)(int)(c + 0x2ec)) & 0xFFFFFFFFFFFFFFFFLL) &= ~2;
    *(unsigned char*)(c + 0x6e3) = 2;
    *(unsigned short*)(c + 0x6a4) = 4;
    *(unsigned char*)(c + 0x708) = 1;
    *(int*)(c + 0xd0) = 0;
    _ZN11RaycastLineD1Ev(rl);
    return 1;
}
