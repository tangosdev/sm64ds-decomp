//cpp
#include "types.h"
#include "IRQ.h"

extern "C" {

/* NitroSDK OS_CreateThread(thread, func, arg, stack, stackSize, prio):
 * take a free thread id (func_02058538), file the thread in the id table
 * (data_020a6148) and the priority list (func_020584d0), mark both ends of
 * the stack with the SDK's check words, build the start context
 * (func_02058568 = OS_InitContext) with arg in r0 and OS_ExitThread
 * (func_020581a8) as the return address, clear the stack, and leave the
 * thread WAITING. Field names follow the SDK's OSThread of this era, whose
 * joinQueue is still the 16-bit thread mask; func_02058308 (OS_InitThread)
 * fills the same fields for the launcher thread. Only the fields set here are
 * spelled out. */
typedef struct OSContext {
    u32 cpsr;            /* 0x00 */
    u32 r[13];           /* 0x04 */
    u32 sp;              /* 0x38 */
    u32 lr;              /* 0x3c */
    u32 pc_plus4;        /* 0x40 */
    u32 sp_svc;          /* 0x44 */
    u8 cp_context[0x1c]; /* 0x48 -- divider / sqrt state */
} OSContext;

typedef struct OSMutexQueue {
    void *head; /* 0x00 */
    void *tail; /* 0x04 */
} OSMutexQueue;

typedef struct OSThread {
    OSContext context;       /* 0x00 */
    s32 state;               /* 0x64 -- 0 WAITING, 1 READY */
    struct OSThread *next;   /* 0x68 */
    u32 id;                  /* 0x6c */
    u32 priority;            /* 0x70 */
    void *profiler;          /* 0x74 */
    void *mutex;             /* 0x78 */
    OSMutexQueue mutexQueue; /* 0x7c */
    u32 stackTop;            /* 0x84 */
    u32 stackBottom;         /* 0x88 */
    u32 stackWarningOffset;  /* 0x8c */
    u16 joinQueue;           /* 0x90 */
} OSThread;

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm: OSContext is 0x64 bytes and joinQueue sits at 0x90. */
typedef char OSContext_size_must_be_0x64[sizeof(OSContext) == 0x64 ? 1 : -1];
typedef char OSThread_joinQueue_must_be_at_0x90[(int)&((OSThread *)0)->joinQueue == 0x90 ? 1 : -1];
#endif

extern int func_02058538(void);
extern void func_020584d0(void *node);
extern void func_02058568(void *a, int b, int c);
extern void MultiStore_Int(int val, int *dst, int len);
extern void func_020581a8(void);
extern int data_020a6148[];

void func_02058200(char *self, int a1, int a2, int end, int arg4, int arg5)
{
    OSThread *thread = (OSThread *)self;
    u32 saved;
    int id;
    char *base;
    volatile int zero;
    saved = IRQ::Disable();
    id = func_02058538();
    if (id < 0) {
        IRQ::Restore(saved);
        return;
    }
    thread->priority = arg5;
    thread->id = id;
    thread->state = 0;
    thread->profiler = 0;
    data_020a6148[id] = (int)self;
    func_020584d0(self);
    base = (char*)(end - arg4);
    thread->stackBottom = end;
    thread->stackTop = (u32)base;
    thread->stackWarningOffset = 0;
    *(int *)(thread->stackBottom - 4) = (int)0xfddb597d;
    *(int *)(thread->stackTop) = 0x7bf9dd5b;
    thread->joinQueue = 0;
    func_02058568(self, a1, end - 4);
    thread->context.r[0] = a2;
    thread->context.lr = (u32)&func_020581a8;
    zero = 0;
    MultiStore_Int(zero, (int*)(base + 4), arg4 - 8);
    thread->mutex = 0;
    thread->mutexQueue.head = 0;
    thread->mutexQueue.tail = 0;
    IRQ::Restore(saved);
}
}
