//cpp
// @symbol _ZN14dScMgD3DBase_c11AfterRenderEj
/* recovered: named members + shared header, real C++ method */
#include "dScMgD3DBase_c.h"

extern "C" void _ZN5Scene11AfterRenderEj(void *self, unsigned int result);

void dScMgD3DBase_c::AfterRender(unsigned int arg)
{
    volatile unsigned short *reg = (volatile unsigned short *)0x04000006;
    int v = *reg;

    if (v > 0xb9 && v <= 0xc0) {
        while ((int)*reg < 0xc0) {
        }
    }

    _ZN5Scene11AfterRenderEj(this, arg);
}
