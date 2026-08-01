//cpp
// @symbol _ZN10StarMarker16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "StarMarker.h"
extern void _ZN13SharedFilePtr7ReleaseEv(void*);
extern char data_ov002_0211092c;
extern char data_ov002_0210d9a8;

int StarMarker::CleanupResources()
{
    if (mState != 0) {
        _ZN13SharedFilePtr7ReleaseEv(&data_ov002_0211092c);
        _ZN13SharedFilePtr7ReleaseEv(&data_ov002_0210d9a8);
    } else {
        _ZN13SharedFilePtr7ReleaseEv(&data_ov002_0211093c);
    }
    return 1;
}
