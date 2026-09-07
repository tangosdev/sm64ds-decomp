//cpp
// @symbol _ZN9FaderWipe14LoadAndSetFileEt
/* recovered: named members + shared header, real C++ method */
#include "FaderWipe.h"

/* This used to reach the model as `(char*)this + 0x10` with Model declared
   nowhere. Going through the member instead is the same bytes and turns the
   offset into a checkable claim: if `model` were at any other offset, or Model
   any other size, this would stop matching. */
void FaderWipe::LoadAndSetFile(u16 ov0ID)
{
    model.LoadAndSetFile(ov0ID, 0, -1);
}
