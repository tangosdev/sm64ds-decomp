//cpp
// @symbol _ZN11ChiefChilly13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "ChiefChilly.h"
/* SharedFilePtr stays incomplete: Model.h forward-declares it and its layout is
   deliberately not recovered (include/SharedFilePtr.h). Used only by address here. */
typedef struct BMD_File BMD_File;
typedef struct dActor_c dActor_c;
typedef struct PMF PMF;

extern SharedFilePtr data_ov073_02123280;
extern SharedFilePtr data_ov073_021232a0;
extern SharedFilePtr data_ov073_02123288;
extern SharedFilePtr data_ov073_021232a8;
extern SharedFilePtr data_ov073_02123290;
extern SharedFilePtr data_ov073_021232b0;
extern SharedFilePtr data_ov073_021232b8;
extern SharedFilePtr data_ov002_0210da30;
extern SharedFilePtr data_ov073_02123298;
extern PMF data_ov073_02123330;

extern "C" {
extern void LoadKeyModels(int idx);
extern BMD_File* _ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr* f);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void* self, BMD_File* f, int a, int b);
extern void _ZN11ShadowModel12InitCylinderEv(void* self);
extern void* _ZN9Animation8LoadFileER13SharedFilePtr(SharedFilePtr* f);
extern void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(void* self, dActor_c* a, Vector3* v, Fix12i r, Fix12i h, unsigned int e, unsigned int g);
extern void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void* self, dActor_c* a, Fix12i r, Fix12i h, Vector3_16* p, Vector3_16* q);
extern short _ZN8dActor_c18HorzAngleToCPlayerEv(void* self);
extern int ChiefChilly_ChangeState(void* c, PMF* p);
}

int ChiefChilly::InitResources()
{
    BMD_File* f;
    Vector3 v;
    int i;
    LoadKeyModels(4);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov073_02123280);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov073_021232a0);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov073_02123288);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov073_021232a8);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov073_02123290);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov073_021232b0);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov073_021232b8);
    _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210da30);
    f = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov073_02123298);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(&mBlendModelAnim, f, 1, -1);
    _ZN11ShadowModel12InitCylinderEv((char*)&mShadowModel);
    mVertAccel = -0x3000;
    mTerminalVelocity = -0x3c000;
    v.x = data_ov073_02123040.x;
    v.y = data_ov073_02123040.y;
    v.z = data_ov073_02123040.z;
    _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(&mdCcAcPos_c, (dActor_c*)((char*)this), &v, 0xa4000, 0x1e4000, 0x200000, 0x567f0);
    mBlendModelAnim.speed = 0x2000;
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, (dActor_c*)((char*)this), 0x96000, 0x94000, 0, 0);
    mSpawnPosX = mPosX;
    mSpawnPosY = mPosY;
    mSpawnPosZ = mPosZ;
    {
        char* p;
        for (i = 0, p = ((char*)this); i < 8; i++) {
            *(int*)(p + 0x3e8) = mPosX;
            *(int*)(p + 0x3ec) = mPosY;
            *(int*)(p + 0x3f0) = mPosZ;
            *(int*)(p + 0x448) = mPosX;
            *(int*)(p + 0x44c) = mPosY;
            *(int*)(p + 0x450) = mPosZ;
            p += 0xc;
        }
    }
    unk_4c5 = 0xff;
    mPrevAngleY = _ZN8dActor_c18HorzAngleToCPlayerEv(((char*)this));
    mAngleY = mPrevAngleY;
    *(short*)(((char*)this)+0x400+0xc6) = mAngleY;
    unk_4bc = 2;
    mHitsRemaining = 3;
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    ChiefChilly_ChangeState(((char*)this), &data_ov073_02123330);
    return 1;
}
