#ifndef NITRO_GX_GXASM_H
#define NITRO_GX_GXASM_H

/* NitroSDK's geometry FIFO block senders (gx/include/gxasm.h): each copies a
 * matrix's words to one FIFO port address, so the destination never
 * advances. GX_SendFifo48B is gxasm.c's asm (src/GX_SendFifo48B.c);
 * GX_SendFifo36B is an inline over mi_memory.c's MI_Copy36B, whose
 * destination does advance, but only through the port's mirrors
 * (0x04000400..0x0400043f), which all feed the FIFO. Parameters unnamed: a
 * named parameter in a declaration costs every C++ includer a compiler @NNN
 * number. */

#include "nitro/mi/memory.h"

#ifdef __cplusplus
extern "C" {
#endif

void GX_SendFifo48B(const void *, void *);

#ifdef __cplusplus
}
#endif

static inline void GX_SendFifo36B(const void *pSrc, void *pDest) {
    MI_Copy36B(pSrc, pDest);
}

#endif /* NITRO_GX_GXASM_H */
