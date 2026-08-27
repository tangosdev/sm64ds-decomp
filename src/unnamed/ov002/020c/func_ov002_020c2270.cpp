//cpp
// @symbol func_ov002_020c2270
/* recovered: shared common types */
#include "common.h"
extern "C" {

extern void _ZN9dBgCh_LinC1Ev(void* self);
extern void _ZN9dBgCh_Lin13SetObjAndLineERK7Vector3S2_P8dActor_c(void* self, void* a, void* b, void* act);
extern int _ZN9dBgCh_Lin10DetectClsnEv(void* self);
extern int func_02037e58(unsigned int* p);
extern void _ZN9dBgCh_Lin10GetClsnPosEv(void* res, void* self);
extern void _ZN9dBgCh_LinD1Ev(void* self);

int func_ov002_020c2270(void* c, void* p1, void* p2) {
    char rl[0x78];
    Vector3 clsnPos;
    _ZN9dBgCh_LinC1Ev(rl);
    _ZN9dBgCh_Lin13SetObjAndLineERK7Vector3S2_P8dActor_c(rl, p1, p2, c);
    if (_ZN9dBgCh_Lin10DetectClsnEv(rl) != 0) {
        if (func_02037e58((unsigned int*)(rl + 0x14)) == 1) {
            _ZN9dBgCh_Lin10GetClsnPosEv(&clsnPos, rl);
            *(int*)((char*)c + 0x5c) = clsnPos.x;
            *(int*)((char*)c + 0x60) = clsnPos.y;
            *(int*)((char*)c + 0x64) = clsnPos.z;
            *((unsigned char*)c + 0x70f) = 1;
            _ZN9dBgCh_LinD1Ev(rl);
            return 1;
        }
    }
    _ZN9dBgCh_LinD1Ev(rl);
    return 0;
}
}
