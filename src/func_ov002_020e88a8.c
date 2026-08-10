// @symbol func_ov002_020e88a8
/* recovered: shared common types, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
struct Flags { unsigned short b0 : 1, b1 : 1, b2 : 1, b3 : 1, fld : 2; };


extern void func_02012694(int a, void* p);
extern char* _ZNK12WithMeshClsn14GetFloorResultEv(void* c);
extern int func_02037e38(void* p);
extern int func_02037e58(void* p);
extern void func_ov002_020e947c(void* self, struct Vector3* p, int d);
extern void func_ov002_020e9448(void* self);
extern char* _ZN5Actor13ClosestPlayerEv(void* self);
extern int Vec3_Dist(struct Vector3* a, struct Vector3* b);
extern short Vec3_HorzAngle(struct Vector3* a, struct Vector3* b);

extern signed char data_0209f2f8;

void func_ov002_020e88a8(char* self) {
    struct Vector3 v;
    struct Vector3 w;
    char* p;
    int r;

    if (((struct Flags*)(self + 0x4a2))->b3) {
        ((struct Flags*)((int)(self + 0x4a2)))->b3 = 0;
        *(unsigned short*)(self + 0x100) = 0;
    }
    func_02012694(0x55, self + 0x74);
    if (((struct Flags*)(self + 0x4a2))->fld == 2) {
        *(int*)(self + 0x98) = 0xc000;
        *(int*)(self + 0xa8) = 0xe000;
    } else {
        *(int*)(self + 0x98) = 0xc000;
        *(int*)(self + 0xa8) = 0x17000;
    }
    r = func_02037e38(_ZNK12WithMeshClsn14GetFloorResultEv(self + 0x150) + 4);
    if (r == 1 || r == 9) {
        w.x = *(int*)(self + 0x448);
        w.y = *(int*)(self + 0x44c);
        w.z = *(int*)(self + 0x450);
        func_ov002_020e947c(self, &w, 0xc8000);
        return;
    }
    if (r == 4 || r == 5) {
        func_ov002_020e8abc(self);
        return;
    }
    func_ov002_020e9448(self);
    *(int*)(self + 0x448) = *(int*)(self + 0x5c);
    *(int*)(self + 0x44c) = *(int*)(self + 0x60);
    *(int*)(self + 0x450) = *(int*)(self + 0x64);
    p = _ZN5Actor13ClosestPlayerEv(self);
    if (p == 0) return;
    {
        int* s = (int*)((int)(p + 0x5c));
        v.x = s[0];
        v.y = s[1];
        v.z = s[2];
    }
    if (Vec3_Dist((struct Vector3*)(self + 0x5c), &v) < 0x4b0000) {
        *(short*)(self + 0x94) = Vec3_HorzAngle(&v, (struct Vector3*)(self + 0x5c));
        if (*(int*)(self + 0xd8) >= *(short*)(data_02082714 + 0x56)) {
            if (data_0209f2f8 != 0x1d) return;
            if (func_02037e58(_ZNK12WithMeshClsn14GetFloorResultEv(self + 0x150) + 4) != 5) return;
        }
        {
            short* a = (short*)((int)(((long long)(int)(self + 0x94)) | 0LL));
            *a = *a + 0x8000;
        }
    } else {
        char* q = _ZN5Actor14FarthestPlayerEv(self);
        if (q != 0) {
            q = q + 0x5c;
            *(short*)(self + 0x94) = Vec3_HorzAngle((struct Vector3*)(self + 0x5c), (struct Vector3*)q);
        }
    }
}
