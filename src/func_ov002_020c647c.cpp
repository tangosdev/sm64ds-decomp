//cpp
// func_ov002_020c647c at 0x020c647c
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).
extern "C" {
struct Vector3 { int x, y, z; };
extern void _ZN11RaycastLineC1Ev(void* self);
extern void _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(void* self, void* a, void* b, void* act);
extern int _ZN11RaycastLine10DetectClsnEv(void* self);
extern void _ZN11RaycastLine10GetClsnPosEv(void* res, void* self);
extern void _ZN11RaycastLineD1Ev(void* self);

int func_ov002_020c647c(char* c, int arg1) {
    Vector3 v1;
    Vector3 v2;
    Vector3 clsnPos;
    char rl[0x78];
    _ZN11RaycastLineC1Ev(rl);
    int z1 = *(int*)(c + 0x64);
    int x1 = *(int*)(c + 0x5c);
    v1.x = x1;
    v1.y = arg1 - 0x1e000;
    v1.z = z1;
    int x2 = *(int*)(c + 0x5c);
    int z2 = *(int*)(c + 0x64);
    v2.x = x2;
    v2.z = z2;
    v2.y = arg1 + 0xaa000;
    int r5 = -64;
    _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(rl, &v1, &v2, c);
    if (_ZN11RaycastLine10DetectClsnEv(rl) != 0) {
        _ZN11RaycastLine10GetClsnPosEv(&clsnPos, rl);
        int d = (clsnPos.y - *(int*)(c + 0x60)) / 0x1000;
        if (d > 0xa0) d = 0xa0;
        if (d < 0) d = 0;
        r5 = d;
    }
    _ZN11RaycastLineD1Ev(rl);
    return r5;
}
}
