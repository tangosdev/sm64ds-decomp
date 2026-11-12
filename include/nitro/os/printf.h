#ifndef NITRO_OS_PRINTF_H
#define NITRO_OS_PRINTF_H

/*
 * The string formatter of the Nitro SDK's os_printf.c, under the SDK's own
 * name (ntrtwl/NitroSDK nitro/os/common/printf.h). The body is in
 * src/engine/os/OS_Printf.c. It writes the formatted string to the buffer
 * and returns its length.
 *
 * The parameters are unnamed on purpose: a named parameter in a shared
 * declaration costs every including C++ file an @NNN number.
 */

#ifdef __cplusplus
extern "C" {
#endif

int OS_SPrintf(char *, const char *, ...);

#ifdef __cplusplus
}
#endif

#endif /* NITRO_OS_PRINTF_H */
