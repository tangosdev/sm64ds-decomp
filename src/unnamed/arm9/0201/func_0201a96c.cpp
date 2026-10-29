//cpp
#include "types.h"
#include "IRQ.h"

extern "C" {

typedef struct {
    char pad1[0x30];
    u32  field_0x30;  /* 0x30 */
    u32  field_0x34;  /* 0x34 */
    char pad2[0x8];
    u8   field_0x40;  /* 0x40 */
} Struct_0201a96c;


extern void func_0201aa50(Struct_0201a96c *self, u32 a, u32 b, void *funcPtr, Struct_0201a96c *ctx);
extern void func_0201aaa4(Struct_0201a96c *self);

extern void func_0201a8b4(void);

/* func_0201a96c - set field_0x40 (bool), fire on-change callbacks */
u8 func_0201a96c(Struct_0201a96c *self, u8 newVal)
{
    u32 saved = IRQ::Disable();
    u8 oldVal = self->field_0x40;
    self->field_0x40 = newVal;

    if (oldVal != 0 && newVal == 0) {
        /* going false: notify */
        func_0201aa50(self, self->field_0x30, self->field_0x34, func_0201a8b4, self);
    } else if (oldVal == 0 && newVal != 0) {
        /* going true: notify */
        func_0201aaa4(self);
    }

    IRQ::Restore(saved);
    return oldVal;
}
}
