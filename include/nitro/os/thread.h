#ifndef NITRO_OS_THREAD_H
#define NITRO_OS_THREAD_H

/*
 * The thread types of the Nitro SDK's os_thread.c, this SDK's layouts. The
 * bodies are in src/engine/os/OS_Thread.cpp.
 *
 * This SDK predates thread queues as linked lists: an OSThreadQueue is a
 * 16-bit mask of thread ids, and OSThreadInfo keeps a 16-entry id table.
 *
 * The parameters are unnamed on purpose: a named parameter in a shared
 * declaration costs every including C++ file an @NNN number.
 */

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define OS_THREAD_MAX_NUM 16

typedef struct OSContext {
    u32 cpsr;            /* 0x00 */
    u32 r[13];           /* 0x04 */
    u32 sp;              /* 0x38 */
    u32 lr;              /* 0x3c */
    u32 pc_plus4;        /* 0x40 */
    u32 sp_svc;          /* 0x44 */
    u8 cp_context[0x1c]; /* 0x48 -- divider / sqrt state */
} OSContext;

struct OSMutex; /* os_mutex.c */

/* The mutexes a thread holds. */
typedef struct OSMutexQueue {
    struct OSMutex *head; /* 0x00 */
    struct OSMutex *tail; /* 0x04 */
} OSMutexQueue;

typedef struct OSThread {
    OSContext context;       /* 0x00 */
    s32 state;               /* 0x64 */
    struct OSThread *next;   /* 0x68 -- the priority-sorted thread list */
    u32 id;                  /* 0x6c */
    u32 priority;            /* 0x70 */
    void *profiler;          /* 0x74 */
    struct OSMutex *mutex;   /* 0x78 -- the mutex it waits on */
    OSMutexQueue mutexQueue; /* 0x7c */
    u32 stackTop;            /* 0x84 */
    u32 stackBottom;         /* 0x88 */
    u32 stackWarningOffset;  /* 0x8c */
    u16 joinQueue;           /* 0x90 -- a mask of thread ids */
} OSThread;

typedef struct OSMutexLink {
    struct OSMutex *prev; /* 0x00 */
    struct OSMutex *next; /* 0x04 */
} OSMutexLink;

/* NitroSDK OSMutex (os_mutex.c), this SDK's layout: 0x14 bytes. */
typedef struct OSMutex {
    u16 queue;        /* 0x00 -- OSThreadQueue: threads waiting for it */
    OSThread *thread; /* 0x04 -- the owner, 0 while free */
    s32 count;        /* 0x08 -- times the owner has locked it */
    OSMutexLink link; /* 0x0c -- the owner's held-mutex list */
} OSMutex;

typedef void (*OSSwitchThreadCallback)(OSThread *, OSThread *);

typedef struct OSThreadInfo {
    u16 isNeedRescheduling;                /* 0x00 */
    u16 max_entry;                         /* 0x02 */
    u16 irqDepth;                          /* 0x04 */
    u16 padding;                           /* 0x06 */
    OSThread *current;                     /* 0x08 */
    OSThread *list;                        /* 0x0c */
    OSSwitchThreadCallback switchCallback; /* 0x10 */
    OSThread *entry[OS_THREAD_MAX_NUM];    /* 0x14 */
} OSThreadInfo;

extern OSThreadInfo data_020a6134; /* OSi_ThreadInfo */
extern OSThread data_020a621c;     /* OSi_LauncherThread: the thread NitroMain() runs on */

void OS_InitThread(void);
void OS_CreateThread(OSThread *, void (*)(void *), void *, void *, u32, u32);
void OS_ExitThread(void);
void OS_JoinThread(OSThread *);
int OS_IsThreadTerminated(const OSThread *);
void OS_SleepThread(u16 *);
void OS_WakeupThread(u16 *);
void OS_WakeupThreadDirect(OSThread *);
OSThread *OS_SelectThread(void);
void OS_RescheduleThread(void);
void OS_SetThreadStackWarningOffset(OSThread *, u32);
int OS_SetThreadPriority(OSThread *, u32);
u32 OS_GetThreadPriority(const OSThread *);

#ifdef __cplusplus
}
#endif

#endif /* NITRO_OS_THREAD_H */
