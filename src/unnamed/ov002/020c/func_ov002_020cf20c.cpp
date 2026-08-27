//cpp
// @symbol func_ov002_020cf20c
/* recovered: shared common types */
#include "common.h"
// func_ov002_020cf20c at 0x020cf20c
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).
extern "C" {

extern void _ZN9dBgCh_LinC1Ev(void* self);
extern void _ZN9dBgCh_Lin13SetObjAndLineERK7Vector3S2_P8dActor_c(void* self, void* a, void* b, void* act);
extern int _ZN9dBgCh_Lin10DetectClsnEv(void* self);
extern void _ZN9dBgCh_Lin10GetClsnPosEv(void* res, void* self);
extern int func_02037e38(unsigned int* p);
extern int func_02037e48(unsigned int* p);
extern void func_0200ca14(void* a, unsigned char b, int c);
extern void _ZN9dBgCh_LinD1Ev(void* self);
extern void* data_0209f318[];

int func_ov002_020cf20c(char* c) {
    Vector3 v1;
    Vector3 v2;
    Vector3 clsnPos;
    char rl[0x78];
    _ZN9dBgCh_LinC1Ev(rl);
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
    _ZN9dBgCh_Lin13SetObjAndLineERK7Vector3S2_P8dActor_c(rl, &v1, &v2, c);
    if (_ZN9dBgCh_Lin10DetectClsnEv(rl) != 0) {
        if (func_02037e38((unsigned int*)(rl + 0x14)) == 3) {
            _ZN9dBgCh_Lin10GetClsnPosEv(&clsnPos, rl);
            void* cam = data_0209f318[0];
            int t = func_02037e48((unsigned int*)(rl + 0x14));
            func_0200ca14(cam, *(unsigned char*)(c + 0x6d8), t);
            int ret = clsnPos.y;
            _ZN9dBgCh_LinD1Ev(rl);
            return ret;
        }
    }
    _ZN9dBgCh_LinD1Ev(rl);
    return (int)0x80000000;
}
}
