#ifndef NITRO_OS_EXCEPTION_H
#define NITRO_OS_EXCEPTION_H

/*
 * The user exception handler type of the Nitro SDK's os_exception.c, under
 * the SDK's own name (nitro/os/common/exception.h). The bodies are in
 * src/engine/os/OS_Exception.cpp.
 *
 * The parameters are unnamed on purpose: a named parameter in a shared
 * declaration costs every including C++ file an @NNN number.
 */

#include "types.h"

/* OS_SetUserExceptionHandler's handler: called with the exception context
 * and the arg registered beside it. */
typedef void (*OSExceptionHandler)(u32, void *);

#endif /* NITRO_OS_EXCEPTION_H */
