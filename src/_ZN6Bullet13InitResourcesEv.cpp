//cpp
// @symbol _ZN6Bullet13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Bullet.h"
typedef unsigned int u32;
typedef int s32;
typedef short s16;
struct Actor;
struct Vector3_16;
struct BMD_File;
extern struct BMD_File* _ZN5Model8LoadFileER13SharedFilePtr(void* fp);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(char* self, struct BMD_File* f, int a, int b);
extern void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(char* self, struct Actor* a, int r, int h, u32 f1, u32 f2);
extern void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(char* self, struct Actor* a, int r, int h, struct Vector3_16* rot, int f);

int Bullet::InitResources()
{
    struct BMD_File* f = _ZN5Model8LoadFileER13SharedFilePtr(func_0211d610);
    if (_ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this)+0x300, f, 1, -1) == 0) return 0;
    unk_358 = unk_008 & 0xff;
    if (unk_358 > 1) unk_358 = 0;
    unk_0a0 = -0x64000;
    _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(((char*)this)+0x110, (struct Actor*)((char*)this), 0xa000, 0xa000, 0x200004, 0);
    unk_08e = unk_094;
    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(((char*)this)+0x144, (struct Actor*)((char*)this), 0xa000, 0xa000, 0, 0);
    func_ov002_020fed2c(((char*)this), data_ov002_02111190);
    return 1;
}
