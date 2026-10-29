//cpp
#include "CP15.h"
#include "IRQ.h"

extern "C" {

// func_02057e34 - idle loop: enable IRQ once, then wait for interrupt forever

void func_02057e34(void) {
    IRQ::Enable();
    for (;;) {
        CP15::WaitForInterrupt();
    }
}
}
