//cpp
// func_ov002_020d1164 at 0x020d1164
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).
extern "C" {
extern void _ZN11RaycastLineC1Ev(void* self);
extern void _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P8dActor_c(void* self, void* a, void* b, void* act);
extern int _ZN11RaycastLine10DetectClsnEv(void* self);
extern void _ZN11RaycastLineD1Ev(void* self);
int func_ov002_020d1164(char* c) {
    char rl[0x78];
    int v1[3];
    int v2[3];
    int y;
    _ZN11RaycastLineC1Ev(rl);
    v1[0] = *(int*)(c+0x5c);
    v1[1] = *(int*)(c+0x60);
    v1[2] = *(int*)(c+0x64);
    v2[0] = *(int*)(c+0x5c);
    y = *(int*)(c+0x60);
    v2[1] = y;
    v2[2] = *(int*)(c+0x64);
    v2[1] = y + 0x96000;
    _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P8dActor_c(rl, v1, v2, c);
    if (_ZN11RaycastLine10DetectClsnEv(rl) != 0) {
        _ZN11RaycastLineD1Ev(rl);
        return 1;
    }
    _ZN11RaycastLineD1Ev(rl);
    return 0;
}
}
