//cpp
#include "types.h"
#include "nitro/os/system.h"

extern "C" {

typedef struct {
    char pad[0xb50];
    u32 field_b50;
} UnkStruct_020a3fc0;

extern UnkStruct_020a3fc0 data_020a3fc0;

/* Store val with bit 0 clear at +0xb50 of data_020a3fc0, under the IRQ
   lock. Its one caller, func_0203ea5c, passes it func_02040a84's result
   when there are more than two players (data_02099e18). */
void func_02040a5c(u32 val)
{
    OSIntrMode saved = OS_DisableInterrupts();
    data_020a3fc0.field_b50 = val & ~1u;
    OS_RestoreInterrupts(saved);
}
}
