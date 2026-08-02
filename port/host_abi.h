/* Platform selection for the dual NDS/PC build.
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

#ifdef SM64DS_PLATFORM_PC
/* The recovered ABI is 32-bit; catch a 64-bit host configuration at compile
 * time rather than as a corrupted struct at runtime. Struct recovery lifts
 * this restriction; until then it is load-bearing. */
typedef char sm64ds_require_4byte_pointers[sizeof(void *) == 4 ? 1 : -1];
#endif

#endif
