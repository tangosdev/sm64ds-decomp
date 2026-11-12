#ifndef NITRO_MI_SWAP_H
#define NITRO_MI_SWAP_H

/*
 * The Nitro SDK's mi_swap.c, under the SDK's own name (ntrtwl/NitroSDK
 * nitro/mi/swap.h). The body is src/MI_SwapWord.c: one `swp` that
 * stores the first argument at the second and returns the word that was
 * there.
 *
 * The parameters are unnamed on purpose: a named parameter in a shared
 * declaration costs every including C++ file an @NNN number.
 */

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

u32 MI_SwapWord(u32, volatile u32 *);

#ifdef __cplusplus
}
#endif

#endif /* NITRO_MI_SWAP_H */
