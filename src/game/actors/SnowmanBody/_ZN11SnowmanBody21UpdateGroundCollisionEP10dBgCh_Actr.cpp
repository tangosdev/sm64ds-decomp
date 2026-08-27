//cpp
#include "SnowmanBody.h"
#include "common.h"

typedef long long s64;

extern "C" void dBgCh_Actr_UpdateContinuous_Veneer(void *p);
extern "C" int _ZNK10dBgCh_Actr10IsOnGroundEv(void *self);
extern "C" char *_ZNK10dBgCh_Actr14GetFloorResultEv(void *self);
extern "C" void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(
    void *self, Vector3 *v);
extern "C" int _ZN4cstd4fdivEii(int a, int b);

void SnowmanBody::UpdateGroundCollision(dBgCh_Actr *mc)
{
    char *c = (char *)this;
    Vector3 n;
    char *fr;
    dBgCh_Actr_UpdateContinuous_Veneer(mc);
    if (_ZNK10dBgCh_Actr10IsOnGroundEv(mc) == 0) return;
    fr = _ZNK10dBgCh_Actr14GetFloorResultEv(mc);
    _ZNK11SurfaceInfo12CopyNormalToER7Vector3(fr + 4, &n);
    if (n.y == 0) return;
    {
        int a = (int)(((s64)n.x * *(int *)(c + 0xa4) + 0x800) >> 12);
        int b = (int)(((s64)n.z * *(int *)(c + 0xac) + 0x800) >> 12);
        *(int *)(c + 0xa8) = -(_ZN4cstd4fdivEii(a + b, n.y) + 0x8000);
    }
}
