//cpp
#include "IRQ.h"

extern "C" {

extern void* func_02059364(void* a, void* n);
extern void* func_0205938c(void* next, void* node);
extern int* data_020a637c[];

/* NitroSDK's heap bookkeeping, SDK names. data_020a637c is OSiHeapInfo, one
 * OSHeapInfo per arena; func_02059040 (OS_InitAlloc) lays it out: the current
 * heap at 0x00 and the HeapDesc array at 0x10, each desc's size -1 and both
 * lists empty. A Cell heads every block, free or allocated, and the caller's
 * memory starts HEADERSIZE (0x20) past it. */
typedef struct Cell {
    struct Cell *prev; /* 0x0 */
    struct Cell *next; /* 0x4 */
    int size;          /* 0x8 -- header included */
} Cell;

typedef struct HeapDesc {
    int size;        /* 0x0 */
    Cell *free;      /* 0x4 */
    Cell *allocated; /* 0x8 */
} HeapDesc;

typedef struct OSHeapInfo {
    int currentHeap;     /* 0x00 */
    int numHeaps;        /* 0x04 */
    void *arenaStart;    /* 0x08 */
    void *arenaEnd;      /* 0x0c */
    HeapDesc *heapArray; /* 0x10 */
} OSHeapInfo;

#define HEADERSIZE 0x20 /* sizeof(Cell) rounded up to 32 */

/* NitroSDK OS_AllocFromHeap(id, heap, size): first fit on the heap's free
 * list (heap < 0 means the current heap) for size plus the header, rounded to
 * 32. A remainder under 0x40 (MINOBJSIZE) goes with the block
 * (func_02059364 = DLExtract); a larger one is split off and stays free. The
 * block joins the allocated list (func_0205938c = DLAddFront). */
void *func_02059170(int id, int heap, int size) {
    OSHeapInfo *heapInfo;
    HeapDesc *hd;
    Cell *cell;
    unsigned int state;
    int leftoverSize;

    state = IRQ::Disable();
    heapInfo = (OSHeapInfo *)data_020a637c[id];
    if (heapInfo == 0) {
        IRQ::Restore(state);
        return 0;
    }
    if (heap < 0)
        heap = heapInfo->currentHeap;
    hd = &heapInfo->heapArray[heap];
    cell = hd->free;
    size = ((int)((char *)size + HEADERSIZE) + 0x1f) & ~0x1f;
    if (cell != 0) {
        do {
            if (size <= cell->size)
                break;
            cell = cell->next;
        } while (cell != 0);
    }
    if (cell == 0) {
        IRQ::Restore(state);
        return 0;
    }
    leftoverSize = cell->size - size;
    if ((unsigned int)leftoverSize < 0x40) {
        hd->free = (Cell *)func_02059364(hd->free, cell);
    } else {
        Cell *newCell;
        cell->size = size;
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
    hd->allocated = (Cell *)func_0205938c(hd->allocated, cell);
    IRQ::Restore(state);
    return (char *)cell + HEADERSIZE;
}
}
