//cpp
// @symbol _ZN4Fish16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Fish.h"
struct SharedFilePtr { unsigned int data[4]; };
extern "C" {
extern void _ZN13SharedFilePtr7ReleaseEv(struct SharedFilePtr *self);
extern void* _ZN5Actor10FindWithIDEj(unsigned int id);
}

extern struct SharedFilePtr data_ov100_021489cc;
extern struct SharedFilePtr *data_ov100_021473a4[];
extern struct SharedFilePtr *data_ov100_021473b0[];

int Fish::CleanupResources()
{
    _ZN13SharedFilePtr7ReleaseEv(&data_ov100_021489cc);
    _ZN13SharedFilePtr7ReleaseEv(data_ov100_021473a4[mModelIndex]);
    _ZN13SharedFilePtr7ReleaseEv(data_ov100_021473b0[mModelIndex]);
    if (unk_159 == 0) {
        if (_ZN5Actor10FindWithIDEj(unk_13c))
            func_ov100_02146280();
    }
    return 1;
}
