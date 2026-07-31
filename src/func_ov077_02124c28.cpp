//cpp
// @symbol func_ov077_02124c28
/* recovered: shared common types */
#include "common.h"

struct Actor;

struct RaycastGround {
    char pad0[0x14];
    int m14[12];
    int m44;
    char pad48[0xc];
};
extern "C" void _ZN13RaycastGroundC1Ev(RaycastGround*);
extern "C" void _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(RaycastGround*, const Vector3&, Actor*);
extern "C" void _ZN4BgCh19StartDetectingWaterEv(void*);
extern "C" int _ZN13RaycastGround10DetectClsnEv(RaycastGround*);
extern "C" int func_02037e78(int* p);
extern "C" void _ZN13RaycastGroundD1Ev(RaycastGround*);
extern "C" void _ZN4BgCh18StopDetectingWaterEv(void*);

extern "C" int func_ov077_02124c28(char* c)
{
    if (*(int*)(c + 0x3dc) == 0) {
        RaycastGround rg;
        Vector3 pos;
        int r;
        _ZN13RaycastGroundC1Ev(&rg);
        {
            int y = *(int*)(c + 0x60);
            int z = *(int*)(c + 0x64);
            int y2 = y + 0xc8000;
            int x = *(int*)(c + 0x5c);
            pos.x = x;
            pos.y = y2;
            pos.z = z;
        }
        _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(&rg, pos, (Actor*)c);
        _ZN4BgCh19StartDetectingWaterEv(&rg);
        if (_ZN13RaycastGround10DetectClsnEv(&rg) == 0) goto fail;
        r = func_02037e78(rg.m14);
        if (r != 0) {
            *(int*)(c + 0x3dc) = rg.m44;
        } else {
        fail:
            _ZN13RaycastGroundD1Ev(&rg);
            return 0;
        }
        _ZN4BgCh18StopDetectingWaterEv(&rg);
        _ZN13RaycastGroundD1Ev(&rg);
    }
    return *(int*)(c + 0x60) - *(int*)(c + 0x3dc);
}
