//cpp
// @symbol _ZN8CapEnemy12Unk_02005d94Ev
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "CapEnemy.h"
extern unsigned char data_0209f2d8;

void CapEnemy::Unk_02005d94()
{
    int b1;
    int b2;
    if (!(((unsigned char *)this)[0x113] & 0x80)) return;
    b1 = (mFlags & 8) ? 1 : 0;
    if (b1 == 0) {
        volatile unsigned char *q = &data_0209f2d8;
        b2 = (q[0] == 1) ? 1 : 0;
        if (b2 == 0) return;
    }
    *(unsigned char *)((int)((unsigned char *)this) + 0x113) &= 7;
    if (func_02005e28(((unsigned char *)this)))
        *(unsigned char *)((int)((unsigned char *)&mCapId)) |= 0x80;
}
