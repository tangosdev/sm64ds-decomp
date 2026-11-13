//cpp
/* arm9/SolidHeapAllocator -- NitroSystem's frame heap (NNS_FND_frameheap.c),
 * .text 0x0204e964..0x0204eda4.
 *
 * The bodies follow the Sonic Rush Adventure decomp's
 * NitroSystem frameheap.c; pret has no copy of this file.
 * mwccarm emits a file's functions in reverse order, and the ROM runs
 * NNS_FndResizeForMBlockFrmHeap up to InitFrameHeap: the source order below
 * reversed. NNSi_FndGetFreeStartForFrmHeap and NNSi_FndGetFreeEndForFrmHeap
 * are not in the cartridge.
 *
 * A frame heap hands memory out from both ends of one free region
 * [headAllocator, tailAllocator) and never frees a single block: it rewinds
 * an end (NNS_FndFreeToFrmHeap) or returns to a restore point
 * (NNS_FndRecordStateForFrmHeap / NNS_FndFreeByStateToFrmHeap), whose
 * record is itself allocated from the heap it describes.
 *
 * Divergences from that source, each forced by this build:
 * - InitFrameHeap, AllocFromHead, AllocFromTail, FreeHead and FreeTail are
 *   static there and have external linkage here, as the expanding heap's
 *   helpers do: the baseline link does not apply symbol_binding_rewrites to
 *   a .text-only TU. AllocFromHead and AllocFromTail would then collide
 *   with the expanding heap's helpers of the same names, so these two keep
 *   C++ linkage, which tells them apart by their NNSiFndFrmHeapHead
 *   parameter.
 * - FillAllocMemory's zero goes through a volatile, as in the expanding
 *   heap. FillFreeMemory is empty in a release build and is left out.
 */
#include "nnsys/fnd.h"
#include "msl/stdlib.h"
#include "nitro/mi/memory.h"

#define MIN_ALIGNMENT 4

#define NNSi_FndRoundUp(value, alignment) (((value) + (alignment - 1)) & ~(alignment - 1))
#define NNSi_FndRoundUpPtr(ptr, alignment) ((void *)NNSi_FndRoundUp(NNSiGetUIntPtr(ptr), alignment))
#define NNSi_FndRoundDown(value, alignment) ((value) & ~(alignment - 1))
#define NNSi_FndRoundDownPtr(ptr, alignment) ((void *)NNSi_FndRoundDown(NNSiGetUIntPtr(ptr), alignment))

#define NNSi_FndGetBitValue(data, st, bits) (((data) >> (st)) & ((1 << (bits)) - 1))

static inline u32 NNSiGetUIntPtr(const void *ptr) {
    return (u32)ptr;
}

static inline u32 GetOffsetFromPtr(const void *start, const void *end) {
    return NNSiGetUIntPtr(end) - NNSiGetUIntPtr(start);
}

static inline void *AddU32ToPtr(void *ptr, u32 val) {
    return (void *)(NNSiGetUIntPtr(ptr) + val);
}

static inline void *SubU32ToPtr(void *ptr, u32 val) {
    return (void *)(NNSiGetUIntPtr(ptr) - val);
}

static inline int ComparePtr(const void *a, const void *b) {
    return (const u8 *)a - (const u8 *)b;
}

static inline u16 GetOptForHeap(const NNSiFndHeapHead *pHeapHd) {
    return (u16)NNSi_FndGetBitValue(pHeapHd->attribute, 0, 8);
}

static inline void FillAllocMemory(NNSiFndHeapHead *pHeapHd, void *address, u32 size) {
    if (GetOptForHeap(pHeapHd) & 1) {
        volatile int fill = 0;
        MI_CpuFill32(address, fill, size);
    }
}

static inline NNSiFndFrmHeapHead *GetFrmHeapHeadPtrFromHeapHead(NNSiFndHeapHead *pHHead) {
    return (NNSiFndFrmHeapHead *)AddU32ToPtr(pHHead, sizeof(NNSiFndHeapHead));
}

static inline NNSiFndHeapHead *GetHeapHeadPtrFromFrmHeapHead(NNSiFndFrmHeapHead *pFrmHeapHd) {
    return (NNSiFndHeapHead *)SubU32ToPtr(pFrmHeapHd, sizeof(NNSiFndHeapHead));
}

NNSiFndHeapHead *InitFrameHeap(void *startAddress, void *endAddress, u16 optFlag) {
    NNSiFndHeapHead *pHeapHd = (NNSiFndHeapHead *)startAddress;
    NNSiFndFrmHeapHead *pFrmHeapHd = GetFrmHeapHeadPtrFromHeapHead(pHeapHd);

    NNSi_FndInitHeapHead(pHeapHd, 0x46524d48, /* 'FRMH' */
                         AddU32ToPtr(pFrmHeapHd, sizeof(NNSiFndFrmHeapHead)), endAddress, optFlag);

    pFrmHeapHd->headAllocator = pHeapHd->heapStart;
    pFrmHeapHd->tailAllocator = pHeapHd->heapEnd;

    pFrmHeapHd->pState = 0;

    return pHeapHd;
}

