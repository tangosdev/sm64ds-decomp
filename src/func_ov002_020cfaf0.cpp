//cpp
#include "dBgCh_Lin.h"
extern short data_02082214[];
extern "C" {

int func_ov002_020cfaf0(char* c) {
    dBgCh_Lin line;
    Vector3 v1;
    Vector3 v2;
    {
        int ang = *(short*)(c + 0x8e);
        int j = 2 * (((unsigned short)(short)(ang + 0x8000)) >> 4);
        int s = data_02082214[j] * 32;
        int t = data_02082214[j + 1] * 32;
        int ax = *(int*)(c + 0x5c) + s;
        int az = *(int*)(c + 0x64) + t;
        int ay = *(int*)(c + 0x60) - 0x20000;
        v1.x = ax;
        v1.y = ay;
        v1.z = az;
        int bx = *(int*)(c + 0x5c) + s;
        int bz = *(int*)(c + 0x64) + t;
        int by = *(int*)(c + 0x60) + 0x20000;
        v2.x = bx;
        v2.y = by;
        v2.z = bz;
    }
    line.SetObjAndLine(v1, v2, (dActor_c*)c);
    if (line.DetectClsn()) {
        return 1;
    }
    return 0;
}
}
