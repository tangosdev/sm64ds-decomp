// @symbol func_ov002_020e86ec
/* recovered: shared common types, declarations from a shared header */
#include "decl_WithMeshClsn.h"
/* recovered: shared common types */
#include "common.h"
struct Flags { unsigned short b0 : 1, b1 : 1, b2 : 1, b3 : 1, fld : 2; };


extern int _ZNK12WithMeshClsn12TouchesWaterEv(void* c);
extern void _ZN12WithMeshClsn19StartDetectingWaterEv(void* c);
extern void _ZN13RaycastGroundC1Ev(void* r);
extern void _ZN13RaycastGroundD1Ev(void* r);
extern void _ZN4BgCh19StartDetectingWaterEv(void* r);
extern void _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(void* r, struct Vector3* p, void* a);
extern int _ZN13RaycastGround10DetectClsnEv(void* r);
extern int SurfaceInfo_TestFlag0x20(void* p);
extern void func_ov002_020e9448(void* p);

extern int data_0209f32c;

void func_ov002_020e86ec(char* self) {
    struct Vector3 v;
    char rc[0x54];
    int tx, ty, tz, ta;

    if (((struct Flags*)(self + 0x4a2))->fld < 2) {
        if (((struct Flags*)(self + 0x4a2))->fld != 1) {
            if (_ZNK12WithMeshClsn12TouchesWaterEv(self + 0x150) == 0) return;
        }
        if (((struct Flags*)(self + 0x4a2))->fld == 0) {
            ((struct Flags*)((int)(((long long)(int)(self + 0x4a2)) & 0xFFFFFFFFFFFFFFFFLL)))->fld = 1;
            *(int*)(self + 0x60) = *(int*)(self + 0x6c);
            _ZN12WithMeshClsn15ClearGroundFlagEv(self + 0x150);
            _ZN12WithMeshClsn22ClearJustHitGroundFlagEv(self + 0x150);
            _ZN12WithMeshClsn18StopDetectingWaterEv(self + 0x150);
        }
        _ZN13RaycastGroundC1Ev(rc);
        _ZN4BgCh19StartDetectingWaterEv(rc);
        ty = *(int*)(self + 0x60);
        tz = *(int*)(self + 0x64);
        tx = *(int*)(self + 0x5c);
        ta = ty + 0xa0000;
        v.x = tx;
        v.y = ta;
        v.z = tz;
        _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(rc, &v, self);
        if (_ZN13RaycastGround10DetectClsnEv(rc) != 0) {
            if (SurfaceInfo_TestFlag0x20(rc + 0x14) != 0) {
                *(int*)(self + 0x488) = *(int*)(rc + 0x44);
                data_0209f32c = *(int*)(self + 0x488);
                if (*(int*)(self + 0x488) >= *(int*)(self + 0x60) + 0x3c000) {
                    ((struct Flags*)((int)(((unsigned long long)(unsigned)(self + 0x4a2)) & 0xFFFFFFFFFFFFFFFFULL)))->fld = 2;
                    *(int*)(self + 0x9c) = -0x700;
                    *(int*)(self + 0xa0) = -0x10000;
                    *(int*)(self + 0x98) = 0xc000;
                    *(int*)(self + 0xa8) = 0;
                }
            }
        }
        _ZN13RaycastGroundD1Ev(rc);
    } else {
        if (data_0209f32c < *(int*)(self + 0x60) + 0x3c000) {
            ((struct Flags*)((int)(((long long)(int)(self + 0x4a2)) | 0LL)))->fld = 0;
            func_ov002_020e9448(self);
            _ZN12WithMeshClsn19StartDetectingWaterEv(self + 0x150);
        }
    }
}
