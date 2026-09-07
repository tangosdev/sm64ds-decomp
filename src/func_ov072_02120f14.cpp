//cpp
// @symbol func_ov072_02120f14
/* recovered: shared common types */
#include "common.h"
#include "dBgCh_Gnd.h"

extern "C" int _ZNK10dBgCh_Actr10IsOnGroundEv(void *thiz);

extern "C" int func_ov072_02120f14(void *thiz)
{
    unsigned char *c = (unsigned char *)thiz;
    struct Vector3 v;

    if (_ZNK10dBgCh_Actr10IsOnGroundEv(c + 0x194)) {
        dBgCh_Gnd rg;
        {
            int z = *(int *)(c + 0x64);
            int y = *(int *)(c + 0x60) + 0x1e000;
            int x = *(int *)(c + 0x5c);
            v.x = x;
            v.y = y;
            v.z = z;
        }
        rg.SetObjAndPos(v, (dActor_c *)c);
        if (rg.DetectClsn() != 0) {
            int d = rg.clsnY - *(int *)(c + 0x60);
            if (d < 0) d = -d;
            if (d <= 0x1900) goto bail;
        }
        *(int *)(c + 0x5c) = *(int *)(c + 0x68);
        *(int *)(c + 0x60) = *(int *)(c + 0x6c);
        *(int *)(c + 0x64) = *(int *)(c + 0x70);
        return 1;
    bail:
        ;
    }
    return 0;
}
