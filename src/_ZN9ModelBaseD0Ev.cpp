//cpp
// @symbol _ZN9ModelBaseD0Ev
/* recovered: deleting ABI variant emitted from the real C++ destructor
 *
 * The source body owns only the model-file release. mwccarm supplies the vptr
 * reset and routes deletion through ModelBase::operator delete, which is why
 * this definition also reproduces the ROM's Memory::operator_delete2 call.
 */
#include "ModelBase.h"

extern "C" void Deallocate(void *ptr);

ModelBase::~ModelBase()
{
    if (modelFile)
        Deallocate(modelFile);
}
