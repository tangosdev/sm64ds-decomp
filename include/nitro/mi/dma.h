#ifndef NITRO_MI_DMA_H
#define NITRO_MI_DMA_H

/* NitroSDK's DMA API (nitro/mi/dma.h): mi_dma.c, defined in
 * src/engine/mi/MI_Dma.cpp, and the geometry-FIFO sends of mi_dma_gxcommand.c
 * (src/engine/mi/MI_DmaGXCommand.cpp, src/MI_SendGXCommand.c), with the SDK's
 * signatures. As in memory.h, the parameters stay unnamed (a named parameter
 * in a declaration costs every C++ includer an @NNN number), and the sizes
 * and channel numbers are spelled unsigned int, the SDK's u32. */

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*MIDmaCallback)(void *);

void MI_DmaFill32(unsigned int, void *, unsigned int, unsigned int);
void MI_DmaCopy32(unsigned int, const void *, void *, unsigned int);
void MI_DmaCopy16(unsigned int, const void *, void *, unsigned int);
void MI_DmaFill32Async(unsigned int, void *, unsigned int, unsigned int, MIDmaCallback, void *);
void MI_DmaCopy32Async(unsigned int, const void *, void *, unsigned int, MIDmaCallback, void *);
void MI_WaitDma(unsigned int);
void MI_StopDma(unsigned int);

/* The SDK's clear wrappers, as macros for the reason memory.h gives. */
#define MI_DmaClear32(dmaNo, dest, size) MI_DmaFill32((dmaNo), (dest), 0, (size))
#define MI_DmaClear32Async(dmaNo, dest, size, callback, arg) \
    MI_DmaFill32Async((dmaNo), (dest), 0, (size), (callback), (arg))

void MI_SendGXCommand(unsigned int, const void *, unsigned int);
void MI_SendGXCommandAsync(unsigned int, const void *, unsigned int, MIDmaCallback, void *);

/* Arms one channel: source, destination, then the control word that starts
 * it, with interrupts masked (pret/pokediamond MI_dma.h). The body is in
 * ITCM, src/MIi_DmaSetParams.cpp. */
void MIi_DmaSetParams(unsigned int, unsigned int, unsigned int, unsigned int);

#ifdef __cplusplus
}
#endif

#endif
