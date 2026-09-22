//cpp
// @symbol _ZN6Fwoosh16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "Fwoosh.h"
extern "C" {
extern void _ZN13SharedFilePtr7ReleaseEv(struct SharedFilePtr *self);
}
extern struct SharedFilePtr data_ov002_0210da40;
extern struct SharedFilePtr data_ov002_0210d9a0;
extern struct SharedFilePtr data_ov002_0210d9c0;
extern struct SharedFilePtr data_ov091_02135674;
extern struct SharedFilePtr data_ov091_0213567c;
extern struct SharedFilePtr data_ov091_02135684;

int Fwoosh::CleanupResources()
{
    if (mVariant == 1) return 1;
    _ZN13SharedFilePtr7ReleaseEv(&data_ov002_0210da40);
    _ZN13SharedFilePtr7ReleaseEv(&data_ov002_0210d9a0);
    _ZN13SharedFilePtr7ReleaseEv(&data_ov002_0210d9c0);
    _ZN13SharedFilePtr7ReleaseEv(&data_ov091_02135674);
    _ZN13SharedFilePtr7ReleaseEv(&data_ov091_0213567c);
    _ZN13SharedFilePtr7ReleaseEv(&data_ov091_02135684);
    return 1;
}
