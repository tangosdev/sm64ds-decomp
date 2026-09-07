//cpp
// @symbol _ZN6Dorrie16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Dorrie.h"
#include "dBgW.h"
struct SharedFilePtr { unsigned int data[4]; };
extern "C" {
extern void _ZN13SharedFilePtr7ReleaseEv(struct SharedFilePtr *);
}
extern struct SharedFilePtr data_ov002_0210d9c0;
extern struct SharedFilePtr *data_ov065_0211c08c[];
extern struct SharedFilePtr *data_ov065_0211c080[];
extern struct SharedFilePtr data_ov065_0211d720;

int Dorrie::CleanupResources()
{
    int i;
    char *p;
    _ZN13SharedFilePtr7ReleaseEv(&data_ov002_0210d9c0);
    p = ((char *)this) + 0x180;
    for (i = 0; i < 7; i++) {
        ((dBgW *)(p))->Disable();
        _ZN13SharedFilePtr7ReleaseEv(data_ov065_0211c08c[i]);
        p += 0x200;
    }
    for (i = 0; i < 3; i++)
        _ZN13SharedFilePtr7ReleaseEv(data_ov065_0211c080[i]);
    _ZN13SharedFilePtr7ReleaseEv(&data_ov065_0211d720);
    return 1;
}
