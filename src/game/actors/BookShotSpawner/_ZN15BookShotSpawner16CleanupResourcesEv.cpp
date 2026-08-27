//cpp
// @symbol _ZN15BookShotSpawner16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "BookShotSpawner.h"
class SharedFilePtr {
public:
    void Release();
};

extern "C" {
extern void UnloadBlueCoinModel(char *c);
}
extern int data_ov020_02114aa0;
extern int data_ov020_02114ab8;

int BookShotSpawner::CleanupResources()
{
    ((SharedFilePtr *)&data_ov020_02114aa0)->Release();
    ((SharedFilePtr *)&data_ov020_02114ab8)->Release();
    UnloadBlueCoinModel(((char *)this));
    return 1;
}
