//cpp
#include "types.h"
// @symbol _ZN6Bullet13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Bullet.h"
struct dActor_c;
struct Vector3_16;
struct BMD_File;
/* Mangled ROM names need extern "C" in a .cpp file, or the compiler re-mangles them
   into phantoms no module defines. */
extern "C" struct BMD_File* _ZN5Model8LoadFileER13SharedFilePtr(void* fp);
extern "C" int _ZN9ModelBase7SetFileEP8BMD_Fileii(char* self, struct BMD_File* f, int a, int b);
extern "C" void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(char* self, struct dActor_c* a, int r, int h, u32 f1, u32 f2);
extern "C" void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(char* self, struct dActor_c* a, int r, int h, struct Vector3_16* rot, int f);

int Bullet::InitResources()
{
    struct BMD_File* f = _ZN5Model8LoadFileER13SharedFilePtr(func_0211d610);
    if (_ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this)+0x300, f, 1, -1) == 0) return 0;
    unk_358 = param1 & 0xff;
    if (unk_358 > 1) unk_358 = 0;
    mTerminalVelocity = -0x64000;
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(((char*)this)+0x110, (struct dActor_c*)((char*)this), 0xa000, 0xa000, 0x200004, 0);
    mAngleY = mPrevAngleY;
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(((char*)this)+0x144, (struct dActor_c*)((char*)this), 0xa000, 0xa000, 0, 0);
    func_ov002_020fed2c(((char*)this), data_ov002_02111190);
    return 1;
}
