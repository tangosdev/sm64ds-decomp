//cpp
#include "dBgCh_Gnd.h"

extern "C" {
extern int _ZNK10dBgCh_Actr10IsOnGroundEv(void* thiz);
extern int func_02038ea4(void* thiz);

int func_ov030_02111dd0(char* c)
{
    if (_ZNK10dBgCh_Actr10IsOnGroundEv(c + 0x194) != 0) {
        dBgCh_Gnd rg;
        Vector3 v;
        int y, z, x, s;
        y = *(int*)(c + 0x60);
        z = *(int*)(c + 0x64);
        x = *(int*)(c + 0x5c);
        s = y + 0x1e000;
        v.x = x;
        v.y = s;
        v.z = z;
        rg.SetObjAndPos(v, (dActor_c*)c);
        if (func_02038ea4(&rg) == 0 || *(int*)(c + 0x60) - rg.clsnY > 0x2000) {
            *(int*)(c + 0x5c) = *(int*)(c + 0x38c);
            *(int*)(c + 0x60) = *(int*)(c + 0x390);
            *(int*)(c + 0x64) = *(int*)(c + 0x394);
            return 1;
        }
        *(int*)(c + 0x38c) = *(int*)(c + 0x5c);
        *(int*)(c + 0x390) = *(int*)(c + 0x60);
        *(int*)(c + 0x394) = *(int*)(c + 0x64);
    }
    return 0;
}
}
