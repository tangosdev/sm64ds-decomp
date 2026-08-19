//cpp
// @symbol func_ov002_020b5ab4
/* recovered: shared common types */
#include "common.h"
typedef int s32;
typedef signed char s8;

struct dActor_c;

struct dBgCh_Gnd {
    char pad0[0x14];
    int field14;     /* 0x14 */
    char pad18[0x44 - 0x18];
    int field44;     /* 0x44 */
    char pad48[0x54 - 0x48];
    dBgCh_Gnd();
    ~dBgCh_Gnd();
    void StartDetectingWater();
    void SetObjAndPos(const Vector3& pos, dActor_c* a);
    int DetectClsn();
};
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" void _ZN5dBgCh19StartDetectingWaterEv(void *);


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
        _ZN5dBgCh19StartDetectingWaterEv(&(rg));
        rg.SetObjAndPos(vec, (dActor_c*)c);
        if (rg.DetectClsn() != 0) {
            *(int*)(c+0x324) = rg.field44;
            if (SurfaceInfo_TestFlag0x20(&rg.field14) != 0) {
                return 1;
            }
        }
    }
    return 0;
}
