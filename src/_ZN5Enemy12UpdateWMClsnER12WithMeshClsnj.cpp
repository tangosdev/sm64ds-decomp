//cpp
extern "C" {
typedef int s32;
typedef unsigned int u32;

struct WithMeshClsn;
struct CylinderClsn;
struct Vector3 { s32 x, y, z; };
struct SurfaceInfo { s32 pad; };

extern void func_020383f0(struct WithMeshClsn *);
extern void func_02038420(struct WithMeshClsn *);
extern void func_02038414(struct WithMeshClsn *);
extern void func_020383fc(struct WithMeshClsn *);
extern int _ZNK12WithMeshClsn10IsOnGroundEv(struct WithMeshClsn *);
extern struct SurfaceInfo *_ZNK12WithMeshClsn14GetFloorResultEv(struct WithMeshClsn *);
extern void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(struct SurfaceInfo *, struct Vector3 *);
extern int _ZNK12WithMeshClsn13GetLimMovFlagEv(struct WithMeshClsn *);
extern int _ZN4cstd4fdivEii(int, int);
extern int _ZNK12WithMeshClsn8IsOnWallEv(struct WithMeshClsn *);
extern struct SurfaceInfo *_ZNK12WithMeshClsn13GetWallResultEv(struct WithMeshClsn *);

struct Enemy { char pad[0x200]; };

void _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(struct Enemy *thiz, struct WithMeshClsn *clsn, u32 sel)
{

    switch (sel) {
    case 1: func_020383f0(clsn); break;
    case 2: func_02038420(clsn); break;
    case 3: func_02038414(clsn); break;
    default: func_020383fc(clsn); break;
    }
    if (_ZNK12WithMeshClsn10IsOnGroundEv(clsn)) {

        _ZNK11SurfaceInfo12CopyNormalToER7Vector3((struct SurfaceInfo*)((char*)_ZNK12WithMeshClsn14GetFloorResultEv(clsn)+4), (struct Vector3*)((char*)thiz+0xd4));
        if (_ZNK12WithMeshClsn13GetLimMovFlagEv(clsn) == 0) {
            int dz = *(int*)((char*)thiz+0xd8);
            if (dz != 0) {
                int nx = *(int*)((char*)thiz+0xd4);
                int vx = *(int*)((char*)thiz+0xa4);
                int nz = *(int*)((char*)thiz+0xdc);
                int vz = *(int*)((char*)thiz+0xac);
                long long a = (long long)nx * vx + 0x800;
                long long b = (long long)nz * vz + 0x800;
                int num = (int)(a >> 12) + (int)(b >> 12);
                int q = _ZN4cstd4fdivEii(num, dz);
                *(int*)((char*)thiz+0xa8) = -(q + 0x8000);
            }
        }
    }
    if (_ZNK12WithMeshClsn8IsOnWallEv(clsn)) {
        _ZNK11SurfaceInfo12CopyNormalToER7Vector3((struct SurfaceInfo*)((char*)_ZNK12WithMeshClsn13GetWallResultEv(clsn)+4), (struct Vector3*)((char*)thiz+0xe0));
    }

}
}
