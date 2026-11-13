#ifndef NITRO_OS_SYSTEM_H
#define NITRO_OS_SYSTEM_H
#ifndef __cplusplus
#define OS_Terminate _Z12OS_Terminatev
#endif

/*
 * The CPSR interrupt primitives of the Nitro SDK's os_system.c, under the
 * SDK's own names (pret/pokediamond arm9/lib/NitroSDK/include/OS_system.h).
 * The bodies are hand-written asm, in src/IRQ3.cpp.
 *
 * OS_GetProcMode answers the CPSR mode field (bits 0-4) as an OSProcMode.
 * OS_Halt writes CP15 c7,c0,4 (wait for interrupt), which halts the core
 * until an interrupt arrives. pret's later SDK files it in
 * os_terminate_proc.c (OS_terminate_proc.h); this ROM links it inside the
 * os_system.c run, between OS_GetProcMode and OS_SpinWait.
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


#if defined(_MSC_VER) && defined(__cplusplus)
extern "C" {
#endif
typedef enum OSProcMode {
    OS_PROCMODE_USER = 0x10,
    OS_PROCMODE_FIQ = 0x11,
    OS_PROCMODE_IRQ = 0x12,
    OS_PROCMODE_SVC = 0x13,
    OS_PROCMODE_ABORT = 0x17,
    OS_PROCMODE_UNDEF = 0x1b,
    OS_PROCMODE_SYS = 0x1f
} OSProcMode;

typedef enum OSIntrMode {
    OS_INTRMODE_DISABLE_IRQ = 0x80, /* HW_PSR_DISABLE_IRQ */
    OS_INTRMODE_DISABLE_FIQ = 0x40, /* HW_PSR_DISABLE_FIQ */
    OS_INTRMODE_ENABLE = 0
} OSIntrMode;

#ifdef __cplusplus
extern "C"
#endif
OSIntrMode OS_EnableInterrupts(void);
#ifdef __cplusplus
extern "C"
#endif
OSIntrMode OS_DisableInterrupts(void);
#ifdef __cplusplus
extern "C"
#endif
OSIntrMode OS_RestoreInterrupts(OSIntrMode);
#ifdef __cplusplus
extern "C"
#endif
OSIntrMode OS_DisableInterrupts_IrqAndFiq(void);
#ifdef __cplusplus
extern "C"
#endif
OSIntrMode OS_RestoreInterrupts_IrqAndFiq(OSIntrMode);
#ifdef __cplusplus
extern "C"
#endif
OSProcMode OS_GetProcMode(void);
#ifdef __cplusplus
extern "C"
#endif
void OS_Halt(void);

#ifdef __cplusplus
extern "C"
#endif
/* Burns about the given number of CPU cycles, four per loop pass. The
 * body is hand-written asm, in src/OS_SpinWait.c. */
void OS_SpinWait(u32);

/* Halts for good. The SDK's lives in os_terminate_proc.c; this ROM links the
 * game's own in its place (src/OS_Terminate.cpp). */
void OS_Terminate(void);
#if defined(_MSC_VER) && defined(__cplusplus)
}
#endif


#endif /* NITRO_OS_SYSTEM_H */
