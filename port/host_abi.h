/* dBgActor_c selection for the dual NDS/PC build.
 *
 * Exactly one of SM64DS_PLATFORM_NDS / SM64DS_PLATFORM_PC is defined by the
 * build system. src/ files do not include this header today; it exists so the
 * first platform seams (timing, input, register access) have one switch to
 * stand on instead of each inventing its own.
 *
 * The NDS side must expand to the original expressions byte-for-byte -- the
 * matching gate keeps that honest. The PC side routes to host implementations
 * under port/hal/ as they appear.
 */
#ifndef SM64DS_PORT_PLATFORM_H
#define SM64DS_PORT_PLATFORM_H

#if !defined(SM64DS_PLATFORM_NDS) && !defined(SM64DS_PLATFORM_PC)
#error "define SM64DS_PLATFORM_NDS or SM64DS_PLATFORM_PC"
#endif
#if defined(SM64DS_PLATFORM_NDS) && defined(SM64DS_PLATFORM_PC)
#error "define exactly one platform"
#endif

#include "ds_width.h"

#ifdef SM64DS_PLATFORM_PC
#ifndef SM64DS_HOST_64
/* The recovered ABI is 32-bit. A 32-bit host that grew an 8-byte pointer is
 * a misconfigured build, not a layout we can run. */
typedef char sm64ds_require_4byte_pointers[sizeof(void *) == 4 ? 1 : -1];
#else
/* 64-bit host. File images and DS-bus records use `T * DS_P32` so the slot
 * stays 4 bytes (__ptr32). A value stored through one of those slots has to
 * sit in the low 4GB. Runtime objects keep native pointers; the vptr is 8
 * bytes and later fields move with it. Named members follow. Numeric offsets
 * from a byte pointer do not. */
#endif
#endif

#endif
