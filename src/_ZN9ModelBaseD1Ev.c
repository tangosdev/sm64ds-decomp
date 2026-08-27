//cpp
// @symbol _ZN9ModelBaseD1Ev
/* D1, the complete-object destructor. One `ModelBase::~ModelBase()` definition makes mwcc
 * emit D0, D1 and D2 together; objisolate keeps the one this file is bound to
 * by its delinks entry, so the siblings carry the same definition. */
#include "ModelBase.h"

extern "C" void Deallocate(void *ptr);

ModelBase::~ModelBase()
{
    if (modelFile != 0) {
        Deallocate(modelFile);
    }
}
