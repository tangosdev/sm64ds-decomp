//cpp
// @symbol _ZN9ModelBaseD1Ev
/* recovered: real C++ ModelBase destructor, D1 retained by objisolate
 *
 * The compiler owns the ABI scaffolding. The period-authentic source body is
 * just the release of ModelBase's owned model file.
 */
#include "ModelBase.h"

extern "C" void Deallocate(void *ptr);

ModelBase::~ModelBase()
{
    if (modelFile)
        Deallocate(modelFile);
}
