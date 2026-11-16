//cpp
#include "Heap.h"
#include "nitro/os/exception.h"
#include "nitro/os/thread.h"
#include "nitro/os/alarm.h"

extern "C" void OS_InitTick(void);
extern "C" void OS_SetUserExceptionHandler(OSExceptionHandler handler, void *arg);
extern void func_0201a054(void);
extern void func_020197b8(void);
extern "C" {

/* The game's exception handler: it takes the context and ignores arg, which
 * NitroMain passes as the launcher thread (the thread NitroMain runs on). */
extern void func_0201a03c(u32 context, void *arg);
}

extern mHeap::Heap_t *data_020a0e9c;

void NitroMain(void) {
    OS_InitTick();
    OS_InitAlarm();
    OS_SetUserExceptionHandler(func_0201a03c, &OSi_LauncherThread);
    data_020a0e9c->SetNodeID(2);
    func_0201a054();
    data_020a0e9c->SetNodeID(3);
    func_020197b8();
}
