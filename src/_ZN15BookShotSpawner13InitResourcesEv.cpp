//cpp
// @symbol _ZN15BookShotSpawner13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "BookShotSpawner.h"
extern void _ZN5Model8LoadFileER13SharedFilePtr(void *);
extern void LoadBlueCoinModel(void *);
extern int G0[];

int BookShotSpawner::InitResources()
{
    unk_0d4 = 0;
    _ZN5Model8LoadFileER13SharedFilePtr(G0);
    _ZN5Model8LoadFileER13SharedFilePtr(G1);
    LoadBlueCoinModel(((char *)this));
    return 1;
}
