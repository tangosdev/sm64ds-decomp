//cpp
#include "types.h"
// @symbol _ZN8BookShot13InitResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "BookShot.h"
struct dActor_c; struct Vector3; struct Vector3_16; struct BMD_File;
/* SharedFilePtr stays incomplete: Model.h forward-declares it and its layout is
   deliberately not recovered (include/SharedFilePtr.h). Used only by address here. */

extern "C" {
extern struct BMD_File* _ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr* fp);
extern void* _ZN9Animation8LoadFileER13SharedFilePtr(SharedFilePtr* fp);
extern void LoadBlueCoinModel(void* c);
extern int _ZN11ShadowModel12InitCylinderEv(char* self);
extern void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(char* self, struct dActor_c* a, int r, int h, struct Vector3_16* rot, int f);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(char* self, struct BMD_File* f, int a, int b);
extern void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(char* self, struct dActor_c* a, struct Vector3* pos, int r, int h, u32 f1, u32 f2);
}

extern SharedFilePtr data_ov020_02114aa0;
extern SharedFilePtr data_ov020_02114ab8;
extern SharedFilePtr data_ov020_02114aa8;
extern SharedFilePtr data_ov020_02114ab0;

#define LDR(p) (p)

struct M48 { int w[12]; };
extern struct M48 IDENTITY_MATRIX4X3;

int BookShot::InitResources()
{
    _ZN5Model8LoadFileER13SharedFilePtr(&data_ov020_02114aa0);
    _ZN5Model8LoadFileER13SharedFilePtr(&data_ov020_02114ab8);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov020_02114aa8);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov020_02114ab0);
    LoadBlueCoinModel(((char*)this));

    if (_ZN11ShadowModel12InitCylinderEv((char*)&mShadowModel) == 0)
        return 0;

    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(((char*)this)+0x25c, (struct dActor_c*)((char*)this), 0x32000, 0x32000, 0, 0);

    unk_438 = 0;
    unk_43c = 0;
    unk_440 = 0;
    _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(((char*)this)+0x21c, (struct dActor_c*)((char*)this), (struct Vector3*)((char*)&unk_438), 0x19000, 0x32000, 0x200001, 0);

    unk_418 = 0;
    unk_41c = 0;
    mScaleX = 0x800;
    mScaleY = 0x800;
    mScaleZ = 0x800;
    unk_42c = mPosX;
    unk_430 = mPosY;
    unk_434 = mPosZ;

    if (_ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this)+0x174, (struct BMD_File*)((int*)&data_ov020_02114ab8)[1], 1, -1) == 0)
        return 0;

    *(struct M48*)((char*)&unk_1ec) = IDENTITY_MATRIX4X3;
    unk_450 = 0;
    *(short*)(int)LDR((char*)&mAngleY) = *(short*)(int)LDR((char*)&mAngleY) + 0x8000;
    unk_44c = 0x800;

    {
        unsigned short id = actorID;
        unsigned int match1 = (id == 0x145);

        if (match1)
        {
            unk_420 = 0;
            mState = 4;
            unk_108 = 0;
            *(int*)(int)LDR((char*)&mdCcAcPos_c.vulnFlags) |= 0x26fe0;
            goto success;
        }
        {
            unsigned int match2 = (id == 0x147);
            if (match2)
            {
                unk_420 = 0;
                mState = 0;
                unk_108 = 3;
                *(int*)(int)LDR((char*)&mdCcAcPos_c.vulnFlags) |= 0x2efe0;
                goto success;
            }
        }
        {
            unsigned int match3 = (id == 0xd5);
            if (match3)
            {
                unk_420 = 1;
                mState = 6;
                unk_108 = 0;
                *(int*)(int)LDR((char*)&mdCcAcPos_c.flags) |= 4;
                *(int*)(int)LDR((char*)&mdCcAcPos_c.vulnFlags) |= 0x3c0;
                mScaleX = 0x1000;
                mScaleY = 0x800;
                mScaleZ = 0x800;
                goto success;
            }
        }
    }
    return 0;

success:
    return 1;
}
