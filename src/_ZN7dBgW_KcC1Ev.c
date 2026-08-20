#include "types.h"
/* _ZN7dBgW_KcC1Ev - dBgW_Kc constructor.
 * Attempt 3: constructor returns self (C++ C1 ctors return this).
 * ROM has mov r0,r4 before final str+epilogue - this is "return self" pattern.
 * Adding return self fixes: pool offset (0x18->0x1c) AND mov r0,r4 at +0x20.
 */
struct dBgW { u32 _pad[8]; }; /* sizeof=0x20 */

extern void _ZN4dBgWC2Ev(struct dBgW* self);

extern void func_02038228(u32* obj);

extern void* _ZTV7dBgW_Kc;

struct dBgW_Kc {
    void* vtable;                 /* 0x00 */
    u32 pad04[7];                 /* 0x04 - 0x1f */
    u32 fld20;                    /* 0x20 */
    u32 fld24;                    /* 0x24 */
    u32 pad28[10];               /* 0x28 - 0x4f */
};

struct dBgW_Kc* _ZN7dBgW_KcC1Ev(struct dBgW_Kc* self)
{
    _ZN4dBgWC2Ev((struct dBgW*)self);
    self->vtable = &_ZTV7dBgW_Kc;
    func_02038228(&self->fld24);
    self->fld20 = 0;
    return self;
}