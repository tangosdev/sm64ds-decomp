//cpp
// @symbol _ZN5Stump6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "Stump.h"
extern "C" {
struct V { virtual void m0(); virtual void m1(); virtual void m2(); virtual void m3(); virtual void m4(); virtual int m5(int); };
}

int Stump::Render()
{
    if (mVariant == 1) return 1;
    int b = (unk_0b0 & 0x40000) != 0;
    if (b) return 1;
    ((V*)((char *)&mModelAnim))->m5(0);
    return 1;
}
