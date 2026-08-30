//cpp
// @symbol func_ov002_020c72a4
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
#include "dBgCh_Gnd.h"

struct dActor_c;
extern "C" int SurfaceInfo_TestFlag0x20(int* p);
extern int data_0209f32c;

extern "C" void func_ov002_020c72a4(void* thisptr)
{
    unsigned char* r4 = (unsigned char*)thisptr;
    dBgCh_Gnd rg;
    Vector3 v;
    int z = *(int*)(r4 + 0x64);
    int d = data_0209212c;
    int x = *(int*)(r4 + 0x5c);
    v.x = x;
    v.y = d;
    v.z = z;
    rg.mProbeHeight = d << 1;
    rg.SetObjAndPos(v, (dActor_c*)thisptr);
    rg.StartDetectingWater();
    if (rg.DetectClsn()) {
        if (SurfaceInfo_TestFlag0x20((int*)&rg.surface) != 0) {
            *(int*)(r4 + 0x64c) = rg.clsnY;
            data_0209f32c = *(int*)(r4 + 0x64c);
        }
    }
    rg.mProbeHeight = 0x1f4000;
    func_ov002_020c71e0(thisptr);
}
