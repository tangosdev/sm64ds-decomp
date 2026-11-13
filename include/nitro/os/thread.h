#ifndef NITRO_OS_THREAD_H
#define NITRO_OS_THREAD_H
#ifndef __cplusplus
#define OS_GetThreadPriority _Z20OS_GetThreadPriorityPK8OSThread
#define OS_SetThreadPriority _Z20OS_SetThreadPriorityP8OSThreadj
#define OS_SetThreadStackWarningOffset _Z30OS_SetThreadStackWarningOffsetP8OSThreadj
#define OS_RescheduleThread _Z19OS_RescheduleThreadv
#define OS_SelectThread _Z15OS_SelectThreadv
#define OS_WakeupThreadDirect _Z21OS_WakeupThreadDirectP8OSThread
#define OS_WakeupThread _Z15OS_WakeupThreadPt
#define OS_SleepThread _Z14OS_SleepThreadPt
#define OS_IsThreadTerminated _Z21OS_IsThreadTerminatedPK8OSThread
#define OS_JoinThread _Z13OS_JoinThreadP8OSThread
#define OS_ExitThread _Z13OS_ExitThreadv
#define OS_CreateThread _Z15OS_CreateThreadP8OSThreadPFvPvES1_S1_jj
#define OS_InitThread _Z13OS_InitThreadv
#endif

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
#include "nitro/os/context.h"


#define OS_THREAD_MAX_NUM 16

/* A set of waiting threads, one bit per thread id. */
typedef u16 OSThreadQueue;

/* OSThread::state */
#define OS_THREAD_STATE_WAITING 0
#define OS_THREAD_STATE_READY 1
#define OS_THREAD_STATE_TERMINATED 2

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

/* The asm offsets of the SDK's thread.h, for the IRQ thread switch. */
#define OS_THREADINFO_OFFSET_ISNEEDRESCHEDULING 0
#define OS_THREADINFO_OFFSET_MAX_ENTRY 2
#define OS_THREADINFO_OFFSET_IRQDEPTH 4
#define OS_THREADINFO_OFFSET_PADDING 6
#define OS_THREADINFO_OFFSET_CURRENT 8
#define OS_THREADINFO_OFFSET_LIST 12
#define OS_THREADINFO_OFFSET_SWITCHCALLBACK 16
#define OS_THREADINFO_OFFSET_ENTRY 20

#define OS_THREAD_OFFSET_CONTEXT 0
#define OS_THREAD_OFFSET_STATE (sizeof(OSContext))
#define OS_THREAD_OFFSET_NEXT (sizeof(OSContext) + sizeof(s32))
#define OS_THREAD_OFFSET_ID (OS_THREAD_OFFSET_NEXT + sizeof(OSThread *))

extern OSThreadInfo OSi_ThreadInfo;
extern OSThread OSi_LauncherThread;     /* the thread NitroMain() runs on */

void OS_InitThread(void);
void OS_CreateThread(OSThread *, void (*)(void *), void *, void *, u32, u32);
void OS_ExitThread(void);
void OS_JoinThread(OSThread *);
int OS_IsThreadTerminated(const OSThread *);
void OS_SleepThread(OSThreadQueue *);
void OS_WakeupThread(OSThreadQueue *);
void OS_WakeupThreadDirect(OSThread *);
OSThread *OS_SelectThread(void);
void OS_RescheduleThread(void);
void OS_SetThreadStackWarningOffset(OSThread *, u32);
int OS_SetThreadPriority(OSThread *, u32);
u32 OS_GetThreadPriority(const OSThread *);


#endif /* NITRO_OS_THREAD_H */
