//cpp
#include "types.h"
// @symbol _ZN22ClockPaintingHandShort13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "ClockPaintingHandShort.h"
extern int _ZN5Model8LoadFileER13SharedFilePtr(void*);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void*, int, int, int);

int ClockPaintingHandShort::InitResources()
{
    int eq = (int)(*(unsigned short*)((char*)&unk_00c) == 0x125);
    if (eq != 0)
        *(unsigned char*)((char*)&mHandIndex) = 0;
    else
        *(unsigned char*)((char*)&mHandIndex) = 1;
    {
        unsigned char i = *(unsigned char*)((char*)&mHandIndex);
        int file = _ZN5Model8LoadFileER13SharedFilePtr(data_ov051_021116b0[i]);
        _ZN9ModelBase7SetFileEP8BMD_Fileii((char*)((void*)this) + 0xd4, file, 1, -1);
    }
    func_ov013_02111430((char*)((void*)this));
    return 1;
}
