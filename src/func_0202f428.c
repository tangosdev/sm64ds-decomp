// @symbol func_0202f428
/* recovered: renamed to Class_Method, RTTI class fields named, declarations from a shared header */
#include "decl_FaderColor.h"
#include "decl_common.h"
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "dWipe_c.h"
// recovered name: dWipe_c_AdvanceFade
/* recovered: renamed to Class_Method */
/* dWipe_c::AdvanceFade - recovered from vtable slot identity */
extern void _ZN3G2x18SetBlendBrightnessEPVtts(unsigned short *p, unsigned short a, int b);

void func_0202f428(char *obj)
{
    struct dWipe_c *self = (struct dWipe_c *)(void *)obj;
    if (self->unk_014 == 1) {
        _ZN10FaderColor11AdvanceFadeEv();
        return;
    }
    switch (self->unk_010) {
    case 0:
        return;
    case 1:
        *(int *)(obj + 0x1c) += self->unk_020;
        *(int *)(obj + 0x20) += self->unk_024;
        if (self->unk_01c >= 0x200000) {
            self->unk_01c = 0x200000;
            self->unk_00f = 0;
            func_0202fb30(obj);
            self->unk_010 = 2;
        }
        break;
    case 2:
        return;
    case 3:
        *(int *)(obj + 0x1c) += self->unk_020;
        *(int *)(obj + 0x20) += self->unk_024;
        if (self->unk_01c <= 0) {
            int b;
            self->unk_01c = 0;
            self->unk_00f = 0;
            b = (self->unk_014 == 0) ? 0x10 : -0x10;
            _ZN3G2x18SetBlendBrightnessEPVtts((unsigned short *)0x4000050, 0x3f, b);
            _ZN3G2x18SetBlendBrightnessEPVtts((unsigned short *)0x4001050, 0x3f, b);
            func_0202fb30(obj);
            self->unk_010 = 4;
        }
        break;
    case 4:
    default:
        return;
    }
    func_0202f290(obj);
}
