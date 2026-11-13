//cpp
/* Mutexes (the Nitro SDK's os_mutex.c), arm9 .text 0x02058960..0x02058bb0.
 *
 * pret/pokediamond's NitroSDK OS_mutex.c has, in this order, OS_InitMutex,
 * OS_LockMutex, OS_UnlockMutex, OSi_UnlockAllMutex, OS_TryLockMutex,
 * OSi_EnqueueTail and OSi_DequeueItem. mwccarm defers the bodies and emits
 * them in reverse, so the ROM runs bottom-up: func_02058960 at 0x02058960,
 * OS_InitMutex at 0x02058b9c.
 *
 * This SDK is older than pret's. Its thread queue is a 16-bit mask of thread
 * ids (nitro/os/thread.h), OS_TryLockMutex does not put the mutex on the
 * owner's held list, and the list unlink OSi_UnlockAllMutex uses is the last
 * function of this file, taking the thread (pret's OSi_RemoveMutexLinkFromQueue
 * lives in OS_thread.c and takes the queue).
 *
 * leftovers: func_02058960 keeps its address name; no SDK source spells it.
 */
#include "nitro/os/system.h"
#include "nitro/os/mutex.h"



void OSi_EnqueueTail(OSThread *thread, OSMutex *mutex);
void OSi_DequeueItem(OSThread *thread, OSMutex *mutex);
OSMutex *func_02058960(OSThread *thread);

void OS_InitMutex(OSMutex *mutex) {
    mutex->queue = 0;
    mutex->thread = 0;
    mutex->count = 0;
}

void OS_LockMutex(OSMutex *mutex) {
    OSIntrMode saved = OS_DisableInterrupts();
    OSThread *current = OSi_ThreadInfo.current;

    for (;;) {
        if (mutex->thread == 0) {
            mutex->thread = current;
            mutex->count++;
            OSi_EnqueueTail(current, mutex);
            break;
        }
        if (mutex->thread == current) {
            mutex->count++;
            break;
        }
        current->mutex = mutex;
        OS_SleepThread(&mutex->queue);
        current->mutex = 0;
    }

    OS_RestoreInterrupts(saved);
}

void OS_UnlockMutex(OSMutex *mutex) {
    OSIntrMode saved = OS_DisableInterrupts();
    OSThread *current = OSi_ThreadInfo.current;
    if (mutex->thread == current) {
        if (--mutex->count == 0) {
            OSi_DequeueItem(current, mutex);
            mutex->thread = 0;
            OS_WakeupThread(&mutex->queue);
        }
    }
    OS_RestoreInterrupts(saved);
}

void OSi_UnlockAllMutex(OSThread *thread) {
    OSMutex *mutex;
    while (thread->mutexQueue.head != 0) {
        mutex = func_02058960(thread);
        mutex->count = 0;
        mutex->thread = 0;
        OS_WakeupThread(&mutex->queue);
    }
}

int OS_TryLockMutex(OSMutex *mutex) {
    OSIntrMode saved = OS_DisableInterrupts();
    OSThread *current = OSi_ThreadInfo.current;
    int locked;
    if (mutex->thread == 0) {
        mutex->thread = current;
        mutex->count++;
        locked = 1;
    } else if (mutex->thread == current) {
        mutex->count++;
        locked = 1;
    } else {
        locked = 0;
    }
    OS_RestoreInterrupts(saved);
    return locked;
}

/* Unlike pret, this SDK links the new mutex after the list's head rather
 * than after prev: the ROM reloads mutexQueue.head for the store. */
void OSi_EnqueueTail(OSThread *thread, OSMutex *mutex) {
    OSMutex *prev = thread->mutexQueue.tail;
    if (prev == 0)
        thread->mutexQueue.head = mutex;
    else
        thread->mutexQueue.head->link.next = mutex;
    mutex->link.prev = prev;
    mutex->link.next = 0;
    thread->mutexQueue.tail = mutex;
}

void OSi_DequeueItem(OSThread *thread, OSMutex *mutex) {
    OSMutex *next = mutex->link.next;
    OSMutex *prev = mutex->link.prev;
    if (next == 0)
        thread->mutexQueue.tail = prev;
    else
        next->link.prev = prev;
    if (prev == 0)
        thread->mutexQueue.head = next;
    else
        prev->link.next = next;
}

/* Unlink and return the first mutex the thread holds. */
OSMutex *func_02058960(OSThread *thread) {
    OSMutex *mutex = thread->mutexQueue.head;
    OSMutex *next = mutex->link.next;
    if (next == 0)
        thread->mutexQueue.tail = 0;
    else
        next->link.prev = 0;
    thread->mutexQueue.head = next;
    return mutex;
}
