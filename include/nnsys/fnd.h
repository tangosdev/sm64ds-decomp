#ifndef NNSYS_FND_H
#define NNSYS_FND_H

#include "types.h"

/*
 * NitroSystem foundation library (fnd): the intrusive list and the heap
 * header. Names as the Sonic Rush Adventure decomp and pret/pokediamond
 * (arm9/lib/libnns, NNS_FND_list.c and NNS_FND_heapcommon.c) spell them.
 */

#ifdef __cplusplus
extern "C" {
#endif

typedef struct NNSFndLink {
    void *prevObject;
    void *nextObject;
} NNSFndLink;

/* An intrusive list: each object carries an NNSFndLink `offset' bytes in. */
typedef struct NNSFndList {
    void *headObject;
    void *tailObject;
    u16 numObjects;
    u16 offset;
} NNSFndList;

/* The header every fnd heap starts with; childList holds the heaps carved
 * out of this one, linked through their own `link'. */
typedef struct NNSiFndHeapHead {
    u32 signature;
    NNSFndLink link;
    NNSFndList childList;
    void *heapStart;
    void *heapEnd;
    u32 attribute;
} NNSiFndHeapHead;

typedef NNSiFndHeapHead *NNSFndHeapHandle;

void NNS_FndInitList(NNSFndList *list, u16 offset);
void NNS_FndAppendListObject(NNSFndList *list, void *object);
void NNS_FndPrependListObject(NNSFndList *list, void *object);
void NNS_FndInsertListObject(NNSFndList *list, void *target, void *object);
void NNS_FndRemoveListObject(NNSFndList *list, void *object);
void *NNS_FndGetNextListObject(NNSFndList *list, void *object);
void *NNS_FndGetPrevListObject(NNSFndList *list, void *object);

void NNSi_FndInitHeapHead(NNSiFndHeapHead *heap, u32 signature, void *heapStart, void *heapEnd,
                          u16 optFlag);
void NNSi_FndFinalizeHeap(NNSiFndHeapHead *heap);

void NNS_FndDestroyExpHeap(NNSFndHeapHandle heap);
void NNS_FndDestroyFrmHeap(NNSFndHeapHandle heap);

#ifdef __cplusplus
}
#endif

#endif
