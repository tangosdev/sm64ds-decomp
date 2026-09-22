//cpp
// @symbol _ZN9PowerStar16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "PowerStar.h"
#include "SharedFilePtr.h"
extern "C" void _ZN8dActor_c11UntrackStarERa(void* self, signed char* star);

extern char data_ov002_02110944;
extern char data_ov002_02110924;
extern char data_ov002_02110964;
extern char data_ov002_02110934;

int PowerStar::CleanupResources()
{
    int b = (actorID == 0xb2);
    if (b) {
        int v = unk_43c;
        if (v != 8) {
            if (v == 6)
                UnloadSilverStarAndNumber();
            _ZN8dActor_c11UntrackStarERa(((char*)this), (signed char*)((char*)&unk_498));
        }
    } else {
        _ZN8dActor_c11UntrackStarERa(((char*)this), (signed char*)((char*)&unk_498));
        UnloadSilverStarAndNumber();
    }
    ((SharedFilePtr *)(&data_ov002_02110944))->Release();
    ((SharedFilePtr *)(&data_ov002_02110924))->Release();
    ((SharedFilePtr *)(&data_ov002_02110964))->Release();
    ((SharedFilePtr *)(&data_ov002_02110934))->Release();
    return 1;
}
