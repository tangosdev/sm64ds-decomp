//cpp
#include "types.h"
#include "IRQ.h"

extern "C" {

void func_0205b470(void *arg);

void func_0205aed0(void *arg0, void *arg1) {
    u32 r4 = IRQ::Disable();
    func_0205b470(arg1);
    IRQ::Restore(r4);
}
}
