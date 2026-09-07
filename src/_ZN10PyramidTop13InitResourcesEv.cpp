//cpp
// @symbol _ZN10PyramidTop13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "PyramidTop.h"
#include "dBgW.h"
extern "C" {
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void* fp);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void* thiz, void* f, int a, int b);
extern void* _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(void* fp);
extern void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void* thiz, void* kcl, void* mtx, int fix, short s, void* clps);
extern void func_020393d4(void* p, void* v);
extern void _ZN5Event8ClearBitEj(unsigned int n);
extern void _ZN4dBgW16UpdatePosAndAngsERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_(void);
}

int PyramidTop::InitResources()
{
    void* m = _ZN5Model8LoadFileER13SharedFilePtr(data_ov024_02113968);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this) + 0x320, m, 1, -1);
    func_ov024_021114c4(((char*)this));
    func_ov024_02111480(((char*)this));
    void* mc = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(data_ov024_02113960);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        ((char*)this) + 0x124, mc, ((char*)this) + 0x370, 0x199, mAngleY, data_ov024_021129f0);
    func_020393d4(((char*)this) + 0x124, (void*)&_ZN4dBgW16UpdatePosAndAngsERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);
    ((dBgW *)(((char*)this) + 0x124))->Enable((dActor_c *)(((char*)this)));
    mHomePosX = mPosX;
    mHomePosY = mPosY;
    mHomePosZ = mPosZ;
    mAngVelY = 0;
    mNumTagsTriggered = 0;
    mState = 0;
    mSpinParticleID = 0;
    mSoundTimer = 0;
    _ZN5Event8ClearBitEj(0xe);
    return 1;
}
