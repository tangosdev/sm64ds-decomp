//cpp
// @symbol _ZN9ModelBaseD2Ev
/* D2, the base-object destructor. Same definition as the D1 file, and
 * deliberately so: ModelBase has no virtual bases, so mwcc emits D1 and D2 as
 * byte-identical code. Only how the ROM REACHES an address separates them --
 * a vtable slot holds D1, a derived destructor's base-chain `bl` reaches D2. */
#include "ModelBase.h"

extern "C" void Deallocate(void *ptr);

ModelBase::~ModelBase()
{
    if (modelFile != 0) {
        Deallocate(modelFile);
    }
}
