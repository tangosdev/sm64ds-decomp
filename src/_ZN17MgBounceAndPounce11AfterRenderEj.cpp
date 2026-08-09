//cpp
// @symbol _ZN17MgBounceAndPounce11AfterRenderEj
/* recovered: named members + shared header, real C++ method */
#include "MgBounceAndPounce.h"

extern "C" void _ZN5Scene11AfterRenderEj(void *self, unsigned int result);

void MgBounceAndPounce::AfterRender(unsigned int arg)
{
    volatile unsigned short *reg = (volatile unsigned short *)0x04000006;
    int v = *reg;

    if (v > 0xb9 && v <= 0xc0) {
        while ((int)*reg < 0xc0) {
        }
    }

    _ZN5Scene11AfterRenderEj(this, arg);
}
