//cpp
// func_ov002_020d1164 at 0x020d1164
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).
#include "dBgCh_Lin.h"
extern "C" {
int func_ov002_020d1164(char* c) {
    dBgCh_Lin line;
    Vector3 v1;
    Vector3 v2;
    int y;
    v1.x = *(int*)(c+0x5c);
    v1.y = *(int*)(c+0x60);
    v1.z = *(int*)(c+0x64);
    v2.x = *(int*)(c+0x5c);
    y = *(int*)(c+0x60);
    v2.y = y;
    v2.z = *(int*)(c+0x64);
    v2.y = y + 0x96000;
    line.SetObjAndLine(v1, v2, (dActor_c*)c);
    if (line.DetectClsn()) {
        return 1;
    }
    return 0;
}
}
