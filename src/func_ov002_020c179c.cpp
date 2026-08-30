//cpp
// @symbol func_ov002_020c179c
/* recovered: shared common types */
#include "common.h"
#include "dBgCh_Gnd.h"
// func_ov002_020c179c at 0x020c179c
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).

extern "C" {
extern int _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);
extern short data_02082214[];

int func_ov002_020c179c(char* self, int angle) {
    Vector3 v1;
    Vector3 v2;
    int idx = (unsigned short)(short)(*(short*)(self + 0x94) + angle);
    int y1 = *(int*)(self + 0x60);
    int y2 = y1;
    idx >>= 4;
    short dx = data_02082214[idx * 2];
    short dy = data_02082214[idx * 2 + 1];
    int ox = dx * 5;
    int oy = dy * 5;
    dBgCh_Gnd rg;
    {
        int z = *(int*)(self + 0x64) + oy;
        int y = *(int*)(self + 0x60) + 0x64000;
        int x = *(int*)(self + 0x5c) + ox;
        v1.x = x;
        v1.y = y;
        v1.z = z;
    }
    rg.SetObjAndPos(v1, (dActor_c*)self);
    if (rg.DetectClsn()) y1 = rg.clsnY;
    {
        int z = *(int*)(self + 0x64) - oy;
        int x = *(int*)(self + 0x5c) - ox;
        int y = *(int*)(self + 0x60) + 0x64000;
        v2.x = x;
        v2.y = y;
        v2.z = z;
    }
    rg.SetObjAndPos(v2, (dActor_c*)self);
    if (rg.DetectClsn()) y2 = rg.clsnY;
    {
        int r = _ZN4cstd5atan2E5Fix12IiES1_(y1 - y2, 0xa000);
        return r;
    }
}
}
