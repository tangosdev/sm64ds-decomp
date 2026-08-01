//cpp
// @symbol _ZN22RotatingUpDownPlatform13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_PathPtr.h"
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "RotatingUpDownPlatform.h"
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void* sfp);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void* self, void* file, int a, int b);
extern void* _ZN12MeshCollider8LoadFileER13SharedFilePtr(void* sfp);
extern void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void* self, void* kcl, void* mtx, int fix, short s, void* clps);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(void* c);
extern void func_020393d4(void* p, void* v);
extern void func_020393c4(void* p, void* v);
extern void _ZN7PathPtr6FromIDEj(void* self, unsigned int id);
extern void _ZNK7PathPtr7GetNodeER7Vector3j(void* self, void* out, unsigned int idx);

extern int data_ov091_021344fc[];
extern int data_ov091_021344f4[];
extern char _ZN16MeshColliderBase21UpdatePosWithVelocityERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_;

int RotatingUpDownPlatform::InitResources()
{
    unsigned char idx;

    mVariant = (unsigned char)(*(unsigned*)((char*)&mParam) >> 8);
    idx = mVariant;
    _ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this) + 0xd4, _ZN5Model8LoadFileER13SharedFilePtr((void*)data_ov091_021344fc[idx]), 1, -1);

    idx = mVariant;
    _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        ((char*)this) + 0x124,
        _ZN12MeshCollider8LoadFileER13SharedFilePtr((void*)data_ov091_021344f4[idx]),
        ((char*)this) + 0x2ec,
        0x199,
        unk_08e,
        (void*)data_ov091_02134e5c[idx]);

    _ZN8Platform19UpdateClsnPosAndRotEv(((char*)this));
    func_020393d4(((char*)this) + 0x124, &_ZN16MeshColliderBase21UpdatePosWithVelocityERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_);
    func_020393c4(((char*)this) + 0x124, &func_ov091_02132380);

    _ZN7PathPtr6FromIDEj(((char*)this) + 0x344, *(unsigned*)((char*)&mParam) & 0xf);
    unk_324 = _ZNK7PathPtr8NumNodesEv((char*)&mPathPtr);
    unk_328 = 0;
    unk_32c = mPosX;
    unk_330 = mPosY;
    unk_334 = mPosZ;
    _ZNK7PathPtr7GetNodeER7Vector3j(((char*)this) + 0x344, ((char*)this) + 0x338, *(unsigned*)((char*)&unk_328));

    if (Vec3_equal(((char*)this) + 0x338, ((char*)this) + 0x32c)) {
        int* p = (int*)(((int)((char*)this) + 0x328));
        *p = *p + 1;
        _ZNK7PathPtr7GetNodeER7Vector3j(((char*)this) + 0x344, ((char*)this) + 0x338, *(unsigned*)((char*)&unk_328));
    }

    unk_350 = unk_08e;
    unk_34c = 0;
    unk_356 = 0;
    return 1;
}
