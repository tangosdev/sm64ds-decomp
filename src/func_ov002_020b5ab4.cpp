//cpp
// @symbol func_ov002_020b5ab4
/* recovered: shared common types */
#include "common.h"
#include "dBgCh_Gnd.h"

struct dActor_c;

extern "C" int SurfaceInfo_TestFlag0x20(int* p);
extern "C" s8 data_0209f2f8;
extern "C" int data_0209f32c;

extern "C" int func_ov002_020b5ab4(char* c);
int func_ov002_020b5ab4(char* c)
{
    if (data_0209f2f8 == 0x15) {
        int x = *(int*)(c+0x344);
        int y;
        *(int*)(c+0x324) = x;
        y = data_0209f32c;
        if (y > x) {
            *(int*)(c+0x324) = y;
            return 1;
        }
        return 0;
    }
    {
        Vector3 vec;
        dBgCh_Gnd rg;
        {
            int vx = *(int*)(c+0x5c);
            int vz = *(int*)(c+0x64);
            int vy = *(int*)(c+0x60) + 0x64000;
            vec.x = vx;
            vec.y = vy;
            vec.z = vz;
        }
        rg.StartDetectingWater();
        rg.SetObjAndPos(vec, (dActor_c*)c);
        if (rg.DetectClsn() != 0) {
            *(int*)(c+0x324) = rg.clsnY;
            if (SurfaceInfo_TestFlag0x20((int*)&rg.surface) != 0) {
                return 1;
            }
        }
    }
    return 0;
}
