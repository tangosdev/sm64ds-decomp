//cpp
/* arm9/ExpandingHeapAllocator -- NitroSystem's expanding heap
 * (NNS_FND_expheap.c), .text 0x0204e084..0x0204e964.
 *
 * The bodies follow pret/pokediamond's arm9/lib/libnns NNS_FND_expheap.c,
 * which carries twelve of these twenty functions, and the Sonic Rush
 * Adventure decomp's NitroSystem expheap.c for the rest.
 * mwccarm emits a file's functions in reverse order, and the ROM runs
 * NNS_FndGetSizeForMBlockExpHeap up to GetRegionOfMBlock: the source order
 * below reversed. The calls the game never makes (the allocation-mode and
 * per-block group/direction getters) are not in the cartridge.
 *
 * An expanding heap keeps two address-ordered lists of 0x10-byte block
 * headers right after its NNSiFndHeapHead: the free blocks and the used
 * ones. Allocation splits a free block into up to three pieces; freeing
 * merges the block back with its free neighbours (RecycleRegion).
 *
 * Divergences from those sources, each forced by this build:
 * - GetRegionOfMBlock, RemoveMBlock, InsertMBlock, InitMBlock, InitExpHeap,
 *   AllocUsedBlockFromFreeBlock, AllocFromHead, AllocFromTail and
 *   RecycleRegion are static there and have external linkage here, as
 *   pret's own copy and the NNS_FND_list TU's helpers do: the baseline link
 *   does not apply symbol_binding_rewrites to a .text-only TU.
 * - FillAllocMemory's zero goes through a volatile: the ROM passes
 *   MI_CpuFill32's value through a stack slot (OS_CreateThread's fill does
 *   the same).
 * - NNS_FndSetGroupIDForExpHeap takes a u32 groupID, not the SDK's u16: its
 *   one caller passes a u32 through un-narrowed (see include/nnsys/fnd.h).
 */
#include "nnsys/fnd.h"
#include "msl/stdlib.h"
#include "nitro/mi/memory.h"

/* local extern: no header declares this yet. */

