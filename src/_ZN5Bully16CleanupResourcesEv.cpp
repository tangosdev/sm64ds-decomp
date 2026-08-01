//cpp
// @symbol _ZN5Bully16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "Bully.h"
extern "C" {
int _ZN13SharedFilePtr7ReleaseEv(void *);
}

int Bully::CleanupResources()
{
    _ZN13SharedFilePtr7ReleaseEv(*(void**)(*(char**)((char *)&mFileTable)+0));
    _ZN13SharedFilePtr7ReleaseEv(*(void**)(*(char**)((char *)&mFileTable)+4));
    _ZN13SharedFilePtr7ReleaseEv(*(void**)(*(char**)((char *)&mFileTable)+8));
    _ZN13SharedFilePtr7ReleaseEv(*(void**)(*(char**)((char *)&mFileTable)+0xc));
    _ZN13SharedFilePtr7ReleaseEv(*(void**)(*(char**)((char *)&mFileTable)+0x10));
    return 1;
}
