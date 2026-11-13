//cpp
/* arm9/NestedHeapIterator -- NitroSystem's foundation list and heap-common
 * code (NNS_FND_list.c, NNS_FND_heapcommon.c), .text 0x0204dcfc..0x0204e084.
 *
 * The bodies are pret/pokediamond's arm9/lib/libnns sources as written,
 * and every one reproduces the ROM. mwccarm emits a file's functions in
 * reverse order, and the ROM runs GetPrev..InitList, then FinalizeHeap..
 * FindContainHeap: list.c's order reversed, followed by heapcommon.c's.
 *
 *   NNS_FndInitList and the Append/Prepend/Insert/Remove/GetNext/GetPrev
 *   ListObject calls -- an intrusive doubly-linked list whose objects carry
 *   their NNSFndLink `offset' bytes in.
 *   NNSi_FndInitHeapHead -- stamps a heap header (signature, range, option
 *   bits), sets up its child list, sets up the root list once, and links
 *   the heap into whichever list contains it. The expanding and frame heaps
 *   both call it.
 *   NNSi_FndFinalizeHeap -- unlinks a heap from that list.
 *
 * The two files are folded into one translation unit here. heapcommon.c's
 * .bss (sRootListInitialized, sRootList at 0x020a4d34) is declared rather
 * than defined: no arm9 .bss range is enrolled to a TU yet, and defining
 * them here would add storage the build has nowhere to place. pret's
 * FindContainHeap, FindListContainHeap and SetFirstObject are static; here
 * they have external linkage, as the FS_Archive TU's static helpers do,
 * because dsd cannot record a local symbol in symbols.txt.
 */
#include "nnsys/fnd.h"

/* ---- NNS_FND_heapcommon.c ---- */

/* Data. mwcc does not mangle it. MSVC does, and the host defines the C name. */
#if defined(_MSC_VER)
extern "C"
#endif
    extern int sRootListInitialized;
#if defined(_MSC_VER)
extern "C"
#endif
    extern NNSFndList sRootList;

#define OBJ_TO_LINK(list, obj) ((NNSFndLink *)((char *)(obj) + (list)->offset))

/* The innermost heap in `list' (searched depth-first through each heap's
 * childList) whose [heapStart, heapEnd) contains memBlock. */
NNSiFndHeapHead *FindContainHeap(NNSFndList *list, const void *memBlock) {
    NNSiFndHeapHead *heap = 0;

    while ((heap = (NNSiFndHeapHead *)NNS_FndGetNextListObject(list, heap)) != 0) {
        if (heap->heapStart <= memBlock && memBlock < heap->heapEnd) {
            NNSiFndHeapHead *ret = FindContainHeap(&heap->childList, memBlock);
            if (ret == 0)
                ret = heap;
            return ret;
        }
    }
    return 0;
}

/* The list a heap at memBlock belongs in: the child list of the heap that
 * contains it, or the root list. */
NNSFndList *FindListContainHeap(const void *memBlock) {
    NNSFndList *ret = &sRootList;
    NNSiFndHeapHead *heap = FindContainHeap(&sRootList, memBlock);
    if (heap != 0)
        ret = &heap->childList;
    return ret;
}

void NNSi_FndInitHeapHead(NNSiFndHeapHead *heap, u32 signature, void *heapStart, void *heapEnd,
                          u16 optFlag) {
    heap->signature = signature;
    heap->heapStart = heapStart;
    heap->heapEnd = heapEnd;
    heap->attribute = 0;
    heap->attribute &= ~0xffu;
    heap->attribute |= optFlag & 0xffu;
    NNS_FndInitList(&heap->childList, 4);
    if (!sRootListInitialized) {
        NNS_FndInitList(&sRootList, 4);
        sRootListInitialized = 1;
    }
    NNS_FndAppendListObject(FindListContainHeap(heap), heap);
}

void NNSi_FndFinalizeHeap(NNSiFndHeapHead *heap) {
    NNS_FndRemoveListObject(FindListContainHeap(heap), heap);
}

/* ---- NNS_FND_list.c ---- */

void NNS_FndInitList(NNSFndList *list, u16 offset) {
    list->headObject = 0;
    list->tailObject = 0;
    list->numObjects = 0;
    list->offset = offset;
}

void SetFirstObject(NNSFndList *list, void *object) {
    NNSFndLink *link = OBJ_TO_LINK(list, object);
    link->nextObject = 0;
    link->prevObject = 0;
    list->headObject = object;
    list->tailObject = object;
    list->numObjects++;
}

void NNS_FndAppendListObject(NNSFndList *list, void *object) {
    if (list->headObject == 0) {
        SetFirstObject(list, object);
    } else {
        NNSFndLink *link = OBJ_TO_LINK(list, object);
        link->prevObject = list->tailObject;
        link->nextObject = 0;
        OBJ_TO_LINK(list, list->tailObject)->nextObject = object;
        list->tailObject = object;
        list->numObjects++;
    }
}

void NNS_FndPrependListObject(NNSFndList *list, void *object) {
    if (list->headObject == 0) {
        SetFirstObject(list, object);
    } else {
        NNSFndLink *link = OBJ_TO_LINK(list, object);
        link->prevObject = 0;
        link->nextObject = list->headObject;
        OBJ_TO_LINK(list, list->headObject)->prevObject = object;
        list->headObject = object;
        list->numObjects++;
    }
}

void NNS_FndInsertListObject(NNSFndList *list, void *target, void *object) {
    if (target == 0) {
        NNS_FndAppendListObject(list, object);
    } else if (target == list->headObject) {
        NNS_FndPrependListObject(list, object);
    } else {
        NNSFndLink *link = OBJ_TO_LINK(list, object);
        void *prevObject = OBJ_TO_LINK(list, target)->prevObject;
        NNSFndLink *prevLink = OBJ_TO_LINK(list, prevObject);
        link->prevObject = prevObject;
        link->nextObject = target;
        prevLink->nextObject = object;
        OBJ_TO_LINK(list, target)->prevObject = object;
        list->numObjects++;
    }
}

void NNS_FndRemoveListObject(NNSFndList *list, void *object) {
    NNSFndLink *link = OBJ_TO_LINK(list, object);
    if (link->prevObject == 0)
        list->headObject = link->nextObject;
    else
        OBJ_TO_LINK(list, link->prevObject)->nextObject = link->nextObject;
    if (link->nextObject == 0)
        list->tailObject = link->prevObject;
    else
        OBJ_TO_LINK(list, link->nextObject)->prevObject = link->prevObject;
    link->prevObject = 0;
    link->nextObject = 0;
    list->numObjects--;
}

void *NNS_FndGetNextListObject(NNSFndList *list, void *object) {
    if (object == 0)
        return list->headObject;
    return OBJ_TO_LINK(list, object)->nextObject;
}

void *NNS_FndGetPrevListObject(NNSFndList *list, void *object) {
    if (object == 0)
        return list->tailObject;
    return OBJ_TO_LINK(list, object)->prevObject;
}
