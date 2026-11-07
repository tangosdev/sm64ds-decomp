//cpp
/* Thread scheduler (the Nitro SDK's os_thread.c),
 * arm9 .text 0x02057e34..0x02058568.
 *
 * pret/pokediamond's NitroSDK OS_thread.c is a later revision of this file.
 * Its functions that this ROM keeps appear here in the same source order:
 * OSi_GetUnusedThreadId, OSi_InsertThreadToList, OSi_RemoveThreadFromList,
 * OS_InitThread, OS_CreateThread, OS_ExitThread, OS_JoinThread,
 * OS_IsThreadTerminated, OS_SleepThread, OS_WakeupThread,
 * OS_WakeupThreadDirect, OS_SelectThread, OS_RescheduleThread,
 * OS_SetThreadPriority, OS_GetThreadPriority, OS_SetSwitchThreadCallback,
 * OSi_IdleThreadProc. OS_SetThreadStackWarningOffset is an SDK function pret
 * does not carry; it sits between OS_RescheduleThread and
 * OS_SetThreadPriority.
 *
 * mwccarm defers C bodies and emits them in reverse, so the ROM runs
 * bottom-up: OSi_IdleThreadProc at 0x02057e34, OSi_GetUnusedThreadId at
 * 0x02058538.
 *
 * This SDK predates thread queues as linked lists. A queue is a 16-bit mask
 * of thread ids, OSThreadInfo keeps a 16-entry id table, a thread id is the
 * first free slot of that table, and OS_ExitThread clears the thread's slot.
 * There is no scheduler lock (OS_DisableScheduler), no destructor and no
 * sleep alarm. OS_RescheduleThread is the scheduler itself, called with
 * interrupts already off; pret's interrupts-off wrapper of that name and its
 * static OSi_RescheduleThread are a later split.
 */
#include "types.h"
#include "IRQ.h"
#include "CP15.h"
#include "nitro/os/thread.h"

/* The SDK's nitro/types.h spellings; include/types.h has no BOOL. */
typedef int BOOL;
#define TRUE 1
#define FALSE 0

#define OS_THREAD_LAUNCHER_PRIORITY 16
#define OS_THREAD_PRIORITY_MAX 31

#define OS_THREAD_STATE_WAITING 0
#define OS_THREAD_STATE_READY 1
#define OS_THREAD_STATE_TERMINATED 2

#define OS_PROCMODE_IRQ 0x12

#define OSi_STACK_CHECKNUM_BOTTOM 0xfddb597dUL
#define OSi_STACK_CHECKNUM_TOP 0x7bf9dd5bUL
#define OSi_STACK_CHECKNUM_WARN 0x597dfbd9UL

#ifndef SM64DS_PLATFORM_PC
/* The ROM's layout: the idle and launcher threads sit 0x94 apart, and the
 * thread info block ends where the idle thread begins. */
typedef char OSThread_size_must_be_0x94[sizeof(OSThread) == 0x94 ? 1 : -1];
typedef char OSThreadInfo_size_must_be_0x54[sizeof(OSThreadInfo) == 0x54 ? 1 : -1];
#endif

