//cpp
// @symbol _ZN13PeachPainting13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "PeachPainting.h"
extern "C" {
extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *fp);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, void *file, int a, int b);
extern void func_ov010_02111e84(void*);
}

int PeachPainting::InitResources()
{
    void *file = _ZN5Model8LoadFileER13SharedFilePtr((void*)RollingRock_Spawn);
    _ZN9ModelBase7SetFileEP8BMD_Fileii((char*)((void *)this) + 0xd4, file, 1, -1);
    *(unsigned char*)((char*)&mOpacity) = 0xff;
    func_ov010_02111e84(((void *)this));
    return 1;
}
