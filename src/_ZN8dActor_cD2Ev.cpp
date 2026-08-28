//cpp
// @symbol _ZN8dActor_cD2Ev
/* D2, the base-object destructor. Same definition as the D1 file, and
 * deliberately so: dActor_c has no virtual bases, so mwcc emits D1 and D2 as
 * byte-identical code. Only how the ROM REACHES an address separates them --
 * a vtable slot holds D1, a derived destructor's base-chain `bl` reaches D2. */
#include "dActor_c.h"

extern "C" void func_0203b27c(void *list, void *node);
extern "C" void func_02044104(void *node);
extern "C" void *data_0209b468;

dActor_c::~dActor_c()
{
    func_0203b27c(&data_0209b468, (char *)this + 0x50);
    func_02044104(&mListPrev);
}
