//cpp
// @symbol _ZN20daObjCannonShutter_c13InitResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "daObjCannonShutter_c.h"
typedef short s16;
struct SharedFilePtr; struct BMD_File; struct KCL_File; struct Matrix4x3; struct CLPS_Block;
extern struct SharedFilePtr data_ov002_0210e12c;
extern struct SharedFilePtr data_ov002_0210e124;
extern struct CLPS_Block data_ov002_0210d7f4;
extern "C" {
struct BMD_File *_ZN5Model8LoadFileER13SharedFilePtr(struct SharedFilePtr &f);
void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, struct BMD_File *f, int a, int b);
void _ZN10dBgActor_c21UpdateModelPosAndRotYEv(void *self);
void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void *self);
struct KCL_File *_ZN7dBgW_Kc8LoadFileER13SharedFilePtr(struct SharedFilePtr &f);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *self, struct KCL_File *k, struct Matrix4x3 &m, int fx, short s, struct CLPS_Block &c);
int IsCannonOpenInCurLevel(void);
}

int daObjCannonShutter_c::InitResources()
{
    struct BMD_File *bmd = _ZN5Model8LoadFileER13SharedFilePtr(data_ov002_0210e12c);
    _ZN9ModelBase7SetFileEP8BMD_Fileii((unsigned char *)&mModel, bmd, 1, -1);
    _ZN10dBgActor_c21UpdateModelPosAndRotYEv(((unsigned char *)this));
    _ZN10dBgActor_c19UpdateClsnPosAndRotEv(((unsigned char *)this));
    {
        struct KCL_File *kcl = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(data_ov002_0210e124);
        _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
            (unsigned char *)&mMeshCollider, kcl, *(struct Matrix4x3 *)&mClsnMat, 0x199,
            mAngleY, data_ov002_0210d7f4);
    }
    mHomePosX = mPosX;
    mHomePosY = mPosY;
    mHomePosZ = mPosZ;
    if (IsCannonOpenInCurLevel() != 0) {
        mCannonOpen = 1;
    }
    return 1;
}