extern "C" {

#define MBLOCK_FREE_SIGNATURE 0x4652 /* 'FR' */
#define MBLOCK_USED_SIGNATURE 0x5544 /* 'UD' */
#define MIN_ALIGNMENT 4

#define NNSi_FndRoundUp(value, alignment) (((value) + (alignment - 1)) & ~(alignment - 1))
#define NNSi_FndRoundUpPtr(ptr, alignment) ((void *)NNSi_FndRoundUp(NNSiGetUIntPtr(ptr), alignment))
#define NNSi_FndRoundDown(value, alignment) ((value) & ~(alignment - 1))
#define NNSi_FndRoundDownPtr(ptr, alignment) ((void *)NNSi_FndRoundDown(NNSiGetUIntPtr(ptr), alignment))

#define NNSi_FndGetBitValue(data, st, bits) (((data) >> (st)) & ((1 << (bits)) - 1))
#define NNSi_FndSetBitValue(data, st, bits, val)                                                   \
    do {                                                                                           \
        u32 maskBits = (u32)((1 << (bits)) - 1);                                                   \
        u32 newVal = (val) & maskBits;                                                             \
        (void)(maskBits <<= st);                                                                   \
        (data) &= ~maskBits;                                                                       \
        (data) |= newVal << (st);                                                                  \
    } while (0)

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

static inline NNSiFndExpHeapHead *GetExpHeapHeadPtrFromHeapHead(NNSiFndHeapHead *pHHead) {
    return (NNSiFndExpHeapHead *)AddU32ToPtr(pHHead, sizeof(NNSiFndHeapHead));
}

static inline NNSiFndHeapHead *GetHeapHeadPtrFromExpHeapHead(NNSiFndExpHeapHead *pEHHead) {
    return (NNSiFndHeapHead *)SubU32ToPtr(pEHHead, sizeof(NNSiFndHeapHead));
}

static inline NNSiFndExpHeapHead *GetExpHeapHeadPtrFromHandle(NNSFndHeapHandle heap) {
    return GetExpHeapHeadPtrFromHeapHead(heap);
}

static inline void *GetMemPtrForMBlock(NNSiFndExpHeapMBlockHead *pMBlkHd) {
    return AddU32ToPtr(pMBlkHd, sizeof(NNSiFndExpHeapMBlockHead));
}

static inline NNSiFndExpHeapMBlockHead *GetMBlockHeadPtr(void *memBlock) {
    return (NNSiFndExpHeapMBlockHead *)SubU32ToPtr(memBlock, sizeof(NNSiFndExpHeapMBlockHead));
}

static inline const NNSiFndExpHeapMBlockHead *GetMBlockHeadCPtr(const void *memBlock) {
    return (const NNSiFndExpHeapMBlockHead *)(NNSiGetUIntPtr(memBlock) -
                                              sizeof(NNSiFndExpHeapMBlockHead));
}

static inline void *GetMBlockEndAddr(NNSiFndExpHeapMBlockHead *pMBHead) {
    return AddU32ToPtr(GetMemPtrForMBlock(pMBHead), pMBHead->blockSize);
}

static inline u16 GetAlignmentForMBlock(const NNSiFndExpHeapMBlockHead *pMBlkHd) {
    return (u16)NNSi_FndGetBitValue(pMBlkHd->attribute, 8, 7);
}

static inline void SetAlignmentForMBlock(NNSiFndExpHeapMBlockHead *pMBlkHd, u16 alignment) {
    NNSi_FndSetBitValue(pMBlkHd->attribute, 8, 7, alignment);
}

static inline void SetGroupIDForMBlock(NNSiFndExpHeapMBlockHead *pMBHead, u16 id) {
    NNSi_FndSetBitValue(pMBHead->attribute, 0, 8, id);
}

static inline void SetAllocDirForMBlock(NNSiFndExpHeapMBlockHead *pMBHead, u16 mode) {
    NNSi_FndSetBitValue(pMBHead->attribute, 15, 1, mode);
}

static inline u16 GetAllocMode(NNSiFndExpHeapHead *pEHHead) {
    return (u16)NNSi_FndGetBitValue(pEHHead->feature, 0, 1);
}

static inline void SetAllocMode(NNSiFndExpHeapHead *pEHHead, u16 mode) {
    NNSi_FndSetBitValue(pEHHead->feature, 0, 1, mode);
}

/* The span a block really occupies: its header, plus the alignment padding
   in front of it. */
void GetRegionOfMBlock(NNSiMemRegion *region, NNSiFndExpHeapMBlockHead *block) {
    region->start = SubU32ToPtr(block, GetAlignmentForMBlock(block));
    region->end = GetMBlockEndAddr(block);
}

/* Unlinks block and returns its predecessor, which is where a merged
   replacement goes back in. */
NNSiFndExpHeapMBlockHead *RemoveMBlock(NNSiFndExpMBlockList *list,
                                       NNSiFndExpHeapMBlockHead *block) {
    NNSiFndExpHeapMBlockHead *const prev = block->pMBHeadPrev;
    NNSiFndExpHeapMBlockHead *const next = block->pMBHeadNext;

    if (prev != 0) {
        prev->pMBHeadNext = next;
    } else {
        list->head = next;
    }

    if (next != 0) {
        next->pMBHeadPrev = prev;
    } else {
        list->tail = prev;
    }

    return prev;
}

/* Links target in after prev, or at the head when prev is 0. */
NNSiFndExpHeapMBlockHead *InsertMBlock(NNSiFndExpMBlockList *list, NNSiFndExpHeapMBlockHead *target,
                                       NNSiFndExpHeapMBlockHead *prev) {
    NNSiFndExpHeapMBlockHead *next;

    target->pMBHeadPrev = prev;
    if (prev != 0) {
        next = prev->pMBHeadNext;
        prev->pMBHeadNext = target;
    } else {
        next = list->head;
        list->head = target;
    }

    target->pMBHeadNext = next;
    if (next != 0) {
        next->pMBHeadPrev = target;
    } else {
        list->tail = target;
    }

    return target;
}

static inline void AppendMBlock(NNSiFndExpMBlockList *list, NNSiFndExpHeapMBlockHead *block) {
    (void)InsertMBlock(list, block, list->tail);
}

/* Stamps a block header at the start of a region. */
NNSiFndExpHeapMBlockHead *InitMBlock(const NNSiMemRegion *pRegion, u16 signature) {
    NNSiFndExpHeapMBlockHead *block = (NNSiFndExpHeapMBlockHead *)pRegion->start;

    block->signature = signature;
    block->attribute = 0;
    block->blockSize = GetOffsetFromPtr(GetMemPtrForMBlock(block), pRegion->end);
    block->pMBHeadPrev = 0;
    block->pMBHeadNext = 0;

    return block;
}

static inline NNSiFndExpHeapMBlockHead *InitFreeMBlock(const NNSiMemRegion *pRegion) {
    return InitMBlock(pRegion, MBLOCK_FREE_SIGNATURE);
}

/* The whole heap starts as one free block. */
NNSiFndHeapHead *InitExpHeap(void *startAddress, void *endAddress, u16 optFlag) {
    NNSiFndHeapHead *pHeapHd = (NNSiFndHeapHead *)startAddress;
    NNSiFndExpHeapHead *pExpHeapHd = GetExpHeapHeadPtrFromHeapHead(pHeapHd);

    NNSi_FndInitHeapHead(pHeapHd, 0x45585048, /* 'EXPH' */
                         AddU32ToPtr(pExpHeapHd, sizeof(NNSiFndExpHeapHead)), endAddress, optFlag);

    pExpHeapHd->groupID = 0;
    pExpHeapHd->feature = 0;
    SetAllocMode(pExpHeapHd, 0);

    {
        NNSiFndExpHeapMBlockHead *pMBHead;
        NNSiMemRegion region;

        region.start = pHeapHd->heapStart;
        region.end = pHeapHd->heapEnd;
        pMBHead = InitFreeMBlock(&region);

        pExpHeapHd->mbFreeList.head = pMBHead;
        pExpHeapHd->mbFreeList.tail = pMBHead;
        pExpHeapHd->mbUsedList.head = 0;
        pExpHeapHd->mbUsedList.tail = 0;
    }

    return pHeapHd;
}

/* Carves mblock (size bytes) out of a free block: the free space left in
   front of it and behind it each become a free block again unless too small
   to hold a header, in which case it is absorbed into the allocation. */
void *AllocUsedBlockFromFreeBlock(NNSiFndExpHeapHead *pEHHead,
                                  NNSiFndExpHeapMBlockHead *pMBHeadFree, void *mblock, u32 size,
                                  u16 direction) {
    NNSiMemRegion freeRgnT;
    NNSiMemRegion freeRgnB;
    NNSiFndExpHeapMBlockHead *pMBHeadFreePrev;

    GetRegionOfMBlock(&freeRgnT, pMBHeadFree);
    freeRgnB.end = freeRgnT.end;
    freeRgnB.start = AddU32ToPtr(mblock, size);
    freeRgnT.end = SubU32ToPtr(mblock, sizeof(NNSiFndExpHeapMBlockHead));

    pMBHeadFreePrev = RemoveMBlock(&pEHHead->mbFreeList, pMBHeadFree);

    if (GetOffsetFromPtr(freeRgnT.start, freeRgnT.end) < sizeof(NNSiFndExpHeapMBlockHead)) {
        freeRgnT.end = freeRgnT.start;
    } else {
        pMBHeadFreePrev =
            InsertMBlock(&pEHHead->mbFreeList, InitFreeMBlock(&freeRgnT), pMBHeadFreePrev);
    }

    if (GetOffsetFromPtr(freeRgnB.start, freeRgnB.end) < sizeof(NNSiFndExpHeapMBlockHead)) {
        freeRgnB.start = freeRgnB.end;
    } else {
        (void)InsertMBlock(&pEHHead->mbFreeList, InitFreeMBlock(&freeRgnB), pMBHeadFreePrev);
    }

    FillAllocMemory(GetHeapHeadPtrFromExpHeapHead(pEHHead), freeRgnT.end,
                    GetOffsetFromPtr(freeRgnT.end, freeRgnB.start));

    {
        NNSiFndExpHeapMBlockHead *pMBHeadNewUsed;
        NNSiMemRegion region;

        region.start = SubU32ToPtr(mblock, sizeof(NNSiFndExpHeapMBlockHead));
        region.end = freeRgnB.start;

        pMBHeadNewUsed = InitMBlock(&region, MBLOCK_USED_SIGNATURE);
        SetAllocDirForMBlock(pMBHeadNewUsed, direction);
        SetAlignmentForMBlock(pMBHeadNewUsed, (u16)GetOffsetFromPtr(freeRgnT.end, pMBHeadNewUsed));
        SetGroupIDForMBlock(pMBHeadNewUsed, pEHHead->groupID);
        AppendMBlock(&pEHHead->mbUsedList, pMBHeadNewUsed);
    }

    return mblock;
}

/* Searches the free list from the low end, aligning upward. */
void *AllocFromHead(NNSiFndHeapHead *pHeapHd, u32 size, int alignment) {
    NNSiFndExpHeapHead *pExpHeapHd = GetExpHeapHeadPtrFromHeapHead(pHeapHd);
    const int bAllocFirst = GetAllocMode(pExpHeapHd) == 0;

    NNSiFndExpHeapMBlockHead *pMBlkHd = 0;
    NNSiFndExpHeapMBlockHead *pMBlkHdFound = 0;
    u32 foundSize = 0xffffffff;
    void *foundMBlock = 0;

    for (pMBlkHd = pExpHeapHd->mbFreeList.head; pMBlkHd; pMBlkHd = pMBlkHd->pMBHeadNext) {
        void *const mblock = GetMemPtrForMBlock(pMBlkHd);
        void *const reqMBlock = NNSi_FndRoundUpPtr(mblock, alignment);
        const u32 offset = GetOffsetFromPtr(mblock, reqMBlock);

        if (pMBlkHd->blockSize >= size + offset && foundSize > pMBlkHd->blockSize) {
            pMBlkHdFound = pMBlkHd;
            foundSize = pMBlkHd->blockSize;
            foundMBlock = reqMBlock;

            if (bAllocFirst || foundSize == size) {
                break;
            }
        }
    }

    if (!pMBlkHdFound) {
        return 0;
    }

    return AllocUsedBlockFromFreeBlock(pExpHeapHd, pMBlkHdFound, foundMBlock, size, 0);
}

/* Searches the free list from the high end, aligning downward. */
void *AllocFromTail(NNSiFndHeapHead *pHeapHd, u32 size, int alignment) {
    NNSiFndExpHeapHead *pExpHeapHd = GetExpHeapHeadPtrFromHeapHead(pHeapHd);
    const int bAllocFirst = GetAllocMode(pExpHeapHd) == 0;

    NNSiFndExpHeapMBlockHead *pMBlkHd = 0;
    NNSiFndExpHeapMBlockHead *pMBlkHdFound = 0;
    u32 foundSize = 0xffffffff;
    void *foundMBlock = 0;

    for (pMBlkHd = pExpHeapHd->mbFreeList.tail; pMBlkHd; pMBlkHd = pMBlkHd->pMBHeadPrev) {
        void *const mblock = GetMemPtrForMBlock(pMBlkHd);
        void *const mblockEnd = AddU32ToPtr(mblock, pMBlkHd->blockSize);
        void *const reqMBlock = NNSi_FndRoundDownPtr(SubU32ToPtr(mblockEnd, size), alignment);

        if (ComparePtr(reqMBlock, mblock) >= 0 && foundSize > pMBlkHd->blockSize) {
            pMBlkHdFound = pMBlkHd;
            foundSize = pMBlkHd->blockSize;
            foundMBlock = reqMBlock;

            if (bAllocFirst || foundSize == size) {
                break;
            }
        }
    }

    if (!pMBlkHdFound) {
        return 0;
    }

    return AllocUsedBlockFromFreeBlock(pExpHeapHd, pMBlkHdFound, foundMBlock, size, 1);
}

/* Returns a region to the free list, merging it with the free blocks
   directly after and before it. */
int RecycleRegion(NNSiFndExpHeapHead *pEHHead, const NNSiMemRegion *pRegion) {
    NNSiFndExpHeapMBlockHead *pBlkPrFree = 0;
    NNSiMemRegion freeRgn = *pRegion;

    {
        NNSiFndExpHeapMBlockHead *pBlk;

        for (pBlk = pEHHead->mbFreeList.head; pBlk; pBlk = pBlk->pMBHeadNext) {
            if (pBlk < pRegion->start) {
                pBlkPrFree = pBlk;
                continue;
            }

            if (pBlk == pRegion->end) {
                freeRgn.end = GetMBlockEndAddr(pBlk);
                (void)RemoveMBlock(&pEHHead->mbFreeList, pBlk);
            }
            break;
        }
    }

    if (pBlkPrFree && GetMBlockEndAddr(pBlkPrFree) == pRegion->start) {
        freeRgn.start = pBlkPrFree;
        pBlkPrFree = RemoveMBlock(&pEHHead->mbFreeList, pBlkPrFree);
    }

    if (GetOffsetFromPtr(freeRgn.start, freeRgn.end) < sizeof(NNSiFndExpHeapMBlockHead)) {
        return 0;
    }

    (void)InsertMBlock(&pEHHead->mbFreeList, InitFreeMBlock(&freeRgn), pBlkPrFree);

    return 1;
}

/* Builds an expanding heap in [startAddress, startAddress + size), or
   returns 0 when the 4-aligned span cannot hold the two heap headers, a
   block header and one word. */
NNSFndHeapHandle NNS_FndCreateExpHeapEx(void *startAddress, u32 size, u16 optFlag) {
    void *endAddress;

    endAddress = NNSi_FndRoundDownPtr(AddU32ToPtr(startAddress, size), MIN_ALIGNMENT);
    startAddress = NNSi_FndRoundUpPtr(startAddress, MIN_ALIGNMENT);

    if (NNSiGetUIntPtr(startAddress) > NNSiGetUIntPtr(endAddress) ||
        GetOffsetFromPtr(startAddress, endAddress) <
            sizeof(NNSiFndHeapHead) + sizeof(NNSiFndExpHeapHead) +
                sizeof(NNSiFndExpHeapMBlockHead) + MIN_ALIGNMENT) {
        return 0;
    }

    {
        NNSiFndHeapHead *pHeapHd = InitExpHeap(startAddress, endAddress, optFlag);
        return pHeapHd;
    }
}

void NNS_FndDestroyExpHeap(NNSFndHeapHandle heap) {
    NNSi_FndFinalizeHeap(heap);
}

/* A non-negative alignment allocates from the low end, a negative one from
   the high end. */
void *NNS_FndAllocFromExpHeapEx(NNSFndHeapHandle heap, u32 size, int alignment) {
    void *memory = 0;

    if (size == 0) {
        size = 1;
    }

    size = NNSi_FndRoundUp(size, MIN_ALIGNMENT);

    if (alignment >= 0) {
        memory = AllocFromHead(heap, size, alignment);
    } else {
        memory = AllocFromTail(heap, size, -alignment);
    }

    return memory;
}

/* Grows a block in place into the free block right after it, or shrinks it
   and returns the tail to the free list. Returns the new size, or 0. */
u32 NNS_FndResizeForMBlockExpHeap(NNSFndHeapHandle heap, void *memBlock, u32 size) {
    NNSiFndExpHeapHead *pEHHead;
    NNSiFndExpHeapMBlockHead *pMBHead;

    pEHHead = GetExpHeapHeadPtrFromHandle(heap);
    pMBHead = GetMBlockHeadPtr(memBlock);

    size = NNSi_FndRoundUp(size, MIN_ALIGNMENT);
    if (size == pMBHead->blockSize) {
        return size;
    }

    if (size > pMBHead->blockSize) {
        void *crUsedEnd = GetMBlockEndAddr(pMBHead);
        NNSiFndExpHeapMBlockHead *block;

        for (block = pEHHead->mbFreeList.head; block; block = block->pMBHeadNext) {
            if (block == crUsedEnd) {
                break;
            }
        }

        if (!block ||
            size > pMBHead->blockSize + sizeof(NNSiFndExpHeapMBlockHead) + block->blockSize) {
            return 0;
        }

        {
            NNSiMemRegion rgnNewFree;
            void *oldFreeStart;
            NNSiFndExpHeapMBlockHead *nextBlockPrev;

            GetRegionOfMBlock(&rgnNewFree, block);
            nextBlockPrev = RemoveMBlock(&pEHHead->mbFreeList, block);

            oldFreeStart = rgnNewFree.start;
            rgnNewFree.start = AddU32ToPtr(memBlock, size);

            if (GetOffsetFromPtr(rgnNewFree.start, rgnNewFree.end) <
                sizeof(NNSiFndExpHeapMBlockHead)) {
                rgnNewFree.start = rgnNewFree.end;
            }

            pMBHead->blockSize = GetOffsetFromPtr(memBlock, rgnNewFree.start);

            if (GetOffsetFromPtr(rgnNewFree.start, rgnNewFree.end) >=
                sizeof(NNSiFndExpHeapMBlockHead)) {
                (void)InsertMBlock(&pEHHead->mbFreeList, InitFreeMBlock(&rgnNewFree),
                                   nextBlockPrev);
            }

            FillAllocMemory(heap, oldFreeStart, GetOffsetFromPtr(oldFreeStart, rgnNewFree.start));
        }
    } else {
        NNSiMemRegion rgnNewFree;
        const u32 oldBlockSize = pMBHead->blockSize;

        rgnNewFree.start = AddU32ToPtr(memBlock, size);
        rgnNewFree.end = GetMBlockEndAddr(pMBHead);

        pMBHead->blockSize = size;

        if (!RecycleRegion(pEHHead, &rgnNewFree)) {
            pMBHead->blockSize = oldBlockSize;
        }
    }

    return pMBHead->blockSize;
}

void NNS_FndFreeToExpHeap(NNSFndHeapHandle heap, void *memBlock) {
    NNSiFndHeapHead *pHeapHd = heap;
    NNSiFndExpHeapHead *pExpHeapHd = GetExpHeapHeadPtrFromHandle(pHeapHd);
    NNSiFndExpHeapMBlockHead *pMBHead = GetMBlockHeadPtr(memBlock);
    NNSiMemRegion region;

    GetRegionOfMBlock(&region, pMBHead);
    (void)RemoveMBlock(&pExpHeapHd->mbUsedList, pMBHead);
    (void)RecycleRegion(pExpHeapHd, &region);
}

/* The sum of every free block; fragmentation can keep any single
   allocation from getting this much. */
u32 NNS_FndGetTotalFreeSizeForExpHeap(NNSFndHeapHandle heap) {
    u32 sumSize = 0;
    NNSiFndExpHeapHead *pEHHead = GetExpHeapHeadPtrFromHandle(heap);
    NNSiFndExpHeapMBlockHead *pMBHead;

    for (pMBHead = pEHHead->mbFreeList.head; pMBHead; pMBHead = pMBHead->pMBHeadNext) {
        sumSize += pMBHead->blockSize;
    }

    return sumSize;
}

/* The largest single block obtainable at this alignment; ties go to the
   block that wastes least on padding. */
u32 NNS_FndGetAllocatableSizeForExpHeapEx(NNSFndHeapHandle heap, int alignment) {
    alignment = abs(alignment);

    {
        NNSiFndExpHeapHead *pEHHead = GetExpHeapHeadPtrFromHandle(heap);
        u32 maxSize = 0;
        u32 offsetMin = 0xffffffff;
        NNSiFndExpHeapMBlockHead *pMBlkHd;

        for (pMBlkHd = pEHHead->mbFreeList.head; pMBlkHd; pMBlkHd = pMBlkHd->pMBHeadNext) {
            void *baseAddress = NNSi_FndRoundUpPtr(GetMemPtrForMBlock(pMBlkHd), alignment);

            if (NNSiGetUIntPtr(baseAddress) < NNSiGetUIntPtr(GetMBlockEndAddr(pMBlkHd))) {
                const u32 blockSize = GetOffsetFromPtr(baseAddress, GetMBlockEndAddr(pMBlkHd));
                const u32 offset = GetOffsetFromPtr(GetMemPtrForMBlock(pMBlkHd), baseAddress);

                if (maxSize < blockSize || (maxSize == blockSize && offsetMin > offset)) {
                    maxSize = blockSize;
                    offsetMin = offset;
                }
            }
        }

        return maxSize;
    }
}

/* The group ID every block allocated from now on is stamped with. */
u16 NNS_FndSetGroupIDForExpHeap(NNSFndHeapHandle heap, u32 groupID) {
    NNSiFndExpHeapHead *pEHHead = GetExpHeapHeadPtrFromHandle(heap);
    u16 oldGroupID = pEHHead->groupID;
    pEHHead->groupID = groupID;

    return oldGroupID;
}

u16 NNS_FndGetGroupIDForExpHeap(NNSFndHeapHandle heap) {
    return GetExpHeapHeadPtrFromHandle(heap)->groupID;
}

/* Calls visitor on every allocated block. The next block is read first, so
   the visitor may free the one it is given. */
void NNS_FndVisitAllocatedForExpHeap(NNSFndHeapHandle heap, NNSFndHeapVisitor visitor,
                                     u32 userParam) {
    NNSiFndExpHeapMBlockHead *pMBHead = GetExpHeapHeadPtrFromHandle(heap)->mbUsedList.head;

    while (pMBHead) {
        NNSiFndExpHeapMBlockHead *pMBHeadNext = pMBHead->pMBHeadNext;
        (*visitor)(GetMemPtrForMBlock(pMBHead), heap, userParam);
        pMBHead = pMBHeadNext;
    }
}

u32 NNS_FndGetSizeForMBlockExpHeap(const void *memBlock) {
    return GetMBlockHeadCPtr(memBlock)->blockSize;
}

} /* extern "C" */
