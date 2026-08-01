//cpp
// @symbol func_ov030_02111384
/* recovered: renamed to Class_Method, RTTI class fields named, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "daObjHmBskt_c.h"
// recovered name: daObjHmBskt_c_Behavior
/* recovered: renamed to Class_Method */
/* daObjHmBskt_c::Behavior - recovered from vtable slot identity */
extern "C" int _ZN8Platform21IsClsnInRangeOnScreenE5Fix12IiES1_(char *c, int a, int b);
extern "C" void _ZN5Actor9UpdatePosEP12CylinderClsn(char *c, char *clsn);
extern "C" int func_ov030_02111384(char *c) {
    struct daObjHmBskt_c *self = (struct daObjHmBskt_c *)(void *)c;
    volatile int dummy[4];
    (void)&dummy;
    _ZN8Platform21IsClsnInRangeOnScreenE5Fix12IiES1_(c, 0, 0);
    self->unk_08e = self->unk_08e + self->unk_098;
    if (self->unk_09c != 0) {
        *(int *)(((int)c + 0xb0)) = *(int *)(((int)c + 0xb0)) & ~1;
        _ZN5Actor9UpdatePosEP12CylinderClsn(c, (char*)0);
        func_ov030_0211124c(c, c+0x320);
        int a0 = self->unk_060;
        int a1 = self->unk_064;
        int m = self->unk_4dc;
        int a2 = self->unk_05c;
        *(int*)(m + 0x5c) = a2;
        *(int*)(m + 0x60) = a0 + 0x3c000;
        *(int*)(m + 0x64) = a1;
    }
    return 1;
}
