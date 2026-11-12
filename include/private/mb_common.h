#ifndef PRIVATE_MB_COMMON_H
#define PRIVATE_MB_COMMON_H

/* The multiboot helpers both sides share, as NitroSDK's private
 * libraries/mb/include/mb_common.h declares them: mb_common.c
 * (src/engine/mb/MB_Common.c) defines them, the parent's senders in
 * MB_Parent.cpp end each block with MBi_BlockHeaderEnd, and the beacon
 * builder in MB_GameInfo.c checksums the game record with MBi_calc_cksum.
 * The cartridge carries only these two of the header's functions.
 *
 * The parameters are unnamed: a named parameter in a declaration costs
 * every including C++ file an @NNN number. */

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

int MBi_BlockHeaderEnd(int, u16, u32 *);
u16 MBi_calc_cksum(const u16 *, int);

#ifdef __cplusplus
}
#endif

#endif
