#ifndef NITRO_OS_INIT_H
#define NITRO_OS_INIT_H
#ifndef __cplusplus
#define OS_Init _Z7OS_Initv
#endif

/*
 * The Nitro SDK's OS start-up call, under its own name (ntrtwl/NitroSDK
 * nitro/os/common/init.h). The body is in src/OS_Init.cpp; the
 * game's NitroStartUp runs it before any static constructor.
 */

#ifdef __cplusplus
#ifdef _MSC_VER
extern "C" {
#endif
#endif

void OS_Init(void);

#ifdef __cplusplus
#ifdef _MSC_VER
}
#endif
#endif

#endif /* NITRO_OS_INIT_H */
