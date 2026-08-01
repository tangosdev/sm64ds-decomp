//cpp
// @symbol _ZN4Toad16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Toad.h"
extern void _ZN13SharedFilePtr7ReleaseEv(void *self);
extern int data_ov002_0210da40[];
extern int data_ov002_0210d9a0[];
extern int data_ov002_0210d9c0[];
extern int data_ov085_02130480[];

int Toad::CleanupResources()
{
    if (*(unsigned char *)((char *)&unk_20b) == 1) {
        _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210da40);
        _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210d9a0);
        _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210d9c0);
    }
    _ZN13SharedFilePtr7ReleaseEv(data_ov085_02130480);
    _ZN13SharedFilePtr7ReleaseEv(data_ov085_02130488);
    _ZN13SharedFilePtr7ReleaseEv(data_ov085_02130490);
    return 1;
}
