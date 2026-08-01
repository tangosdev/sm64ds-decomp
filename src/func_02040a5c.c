#include "types.h"
extern unsigned int _ZN3IRQ7DisableEv(void);
extern void _ZN3IRQ7RestoreEj(unsigned int savedState);
typedef struct {
    char pad[0xb50];
    u32 field_b50;
} UnkStruct_020a3fc0;

extern UnkStruct_020a3fc0 data_020a3fc0;

void func_02040a5c(u32 val)
{
    u32 saved = _ZN3IRQ7DisableEv();
    data_020a3fc0.field_b50 = val & ~1u;
    _ZN3IRQ7RestoreEj(saved);
}
