//cpp
// @symbol _ZN9ModelBaseD2Ev
/* recovered: real C++ ModelBase destructor, D2 retained by objisolate
 *
 * Only releasing the owned model file belongs in the source body. mwccarm
 * emits the ModelBase vptr reset and this-return convention around it.
 */
#include "ModelBase.h"

extern "C" void Deallocate(void *ptr);

ModelBase::~ModelBase()
{
    if (modelFile)
        Deallocate(modelFile);
}
