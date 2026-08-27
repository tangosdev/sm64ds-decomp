#include "types.h"
extern void *data_ov006_0213d564;
extern u8 data_020a0e40[];
extern u8 data_020a0de8[];
extern u8 data_020a0de9[];
extern u8 data_020a0dea[];
extern u8 data_020a0deb[];
extern int func_ov006_020f7b10(void);

struct Obj { char pad[0xc]; int fc; int f10; };

int func_ov006_020f8154(struct Obj *c)
{
    u8 idx;
    int off;
    int has;
    int a, b;

    if (data_ov006_0213d564 == 0) return 0;
    if (func_ov006_020f7b10() != 0) goto fail;

    idx = data_020a0e40[0];
    off = idx * 4;
    has = 0;
    if (data_020a0de8[off]) {
        if (data_020a0de9[off]) has = 1;
    }
    if (has == 0) goto fail;

    a = data_020a0dea[idx * 4] - (c->fc >> 12);
    b = data_020a0deb[idx * 4] - (c->f10 >> 12);
    if (a > 7 && a < 0x28 && b > 0 && b < 0x31) return 1;
fail:
    return 0;
}
