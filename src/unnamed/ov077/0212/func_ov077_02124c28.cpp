//cpp
// @symbol func_ov077_02124c28
/* recovered: shared common types */
#include "common.h"

struct dActor_c;

struct dBgCh_Gnd {
    char pad0[0x14];
    int m14[12];
    int m44;
    char pad48[0xc];
};
extern "C" void _ZN9dBgCh_GndC1Ev(dBgCh_Gnd*);
extern "C" void _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(dBgCh_Gnd*, const Vector3&, dActor_c*);
extern "C" void _ZN5dBgCh19StartDetectingWaterEv(void*);
extern "C" int _ZN9dBgCh_Gnd10DetectClsnEv(dBgCh_Gnd*);
extern "C" int SurfaceInfo_TestFlag0x20(int* p);
extern "C" void _ZN9dBgCh_GndD1Ev(dBgCh_Gnd*);
extern "C" void _ZN5dBgCh18StopDetectingWaterEv(void*);

extern "C" int func_ov077_02124c28(char* c)
{
    if (*(int*)(c + 0x3dc) == 0) {
        dBgCh_Gnd rg;
        Vector3 pos;
        int r;
        _ZN9dBgCh_GndC1Ev(&rg);
        {
            int y = *(int*)(c + 0x60);
            int z = *(int*)(c + 0x64);
            int y2 = y + 0xc8000;
            int x = *(int*)(c + 0x5c);
            pos.x = x;
            pos.y = y2;
            pos.z = z;
        }
        _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(&rg, pos, (dActor_c*)c);
        _ZN5dBgCh19StartDetectingWaterEv(&rg);
        if (_ZN9dBgCh_Gnd10DetectClsnEv(&rg) == 0) goto fail;
        r = SurfaceInfo_TestFlag0x20(rg.m14);
        if (r != 0) {
            *(int*)(c + 0x3dc) = rg.m44;
        } else {
        fail:
            _ZN9dBgCh_GndD1Ev(&rg);
            return 0;
        }
        _ZN5dBgCh18StopDetectingWaterEv(&rg);
        _ZN9dBgCh_GndD1Ev(&rg);
    }
    return *(int*)(c + 0x60) - *(int*)(c + 0x3dc);
}
