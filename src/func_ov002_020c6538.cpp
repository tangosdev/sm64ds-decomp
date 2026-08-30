//cpp
#include "dBgCh_Gnd.h"

extern "C" {
int func_ov002_020c6538(char* c) {
    dBgCh_Gnd rg;
    Vector3 v;
    int y;
    v.x = *(int*)(c+0x5c);
    y = *(int*)(c+0x60);
    v.y = y;
    v.z = *(int*)(c+0x64);
    v.y = y + 0x32000;
    rg.SetObjAndPos(v, (dActor_c*)c);
    if (rg.DetectClsn() == 0) {
        return 0;
    }
    {
        int hit = rg.clsnY;
        int d = hit - *(int*)(c+0x60);
        if (d < 0) d = -d;
        if (d >= 0x64000) {
            return 0;
        }
        *(int*)(c+0x60) = hit;
        *(int*)(c+0x688) = *(int*)(c+0x60);
        return 1;
    }
}
}
