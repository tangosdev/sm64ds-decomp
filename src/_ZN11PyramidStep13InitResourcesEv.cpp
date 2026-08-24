//cpp
// @symbol _ZN11PyramidStep13InitResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "PyramidStep.h"
typedef short s16;
struct SharedFilePtr { int x; }; struct BMD_File; struct KCL_File; struct Matrix4x3; struct CLPS_Block;
extern int _ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_;
extern "C" {
struct BMD_File *_ZN5Model8LoadFileER13SharedFilePtr(struct SharedFilePtr &f);
void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, struct BMD_File *f, int a, int b);
void func_ov025_02111e30(char *t);
void func_ov025_02111dec(char *c);
struct KCL_File *_ZN7dBgW_Kc8LoadFileER13SharedFilePtr(struct SharedFilePtr &f);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *self, struct KCL_File *k, struct Matrix4x3 &m, int fx, short s, struct CLPS_Block &c);
void func_020393d4(int *p, int v);
extern struct SharedFilePtr data_ov025_02113ab8;
extern struct SharedFilePtr data_ov025_02113ab0;
extern struct CLPS_Block data_ov025_02112ce8;
}

int PyramidStep::InitResources()
{
    struct BMD_File *bmd = _ZN5Model8LoadFileER13SharedFilePtr(data_ov025_02113ab8);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(((char *)this) + 0x320, bmd, 1, -1);
    func_ov025_02111e30(((char *)this));
    func_ov025_02111dec(((char *)this));
    {
        struct KCL_File *kcl = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(data_ov025_02113ab0);
        _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
            &mMeshCollider, kcl, *(struct Matrix4x3 *)((char *)&mClsnMat2), 0x1000,
            mAngleY, data_ov025_02112ce8);
    }
    func_020393d4((int *)((char *)&(*(u8 *)&mMeshCollider)), (int)&_ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);
    {
        int v = 0x5000;
        int k = (*(s32 *)&param1) & 3;
        mVertSpeed = -v;
        mState = 0;
        mStateTimer = 0;
        switch (k) {
        case 0:
            break;
        case 1:
            *(int*)(((int)((char *)this) + 0x60)) -= 0xfa000;
            *(unsigned short*)(((int)((char *)this) + 0x370)) += 0x32;
            break;
        case 2:
            *(int*)(((int)((char *)this) + 0x60)) -= 0x1f4000;
            mState = 1;
            mVertSpeed = v;
            break;
        }
    }
    return 1;
}
