#ifndef NITRO_OS_ALLOC_H
#define NITRO_OS_ALLOC_H

/*
 * The heap calls of the Nitro SDK's os_alloc.c, under the SDK's own names
 * (ntrtwl/NitroSDK nitro/os/common/alloc.h). The bodies are in
 * src/engine/os/OS_Alloc.cpp.
 *
 * The first parameter is the SDK's OSArenaId, spelled int: the arena ids are
 * still OS_Arena.cpp's own macros. A negative heap handle means the arena's
 * current heap.
 *
 * The parameters are unnamed on purpose: a named parameter in a shared
 * declaration costs every including C++ file an @NNN number.
 */

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef int OSHeapHandle;

void *OS_AllocFromHeap(int, OSHeapHandle, u32);
void OS_FreeToHeap(int, OSHeapHandle, void *);

#ifdef __cplusplus
}
#endif

#endif /* NITRO_OS_ALLOC_H */