/* Rounds the head up to the alignment and bumps it past the block. */
void *AllocFromHead(NNSiFndFrmHeapHead *pFrmHeapHd, u32 size, int alignment) {
    void *newBlock = NNSi_FndRoundUpPtr(pFrmHeapHd->headAllocator, alignment);
    void *endAddress = AddU32ToPtr(newBlock, size);

    if (NNSiGetUIntPtr(endAddress) > NNSiGetUIntPtr(pFrmHeapHd->tailAllocator)) {
        return 0;
    }

    FillAllocMemory(GetHeapHeadPtrFromFrmHeapHead(pFrmHeapHd), pFrmHeapHd->headAllocator,
                    GetOffsetFromPtr(pFrmHeapHd->headAllocator, endAddress));

    pFrmHeapHd->headAllocator = endAddress;

    return newBlock;
}

/* Pulls the tail down by the size and rounds it down to the alignment, so
   the padding lands above the block. */
void *AllocFromTail(NNSiFndFrmHeapHead *pFrmHeapHd, u32 size, int alignment) {
    void *newBlock = NNSi_FndRoundDownPtr(SubU32ToPtr(pFrmHeapHd->tailAllocator, size), alignment);

    if (NNSiGetUIntPtr(newBlock) < NNSiGetUIntPtr(pFrmHeapHd->headAllocator)) {
        return 0;
    }

    FillAllocMemory(GetHeapHeadPtrFromFrmHeapHead(pFrmHeapHd), newBlock,
                    GetOffsetFromPtr(newBlock, pFrmHeapHd->tailAllocator));

    pFrmHeapHd->tailAllocator = newBlock;

    return newBlock;
}



/* Frees every block allocated from the head, and every restore point with
   them: the records live in that space. */
void FreeHead(NNSiFndHeapHead *pHeapHd) {
    NNSiFndFrmHeapHead *pFrmHeapHd = GetFrmHeapHeadPtrFromHeapHead(pHeapHd);

    pFrmHeapHd->headAllocator = pHeapHd->heapStart;
    pFrmHeapHd->pState = 0;
}

/* Frees every block allocated from the tail, and moves each restore
   point's tail to match so none restores freed space. */
void FreeTail(NNSiFndHeapHead *pHeapHd) {
    NNSiFndFrmHeapHead *pFrmHeapHd = GetFrmHeapHeadPtrFromHeapHead(pHeapHd);

    {
        NNSiFndFrmHeapState *pState;
        for (pState = pFrmHeapHd->pState; pState; pState = pState->pPrevState) {
            pState->tailAllocator = pHeapHd->heapEnd;
        }
    }

    pFrmHeapHd->tailAllocator = pHeapHd->heapEnd;
}

/* Builds a frame heap in [startAddress, startAddress + size), or returns
   NULL when the 4-aligned span cannot hold the two heap headers. */
NNSFndHeapHandle NNS_FndCreateFrmHeapEx(void *startAddress, u32 size, u16 optFlag) {
    void *endAddress;

    endAddress = NNSi_FndRoundDownPtr(AddU32ToPtr(startAddress, size), MIN_ALIGNMENT);
    startAddress = NNSi_FndRoundUpPtr(startAddress, MIN_ALIGNMENT);

    if (NNSiGetUIntPtr(startAddress) > NNSiGetUIntPtr(endAddress) ||
        GetOffsetFromPtr(startAddress, endAddress) <
            sizeof(NNSiFndHeapHead) + sizeof(NNSiFndFrmHeapHead)) {
        return 0;
    }

    {
        NNSiFndHeapHead *pHHead = InitFrameHeap(startAddress, endAddress, optFlag);
        return pHHead;
    }
}

void NNS_FndDestroyFrmHeap(NNSFndHeapHandle heap) {
    NNSi_FndFinalizeHeap(heap);
}

/* A non-negative alignment allocates from the head, a negative one from
   the tail. */
void *NNS_FndAllocFromFrmHeapEx(NNSFndHeapHandle heap, u32 size, int alignment) {
    void *memory = 0;
    NNSiFndFrmHeapHead *pFrmHeapHd;

    pFrmHeapHd = GetFrmHeapHeadPtrFromHeapHead(heap);

    if (size == 0) {
        size = 1;
    }

    size = NNSi_FndRoundUp(size, MIN_ALIGNMENT);

    if (alignment >= 0) {
        memory = AllocFromHead(pFrmHeapHd, size, alignment);
    } else {
        memory = AllocFromTail(pFrmHeapHd, size, -alignment);
    }

    return memory;
}

