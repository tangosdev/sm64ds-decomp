//cpp
// @symbol _ZN9PowerStar16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "PowerStar.h"
extern "C" void _ZN5Actor11UntrackStarERa(void* self, signed char* star);
extern "C" void _ZN13SharedFilePtr7ReleaseEv(void* p);

extern char data_ov002_02110944;
extern char data_ov002_02110924;
extern char data_ov002_02110964;
extern char data_ov002_02110934;

int PowerStar::CleanupResources()
{
    int b = (unk_00c == 0xb2);
    if (b) {
        int v = unk_43c;
        if (v != 8) {
            if (v == 6)
                UnloadSilverStarAndNumber();
            _ZN5Actor11UntrackStarERa(((char*)this), (signed char*)((char*)&unk_498));
        }
    } else {
        _ZN5Actor11UntrackStarERa(((char*)this), (signed char*)((char*)&unk_498));
        UnloadSilverStarAndNumber();
    }
    _ZN13SharedFilePtr7ReleaseEv(&data_ov002_02110944);
    _ZN13SharedFilePtr7ReleaseEv(&data_ov002_02110924);
    _ZN13SharedFilePtr7ReleaseEv(&data_ov002_02110964);
    _ZN13SharedFilePtr7ReleaseEv(&data_ov002_02110934);
    return 1;
}
