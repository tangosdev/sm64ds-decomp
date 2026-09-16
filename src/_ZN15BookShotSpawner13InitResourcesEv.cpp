//cpp
#include "Model.h"
// @symbol _ZN15BookShotSpawner13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "BookShotSpawner.h"
extern int data_ov020_02114ab8[];
extern "C" {
extern void LoadBlueCoinModel(void *);
}
extern int data_ov020_02114aa0[];

int BookShotSpawner::InitResources()
{
    mSpawnTimer = 0;
    Model::LoadFile(*(SharedFilePtr *)data_ov020_02114aa0);
    Model::LoadFile(*(SharedFilePtr *)data_ov020_02114ab8);
    LoadBlueCoinModel(((char *)this));
    return 1;
}