void NNS_FndFreeToFrmHeap(NNSFndHeapHandle heap, int mode) {
    if (mode & NNS_FND_FRMHEAP_FREE_HEAD) {
        FreeHead(heap);
    }

    if (mode & NNS_FND_FRMHEAP_FREE_TAIL) {
        FreeTail(heap);
    }
}

/* What a head allocation at this alignment could still get. */
u32 NNS_FndGetAllocatableSizeForFrmHeapEx(NNSFndHeapHandle heap, int alignment) {
    alignment = abs(alignment);

    {
        const NNSiFndFrmHeapHead *pFrmHeapHd = GetFrmHeapHeadPtrFromHeapHead(heap);
        const void *block = NNSi_FndRoundUpPtr(pFrmHeapHd->headAllocator, alignment);

        if (NNSiGetUIntPtr(block) > NNSiGetUIntPtr(pFrmHeapHd->tailAllocator)) {
            return 0;
        }

        return GetOffsetFromPtr(block, pFrmHeapHd->tailAllocator);
    }
}

/* Pushes a restore point tagged tagName. Its record is allocated from the
   head, after the head it records, so restoring it frees the record too. */
int NNS_FndRecordStateForFrmHeap(NNSFndHeapHandle heap, u32 tagName) {
    {
        NNSiFndFrmHeapHead *pFrmHeapHd = GetFrmHeapHeadPtrFromHeapHead(heap);
        void *oldHeadAllocator = pFrmHeapHd->headAllocator;

        NNSiFndFrmHeapState *pState = (NNSiFndFrmHeapState *)AllocFromHead(
            pFrmHeapHd, sizeof(NNSiFndFrmHeapState), MIN_ALIGNMENT);
        if (!pState) {
            return 0;
        }

        pState->tagName = tagName;
        pState->headAllocator = oldHeadAllocator;
        pState->tailAllocator = pFrmHeapHd->tailAllocator;
        pState->pPrevState = pFrmHeapHd->pState;

        pFrmHeapHd->pState = pState;

        return 1;
    }
}

/* Returns to the restore point tagged tagName, or to the most recent one
   when tagName is 0. */
int NNS_FndFreeByStateToFrmHeap(NNSFndHeapHandle heap, u32 tagName) {
    {
        NNSiFndFrmHeapHead *pFrmHeapHd = GetFrmHeapHeadPtrFromHeapHead(heap);
        NNSiFndFrmHeapState *pState = pFrmHeapHd->pState;

        if (tagName != 0) {
            for (; pState; pState = pState->pPrevState) {
                if (pState->tagName == tagName)
                    break;
            }
        }

        if (!pState) {
            return 0;
        }

        pFrmHeapHd->headAllocator = pState->headAllocator;
        pFrmHeapHd->tailAllocator = pState->tailAllocator;

        pFrmHeapHd->pState = pState->pPrevState;

        return 1;
    }
}

/* Shrinks the heap to its head allocations, giving the rest back to the
   heap that contains it. Refused (0) while anything is allocated from the
   tail; otherwise returns the heap's new size. */
u32 NNS_FndAdjustFrmHeap(NNSFndHeapHandle heap) {
    {
        NNSiFndHeapHead *pHeapHd = heap;
        NNSiFndFrmHeapHead *pFrmHeapHd = GetFrmHeapHeadPtrFromHeapHead(pHeapHd);

        if (0 < GetOffsetFromPtr(pFrmHeapHd->tailAllocator, pHeapHd->heapEnd)) {
            return 0;
        }

        pFrmHeapHd->tailAllocator = pHeapHd->heapEnd = pFrmHeapHd->headAllocator;
        return GetOffsetFromPtr(heap, pHeapHd->heapEnd);
    }
}

/* Resizes a block in place, which works only for the most recent head
   allocation: its end is taken to be the head. Returns the new size, or 0
   when growing would run into the tail. */
u32 NNS_FndResizeForMBlockFrmHeap(NNSFndHeapHandle heap, void *memBlock, u32 newSize) {
    NNSiFndHeapHead *pHeapHd = 0;
    NNSiFndFrmHeapHead *pFrmHeapHd = 0;

    pHeapHd = heap;
    pFrmHeapHd = GetFrmHeapHeadPtrFromHeapHead(pHeapHd);

    if (newSize == 0) {
        newSize = 1;
    }

    newSize = NNSi_FndRoundUp(newSize, MIN_ALIGNMENT);

    {
        const u32 oldSize = GetOffsetFromPtr(memBlock, pFrmHeapHd->headAllocator);
        void *endAddress = AddU32ToPtr(memBlock, newSize);

        if (newSize == oldSize) {
            return newSize;
        }

        if (newSize > oldSize) {
            if (ComparePtr(endAddress, pFrmHeapHd->tailAllocator) > 0) {
                return 0;
            }

            FillAllocMemory(heap, pFrmHeapHd->headAllocator, newSize - oldSize);
        }

        pFrmHeapHd->headAllocator = endAddress;
        return newSize;
    }
}

 /* extern "C" */
