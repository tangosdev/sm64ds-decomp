//cpp
#include "types.h"
// @symbol _ZN9WaterRing13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "WaterRing.h"
struct Actor;
struct BMD_File;
struct BTA_File;


extern "C" {
extern struct BMD_File* _ZN5Model8LoadFileER13SharedFilePtr(void* fp);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(char* self, struct BMD_File* f, int a, int b);
extern void _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File(struct BMD_File* f, struct BTA_File* b);
extern void _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(char* self, struct BTA_File* b, int a, int fix, u32 f);
extern void _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(char* self, struct Actor* a, struct Vector3* v, int r, int h, u32 f1, u32 f2);
}

extern char data_ov002_0210da10[];
extern int data_ov064_0211c3d0[3];

int WaterRing::InitResources()
{
    struct BMD_File* f = _ZN5Model8LoadFileER13SharedFilePtr(data_ov002_0210da10);
    if (_ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this) + 0x30c, f, 1, -1) == 0)
        return 0;

    unk_37c = param1 & 0xff;
    if (unk_37c > 2 || unk_37c == 0xff)
    {
        unk_37c = 0;
    }

    _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File(*(struct BMD_File**)(data_ov002_0210da10 + 4), (struct BTA_File*)data_ov002_0210d6dc);
    _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(((char*)this) + 0x35c, (struct BTA_File*)data_ov002_0210d6dc, 0, 0x1000, 0);

    mTextureTransformer.speed = 0x1000;
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;

    int* src = data_ov064_0211c3d0;
    Vector3 v;
    v.x = src[0];
    v.y = src[1];
    v.z = src[2];
    _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(((char*)this) + 0x110, (struct Actor*)((char*)this), &v, 0x88000, 0xe8000, 0x800006, 0);

    unk_380 = 0x1f;
    func_ov064_02119ecc(((char*)this), data_ov064_0211c954);
    return 1;
}
