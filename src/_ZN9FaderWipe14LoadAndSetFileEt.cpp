//cpp
// @symbol _ZN9FaderWipe14LoadAndSetFileEt
/* recovered: named members + shared header, real C++ method */
#include "FaderWipe.h"
extern "C" void _ZN5Model14LoadAndSetFileEtii(void* m, unsigned short id, int a, int b);

void FaderWipe::LoadAndSetFile(unsigned short id)
{
    _ZN5Model14LoadAndSetFileEtii((char*)((void*)this) + 0x10, id, 0, -1);
}
