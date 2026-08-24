//cpp
// @symbol _ZN22RotatingUpDownPlatform13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_PathPtr.h"
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "RotatingUpDownPlatform.h"
extern "C" int Vec3_Equal(void*, void*);
extern "C" void* _ZN5Model8LoadFileER13SharedFilePtr(void* sfp);
extern "C" void _ZN9ModelBase7SetFileEP8BMD_Fileii(void* self, void* file, int a, int b);
extern "C" void* _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(void* sfp);
extern "C" void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void* self, void* kcl, void* mtx, int fix, short s, void* clps);
extern "C" void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void* c);
extern "C" void func_020393d4(void* p, void* v);
extern "C" void func_020393c4(void* p, void* v);
extern "C" void _ZN7PathPtr6FromIDEj(void* self, unsigned int id);
extern "C" void _ZNK7PathPtr7GetNodeER7Vector3j(void* self, void* out, unsigned int idx);

extern int data_ov091_021344fc[];
extern int data_ov091_021344f4[];
extern "C" char _ZN4dBgW21UpdatePosWithVelocityERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_;

int RotatingUpDownPlatform::InitResources()
{
    unsigned char idx;

    mVariant = (unsigned char)(*(unsigned*)((char*)&mParam) >> 8);
    idx = mVariant;
    _ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this) + 0xd4, _ZN5Model8LoadFileER13SharedFilePtr((void*)data_ov091_021344fc[idx]), 1, -1);

    idx = mVariant;
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        ((char*)this) + 0x124,
        _ZN7dBgW_Kc8LoadFileER13SharedFilePtr((void*)data_ov091_021344f4[idx]),
        ((char*)this) + 0x2ec,
        0x199,
        mAngleY,
        (void*)data_ov091_02134e5c[idx]);

    _ZN10dBgActor_c19UpdateClsnPosAndRotEv(((char*)this));
    func_020393d4(((char*)this) + 0x124, &_ZN4dBgW21UpdatePosWithVelocityERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);
    func_020393c4(((char*)this) + 0x124, &func_ov091_02132380);

    _ZN7PathPtr6FromIDEj(((char*)this) + 0x344, *(unsigned*)((char*)&mParam) & 0xf);
    mNodeCount = _ZNK7PathPtr8NumNodesEv((char*)&mPathPtr);
    mNodeIndex = 0;
    mBasePosX = mPosX;
    mBasePosY = mPosY;
    mBasePosZ = mPosZ;
    _ZNK7PathPtr7GetNodeER7Vector3j(((char*)this) + 0x344, ((char*)this) + 0x338, *(unsigned*)((char*)&mNodeIndex));

    if (Vec3_Equal(((char*)this) + 0x338, ((char*)this) + 0x32c)) {
        int* p = (int*)(((int)((char*)this) + 0x328));
        *p = *p + 1;
        _ZNK7PathPtr7GetNodeER7Vector3j(((char*)this) + 0x344, ((char*)this) + 0x338, *(unsigned*)((char*)&mNodeIndex));
    }

    mBaseAngleY = mAngleY;
    mSinkOffsetY = 0;
    mIsPressed = 0;
    return 1;
}
