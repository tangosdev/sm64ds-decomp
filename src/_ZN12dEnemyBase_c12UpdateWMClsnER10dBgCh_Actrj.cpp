//cpp
// @symbol _ZN12dEnemyBase_c12UpdateWMClsnER10dBgCh_Actrj
/* recovered: named members + shared header, real C++ method
 *
 * Runs the mesh collision (sel picks which of four update flavours), then
 * caches whichever surface normals came back: the floor result's into
 * mFloorNormal*, the wall result's into mWallNormal*. Those two
 * CopyNormalTo calls are what evidence both triples as Vector3s.
 *
 * On a floor that does not limit movement, the vertical velocity is
 * re-derived so the motion stays in the floor plane -- the dot product of the
 * horizontal normal and velocity components, divided by the normal's Y.
 */
#include "dEnemyBase_c.h"

#include "SurfaceInfo.h"

extern "C" {
extern void func_020383f0(dBgCh_Actr *);
extern void dBgCh_Actr_UpdateDiscreteNoLava_veneer(dBgCh_Actr *);
extern void func_02038414(dBgCh_Actr *);
extern void dBgCh_Actr_UpdateContinuous_Veneer(dBgCh_Actr *);
extern int _ZNK10dBgCh_Actr10IsOnGroundEv(dBgCh_Actr *);
extern struct SurfaceInfo *_ZNK10dBgCh_Actr14GetFloorResultEv(dBgCh_Actr *);
extern void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(struct SurfaceInfo *, Vector3 *);
extern int _ZNK10dBgCh_Actr13GetLimMovFlagEv(dBgCh_Actr *);
extern int _ZN4cstd4fdivEii(int, int);
extern int _ZNK10dBgCh_Actr8IsOnWallEv(dBgCh_Actr *);
extern struct SurfaceInfo *_ZNK10dBgCh_Actr13GetWallResultEv(dBgCh_Actr *);
}

void dEnemyBase_c::UpdateWMClsn(dBgCh_Actr & clsn_, unsigned int sel)
{
    dBgCh_Actr *clsn = &clsn_;

    switch (sel) {
    case 1: func_020383f0(clsn); break;
    case 2: dBgCh_Actr_UpdateDiscreteNoLava_veneer(clsn); break;
    case 3: func_02038414(clsn); break;
    default: dBgCh_Actr_UpdateContinuous_Veneer(clsn); break;
    }
    if (_ZNK10dBgCh_Actr10IsOnGroundEv(clsn)) {

        _ZNK11SurfaceInfo12CopyNormalToER7Vector3((struct SurfaceInfo*)((char*)_ZNK10dBgCh_Actr14GetFloorResultEv(clsn)+4), (Vector3*)&mFloorNormalX);
        if (_ZNK10dBgCh_Actr13GetLimMovFlagEv(clsn) == 0) {
            int dz = mFloorNormalY;
            if (dz != 0) {
                int nx = mFloorNormalX;
                int vx = unk_0a4;
                int nz = mFloorNormalZ;
                int vz = unk_0ac;
                long long a = (long long)nx * vx + 0x800;
                long long b = (long long)nz * vz + 0x800;
                int num = (int)(a >> 12) + (int)(b >> 12);
                int q = _ZN4cstd4fdivEii(num, dz);
                mVertSpeed = -(q + 0x8000);
            }
        }
    }
    if (_ZNK10dBgCh_Actr8IsOnWallEv(clsn)) {
        _ZNK11SurfaceInfo12CopyNormalToER7Vector3((struct SurfaceInfo*)((char*)_ZNK10dBgCh_Actr13GetWallResultEv(clsn)+4), (Vector3*)&mWallNormalX);
    }

}
