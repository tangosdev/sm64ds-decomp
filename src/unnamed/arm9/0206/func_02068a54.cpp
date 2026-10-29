//cpp
#include "types.h"
#include "IRQ.h"

extern "C" {

extern void func_02068a74(void);

/* func_02068a54 - call func_02068a74 with IRQ disabled */
void func_02068a54(void)
{
    u32 saved = IRQ::Disable();
    func_02068a74();
    IRQ::Restore(saved);
}
}
