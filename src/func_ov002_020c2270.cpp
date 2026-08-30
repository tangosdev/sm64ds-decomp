//cpp
// @symbol func_ov002_020c2270
/* recovered: shared common types */
#include "common.h"
#include "dBgCh_Lin.h"
extern "C" {

extern int func_02037e58(unsigned int* p);
extern void _ZN9dBgCh_Lin10GetClsnPosEv(Vector3* res, dBgCh_Lin* self);

int func_ov002_020c2270(void* c, void* p1, void* p2) {
    dBgCh_Lin line;
    Vector3 clsnPos;
    line.SetObjAndLine(*(Vector3*)p1, *(Vector3*)p2, (dActor_c*)c);
    if (line.DetectClsn()) {
        if (func_02037e58((unsigned int*)&line.surface) == 1) {
            _ZN9dBgCh_Lin10GetClsnPosEv(&clsnPos, &line);
            *(int*)((char*)c + 0x5c) = clsnPos.x;
            *(int*)((char*)c + 0x60) = clsnPos.y;
            *(int*)((char*)c + 0x64) = clsnPos.z;
            *((unsigned char*)c + 0x70f) = 1;
            return 1;
        }
    }
    return 0;
}
}
