//cpp
// @symbol _ZN14KnockDownPlank13InitResourcesEv
#include "types.h"
#include "RaycastGround.h"

extern "C" {
void* _ZN5Model8LoadFileER13SharedFilePtr(void* fp);
void _ZN9ModelBase7SetFileEP8BMD_Fileii(void* self, void* f, int a, int b);
void _ZN11ShadowModel10InitCuboidEv(void* self);
void func_ov015_0211166c(char* t);
void _ZN8Platform19UpdateClsnPosAndRotEv(void* self);
void* _ZN12MeshCollider8LoadFileER13SharedFilePtr(void* fp);
void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void* self, void* f, void* m, int fx, short s, void* b);
void Matrix4x3_FromRotationY(void* m, int angle);
void MulVec3Mat4x3(Vector3* v, void* m, Vector3* out);
void Vec3_Add(Vector3* out, Vector3* a, Vector3* b);
void _ZN13RaycastGroundC1Ev(RaycastGround* self);
void _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(RaycastGround* self, Vector3* v, void* a);
int _ZN13RaycastGround10DetectClsnEv(RaycastGround* self);
int IsStarCollectedInCurLevel(int starID);
void _ZN13RaycastGroundD1Ev(RaycastGround* self);
}

extern int data_ov015_0211497c;
extern int data_ov015_02114974;
extern int data_ov015_02113574;
extern int data_020a0e68;
extern signed char data_0209f2f8;
extern unsigned char data_0209f220;

#pragma opt_propagation off
extern "C" int _ZN14KnockDownPlank13InitResourcesEv(char* self)
{
    RaycastGround rg;
    Vector3 a, b, c, d;
    int zero, one;

    _ZN9ModelBase7SetFileEP8BMD_Fileii(
        self + 0xd4,
        _ZN5Model8LoadFileER13SharedFilePtr(&data_ov015_0211497c),
        1, -1);
    _ZN11ShadowModel10InitCuboidEv(self + 0x320);
    func_ov015_0211166c(self);
    _ZN8Platform19UpdateClsnPosAndRotEv(self);
    _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        self + 0x124,
        _ZN12MeshCollider8LoadFileER13SharedFilePtr(&data_ov015_02114974),
        self + 0x2ec,
        0x1000,
        *(short*)(self + 0x8e),
        &data_ov015_02113574);

    *(short*)((long long)(int)(self + 0x8e) & 0xFFFFFFFFFFFFFFFFLL) += 0x8000;
    zero = 0;
    a.z = -0xfa000; a.x = zero; a.y = zero; b.x = zero; b.y = zero; b.z = zero;

    Matrix4x3_FromRotationY(&data_020a0e68, *(short*)(self + 0x8e));
    MulVec3Mat4x3(&a, &data_020a0e68, &b);
    Vec3_Add(&c, &b, (Vector3*)(self + 0x5c));
    c.y += 0x14000;
    _ZN13RaycastGroundC1Ev(&rg);
    _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(&rg, &c, 0);
    *(int*)(self + 0x384) = c.y;
    if (_ZN13RaycastGround10DetectClsnEv(&rg) != 0)
        *(int*)(self + 0x384) = rg.clsnY;

    zero = 0;
    a.z = 0x32000; a.x = zero; a.y = zero; b.x = zero; b.y = zero; b.z = zero;

    Matrix4x3_FromRotationY(&data_020a0e68, *(short*)(self + 0x8e));
    MulVec3Mat4x3(&a, &data_020a0e68, &b);
    Vec3_Add(&d, &b, (Vector3*)(self + 0x5c));

    one = 1;
    *(int*)(self + 0x378) = d.x;
    *(int*)(self + 0x37c) = d.y;
    *(int*)(self + 0x380) = d.z;
    *(int*)(self + 0x388) = *(int*)(self + 0x60);
    *(unsigned char*)(self + 0x396) = (unsigned char)one;

    if (data_0209f2f8 == 7 && (data_0209f220 == 1 || IsStarCollectedInCurLevel(one) == 0)
        && *(int*)(self + 0x60) >= 0xdac000)
    {
        _ZN13RaycastGroundD1Ev(&rg);
        return 0;
    }
    _ZN13RaycastGroundD1Ev(&rg);
    return 1;
}
