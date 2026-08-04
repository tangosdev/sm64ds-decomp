//cpp
#include "types.h"
// @symbol _ZN12HauntedChair13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "HauntedChair.h"
struct Actor; struct Vector3; struct Vector3_16; struct BMD_File;
extern "C" {
extern struct BMD_File* _ZN5Model8LoadFileER13SharedFilePtr(void* fp);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(char* self, struct BMD_File* f, int a, int b);
extern void _ZN11ShadowModel12InitCylinderEv(char* self);
extern void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(char* self, struct Actor* a, int r, int h, struct Vector3_16* rot, int f);
extern void _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(char* self, struct Actor* a, struct Vector3* pos, int r, int h, u32 f1, u32 f2);
}
struct M48 { int w[12]; };
extern struct M48 data_02082128;

int HauntedChair::InitResources()
{
    struct BMD_File* f = _ZN5Model8LoadFileER13SharedFilePtr(data_ov020_02114af0);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this)+0xd4, f, 1, -1);
    _ZN11ShadowModel12InitCylinderEv((char*)&mShadowModel);
    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(((char*)this)+0x1bc, (struct Actor*)((char*)this), 0x32000, 0x64000, 0, 0);
    unk_38c = 0;
    unk_390 = 0;
    unk_394 = 0;
    _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(((char*)this)+0x17c, (struct Actor*)((char*)this), (struct Vector3*)((char*)&unk_38c), 0x32000, 0x64000, 0x200004, 0);
    unk_378 = 0;
    unk_380 = mPosX;
    unk_384 = mPosY;
    unk_388 = mPosZ;
    *(struct M48*)((char*)&unk_14c) = data_02082128;
    return 1;
}
