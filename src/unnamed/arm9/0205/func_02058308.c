#include "types.h"

/* NitroSDK OS_InitThread(). File the launcher thread (data_020a621c) as the
 * current thread, mark its stack (the system stack under the IRQ stack) with
 * the SDK's check words, publish the thread info block at 0x027fffa0, clear the
 * switch callback (func_02057e48 = OS_SetSwitchThreadCallback) and create the
 * idle thread (data_020a6188) through func_02058200 (OS_CreateThread).
 *
 * The OSThread view copies func_02058200.cpp's names (only the fields used here
 * are spelled out). OSThreadInfo is the SDK's non-SDK_THREAD_INFINITY layout:
 * func_02058538 scans entry[] for a free id, func_02057e84 / func_020584d0
 * walk the priority list from +0x0c, and func_02057e48 swaps the callback at
 * +0x10. */
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

typedef struct OSThreadInfo {
    u16 isNeedRescheduling; /* 0x00 */
    u16 max_entry;          /* 0x02 */
    u16 irqDepth;           /* 0x04 */
    u16 padding;            /* 0x06 */
    OSThread *current;      /* 0x08 */
    OSThread *list;         /* 0x0c -- priority-sorted, linked through next */
    void *switchCallback;   /* 0x10 */
    OSThread *entry[16];    /* 0x14 -- indexed by thread id */
} OSThreadInfo;

#ifndef SM64DS_PLATFORM_PC
typedef char OSThread_joinQueue_must_be_at_0x90[(int)&((OSThread *)0)->joinQueue == 0x90 ? 1 : -1];
typedef char OSThreadInfo_entry_must_be_at_0x14[(int)&((OSThreadInfo *)0)->entry == 0x14 ? 1 : -1];
#endif

/* NitroSDK linker constants: the address is the value (the LCF assigns
 * SDK_SYS_STACKSIZE = 0 and SDK_IRQ_STACKSIZE = 0x600). */
extern void SDK_SYS_STACKSIZE(void);
extern void SDK_IRQ_STACKSIZE(void);

extern int data_020a6130;
extern int data_020a6134[];
extern char data_020a621c[];
extern char data_020a613c[];
extern void *data_020a612c;
extern char data_023c0000;
extern char data_020a6188[];
extern void func_02057e34(void);
extern char data_020a6378[];
extern u32 func_02057e48(u32 newVal);
extern void func_02058200(char *self, int a1, int a2, int end, int arg4, int arg5);

/* Typed through casts: the other files declare these symbols with their own
 * spellings, so the externs keep theirs. */
#define THREAD_INFO (*(OSThreadInfo *)data_020a6134)
#define LAUNCHER_THREAD (*(OSThread *)data_020a621c)
#define IDLE_THREAD (*(OSThread *)data_020a6188)

void func_02058308(void)
{
    int i;
    int ovr;
    int base;
    int end;

    if (data_020a6130 != 0) {
        return;
    }
    data_020a6130 = 1;

    for (i = 0; i < 16; i++) {
        THREAD_INFO.entry[i] = 0;
    }

    data_020a612c = data_020a613c;

    LAUNCHER_THREAD.priority = 0x10;
    LAUNCHER_THREAD.id = 0;
    LAUNCHER_THREAD.state = 1;
    LAUNCHER_THREAD.next = 0;
    LAUNCHER_THREAD.profiler = 0;

    THREAD_INFO.entry[0] = &LAUNCHER_THREAD;
    THREAD_INFO.list = &LAUNCHER_THREAD;
    THREAD_INFO.current = &LAUNCHER_THREAD;

    ovr = (s32)SDK_SYS_STACKSIZE;
    if (ovr == 0) {
        base = 0x23c0020 - ovr;
    } else {
        base = (u32)&data_023c0000 + 0x3fc0 - (s32)SDK_IRQ_STACKSIZE - ovr;
    }

    end = (u32)&data_023c0000 + 0x3fc0 - (s32)SDK_IRQ_STACKSIZE;

    LAUNCHER_THREAD.stackBottom = end;
    LAUNCHER_THREAD.stackTop = base;
    LAUNCHER_THREAD.stackWarningOffset = 0;
    *(int *)(end - 4) = (int)0xfddb597d;
    *(int *)(LAUNCHER_THREAD.stackTop) = 0x7bf9dd5b;
    LAUNCHER_THREAD.joinQueue = 0;
    THREAD_INFO.max_entry = 0x10;
    THREAD_INFO.isNeedRescheduling = 0;
    THREAD_INFO.irqDepth = 0;
    *(OSThreadInfo **)0x027fffa0 = &THREAD_INFO;

    func_02057e48(0);

    func_02058200(data_020a6188, (int)&func_02057e34, 0, (int)data_020a6378, 0xc8, 0x1f);

    IDLE_THREAD.priority = 0x20;
    IDLE_THREAD.state = 1;
}
