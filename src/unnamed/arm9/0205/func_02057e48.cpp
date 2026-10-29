//cpp
#include "types.h"
#include "IRQ.h"

extern "C" {

typedef struct {
    u32 unk[4];
    u32 unk10;
} IRQState;

extern IRQState data_020a6134; // 0x020a6134

u32 func_02057e48(u32 newVal) {
    u32 r5 = newVal;
    u32 saved = IRQ::Disable();
    u32 r4 = data_020a6134.unk10;
    data_020a6134.unk10 = r5;
    IRQ::Restore(saved);
    return r4;
}
}
