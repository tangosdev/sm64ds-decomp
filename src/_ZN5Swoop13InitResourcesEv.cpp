//cpp
// @symbol _ZN5Swoop13InitResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "Swoop.h"
typedef int Fix12;
typedef struct { int w[2]; } SharedFilePtr;
typedef struct { short x,y,z; } Vector3_16;
typedef struct BMD_File BMD_File;
typedef struct Actor Actor;
typedef struct PMF PMF;
extern SharedFilePtr data_ov065_0211d698;
extern SharedFilePtr data_ov065_0211d6a8;
extern SharedFilePtr data_ov065_0211d690;
extern SharedFilePtr data_ov065_0211d6a0;
extern PMF data_ov065_0211d700;
extern BMD_File* _ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr* f);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void* self, BMD_File* f, int a, int b);
extern void _ZN11ShadowModel12InitCylinderEv(void* self);
extern void* _ZN9Animation8LoadFileER13SharedFilePtr(SharedFilePtr* f);
extern void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void* self, Actor* a, Fix12 r, Fix12 h, unsigned int e, unsigned int g);
extern void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(void* self, Actor* a, Fix12 r, Fix12 h, Vector3_16* p, Vector3_16* q);
extern int func_ov065_02117944(void* c, PMF* p);

int Swoop::InitResources()
{
    _ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this)+0x300, _ZN5Model8LoadFileER13SharedFilePtr(&data_ov065_0211d698), 1, -1);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this)+0x364, _ZN5Model8LoadFileER13SharedFilePtr(&data_ov065_0211d6a8), 1, -1);
    _ZN11ShadowModel12InitCylinderEv((char*)&mShadowModel);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov065_0211d690);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov065_0211d6a0);
    unk_0a0 = -0xa000;
    _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(((char*)this)+0x110, (Actor*)((char*)this), 0x28000, 0x28000, 0x200000, 0x7eff0);
    mAngleY = mPrevAngleY;
    mPrevAngleX = -0x8000;
    mAngleX = mPrevAngleX;
    unk_43c = 0;
    unk_35c = 0x1000;
    unk_3c0 = 0x1000;
    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(((char*)this)+0x144, (Actor*)((char*)this), 0x28000, 0x1e000, 0, 0);
    unk_424 = mPosX;
    unk_428 = mPosY;
    unk_42c = mPosZ;
    unk_108 = 1;
    func_ov065_02117944(((char*)this), &data_ov065_0211d700);
    return 1;
}
