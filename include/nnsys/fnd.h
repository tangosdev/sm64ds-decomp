#ifndef NNSYS_FND_H
#define NNSYS_FND_H
#ifndef __cplusplus
#define NNS_FndGetSizeForMBlockExpHeap _Z30NNS_FndGetSizeForMBlockExpHeapPKv
#define NNS_FndVisitAllocatedForExpHeap _Z31NNS_FndVisitAllocatedForExpHeapP15NNSiFndHeapHeadPFvPvS0_jEj
#define NNS_FndGetGroupIDForExpHeap _Z27NNS_FndGetGroupIDForExpHeapP15NNSiFndHeapHead
#define NNS_FndSetGroupIDForExpHeap _Z27NNS_FndSetGroupIDForExpHeapP15NNSiFndHeapHeadj
#define NNS_FndGetAllocatableSizeForExpHeapEx _Z37NNS_FndGetAllocatableSizeForExpHeapExP15NNSiFndHeapHeadi
#define NNS_FndGetTotalFreeSizeForExpHeap _Z33NNS_FndGetTotalFreeSizeForExpHeapP15NNSiFndHeapHead
#define NNS_FndFreeToExpHeap _Z20NNS_FndFreeToExpHeapP15NNSiFndHeapHeadPv
#define NNS_FndResizeForMBlockExpHeap _Z29NNS_FndResizeForMBlockExpHeapP15NNSiFndHeapHeadPvj
#define NNS_FndAllocFromExpHeapEx _Z25NNS_FndAllocFromExpHeapExP15NNSiFndHeapHeadji
#define NNS_FndDestroyExpHeap _Z21NNS_FndDestroyExpHeapP15NNSiFndHeapHead
#define NNS_FndCreateExpHeapEx _Z22NNS_FndCreateExpHeapExPvjt
#define NNS_FndGetPrevListObject _Z24NNS_FndGetPrevListObjectP10NNSFndListPv
#define NNS_FndGetNextListObject _Z24NNS_FndGetNextListObjectP10NNSFndListPv
#define NNS_FndRemoveListObject _Z23NNS_FndRemoveListObjectP10NNSFndListPv
#define NNS_FndInsertListObject _Z23NNS_FndInsertListObjectP10NNSFndListPvS1_
#define NNS_FndPrependListObject _Z24NNS_FndPrependListObjectP10NNSFndListPv
#define NNS_FndAppendListObject _Z23NNS_FndAppendListObjectP10NNSFndListPv
#define NNS_FndInitList _Z15NNS_FndInitListP10NNSFndListt
#define NNSi_FndFinalizeHeap _Z20NNSi_FndFinalizeHeapP15NNSiFndHeapHead
#define NNSi_FndInitHeapHead _Z20NNSi_FndInitHeapHeadP15NNSiFndHeapHeadjPvS1_t
#define NNS_FndResizeForMBlockFrmHeap _Z29NNS_FndResizeForMBlockFrmHeapP15NNSiFndHeapHeadPvj
#define NNS_FndAdjustFrmHeap _Z20NNS_FndAdjustFrmHeapP15NNSiFndHeapHead
#define NNS_FndFreeByStateToFrmHeap _Z27NNS_FndFreeByStateToFrmHeapP15NNSiFndHeapHeadj
#define NNS_FndRecordStateForFrmHeap _Z28NNS_FndRecordStateForFrmHeapP15NNSiFndHeapHeadj
#define NNS_FndGetAllocatableSizeForFrmHeapEx _Z37NNS_FndGetAllocatableSizeForFrmHeapExP15NNSiFndHeapHeadi
#define NNS_FndFreeToFrmHeap _Z20NNS_FndFreeToFrmHeapP15NNSiFndHeapHeadi
#define NNS_FndAllocFromFrmHeapEx _Z25NNS_FndAllocFromFrmHeapExP15NNSiFndHeapHeadji
#define NNS_FndDestroyFrmHeap _Z21NNS_FndDestroyFrmHeapP15NNSiFndHeapHead
#define NNS_FndCreateFrmHeapEx _Z22NNS_FndCreateFrmHeapExPvjt
#endif

#include "types.h"

/*
 * NitroSystem foundation library (fnd): the intrusive list, the heap header,
 * and the expanding and frame heaps built on it. Names as the Sonic Rush
 * Adventure decomp (its NitroSystem fnd sources) and pret/pokediamond
 * (arm9/lib/libnns, NNS_FND_list.c, NNS_FND_heapcommon.c and
 * NNS_FND_expheap.c) spell them.
 */


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


#endif
