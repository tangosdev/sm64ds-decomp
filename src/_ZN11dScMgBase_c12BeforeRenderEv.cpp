//cpp
// @symbol _ZN11dScMgBase_c12BeforeRenderEv
#include "decl_common.h"
#include "dScMgBase_c.h"
// recovered name: dScMgBase_c_BeforeRender
/* dScMgBase_c::BeforeRender - recovered from vtable slot identity. */
typedef short s16;

struct Ent { char pad[0x1a]; s16 f; char pad2[4]; };

int dScMgBase_c::BeforeRender()
{
    char *c = (char *)this;
    int i; char *p; int i2; char *p2; int j;

    if (Scene::BeforeRender() == 0)
        return 0;

    if (unk_4628 != 0) {
        func_ov004_020ae858(c);
        return 0;
    }

    if (unk_0f0 == 0) {
        p = data_ov004_020bf648;
        for (i = 0; i < 3; i++, p += 0x134) {
            if (*(int *)(p + 0x20) == 0x1d) continue;
            if (*(s16 *)(p + 0x30) != 0) continue;
            func_ov004_020b31b4(p);
        }
        p2 = data_ov004_020bf648;
        for (i2 = 0; i2 < 3; i2++, p2 += 0x134) {
            if (*(int *)(p2 + 0x20) == 0x1d) continue;
            if (*(s16 *)(p2 + 0x30) == 0) continue;
            func_ov004_020b31b4(p2);
        }
        func_ov004_020b8714((char *)c + 0xcc);
    }

    p = data_ov004_020bebe8;
    for (j = 0; j < 0x40; j++, p += 0x20) {
        if (((struct Ent *)data_ov004_020bebe8)[j].f != 0)
            func_ov004_020add88(p);
    }

    func_ov004_020b0de0(c);
    return 1;
}
