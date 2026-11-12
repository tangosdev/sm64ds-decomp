//cpp
/* Heaps inside an arena (the Nitro SDK's os_alloc.c), arm9 .text
 * 0x02059040..0x020593a8.
 *
 * pret/pokediamond's NitroSDK OS_alloc.c has, in this order, DLAddFront,
 * DLExtract, DLInsert, OS_AllocFromHeap and OS_FreeToHeap. pret leaves out
 * OS_InitAlloc, which comes after them in the SDK's file; 0x02059040 is it by
 * body. mwccarm defers the bodies and emits them in reverse, so the ROM runs
 * bottom-up: OS_InitAlloc at 0x02059040, DLAddFront at 0x0205938c. The SDK's
 * other heap functions (OS_CreateHeap, OS_SetCurrentHeap, ...) are not called
 * and were dropped by the linker.
 */
#include "nitro/os/system.h"
#include "nitro/os/alloc.h"

#define ALIGNMENT 32
#define ROUND(n, a) (((u32)(n) + (a) - 1) & ~((a) - 1))
#define TRUNC(n, a) (((u32)(n)) & ~((a) - 1))

/* A Cell heads every block, free or allocated; the caller's memory starts
 * HEADERSIZE past it. */
typedef struct Cell {
    struct Cell *prev; /* 0x0 */
    struct Cell *next; /* 0x4 */
    long size;         /* 0x8 -- header included */
} Cell;

typedef struct HeapDesc {
    long size;       /* 0x0 -- -1 while the heap is not created */
    Cell *free;      /* 0x4 -- by address */
    Cell *allocated; /* 0x8 */
} HeapDesc;

typedef struct OSHeapInfo {
    int currentHeap;     /* 0x00 -- the heap a negative handle means */
    int numHeaps;        /* 0x04 */
    void *arenaStart;    /* 0x08 */
    void *arenaEnd;      /* 0x0c */
    HeapDesc *heapArray; /* 0x10 */
} OSHeapInfo;

#define HEADERSIZE ROUND(sizeof(Cell), ALIGNMENT)
#define MINOBJSIZE (HEADERSIZE + ALIGNMENT)

extern "C" {

/* One heap descriptor table per arena (OSArenaId). */
extern OSHeapInfo *OSiHeapInfo[];

/* Push cell onto the front of list. */
Cell *DLAddFront(Cell *list, Cell *cell) {
    cell->next = list;
    cell->prev = 0;
    if (list != 0)
        list->prev = cell;
    return cell;
}

/* Unlink cell from list; return the new head. */
Cell *DLExtract(Cell *list, Cell *cell) {
    if (cell->next != 0)
        cell->next->prev = cell->prev;
    if (cell->prev == 0)
        return cell->next;
    cell->prev->next = cell->next;
    return list;
}

/* Insert inserted into the address-ordered list original, merging it with
 * the neighbours it touches; return the new head. */
Cell *DLInsert(Cell *original, Cell *inserted) {
    Cell *prevCell = 0;
    Cell *nextCell = original;

    for (; nextCell != 0; prevCell = nextCell, nextCell = nextCell->next) {
        if (inserted <= nextCell)
            break;
    }

    inserted->next = nextCell;
    inserted->prev = prevCell;
    if (nextCell != 0) {
        nextCell->prev = inserted;
        if ((char *)inserted + inserted->size == (char *)nextCell) {
            inserted->size += nextCell->size;
            nextCell = nextCell->next;
            inserted->next = nextCell;
            if (nextCell != 0)
                nextCell->prev = inserted;
        }
    }
    if (prevCell != 0) {
        prevCell->next = inserted;
        if ((char *)prevCell + prevCell->size == (char *)inserted) {
            prevCell->size += inserted->size;
            prevCell->next = nextCell;
            if (nextCell != 0)
                nextCell->prev = prevCell;
        }
        return original;
    }
    return inserted;
}

/* First fit on the heap's free list (heap < 0: the current heap). A
 * remainder under MINOBJSIZE goes with the block; a larger one is split off
 * and stays free. */
void *OS_AllocFromHeap(int id, OSHeapHandle heap, u32 size) {
    OSHeapInfo *heapInfo;
    HeapDesc *hd;
    Cell *cell;
    Cell *newCell;
    long leftoverSize;
    OSIntrMode enabled = OS_DisableInterrupts();

    heapInfo = OSiHeapInfo[id];
    if (heapInfo == 0) {
        OS_RestoreInterrupts(enabled);
        return 0;
    }
    if (heap < 0)
        heap = heapInfo->currentHeap;

    hd = &heapInfo->heapArray[heap];

    size += HEADERSIZE;
    size = ROUND(size, ALIGNMENT);

    for (cell = hd->free; cell != 0; cell = cell->next) {
        if ((long)size <= cell->size)
            break;
    }

    if (cell == 0) {
        OS_RestoreInterrupts(enabled);
        return 0;
    }

    leftoverSize = cell->size - (long)size;
    if (leftoverSize < MINOBJSIZE) {
        hd->free = DLExtract(hd->free, cell);
    } else {
        cell->size = (long)size;

        newCell = (Cell *)((char *)cell + size);
        newCell->size = leftoverSize;

        newCell->prev = cell->prev;
        newCell->next = cell->next;
        if (newCell->next != 0)
            newCell->next->prev = newCell;
        if (newCell->prev != 0)
            newCell->prev->next = newCell;
        else
            hd->free = newCell;
    }

    hd->allocated = DLAddFront(hd->allocated, cell);

    OS_RestoreInterrupts(enabled);
    return (char *)cell + HEADERSIZE;
}

void OS_FreeToHeap(int id, OSHeapHandle heap, void *ptr) {
    OSHeapInfo *heapInfo;
    HeapDesc *hd;
    Cell *cell;
    OSIntrMode enabled = OS_DisableInterrupts();

    heapInfo = OSiHeapInfo[id];
    if (heap < 0)
        heap = heapInfo->currentHeap;

    cell = (Cell *)((char *)ptr - HEADERSIZE);
    hd = &heapInfo->heapArray[heap];

    hd->allocated = DLExtract(hd->allocated, cell);
    hd->free = DLInsert(hd->free, cell);

    OS_RestoreInterrupts(enabled);
}

/* Lay out arena id's OSHeapInfo and maxHeaps empty heap descriptors at
 * arenaStart; return the 32-aligned start of the memory after them. */
void *OS_InitAlloc(int id, void *arenaStart, void *arenaEnd, int maxHeaps) {
    OSHeapInfo *heapInfo;
    u32 arraySize;
    int i;
    OSIntrMode enabled = OS_DisableInterrupts();

    heapInfo = (OSHeapInfo *)arenaStart;
    OSiHeapInfo[id] = heapInfo;

    arraySize = sizeof(HeapDesc) * maxHeaps;
    heapInfo->heapArray = (HeapDesc *)((u32)arenaStart + sizeof(OSHeapInfo));
    heapInfo->numHeaps = maxHeaps;

    for (i = 0; i < heapInfo->numHeaps; i++) {
        HeapDesc *hd = &heapInfo->heapArray[i];
        hd->size = -1;
        hd->free = hd->allocated = 0;
    }

    heapInfo->currentHeap = -1;

    arenaStart = (void *)((char *)heapInfo->heapArray + arraySize);
    arenaStart = (void *)ROUND(arenaStart, ALIGNMENT);
    heapInfo->arenaStart = arenaStart;
    heapInfo->arenaEnd = (void *)TRUNC(arenaEnd, ALIGNMENT);

    OS_RestoreInterrupts(enabled);
    return heapInfo->arenaStart;
}
}
