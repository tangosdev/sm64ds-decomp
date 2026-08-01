//cpp
// @symbol _ZN6Snufit13InitResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "Snufit.h"
typedef int Fix12;
typedef struct { int w[2]; } SharedFilePtr;
typedef struct { short x,y,z; } Vector3_16;
typedef struct BMD_File BMD_File;
typedef struct Actor Actor;
typedef struct PMF PMF;
extern SharedFilePtr data_ov065_0211d618;
extern SharedFilePtr data_ov075_0211d610;
extern SharedFilePtr data_ov065_0211d600;
extern SharedFilePtr data_ov075_0211d608;
extern PMF data_ov065_0211d670;
extern BMD_File* _ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr* f);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void* self, BMD_File* f, int a, int b);
extern void _ZN11ShadowModel12InitCylinderEv(void* self);
extern void* _ZN9Animation8LoadFileER13SharedFilePtr(SharedFilePtr* f);
extern void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void* self, Actor* a, Fix12 r, Fix12 h, unsigned int e, unsigned int g);
extern void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(void* self, Actor* a, Fix12 r, Fix12 h, Vector3_16* p, Vector3_16* q);
extern int func_ov065_0211691c(void* c, PMF* p);

int Snufit::InitResources()
{
    _ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this)+0x300, _ZN5Model8LoadFileER13SharedFilePtr(&data_ov065_0211d618), 1, -1);
    _ZN5Model8LoadFileER13SharedFilePtr(&data_ov075_0211d610);
    _ZN11ShadowModel12InitCylinderEv((char*)&mShadowModel);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov065_0211d600);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov075_0211d608);
    unk_0a0 = -0x1e000;
    _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(((char*)this)+0x110, (Actor*)((char*)this), 0x38000, 0x7e000, 0x200000, 0x7eff0);
    mAngleY = mPrevAngleY;
    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(((char*)this)+0x144, (Actor*)((char*)this), 0x46000, 0, 0, 0);
    unk_108 = 1;
    unk_10a = 1;
    unk_3cc = mPosX;
    unk_3d0 = mPosY;
    unk_3d4 = mPosZ;
    unk_35c = 0x1000;
    func_ov065_0211691c(((char*)this), &data_ov065_0211d670);
    return 1;
}
