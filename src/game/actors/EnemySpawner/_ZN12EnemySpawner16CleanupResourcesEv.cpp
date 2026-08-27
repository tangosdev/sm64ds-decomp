//cpp
// @symbol _ZN12EnemySpawner16CleanupResourcesEv

#include "EnemySpawner.h"
#include "SharedFilePtr.h"

extern int data_ov002_0210d9e0[];

int EnemySpawner::CleanupResources()
{
    ((SharedFilePtr *)data_ov002_0210d9e0)->Release();
    return 1;
}
