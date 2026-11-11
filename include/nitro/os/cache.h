#ifndef NITRO_OS_CACHE_H
#define NITRO_OS_CACHE_H

/*
 * The ARM946E-S cache maintenance of the Nitro SDK's os_cache.c, under the
 * SDK's own names (pret/pokediamond arm9/lib/NitroSDK/include/OS_cache.h).
 * The bodies are hand-written asm, in src/engine/cp15/CP15.cpp.
 *
 * The SDK's verbs: Store cleans (writes dirty lines back to memory), Flush
 * cleans and invalidates, Invalidate drops the lines unwritten. The All forms
 * walk the whole data cache by set and way; the Range forms act on the 32-byte
 * lines covering [addr, addr + size). DC_InvalidateAll and IC_InvalidateAll
 * are not linked into this ROM.
 *
 * The parameters are unnamed on purpose: a named parameter in a shared
 * declaration costs every including C++ file an @NNN number.
 */

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

void DC_StoreAll(void);
void DC_FlushAll(void);
void DC_InvalidateRange(void *, u32);
void DC_StoreRange(void *, u32);
void DC_FlushRange(const void *, u32);
void DC_WaitWriteBufferEmpty(void);
void IC_InvalidateRange(void *, u32);

#ifdef __cplusplus
}
#endif

#endif /* NITRO_OS_CACHE_H */
