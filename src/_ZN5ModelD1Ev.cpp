//cpp
// @symbol _ZN5ModelD1Ev
/* recovered: real C++ destructor
 *
 * Model owns the transformed-vertex buffer allocated by DoSetFile. Releasing
 * that buffer through the game's delete veneer is the only part of destruction
 * written by this class: C++ emits the Model vtable restore before the body and
 * the ModelBase destructor call after it. The class-specific operator delete
 * inherited from ModelBase also gives the compiler-generated deleting
 * destructor its ROM callee.
 */
#include "Model.h"

extern "C" void func_0203cbc0(void *ptr);

Model::~Model()
{
    if (transformsBuf)
        func_0203cbc0(transformsBuf);
}
