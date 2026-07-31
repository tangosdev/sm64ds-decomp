//cpp
// @symbol func_ov071_0211f148
/* recovered: shared common types, declarations from a shared header */
#include "decl_WithMeshClsn.h"
/* recovered: shared common types */
#include "common.h"
typedef int Fix12;
typedef short s16;

struct WithMeshClsn;
struct Actor;
struct RaycastGround { char buf0[0x14]; int floor[12]; char buf1[0x50-0x14-0x30]; };
struct ClsnResult;
struct SurfaceInfo;

extern "C" void func_02038420(void* p);
extern "C" int _ZNK12WithMeshClsn10IsOnGroundEv(void* self);
extern "C" void _ZN13RaycastGroundC1Ev(RaycastGround* self);
extern "C" void _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(RaycastGround* self, const Vector3& v, void* actor);
extern "C" int _ZN13RaycastGround10DetectClsnEv(RaycastGround* self);
extern "C" void _ZN13RaycastGroundD1Ev(RaycastGround* self);
extern "C" void* _ZNK12WithMeshClsn14GetFloorResultEv(void* self);
extern "C" void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(void* self, Vector3* out);
extern "C" int _ZN4cstd4fdivEii(int a, int b);
extern "C" int _ZNK12WithMeshClsn8IsOnWallEv(void* self);

extern "C" void func_ov071_0211f148(char* a, char* w) {
    RaycastGround rc;
    Vector3 pos;
    Vector3 normal;
    Vector3 wallnormal;

    func_02038420(w);
    if (_ZNK12WithMeshClsn10IsOnGroundEv(w)) {
        _ZN13RaycastGroundC1Ev(&rc);
        {
            int p60 = *(int*)(a+0x60);
            int pz = *(int*)(a+0x64);
            int py = p60 + 0x1e000;
            pos.x = *(int*)(a+0x5c);
            pos.y = py;
            pos.z = pz;
        }
        _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(&rc, pos, a);
        if (!_ZN13RaycastGround10DetectClsnEv(&rc) || rc.floor[(0x44-0x14)/4] < *(int*)(a+0x60) - 0x32000) {
            *(int*)(a+0x98) = 0;
            *(int*)(a+0x5c) = *(int*)(a+0x68);
            *(int*)(a+0x60) = *(int*)(a+0x6c);
            *(int*)(a+0x64) = *(int*)(a+0x70);
        } else {
            void* fr = _ZNK12WithMeshClsn14GetFloorResultEv(w);
            _ZNK11SurfaceInfo12CopyNormalToER7Vector3((char*)fr + 4, &normal);
            if (normal.y != 0) {
                *(int*)(a+0xa8) = -(_ZN4cstd4fdivEii(
                    (int)(((long long)normal.x * *(int*)(a+0xa4) + 0x800) >> 12)
                  + (int)(((long long)normal.z * *(int*)(a+0xac) + 0x800) >> 12),
                    normal.y) + 0x8000);
            }
        }
        _ZN13RaycastGroundD1Ev(&rc);
    }
    if (_ZNK12WithMeshClsn8IsOnWallEv(w)) {
        void* wr = _ZNK12WithMeshClsn13GetWallResultEv(w);
        _ZNK11SurfaceInfo12CopyNormalToER7Vector3((char*)wr + 4, &wallnormal);
    }
}
