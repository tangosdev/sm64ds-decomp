#include "types.h"
extern u32 _ZN3IRQ7DisableEv(void);
extern void _ZN3IRQ7RestoreEj(u32 prev);

typedef struct {
    u32 unk[4];
    u32 unk10;
} IRQState;

extern IRQState data_020a6134; // 0x020a6134

u32 func_02057e48(u32 newVal) {
    u32 r5 = newVal;
    u32 saved = _ZN3IRQ7DisableEv();
    u32 r4 = data_020a6134.unk10;
    data_020a6134.unk10 = r5;
    _ZN3IRQ7RestoreEj(saved);
    return r4;
}
