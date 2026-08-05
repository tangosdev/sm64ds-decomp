//cpp
// @symbol _ZN15BookShotSpawner13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "BookShotSpawner.h"
extern int data_ov020_02114ab8[];
extern void _ZN5Model8LoadFileER13SharedFilePtr(void *);
extern void LoadBlueCoinModel(void *);
extern int data_ov020_02114aa0[];

int BookShotSpawner::InitResources()
{
    unk_0d4 = 0;
    _ZN5Model8LoadFileER13SharedFilePtr(data_ov020_02114aa0);
    _ZN5Model8LoadFileER13SharedFilePtr(data_ov020_02114ab8);
    LoadBlueCoinModel(((char *)this));
    return 1;
}
