#ifndef NITRO_CP_CONTEXT_H
#define NITRO_CP_CONTEXT_H

/* NitroSDK's divider and square-root context (nitro/cp/context.h): the two
 * units' operands and modes, as CP_SaveContext captures them from
 * REG_DIVCNT .. REG_SQRT_PARAM (0x04000280..0x040002b8) and
 * CPi_RestoreContext puts them back. OS_SaveContext and OS_LoadContext call
 * them for a thread's OSContext::cp_context, and the IRQ thread switch does
 * the same.
 *
 * The parameters are unnamed on purpose: a named parameter in a shared
 * declaration costs every including C++ file an @NNN number. */

#include "types.h"

#ifdef __cplusplus
#ifdef _MSC_VER
extern "C" {
#endif
#endif
typedef struct CPContext {
    u64 div_numer; /* 0x00 -- 0x04000290 */
    u64 div_denom; /* 0x08 -- 0x04000298 */
    u64 sqrt;      /* 0x10 -- 0x040002b8 */
    u16 div_mode;  /* 0x18 -- 0x04000280 (low 2 bits) */
    u16 sqrt_mode; /* 0x1a -- 0x040002b0 (low bit) */
} CPContext;

#ifdef __cplusplus
extern "C"
#endif
void CP_SaveContext(CPContext *);
#ifdef __cplusplus
extern "C"
#endif
void CPi_RestoreContext(const CPContext *);
#ifdef __cplusplus
#ifdef _MSC_VER
}
#endif
#endif

#ifndef SM64DS_PLATFORM_PC
typedef char CPContext_size_must_be_0x1c[sizeof(CPContext) == 0x1c ? 1 : -1];
#endif

#endif /* NITRO_CP_CONTEXT_H */
