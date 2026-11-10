#ifndef NITRO_OS_SYSTEM_H
#define NITRO_OS_SYSTEM_H

/*
 * The CPSR interrupt primitives of the Nitro SDK's os_system.c, under the
 * SDK's own names (pret/pokediamond arm9/lib/NitroSDK/include/OS_system.h).
 * The bodies are hand-written asm, in src/IRQ3.cpp.
 *
 * Each call sets or clears the CPSR I bit (0x80), or the I and F bits (0xc0)
 * for the _IrqAndFiq pair, and returns the bits as they were, for the
 * matching Restore. The usual shape is
 *
 *     OSIntrMode enabled = OS_DisableInterrupts();
 *     ...
 *     (void)OS_RestoreInterrupts(enabled);
 *
 * The parameters are unnamed on purpose: a named parameter in a shared
 * declaration costs every including C++ file an @NNN number.
 */

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum OSIntrMode {
    OS_INTRMODE_DISABLE_IRQ = 0x80, /* HW_PSR_DISABLE_IRQ */
    OS_INTRMODE_DISABLE_FIQ = 0x40, /* HW_PSR_DISABLE_FIQ */
    OS_INTRMODE_ENABLE = 0
} OSIntrMode;

OSIntrMode OS_EnableInterrupts(void);
OSIntrMode OS_DisableInterrupts(void);
OSIntrMode OS_RestoreInterrupts(OSIntrMode);
OSIntrMode OS_DisableInterrupts_IrqAndFiq(void);
OSIntrMode OS_RestoreInterrupts_IrqAndFiq(OSIntrMode);

#ifdef __cplusplus
}
#endif

#endif /* NITRO_OS_SYSTEM_H */
