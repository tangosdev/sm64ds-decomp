#ifndef NNSYS_FND_H
#define NNSYS_FND_H

#include "types.h"

/*
 * NitroSystem foundation library (fnd): the intrusive list, the heap header,
 * and the expanding and frame heaps built on it. Names as the Sonic Rush
 * Adventure decomp (its NitroSystem fnd sources) and pret/pokediamond
 * (arm9/lib/libnns, NNS_FND_list.c, NNS_FND_heapcommon.c and
 * NNS_FND_expheap.c) spell them.
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

#ifndef SM64DS_PLATFORM_PC
typedef char NNSFndList_size_must_be_0xc[sizeof(NNSFndList) == 0xc ? 1 : -1];
#endif

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

/* What NNS_FndVisitAllocatedForExpHeap hands each allocated block to. */
typedef void (*NNSFndHeapVisitor)(void *memBlock, NNSFndHeapHandle heap, u32 userParam);

/* ---- expanding heap (NNS_FND_expheap.c) ---- */

/* The 0x10-byte header in front of every block, free ('FR') or used ('UD').
   attribute: bits 0..7 the group ID, 8..14 the alignment padding skipped
   ahead of the header, 15 the direction it was allocated from. */
typedef struct NNSiFndExpHeapMBlockHead NNSiFndExpHeapMBlockHead;
struct NNSiFndExpHeapMBlockHead {
    u16 signature;
    u16 attribute;
    u32 blockSize; /* data bytes, from just past this header */
    NNSiFndExpHeapMBlockHead *pMBHeadPrev;
    NNSiFndExpHeapMBlockHead *pMBHeadNext;
};

typedef struct NNSiFndExpMBlockList {
    NNSiFndExpHeapMBlockHead *head;
    NNSiFndExpHeapMBlockHead *tail;
} NNSiFndExpMBlockList;

/* Follows the NNSiFndHeapHead. feature bit 0 is the allocation mode:
   clear takes the first free block that fits, set the best fit. */
typedef struct NNSiFndExpHeapHead {
    NNSiFndExpMBlockList mbFreeList;
    NNSiFndExpMBlockList mbUsedList;
    u16 groupID;
    u16 feature;
} NNSiFndExpHeapHead;

typedef struct NNSiMemRegion {
    void *start;
    void *end;
} NNSiMemRegion;

/* ---- frame heap (NNS_FND_frameheap.c) ---- */

/* A restore point NNS_FndRecordStateForFrmHeap allocates inside the heap. */
typedef struct NNSiFndFrmHeapState NNSiFndFrmHeapState;
struct NNSiFndFrmHeapState {
    u32 tagName;
    void *headAllocator;
    void *tailAllocator;
    NNSiFndFrmHeapState *pPrevState;
};

/* Follows the NNSiFndHeapHead: the free region is [headAllocator,
   tailAllocator), and pState is the most recent restore point. */
typedef struct NNSiFndFrmHeapHead {
    void *headAllocator;
    void *tailAllocator;
    NNSiFndFrmHeapState *pState;
} NNSiFndFrmHeapHead;

#define NNS_FND_FRMHEAP_FREE_HEAD (1 << 0)
#define NNS_FND_FRMHEAP_FREE_TAIL (1 << 1)
#define NNS_FND_FRMHEAP_FREE_ALL (NNS_FND_FRMHEAP_FREE_HEAD | NNS_FND_FRMHEAP_FREE_TAIL)

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

NNSFndHeapHandle NNS_FndCreateExpHeapEx(void *startAddress, u32 size, u16 optFlag);
void NNS_FndDestroyExpHeap(NNSFndHeapHandle heap);
void *NNS_FndAllocFromExpHeapEx(NNSFndHeapHandle heap, u32 size, int alignment);
u32 NNS_FndResizeForMBlockExpHeap(NNSFndHeapHandle heap, void *memBlock, u32 size);
void NNS_FndFreeToExpHeap(NNSFndHeapHandle heap, void *memBlock);
u32 NNS_FndGetTotalFreeSizeForExpHeap(NNSFndHeapHandle heap);
u32 NNS_FndGetAllocatableSizeForExpHeapEx(NNSFndHeapHandle heap, int alignment);
/* groupID is u16 in the SDK. Here it is word-wide: ExpHeap_t::VSetNodeID
   (0x0203c3f8) hands its u32 id straight through, and a u16 parameter makes
   the caller narrow it first (measured: 0x10 -> 0x18 bytes). */
u16 NNS_FndSetGroupIDForExpHeap(NNSFndHeapHandle heap, u32 groupID);
u16 NNS_FndGetGroupIDForExpHeap(NNSFndHeapHandle heap);
void NNS_FndVisitAllocatedForExpHeap(NNSFndHeapHandle heap, NNSFndHeapVisitor visitor,
                                     u32 userParam);
u32 NNS_FndGetSizeForMBlockExpHeap(const void *memBlock);

/* The two state calls return the SDK's BOOL, an int; include/types.h has none. */
NNSFndHeapHandle NNS_FndCreateFrmHeapEx(void *startAddress, u32 size, u16 optFlag);
void NNS_FndDestroyFrmHeap(NNSFndHeapHandle heap);
void *NNS_FndAllocFromFrmHeapEx(NNSFndHeapHandle heap, u32 size, int alignment);
void NNS_FndFreeToFrmHeap(NNSFndHeapHandle heap, int mode);
u32 NNS_FndGetAllocatableSizeForFrmHeapEx(NNSFndHeapHandle heap, int alignment);
int NNS_FndRecordStateForFrmHeap(NNSFndHeapHandle heap, u32 tagName);
int NNS_FndFreeByStateToFrmHeap(NNSFndHeapHandle heap, u32 tagName);
u32 NNS_FndAdjustFrmHeap(NNSFndHeapHandle heap);
u32 NNS_FndResizeForMBlockFrmHeap(NNSFndHeapHandle heap, void *memBlock, u32 newSize);

#ifdef __cplusplus
}
#endif

#endif
