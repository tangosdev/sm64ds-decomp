//cpp
// @symbol func_ov002_020c647c
/* recovered: shared common types */
#include "common.h"
#include "dBgCh_Lin.h"
// func_ov002_020c647c at 0x020c647c
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).
extern "C" {

extern void _ZN9dBgCh_Lin10GetClsnPosEv(Vector3* res, dBgCh_Lin* self);

int func_ov002_020c647c(char* c, int arg1) {
    Vector3 v1;
    Vector3 v2;
    Vector3 clsnPos;
    dBgCh_Lin line;
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
    line.SetObjAndLine(v1, v2, (dActor_c*)c);
    if (line.DetectClsn()) {
        _ZN9dBgCh_Lin10GetClsnPosEv(&clsnPos, &line);
        int d = (clsnPos.y - *(int*)(c + 0x60)) / 0x1000;
        if (d > 0xa0) d = 0xa0;
        if (d < 0) d = 0;
        r5 = d;
    }
    return r5;
}
}
