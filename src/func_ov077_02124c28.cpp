//cpp
// @symbol func_ov077_02124c28
/* recovered: shared common types */
#include "common.h"
#include "dBgCh_Gnd.h"

extern "C" int SurfaceInfo_TestFlag0x20(int* p);

extern "C" int func_ov077_02124c28(char* c)
{
    if (*(int*)(c + 0x3dc) == 0) {
        dBgCh_Gnd rg;
        Vector3 pos;
        int r;
        {
            int y = *(int*)(c + 0x60);
            int z = *(int*)(c + 0x64);
            int y2 = y + 0xc8000;
            int x = *(int*)(c + 0x5c);
            pos.x = x;
            pos.y = y2;
            pos.z = z;
        }
        rg.SetObjAndPos(pos, (dActor_c*)c);
        rg.StartDetectingWater();
        if (rg.DetectClsn() == 0) goto fail;
        r = SurfaceInfo_TestFlag0x20((int*)&rg.surface);
        if (r != 0) {
            *(int*)(c + 0x3dc) = rg.clsnY;
        } else {
        fail:
            return 0;
        }
        rg.StopDetectingWater();
    }
    return *(int*)(c + 0x60) - *(int*)(c + 0x3dc);
}
