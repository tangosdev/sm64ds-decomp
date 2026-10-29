//cpp
#include "types.h"
#include "IRQ.h"

extern "C" {

typedef struct {
    char pad[0xb50];
    u32 field_b50;
} UnkStruct_020a3fc0;

extern UnkStruct_020a3fc0 data_020a3fc0;

void func_02040a5c(u32 val)
{
    u32 saved = IRQ::Disable();
    data_020a3fc0.field_b50 = val & ~1u;
    IRQ::Restore(saved);
}
}
