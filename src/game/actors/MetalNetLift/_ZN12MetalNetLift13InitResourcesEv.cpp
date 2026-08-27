//cpp
// @symbol _ZN12MetalNetLift13InitResourcesEv
#include "MetalNetLift.h"
// recovered name: daObjFl_Amilift_c_InitResources
/* recovered: shared common types, renamed to Class_Method, declarations from a shared header */
#include "decl_PathPtr.h"
#include "decl_common.h"
/* recovered: shared common types, renamed to Class_Method */
/* daObjFl_Amilift_c::InitResources - recovered from vtable slot identity */
struct SharedFilePtr;
struct BMD_File;
struct KCL_File;
struct Matrix4x3;
struct CLPS_Block;
struct Vector3;

extern "C" BMD_File *_ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr &f);
extern "C" void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, BMD_File *f, int a, int b);
extern "C" void _ZN10dBgActor_c21UpdateModelPosAndRotYEv(void *self);
extern "C" void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void *self);
extern "C" KCL_File *_ZN7dBgW_Kc8LoadFileER13SharedFilePtr(SharedFilePtr &f);
extern "C" void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *self, KCL_File *k, Matrix4x3 *m, int fix, short s, CLPS_Block *clps);
extern "C" void func_020393d4(void *p, void *v);
extern "C" void func_020393c4(void *p, void *v);
extern "C" void _ZN7PathPtr6FromIDEj(void *self, unsigned int id);
extern "C" void _ZNK7PathPtr7GetNodeER7Vector3j(void *self, Vector3 *v, unsigned int idx);
extern "C" void func_ov064_02117a14(char *self, Vector3 *a, Vector3 *b);

extern SharedFilePtr data_ov064_0211c730;
extern SharedFilePtr data_ov064_0211c728;
extern CLPS_Block data_ov064_0211bb6c;
extern "C" {
extern void _ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_();
}

struct V3 { int x, y, z; };

s32 MetalNetLift::InitResources() {
    char * self = (char *)this;
    BMD_File *bmd;
    KCL_File *kcl;
    V3 nodeA;
    V3 nodeB;
    unsigned char b;

    bmd = _ZN5Model8LoadFileER13SharedFilePtr(data_ov064_0211c730);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(self + 0xd4, bmd, 1, -1);

    b = (*(unsigned int *)(self + 8) >> 8) & 1;
    *(unsigned char *)(self + 0x33c) = b;
    b = *(unsigned char *)(self + 0x33c);
    if (b == 0)
        *(unsigned char *)(self + 0x33b) = 0;
    else
        *(unsigned char *)(self + 0x33b) = 1;

    _ZN10dBgActor_c21UpdateModelPosAndRotYEv(self);
    _ZN10dBgActor_c19UpdateClsnPosAndRotEv(self);

    kcl = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(data_ov064_0211c728);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        self + 0x124, kcl, (Matrix4x3 *)(self + 0x2ec), 0x199, *(short *)(self + 0x8e), &data_ov064_0211bb6c);

    func_020393d4(self + 0x124, (void *)&_ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);
    func_020393c4(self + 0x124, (void *)&func_ov064_02117fd4);

    _ZN7PathPtr6FromIDEj(self + 0x360, *(int *)(self + 8) & 0xf);
    *(int *)(self + 0x340) = _ZNK7PathPtr8NumNodesEv(self + 0x360);
    *(int *)(self + 0x344) = 1;

    *(int *)(self + 0x348) = *(int *)(self + 0x5c);
    *(int *)(self + 0x34c) = *(int *)(self + 0x60);
    *(int *)(self + 0x350) = *(int *)(self + 0x64);

    _ZNK7PathPtr7GetNodeER7Vector3j(self + 0x360, (Vector3 *)(self + 0x354), *(int *)(self + 0x344));

    nodeA.x = *(int *)(self + 0x354);
    nodeA.y = *(int *)(self + 0x358);
    nodeA.z = *(int *)(self + 0x35c);
    nodeB.x = *(int *)(self + 0x348);
    nodeB.y = *(int *)(self + 0x34c);
    nodeB.z = *(int *)(self + 0x350);

    func_ov064_02117a14(self, (Vector3 *)&nodeA, (Vector3 *)&nodeB);

    *(int *)(self + 0x32c) = *(int *)(self + 0x5c);
    *(int *)(self + 0x330) = *(int *)(self + 0x60);
    *(int *)(self + 0x334) = *(int *)(self + 0x64);

    return 1;
}
