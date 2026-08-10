//cpp
// @symbol func_ov004_020b04f4
/* recovered: renamed to Class_Method, RTTI class fields named, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "dScMgBase_c.h"
// recovered name: dScMgBase_c_BeforeRender
/* recovered: renamed to Class_Method */
/* dScMgBase_c::BeforeRender - recovered from vtable slot identity */
/* The banner this carried was true of the compiler it named and stale for the one the
 * build uses. It claimed "not byte-matchable from C at mwccarm 1.2/sp2p3"; the pin is
 * 2004/b56, and under that pin the function reproduces exactly -- bytes and relocation
 * destinations both. It is enrolled and it ships in the ROM, so the byte gate has in
 * fact been checking it all along, against a file that told readers not to trust it. */
typedef short s16;

struct Scene;
extern "C" int _ZN5Scene12BeforeRenderEv(struct Scene *);


struct Ent { char pad[0x1a]; s16 f; char pad2[4]; };

extern "C" int func_ov004_020b04f4(void *c)
{
    struct dScMgBase_c *self = (struct dScMgBase_c *)(void *)c;
    int i;
    char *p;
    int i2;
    char *p2;
    int j;

    if (_ZN5Scene12BeforeRenderEv((struct Scene *)c) == 0)
        return 0;

    if (self->unk_4628 != 0) {
        func_ov004_020ae858(c);
        return 0;
    }

    if (self->unk_0f0 == 0) {
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



