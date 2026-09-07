//cpp
// @symbol func_ov071_0211f148
/* recovered: shared common types, declarations from a shared header */
#include "decl_dBgCh_Actr.h"
/* recovered: shared common types */
#include "common.h"
#include "dBgCh_Gnd.h"
typedef int Fix12;
typedef short s16;

struct dBgCh_Actr;
struct dBgPi;
struct SurfaceInfo;

extern "C" void dBgCh_Actr_UpdateDiscreteNoLava_veneer(void* p);
extern "C" int _ZNK10dBgCh_Actr10IsOnGroundEv(void* self);
extern "C" void* _ZNK10dBgCh_Actr14GetFloorResultEv(void* self);
extern "C" void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(void* self, Vector3* out);
extern "C" int _ZN4cstd4fdivEii(int a, int b);
extern "C" int _ZNK10dBgCh_Actr8IsOnWallEv(void* self);

extern "C" void func_ov071_0211f148(char* a, char* w) {
    Vector3 pos;
    Vector3 normal;
    Vector3 wallnormal;

    dBgCh_Actr_UpdateDiscreteNoLava_veneer(w);
    if (_ZNK10dBgCh_Actr10IsOnGroundEv(w)) {
        dBgCh_Gnd rc;
        {
            int p60 = *(int*)(a+0x60);
            int pz = *(int*)(a+0x64);
            int py = p60 + 0x1e000;
            pos.x = *(int*)(a+0x5c);
            pos.y = py;
            pos.z = pz;
        }
        rc.SetObjAndPos(pos, (dActor_c*)a);
        if (!rc.DetectClsn() || rc.clsnY < *(int*)(a+0x60) - 0x32000) {
            *(int*)(a+0x98) = 0;
            *(int*)(a+0x5c) = *(int*)(a+0x68);
            *(int*)(a+0x60) = *(int*)(a+0x6c);
            *(int*)(a+0x64) = *(int*)(a+0x70);
        } else {
            void* fr = _ZNK10dBgCh_Actr14GetFloorResultEv(w);
            _ZNK11SurfaceInfo12CopyNormalToER7Vector3((char*)fr + 4, &normal);
            if (normal.y != 0) {
                *(int*)(a+0xa8) = -(_ZN4cstd4fdivEii(
                    (int)(((long long)normal.x * *(int*)(a+0xa4) + 0x800) >> 12)
                  + (int)(((long long)normal.z * *(int*)(a+0xac) + 0x800) >> 12),
                    normal.y) + 0x8000);
            }
        }
    }
    if (_ZNK10dBgCh_Actr8IsOnWallEv(w)) {
        void* wr = _ZNK10dBgCh_Actr13GetWallResultEv(w);
        _ZNK11SurfaceInfo12CopyNormalToER7Vector3((char*)wr + 4, &wallnormal);
    }
}
