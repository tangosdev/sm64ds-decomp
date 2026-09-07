//cpp
// @symbol func_ov100_02143370
/* recovered: shared common types */
#include "common.h"
#include "dBgCh_Lin.h"
extern "C" {

int func_ov100_02143370(char* c)
{
    Vector3 va;
    Vector3 vb;
    int ya, yb;
    dBgCh_Lin line1;
    dBgCh_Lin line2;
    va.x = 0; va.y = 0; va.z = 0;
    vb.x = 0; vb.y = 0; vb.z = 0;
    va.x = *(int*)(c + 0x5c);
    ya = *(int*)(c + 0x60);
    va.y = ya;
    va.z = *(int*)(c + 0x64);
    vb.x = *(int*)(c + 0x5c);
    yb = *(int*)(c + 0x60);
    vb.y = yb;
    vb.z = *(int*)(c + 0x64);
    va.y = ya + 0xa000;
    vb.y = yb - 0xb8000;
    line1.SetObjAndLine(va, vb, (dActor_c*)c);
    if (line1.DetectClsn()) {
        return 1;
    }
    return 0;
}
}
