#ifndef NITRO_OS_MUTEX_H
#define NITRO_OS_MUTEX_H

/*
 * The mutex calls of the Nitro SDK's os_mutex.c, under the SDK's own names
 * (ntrtwl/NitroSDK nitro/os/common/mutex.h). The bodies are in
 * src/engine/os/OS_Mutex.cpp; the OSMutex record is in nitro/os/thread.h.
 *
 * OS_TryLockMutex answers the SDK's BOOL as an int: 1 when the calling
 * thread now holds the mutex, 0 when another thread does.
 *
 * The parameters are unnamed on purpose: a named parameter in a shared
 * declaration costs every including C++ file an @NNN number.
 */

#include "nitro/os/thread.h"

#ifdef __cplusplus
extern "C" {
#endif

void OS_InitMutex(OSMutex *);
void OS_LockMutex(OSMutex *);
void OS_UnlockMutex(OSMutex *);
int OS_TryLockMutex(OSMutex *);
void OSi_UnlockAllMutex(OSThread *);

#ifdef __cplusplus
}
#endif

#endif /* NITRO_OS_MUTEX_H */
