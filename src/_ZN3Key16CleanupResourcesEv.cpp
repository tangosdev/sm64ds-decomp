//cpp
// @symbol _ZN3Key16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Key.h"
extern void _ZN13SharedFilePtr7ReleaseEv(void *self);
extern void _ZN5Event8ClearBitEj(unsigned int b);
extern int data_ov002_02110964[];
extern int data_ov089_02132c60[];
extern int data_ov089_02132c40[];
extern int data_ov089_02132c70[];
extern int data_ov089_02132c48[];

int Key::CleanupResources()
{
    UnloadKeyModels(*(int *)((char *)&mState));
    _ZN13SharedFilePtr7ReleaseEv(data_ov002_02110964);
    if (*(int *)((char *)&mState) != 7) {
        _ZN13SharedFilePtr7ReleaseEv(data_ov089_02132c60);
        _ZN13SharedFilePtr7ReleaseEv(data_ov089_02132c40);
        _ZN13SharedFilePtr7ReleaseEv(data_ov089_02132c70);
        _ZN13SharedFilePtr7ReleaseEv(data_ov089_02132c48);
    }
    _ZN5Event8ClearBitEj(0x1d);
    return 1;
}
