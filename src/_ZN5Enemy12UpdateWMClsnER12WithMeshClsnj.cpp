//cpp
// @symbol _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj
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
#include "Enemy.h"

struct SurfaceInfo { s32 pad; };

extern "C" {
extern void func_020383f0(WithMeshClsn *);
extern void WithMeshClsn_UpdateDiscreteNoLava_veneer(WithMeshClsn *);
extern void func_02038414(WithMeshClsn *);
extern void WithMeshClsn_UpdateContinuous_Veneer(WithMeshClsn *);
extern int _ZNK12WithMeshClsn10IsOnGroundEv(WithMeshClsn *);
extern struct SurfaceInfo *_ZNK12WithMeshClsn14GetFloorResultEv(WithMeshClsn *);
extern void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(struct SurfaceInfo *, Vector3 *);
extern int _ZNK12WithMeshClsn13GetLimMovFlagEv(WithMeshClsn *);
extern int _ZN4cstd4fdivEii(int, int);
extern int _ZNK12WithMeshClsn8IsOnWallEv(WithMeshClsn *);
extern struct SurfaceInfo *_ZNK12WithMeshClsn13GetWallResultEv(WithMeshClsn *);
}

void Enemy::UpdateWMClsn(WithMeshClsn & clsn_, unsigned int sel)
{
    WithMeshClsn *clsn = &clsn_;

    switch (sel) {
    case 1: func_020383f0(clsn); break;
    case 2: WithMeshClsn_UpdateDiscreteNoLava_veneer(clsn); break;
    case 3: func_02038414(clsn); break;
    default: WithMeshClsn_UpdateContinuous_Veneer(clsn); break;
    }
    if (_ZNK12WithMeshClsn10IsOnGroundEv(clsn)) {

        _ZNK11SurfaceInfo12CopyNormalToER7Vector3((struct SurfaceInfo*)((char*)_ZNK12WithMeshClsn14GetFloorResultEv(clsn)+4), (Vector3*)&mFloorNormalX);
        if (_ZNK12WithMeshClsn13GetLimMovFlagEv(clsn) == 0) {
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
                unk_0a8 = -(q + 0x8000);
            }
        }
    }
    if (_ZNK12WithMeshClsn8IsOnWallEv(clsn)) {
        _ZNK11SurfaceInfo12CopyNormalToER7Vector3((struct SurfaceInfo*)((char*)_ZNK12WithMeshClsn13GetWallResultEv(clsn)+4), (Vector3*)&mWallNormalX);
    }

}
