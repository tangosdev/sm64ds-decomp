//cpp
#include "types.h"
// @symbol _ZN5Stage22RenderModelTransparentEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Stage.h"
struct VBase {
    virtual void m0();
    virtual void m1();
    virtual void m2();
    virtual void m3();
    virtual void m4();
    virtual void m5(void *arg);
};

extern "C" {
extern char *data_0209f340;
}

void Stage::RenderModelTransparent()
{
    char *p = (char *)(((int)((char *)this) + 0x874));
    char *arr = *(char **)(*(char **)p + 8);
    char *q = ((char *)this) + 0x8bc;
    int i;
    for (i = 0; i < *(u8 *)(data_0209f340 + 0x14); i++, q += 0xc, arr += 0x40) {
        if (*(u8 *)(q + 4) != 0) {
            u8 *bp = *(u8 **)(arr + 0x34);
            u16 j;
            for (j = 0; j < *(u16 *)(arr + 0x30); j++) {
                char *e = *(char **)(p + 4) + *bp * 0x30;
                if ((*(int *)(e + 0x24) & 0x1f0000) != 0x1f0000)
                    *(int *)(((int)e + 0x24)) &= ~0x80000000;
                else
                    *(int *)(((int)e + 0x24)) |= 0x80000000;
                bp++;
            }
        }
    }
    ((VBase *)((char *)&mModel))->m5(&data_020755d4);
}
