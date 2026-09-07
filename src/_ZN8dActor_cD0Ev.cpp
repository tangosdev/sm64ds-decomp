//cpp
// @symbol _ZN8dActor_cD0Ev
/* D0, the DELETING destructor: run the destructor body, then hand the object
 * to operator delete. Longer than the D1/D2 pair, but built from the same one
 * `dActor_c::~dActor_c()` definition. */
#include "dActor_c.h"

extern "C" void func_0203b27c(void *list, void *node);
extern "C" void *data_0209b468;

dActor_c::~dActor_c()
{
    func_0203b27c(&data_0209b468, &mActorListNode);
}
