//cpp
// @symbol _ZN17MgBounceAndPounce11AfterRenderEj
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "MgBounceAndPounce.h"

void MgBounceAndPounce::AfterRender(unsigned int arg)
{
    volatile unsigned short *reg = (volatile unsigned short *)0x04000006;
    int v = *reg;

    if (v > 0xb9 && v <= 0xc0) {
        while ((int)*reg < 0xc0) {
        }
    }

    Scene_AfterRender(((void *)this), arg);
}
