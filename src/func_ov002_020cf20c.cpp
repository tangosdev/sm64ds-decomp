//cpp
// func_ov002_020cf20c at 0x020cf20c
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).
extern "C" {
struct Vector3 { int x, y, z; };
extern void _ZN11RaycastLineC1Ev(void* self);
extern void _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(void* self, void* a, void* b, void* act);
extern int _ZN11RaycastLine10DetectClsnEv(void* self);
extern void _ZN11RaycastLine10GetClsnPosEv(void* res, void* self);
extern int func_02037e38(unsigned int* p);
extern int func_02037e48(unsigned int* p);
extern void func_0200ca14(void* a, unsigned char b, int c);
extern void _ZN11RaycastLineD1Ev(void* self);
extern void* CAMERA[];

int func_ov002_020cf20c(char* c) {
    Vector3 v1;
    Vector3 v2;
    Vector3 clsnPos;
    char rl[0x78];
    _ZN11RaycastLineC1Ev(rl);
    v1.x = *(int*)(c + 0x5c);
    int y1 = *(int*)(c + 0x60);
    v1.y = y1;
    v1.z = *(int*)(c + 0x64);
    v2.x = *(int*)(c + 0x5c);
    int y2 = *(int*)(c + 0x60);
    v2.y = y2;
    v2.z = *(int*)(c + 0x64);
    v1.y = y1 + 0x64000;
    v2.y = y2 + 0xb4000;
    _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(rl, &v1, &v2, c);
    if (_ZN11RaycastLine10DetectClsnEv(rl) != 0) {
        if (func_02037e38((unsigned int*)(rl + 0x14)) == 3) {
            _ZN11RaycastLine10GetClsnPosEv(&clsnPos, rl);
            void* cam = CAMERA[0];
            int t = func_02037e48((unsigned int*)(rl + 0x14));
            func_0200ca14(cam, *(unsigned char*)(c + 0x6d8), t);
            int ret = clsnPos.y;
            _ZN11RaycastLineD1Ev(rl);
            return ret;
        }
    }
    _ZN11RaycastLineD1Ev(rl);
    return (int)0x80000000;
}
}