extern "C" {

/* NitroSDK linker constants: the address is the value (the LCF assigns
 * SDK_SYS_STACKSIZE = 0 and SDK_IRQ_STACKSIZE = 0x600). */
extern void SDK_SYS_STACKSIZE(void);
extern void SDK_IRQ_STACKSIZE(void);

/* local extern: no header declares these yet. Their SDK names:
 * ARMProcessorMode is OS_GetProcMode, ARMSaveContext OS_SaveContext,
 * ARMRestoreContext OS_LoadContext, func_02058568 OS_InitContext
 * and MultiStore_Int MIi_CpuClear32; OSi_UnlockAllMutex is src/engine/os/OS_Mutex.cpp. */
extern int ARMProcessorMode(void);
extern int ARMSaveContext(void *ctx);
extern void ARMRestoreContext(void *ctx);
extern void func_02058568(void *ctx, unsigned int pc, unsigned int sp);
extern void MultiStore_Int(int val, int *dst, int len);
extern void OSi_UnlockAllMutex(OSThread *thread);

extern OSSwitchThreadCallback data_020a6128; /* OSi_SystemCallbackInSwitchThread */
extern OSThread **data_020a612c;             /* OSi_CurrentThreadPtr */
extern BOOL data_020a6130;                   /* OSi_IsThreadInitialized */
extern OSThread data_020a6188;               /* OSi_IdleThread */
/* The word just past OSi_IdleThreadStack (200 bytes from 0x020a62b0): its
 * address is the idle stack's top. The word itself is OS_Arena.cpp's
 * OSi_Initialized. */
extern u32 data_020a6378;
extern short data_023c0000[]; /* HW_DTCM */

OSSwitchThreadCallback OS_SetSwitchThreadCallback(OSSwitchThreadCallback callback);
void OSi_IdleThreadProc(void *unused);

s32 OSi_GetUnusedThreadId(void) {
    int i;
    for (i = 0; i < OS_THREAD_MAX_NUM; i++) {
        if (data_020a6134.entry[i] == 0) {
            return i;
        }
    }
    return -1;
}

void OSi_InsertThreadToList(OSThread *thread) {
    OSThread *t = data_020a6134.list;
    OSThread *pre = 0;

    while (t != 0 && t->priority < thread->priority) {
        pre = t;
        t = t->next;
    }

    if (pre == 0) {
        thread->next = data_020a6134.list;
        data_020a6134.list = thread;
    } else {
        thread->next = pre->next;
        pre->next = thread;
    }
}

void OSi_RemoveThreadFromList(OSThread *thread) {
    OSThread *t = data_020a6134.list;
    OSThread *pre = 0;

    while (t != 0 && t != thread) {
        pre = t;
        t = t->next;
    }

    if (pre == 0) {
        data_020a6134.list = thread->next;
    } else {
        pre->next = thread->next;
    }
}

void OS_InitThread(void) {
    int i;
    u32 stackLo;
    u32 stackHi;

    if (data_020a6130) {
        return;
    }
    data_020a6130 = TRUE;

    for (i = 0; i < OS_THREAD_MAX_NUM; i++) {
        data_020a6134.entry[i] = 0;
    }

    data_020a612c = &data_020a6134.current;

    data_020a621c.priority = OS_THREAD_LAUNCHER_PRIORITY;
    data_020a621c.id = 0;
    data_020a621c.state = OS_THREAD_STATE_READY;
    data_020a621c.next = 0;
    data_020a621c.profiler = 0;

    data_020a6134.entry[0] = &data_020a621c;
    data_020a6134.list = &data_020a621c;
    data_020a6134.current = &data_020a621c;

    if ((s32)SDK_SYS_STACKSIZE == 0) {
        stackLo = 0x23c0020 - (s32)SDK_SYS_STACKSIZE;
    } else {
        stackLo = (u32)data_023c0000 + 0x3fc0 - (s32)SDK_IRQ_STACKSIZE - (s32)SDK_SYS_STACKSIZE;
    }
    stackHi = (u32)data_023c0000 + 0x3fc0 - (s32)SDK_IRQ_STACKSIZE;

    data_020a621c.stackBottom = stackHi;
    data_020a621c.stackTop = stackLo;
    data_020a621c.stackWarningOffset = 0;
    *(u32 *)(stackHi - sizeof(u32)) = OSi_STACK_CHECKNUM_BOTTOM;
    *(u32 *)data_020a621c.stackTop = OSi_STACK_CHECKNUM_TOP;
    data_020a621c.joinQueue = 0;

    data_020a6134.max_entry = OS_THREAD_MAX_NUM;
    data_020a6134.isNeedRescheduling = FALSE;
    data_020a6134.irqDepth = 0;
    *(OSThreadInfo **)0x027fffa0 = &data_020a6134;

    OS_SetSwitchThreadCallback(0);

    OS_CreateThread(&data_020a6188, OSi_IdleThreadProc, 0, &data_020a6378, 200,
                    OS_THREAD_PRIORITY_MAX);

    data_020a6188.priority = OS_THREAD_PRIORITY_MAX + 1;
    data_020a6188.state = OS_THREAD_STATE_READY;
}

void OS_CreateThread(OSThread *thread, void (*func)(void *), void *arg, void *stack, u32 stackSize,
                     u32 prio) {
    u32 enabled = IRQ::Disable();
    s32 index = OSi_GetUnusedThreadId();
    if (index < 0) {
        IRQ::Restore(enabled);
        return;
    }

    thread->priority = prio;
    thread->id = index;
    thread->state = OS_THREAD_STATE_WAITING;
    thread->profiler = 0;
    data_020a6134.entry[index] = thread;
    OSi_InsertThreadToList(thread);

    thread->stackBottom = (u32)stack;
    thread->stackTop = (u32)stack - stackSize;
    thread->stackWarningOffset = 0;
    *(u32 *)(thread->stackBottom - sizeof(u32)) = OSi_STACK_CHECKNUM_BOTTOM;
    *(u32 *)thread->stackTop = OSi_STACK_CHECKNUM_TOP;
    thread->joinQueue = 0;

    func_02058568(&thread->context, (u32)func, (u32)stack - 4);
    thread->context.r[0] = (u32)arg;
    thread->context.lr = (u32)OS_ExitThread;

    {
        /* MI_CpuClear32's value: the ROM passes it through a stack slot. */
        volatile int fill = 0;
        MultiStore_Int(fill, (int *)((u32)stack - stackSize + 4), stackSize - 8);
    }

    thread->mutex = 0;
    thread->mutexQueue.head = 0;
    thread->mutexQueue.tail = 0;
    IRQ::Restore(enabled);
}

void OS_ExitThread(void) {
    OSThread *currentThread;

    IRQ::Disable();
    currentThread = *data_020a612c;
    OSi_UnlockAllMutex(currentThread);
    OSi_RemoveThreadFromList(currentThread);
    data_020a6134.entry[currentThread->id] = 0;
    currentThread->state = OS_THREAD_STATE_TERMINATED;
    OS_WakeupThread(&currentThread->joinQueue);
    OS_RescheduleThread();
}

void OS_JoinThread(OSThread *thread) {
    u32 enabled = IRQ::Disable();
    if (thread->state != OS_THREAD_STATE_TERMINATED) {
        OS_SleepThread(&thread->joinQueue);
    }
    IRQ::Restore(enabled);
}

BOOL OS_IsThreadTerminated(const OSThread *thread) {
    return thread->state == OS_THREAD_STATE_TERMINATED;
}

void OS_SleepThread(u16 *queue) {
    u32 enabled = IRQ::Disable();
    OSThread *currentThread = *data_020a612c;
    if (queue != 0) {
        *queue = *queue | (u16)(1 << currentThread->id);
    }
    currentThread->state = OS_THREAD_STATE_WAITING;
    OS_RescheduleThread();
    IRQ::Restore(enabled);
}

void OS_WakeupThread(u16 *queue) {
    u32 enabled = IRQ::Disable();
    u32 mask = *queue;
    if (mask != 0) {
        OSThread *thread;
        for (thread = data_020a6134.list; thread != 0; thread = thread->next) {
            if (mask & (1 << thread->id)) {
                thread->state = OS_THREAD_STATE_READY;
            }
        }
        *queue = 0;
        OS_RescheduleThread();
    }
    IRQ::Restore(enabled);
}

void OS_WakeupThreadDirect(OSThread *thread) {
    u32 enabled = IRQ::Disable();
    thread->state = OS_THREAD_STATE_READY;
    OS_RescheduleThread();
    IRQ::Restore(enabled);
}

OSThread *OS_SelectThread(void) {
    OSThread *thread = data_020a6134.list;
    while (thread != 0 && thread->state != OS_THREAD_STATE_READY) {
        thread = thread->next;
    }
    return thread;
}

void OS_RescheduleThread(void) {
    OSThread *currentThread;
    OSThread *nextThread;
    OSThreadInfo *info = &data_020a6134;

    if (info->irqDepth != 0 || ARMProcessorMode() == OS_PROCMODE_IRQ) {
        info->isNeedRescheduling = TRUE;
        return;
    }

    currentThread = *data_020a612c;
    nextThread = OS_SelectThread();
    if (currentThread == nextThread || nextThread == 0) {
        return;
    }
    if (ARMSaveContext(&currentThread->context) != 0) {
        return;
    }
    if (data_020a6128 != 0) {
        data_020a6128(currentThread, nextThread);
    }
    if (info->switchCallback != 0) {
        info->switchCallback(currentThread, nextThread);
    }
    data_020a6134.current = nextThread;
    ARMRestoreContext(&nextThread->context);
}

void OS_SetThreadStackWarningOffset(OSThread *thread, u32 offset) {
    thread->stackWarningOffset = offset;
    *(u32 *)(thread->stackTop + thread->stackWarningOffset) = OSi_STACK_CHECKNUM_WARN;
}

BOOL OS_SetThreadPriority(OSThread *thread, u32 prio) {
    OSThread *t = data_020a6134.list;
    OSThread *pre = 0;
    u32 enabled = IRQ::Disable();

    while (t != 0 && t != thread) {
        pre = t;
        t = t->next;
    }

    if (t == 0 || t == &data_020a6188) {
        IRQ::Restore(enabled);
        return FALSE;
    }

    if (t->priority != prio) {
        if (pre == 0) {
            data_020a6134.list = thread->next;
        } else {
            pre->next = thread->next;
        }
        thread->priority = prio;
        OSi_InsertThreadToList(thread);
        OS_RescheduleThread();
    }

    IRQ::Restore(enabled);
    return TRUE;
}

u32 OS_GetThreadPriority(const OSThread *thread) {
    return thread->priority;
}

OSSwitchThreadCallback OS_SetSwitchThreadCallback(OSSwitchThreadCallback callback) {
    u32 enabled = IRQ::Disable();
    OSSwitchThreadCallback prev = data_020a6134.switchCallback;
    data_020a6134.switchCallback = callback;
    IRQ::Restore(enabled);
    return prev;
}

void OSi_IdleThreadProc(void *unused) {
#pragma unused(unused)
    IRQ::Enable();
    for (;;) {
        CP15::WaitForInterrupt();
    }
}
}
