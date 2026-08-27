//cpp
// @symbol func_ov030_02111ea4
/* recovered: shared common types */
#include "common.h"

struct dActor_c;

struct dBgCh_Gnd {
    char pad[0x54];
};
extern "C" int _ZNK10dBgCh_Actr10IsOnGroundEv(void*);
extern "C" void _ZN9dBgCh_GndC1Ev(dBgCh_Gnd*);
extern "C" void _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(dBgCh_Gnd*, const Vector3&, dActor_c*);
extern "C" int _ZN9dBgCh_Gnd10DetectClsnEv(dBgCh_Gnd*);
extern "C" void _ZN9dBgCh_GndD1Ev(dBgCh_Gnd*);

#define ABS(x) ((x) < 0 ? -(x) : (x))

extern "C" int func_ov030_02111ea4(char* thiz)
{
    char* c = thiz;
    if (_ZNK10dBgCh_Actr10IsOnGroundEv(c + 0x194) != 0) {
        dBgCh_Gnd rg;
        Vector3 pos;
        _ZN9dBgCh_GndC1Ev(&rg);
        {
            int y = *(int*)(c + 0x60);
            int z = *(int*)(c + 0x64);
            int x = *(int*)(c + 0x5c);
            int y2 = y + 0x1e000;
            pos.x = x;
            pos.y = y2;
            pos.z = z;
        }
        _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(&rg, pos, (dActor_c*)c);
        if (_ZN9dBgCh_Gnd10DetectClsnEv(&rg) == 0 ||
            ABS(*(int*)((char*)&rg + 0x44) - *(int*)(c + 0x60)) > 0x1000) {
            *(int*)(c + 0x98) = 0;
            *(int*)(c + 0x5c) = *(int*)(c + 0x68);
            *(int*)(c + 0x60) = *(int*)(c + 0x6c);
            *(int*)(c + 0x64) = *(int*)(c + 0x70);
            _ZN9dBgCh_GndD1Ev(&rg);
            return 1;
        }
        _ZN9dBgCh_GndD1Ev(&rg);
    }
    return 0;
}
