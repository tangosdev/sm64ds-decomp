//cpp
#include "IRQ.h"

extern "C" {

extern void func_02058988(void *list, char *node);
extern void OS_WakeupThread(char *node);

/* NitroSDK OS_UnlockMutex(mutex): when the current thread owns the mutex,
 * drop one lock; on the last, take it off the thread's mutex list
 * (func_02058988), clear the owner and wake the waiters. OSMutex carries
 * func_02058b9c.c's names, OSThreadInfo func_02058308.c's. */
struct OSThread;

typedef struct OSMutex {
    unsigned short queue;    /* 0x00 -- threads waiting for the mutex */
    struct OSThread *thread; /* 0x04 -- the owner, 0 while free */
    int count;               /* 0x08 -- times the owner has locked it */
} OSMutex;

typedef struct OSThreadInfo {
    char pad_00[8];
    struct OSThread *current; /* 0x08 */
} OSThreadInfo;

/* Declared int [] elsewhere (func_02058308.c's THREAD_INFO): reached
   through a cast rather than a retyped extern. */
extern int data_020a6134[];
#define THREAD_INFO (*(OSThreadInfo *)data_020a6134)

void func_02058a94(char *obj) {
    OSMutex *mutex = (OSMutex *)obj;
    unsigned int saved = IRQ::Disable();
    struct OSThread *current = THREAD_INFO.current;
    if (mutex->thread == current) {
        if (--mutex->count == 0) {
            func_02058988(current, obj);
            mutex->thread = 0;
            OS_WakeupThread(obj);
        }
    }
    IRQ::Restore(saved);
}
}
