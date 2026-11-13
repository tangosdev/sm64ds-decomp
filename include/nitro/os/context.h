#ifndef NITRO_OS_CONTEXT_H
#define NITRO_OS_CONTEXT_H

/* A thread's saved CPU state, NitroSDK's nitro/os/common/context.h. The
 * bodies are the hand-written asm of os_context.c: OS_InitContext
 * (src/OS_InitContext.c), OS_SaveContext and OS_LoadContext. The
 * OS_CONTEXT_* offsets are the SDK's, for that asm.
 *
 * The parameters are unnamed on purpose: a named parameter in a shared
 * declaration costs every including C++ file an @NNN number. */

#include "types.h"
#include "nitro/cp/context.h"

#define OS_CONTEXT_CPSR 0
#define OS_CONTEXT_R0 4
#define OS_CONTEXT_R1 8
#define OS_CONTEXT_R2 12
#define OS_CONTEXT_R3 16
#define OS_CONTEXT_R4 20
#define OS_CONTEXT_R5 24
#define OS_CONTEXT_R6 28
#define OS_CONTEXT_R7 32
#define OS_CONTEXT_R8 36
#define OS_CONTEXT_R9 40
#define OS_CONTEXT_R10 44
#define OS_CONTEXT_R11 48
#define OS_CONTEXT_R12 52
#define OS_CONTEXT_R13 56
#define OS_CONTEXT_R14 60
#define OS_CONTEXT_PC_PLUS4 64
#define OS_CONTEXT_SP OS_CONTEXT_R13
#define OS_CONTEXT_LR OS_CONTEXT_R14
#define OS_CONTEXT_R13_SVC 68
#define OS_CONTEXT_SP_SVC OS_CONTEXT_R13_SVC
#define OS_CONTEXT_CP_CONTEXT 72


#if defined(_MSC_VER) && defined(__cplusplus)
extern "C" {
#endif
typedef struct OSContext {
    u32 cpsr;             /* 0x00 */
    u32 r[13];            /* 0x04 */
    u32 sp;               /* 0x38 */
    u32 lr;               /* 0x3c */
    u32 pc_plus4;         /* 0x40 */
    u32 sp_svc;           /* 0x44 */
    CPContext cp_context; /* 0x48 */
} OSContext;

#ifdef __cplusplus
extern "C"
#endif
void OS_InitContext(OSContext *, u32, u32);
#ifdef __cplusplus
extern "C"
#endif
int OS_SaveContext(OSContext *);
#ifdef __cplusplus
extern "C"
#endif
/* the SDK's BOOL */
void OS_LoadContext(OSContext *);
#if defined(_MSC_VER) && defined(__cplusplus)
}
#endif


#endif /* NITRO_OS_CONTEXT_H */
