//cpp
// @symbol _ZN3IRQ13SetIRQHandlerEjPFvvE
// The compiler now owns the namespace symbol; only the ROM's unmangled data
// objects retain C linkage.
#include "IRQ.h"

extern "C" {
extern IRQ::Handler data_02099fe4[];
extern IRQ::HandlerEntry data_020a60c4[];
}

namespace IRQ {

void SetIRQHandler(u32 mask, Handler handler)
{
    for (int i = 0; i < 0x16; i++) {
        if (mask & 1) {
            HandlerEntry* entry = 0;
            if (i >= 8 && i <= 0xb)
                entry = &data_020a60c4[i - 8];
            else if (i >= 3 && i <= 6)
                entry = &data_020a60c4[i + 1];
            else
                data_02099fe4[i] = handler;

            if (entry) {
                entry->handler = handler;
                entry->argument = 0;
                entry->enabled = 1;
            }
        }
        mask >>= 1;
    }
}

}
