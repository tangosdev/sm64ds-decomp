//cpp
// @symbol _ZN5ModelD0Ev
/* The compiler emits the deleting destructor, including the base teardown and
 * ModelBase's class-specific operator delete. objisolate retains D0 here.
 */
#include "Model.h"

extern "C" void func_0203cbc0(void *ptr);

Model::~Model()
{
    if (transformsBuf)
        func_0203cbc0(transformsBuf);
}
