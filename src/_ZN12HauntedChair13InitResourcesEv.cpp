//cpp
#include "types.h"
// @symbol _ZN12HauntedChair13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "HauntedChair.h"
struct dActor_c; struct Vector3; struct Vector3_16; struct BMD_File;
extern "C" {
extern struct BMD_File* _ZN5Model8LoadFileER13SharedFilePtr(void* fp);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(char* self, struct BMD_File* f, int a, int b);
extern void _ZN11ShadowModel12InitCylinderEv(char* self);
extern void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(char* self, struct dActor_c* a, int r, int h, struct Vector3_16* rot, int f);
extern void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(char* self, struct dActor_c* a, struct Vector3* pos, int r, int h, u32 f1, u32 f2);
}
extern struct Matrix4x3 IDENTITY_MATRIX4X3;

int HauntedChair::InitResources()
{
    struct BMD_File* f = _ZN5Model8LoadFileER13SharedFilePtr(data_ov020_02114af0);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this)+0xd4, f, 1, -1);
    _ZN11ShadowModel12InitCylinderEv((char*)&mShadowModel);
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(((char*)this)+0x1bc, (struct dActor_c*)((char*)this), 0x32000, 0x64000, 0, 0);
    mClsnOffsetX = 0;
    mClsnOffsetY = 0;
    mClsnOffsetZ = 0;
    _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(((char*)this)+0x17c, (struct dActor_c*)((char*)this), (struct Vector3*)((char*)&mClsnOffsetX), 0x32000, 0x64000, 0x200004, 0);
    mState = 0;
    mHomePosX = mPosX;
    mHomePosY = mPosY;
    mHomePosZ = mPosZ;
    *(struct Matrix4x3*)((char*)&mShadowMat) = IDENTITY_MATRIX4X3;
    return 1;
}
