//cpp
// @symbol _ZN8dActor_cD1Ev
/* D1, the complete-object destructor. One `dActor_c::~dActor_c()` definition makes mwcc
 * emit D0, D1 and D2 together; objisolate keeps the one this file is bound to
 * by its delinks entry, so the siblings carry the same definition. */
#include "dActor_c.h"

extern "C" void func_0203b27c(void *list, void *node);
extern "C" void *data_0209b468;

dActor_c::~dActor_c()
{
    func_0203b27c(&data_0209b468, &mActorListNode);
}
