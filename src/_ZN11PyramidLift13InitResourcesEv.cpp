//cpp
// @symbol _ZN11PyramidLift13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "PyramidLift.h"
#pragma opt_strength_reduction off
/* Not `Fix12`: this actor's header now reaches math/Fix12.h, where Fix12 is a
   class template. Only the raw word matters at this call. */
typedef int Fix12Raw;
/* SharedFilePtr stays incomplete: Model.h forward-declares it and its layout is
   deliberately not recovered (include/SharedFilePtr.h). Used only by address here. */
typedef struct BMD_File BMD_File;
typedef struct KCL_File KCL_File;
typedef struct Matrix4x3 Matrix4x3;
typedef struct CLPS_Block CLPS_Block;
extern SharedFilePtr data_ov025_02113ae0;
extern SharedFilePtr data_ov002_0210d9f0;
extern SharedFilePtr data_ov025_02113ad8;
extern CLPS_Block data_ov025_02112d08;
extern int _ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_;
extern "C" {
extern BMD_File* _ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr* f);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void* self, BMD_File* f, int a, int b);
extern void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void* self);
extern KCL_File* _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(SharedFilePtr* f);
extern void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void* self, KCL_File* k, Matrix4x3* m, Fix12Raw f, short s, CLPS_Block* b);
extern void func_020393d4(void* p, void* v);
extern void func_020393c4(void* p, void* v);
}

int PyramidLift::InitResources()
{
    BMD_File* bmd;
    KCL_File* kcl;
    bmd = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov025_02113ae0);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this)+0xd4, bmd, 1, -1);
    bmd = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210d9f0);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this)+0x320, bmd, 1, -1);
    _ZN10dBgActor_c19UpdateClsnPosAndRotEv(((char*)this));
    kcl = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(&data_ov025_02113ad8);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        ((char*)this)+0x124, kcl, (Matrix4x3*)((char*)&mClsnMat), 0x199, mAngleY, &data_ov025_02112d08);
    func_020393d4(((char*)this)+0x124, &_ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);
    func_020393c4(((char*)this)+0x124, &func_ov025_021125dc);
    {
        int n;
        char *ip;
        int k;
        mBasePosX = mPosX;
        n = 0;
        mBasePosY = mPosY;
        ip = ((char*)this);
        mBasePosZ = mPosZ;
        mState = (unsigned char)n;
        mHadClsn = (unsigned char)n;
        k = 0x1cc000;
        do {
            int *py;
            int prod;
            n = n + 1;
            *(int*)(ip+0x37c) = mPosX;
            *(int*)(ip+0x380) = mPosY;
            prod = n * k;
            *(int*)(ip+0x384) = mPosZ;
            py = (int*)(((int)ip + 0x380));
            *py = *py - prod;
            ip = ip + 0xc;
        } while (n < 10);
    }
    return 1;
}
