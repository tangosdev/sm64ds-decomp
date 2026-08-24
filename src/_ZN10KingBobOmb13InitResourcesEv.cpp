//cpp
// @symbol _ZN10KingBobOmb13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "KingBobOmb.h"
/* SharedFilePtr stays incomplete: Model.h forward-declares it and its layout is
   deliberately not recovered (include/SharedFilePtr.h). Used only by address here. */
typedef struct BMD_File BMD_File;
typedef struct dActor_c dActor_c;
typedef struct PMF PMF;

extern SharedFilePtr data_ov078_02126f38;
extern SharedFilePtr data_ov078_02126f00;
extern SharedFilePtr data_ov078_02126f20;
extern SharedFilePtr data_ov078_02126f10;
extern SharedFilePtr data_ov078_02126f08;
extern SharedFilePtr data_ov078_02126f18;
extern SharedFilePtr data_ov078_02126ee0;
extern SharedFilePtr data_ov078_02126ef0;
extern SharedFilePtr data_ov078_02126f40;
extern SharedFilePtr data_ov078_02126f30;
extern SharedFilePtr data_ov078_02126ee8;
extern SharedFilePtr data_ov078_02126f28;
extern SharedFilePtr data_ov078_02126ef8;
extern int data_0209e650;
extern PMF data_ov078_0212710c;

extern "C" {
extern BMD_File* _ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr* f);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void* self, BMD_File* f, int a, int b);
extern void _ZN11ShadowModel12InitCylinderEv(void* self);
extern void* _ZN9Animation8LoadFileER13SharedFilePtr(SharedFilePtr* f);
extern void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(void* self, dActor_c* a, Vector3* v, Fix12i r, Fix12i h, unsigned int e, unsigned int g);
extern void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void* self, dActor_c* a, Fix12i r, Fix12i h, Vector3_16* p, Vector3_16* q);
extern unsigned char _ZN8dActor_c9TrackStarEjj(void* self, unsigned int a, unsigned int b);
extern int RandomIntInternal(int* seed);
extern void KingBobOmb_SetState(void* c, PMF* p);
}

int KingBobOmb::InitResources()
{
    BMD_File* f;
    Vector3 v0;
    Vector3 v1;
    int i;
    f = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov078_02126f38);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this)+0x2cc, f, 1, 1);
    _ZN11ShadowModel12InitCylinderEv((char*)&(*(u8 *)&mShadowModel));
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov078_02126f00);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov078_02126f20);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov078_02126f10);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov078_02126f08);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov078_02126f18);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov078_02126ee0);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov078_02126ef0);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov078_02126f40);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov078_02126f30);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov078_02126ee8);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov078_02126f28);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov078_02126ef8);
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
    v0.x = data_ov078_02126e00.x;
    v0.y = data_ov078_02126e00.y;
    v0.z = data_ov078_02126e00.z;
    _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(((char*)this)+0x33c, (dActor_c*)((char*)this), &v0, 0x78000, 0xc8000, 0x200004, 0x206000);
    v1.x = data_ov078_02126e00.x;
    v1.y = data_ov078_02126e00.y;
    v1.z = data_ov078_02126e00.z;
    _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(((char*)this)+0x37c, (dActor_c*)((char*)this), &v1, 0xc8000, 0xc8000, 0x200000, 0x207000);
    unk_498 = 0x1f;
    mHomePosX = mPosX;
    mHomePosY = mPosY;
    mHomePosZ = mPosZ;
    mArenaPosX = 0xb1d000;
    mArenaPosY = 0x1060000;
    mArenaPosZ = 0xfee15000;
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(((char*)this)+0x110, (dActor_c*)((char*)this), 0x190000, 0x190000, 0, 0);
    func_02035550((char*)&(*(dBgCh_Actr *)&mWithMeshClsn));
    mAnimSpeed = 1;
    unk_500 = 3;
    mStarID = (*(s32 *)&param1) & 0xf;
    mStarTracked = _ZN8dActor_c9TrackStarEjj(((char*)this), mStarID, 2);
#pragma opt_strength_reduction off
    {
    int z = 0;
    for (i = 0; i < 2; i++) {
        *(int*)(((char*)this)+0x424+i*4) = z;
        *(unsigned char*)(((char*)this)+0x42c+i) = (unsigned char)z;
    }
    }
    unk_4a0 = ((unsigned int)RandomIntInternal(&data_0209e650) >> 0x1e) & 1;
    {
        int *p = (int*)((char*)&unk_4a0);
        *p = *p + 1;
    }
    mInitAngleY = mAngleY;
    KingBobOmb_SetState(((char*)this), &data_ov078_0212710c);
    return 1;
}
