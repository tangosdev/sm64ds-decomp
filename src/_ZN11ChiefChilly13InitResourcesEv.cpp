//cpp
// @symbol _ZN11ChiefChilly13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "ChiefChilly.h"
typedef int Fix12;
typedef struct { int w[2]; } SharedFilePtr;
typedef struct BMD_File BMD_File;
typedef struct Actor Actor;
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

extern void LoadKeyModels(int idx);
extern BMD_File* _ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr* f);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void* self, BMD_File* f, int a, int b);
extern void _ZN11ShadowModel12InitCylinderEv(void* self);
extern void* _ZN9Animation8LoadFileER13SharedFilePtr(SharedFilePtr* f);
extern void _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(void* self, Actor* a, Vector3* v, Fix12 r, Fix12 h, unsigned int e, unsigned int g);
extern void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(void* self, Actor* a, Fix12 r, Fix12 h, Vector3_16* p, Vector3_16* q);
extern short _ZN5Actor18HorzAngleToCPlayerEv(void* self);
extern int func_ov073_0212157c(void* c, PMF* p);

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
    _ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this)+0x30c, f, 1, -1);
    _ZN11ShadowModel12InitCylinderEv((char*)&mShadowModel);
    unk_09c = -0x3000;
    unk_0a0 = -0x3c000;
    v.x = data_ov073_02123040.x;
    v.y = data_ov073_02123040.y;
    v.z = data_ov073_02123040.z;
    _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(((char*)this)+0x110, (Actor*)((char*)this), &v, 0xa4000, 0x1e4000, 0x200000, 0x567f0);
    unk_368 = 0x2000;
    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(((char*)this)+0x150, (Actor*)((char*)this), 0x96000, 0x94000, 0, 0);
    unk_3d8 = mPosX;
    unk_3dc = mPosY;
    unk_3e0 = mPosZ;
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
    unk_094 = _ZN5Actor18HorzAngleToCPlayerEv(((char*)this));
    unk_08e = unk_094;
    *(short*)(((char*)this)+0x400+0xc6) = unk_08e;
    unk_4bc = 2;
    unk_4cb = 3;
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    func_ov073_0212157c(((char*)this), &data_ov073_02123330);
    return 1;
}
