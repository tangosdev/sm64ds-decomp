//cpp
// @symbol func_ov030_02111ea4
/* recovered: shared common types */
#include "common.h"
#include "dBgCh_Gnd.h"

extern "C" int _ZNK10dBgCh_Actr10IsOnGroundEv(void*);

#define ABS(x) ((x) < 0 ? -(x) : (x))

extern "C" int func_ov030_02111ea4(char* thiz)
{
    char* c = thiz;
    if (_ZNK10dBgCh_Actr10IsOnGroundEv(c + 0x194) != 0) {
        dBgCh_Gnd rg;
        Vector3 pos;
        {
            int y = *(int*)(c + 0x60);
            int z = *(int*)(c + 0x64);
            int x = *(int*)(c + 0x5c);
            int y2 = y + 0x1e000;
            pos.x = x;
            pos.y = y2;
            pos.z = z;
        }
        rg.SetObjAndPos(pos, (dActor_c*)c);
        if (rg.DetectClsn() == 0 ||
            ABS(rg.clsnY - *(int*)(c + 0x60)) > 0x1000) {
            *(int*)(c + 0x98) = 0;
            *(int*)(c + 0x5c) = *(int*)(c + 0x68);
            *(int*)(c + 0x60) = *(int*)(c + 0x6c);
            *(int*)(c + 0x64) = *(int*)(c + 0x70);
            return 1;
        }
    }
    return 0;
}
