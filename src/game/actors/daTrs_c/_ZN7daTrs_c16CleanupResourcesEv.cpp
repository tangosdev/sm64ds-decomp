//cpp
#include "dActor_c.h"
#include "fBase_c.h"
#include "dCapEnemy_c.h"
// @symbol _ZN7daTrs_c16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "daTrs_c.h"
#include "SharedFilePtr.h"
struct dActor_c;
extern "C" void UnloadBlueCoinModel(void *o);
struct SharedFilePtr;

extern SharedFilePtr data_ov063_0211edec;
extern SharedFilePtr data_ov063_0211edc4;
extern SharedFilePtr data_ov063_0211eddc;
extern SharedFilePtr data_ov063_0211edf4;
extern SharedFilePtr data_ov063_0211ede4;
extern SharedFilePtr data_ov063_0211edd4;
extern SharedFilePtr data_ov063_0211edcc;

struct O {
    char pad0[0xc];
    unsigned short f0c;       // +0xc
    char pad1[0x48c - 0xe];
    void *f48c;               // +0x48c
    char pad2[0x494 - 0x490];
    unsigned int f494;        // +0x494
    char pad3[0x49c - 0x498];
    unsigned int f49c;        // +0x49c
    unsigned short f4a0;      // +0x4a0
    char pad5[0x5cf - 0x4a2];
    unsigned char f5cf;       // +0x5cf
};

int daTrs_c::CleanupResources()
{
    int b;
    int *cnt;

    if (((O *)this)->f49c != 0) {
        ((O *)this)->f48c = dActor_c::FindWithID(((O *)this)->f49c);
        if (((O *)this)->f48c != 0)
            ((fBase_c *)(((O *)this)->f48c))->MarkForDestruction();
        ((O *)this)->f48c = 0;
    }
    if (((O *)this)->f494 != 0) {
        ((O *)this)->f48c = dActor_c::FindWithID(((O *)this)->f494);
        if (((O *)this)->f48c != 0) {
            cnt = (int *)(((int)((O *)this)->f48c + 0x5a0));
            (*cnt)++;
        }
        ((O *)this)->f48c = 0;
    }
    if (((O *)this)->f4a0 == 0x122)
        UnloadBlueCoinModel(((O *)this));
    else if (((O *)this)->f4a0 == 0xd4)
        ((SharedFilePtr *)(&data_ov063_0211edec))->Release();

    b = (((O *)this)->f0c == 0xd1);
    if (b != 0) {
        ((SharedFilePtr *)(&data_ov063_0211edc4))->Release();
        ((SharedFilePtr *)(&data_ov063_0211eddc))->Release();
    } else {
        ((SharedFilePtr *)(&data_ov063_0211edf4))->Release();
        ((SharedFilePtr *)(&data_ov063_0211ede4))->Release();
        if (((O *)this)->f5cf == 0xf) {
            UnloadKeyModels(3);
            ((SharedFilePtr *)(&data_ov063_0211edd4))->Release();
            ((SharedFilePtr *)(&data_ov063_0211edcc))->Release();
        }
    }
    ((dCapEnemy_c *)(((O *)this)))->UnloadCapModel();
    return 1;
}
