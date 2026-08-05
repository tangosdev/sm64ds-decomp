//cpp
// @symbol _ZN10ShutterBob13InitResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "ShutterBob.h"
class Actor {};
class MeshColliderBase {
public:
    void Enable(Actor *a);
};

extern "C" {
extern int func_ov002_020bad10(void *c, void **f);
}
extern int data_ov014_021145c4;

int ShutterBob::InitResources()
{
    int r4 = func_ov002_020bad10(((char *)this), (void **)&data_ov014_021145c4);
    ((MeshColliderBase *)((char *)&mMovingMeshCollider))->Enable((Actor *)((char *)this));
    return r4;
}
