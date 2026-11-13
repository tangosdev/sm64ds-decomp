#ifndef PRIVATE_MB_WM_BASE_H
#define PRIVATE_MB_WM_BASE_H
#ifndef __cplusplus
#define MBi_SendMP _Z10MBi_SendMPPKvii
#endif

/* The multiboot WM layer's MP send, as NitroSDK's private
 * libraries/mb/include/mb_wm_base.h declares it: mb_wm_base.c
 * (src/engine/mb/MB_WmBase.cpp) defines it, and mb_common.c's
 * MBi_BlockHeaderEnd (src/engine/mb/MB_Common.c) sends every parent block
 * through it.
 *
 * The parameters are unnamed: a named parameter in a declaration costs
 * every including C++ file an @NNN number. */

#include "types.h"

#ifdef __cplusplus
#ifdef _MSC_VER
extern "C" {
#endif
#endif

int MBi_SendMP(const void *, int, int);

#ifdef __cplusplus
#ifdef _MSC_VER
}
#endif
#endif

#endif
