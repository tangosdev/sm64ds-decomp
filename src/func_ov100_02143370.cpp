//cpp
extern "C" {
struct Vector3 { int x, y, z; };
extern void _ZN11RaycastLineC1Ev(void* self);
extern void _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(void* self, void* a, void* b, void* act);
extern int _ZN11RaycastLine10DetectClsnEv(void* self);
extern void _ZN11RaycastLineD1Ev(void* self);

int func_ov100_02143370(char* c)
{
    char rl1[0x78];
    char rl2[0x78];
    Vector3 va;
    Vector3 vb;
    int ya, yb;
    _ZN11RaycastLineC1Ev(rl1);
    _ZN11RaycastLineC1Ev(rl2);
    va.x = 0; va.y = 0; va.z = 0;
    vb.x = 0; vb.y = 0; vb.z = 0;
    va.x = *(int*)(c + 0x5c);
    ya = *(int*)(c + 0x60);
    va.y = ya;
    va.z = *(int*)(c + 0x64);
    vb.x = *(int*)(c + 0x5c);
    yb = *(int*)(c + 0x60);
    vb.y = yb;
    vb.z = *(int*)(c + 0x64);
    va.y = ya + 0xa000;
    vb.y = yb - 0xb8000;
    _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(rl1, &va, &vb, c);
    if (_ZN11RaycastLine10DetectClsnEv(rl1) != 0) {
        _ZN11RaycastLineD1Ev(rl2);
        _ZN11RaycastLineD1Ev(rl1);
        return 1;
    }
    _ZN11RaycastLineD1Ev(rl2);
    _ZN11RaycastLineD1Ev(rl1);
    return 0;
}
}
