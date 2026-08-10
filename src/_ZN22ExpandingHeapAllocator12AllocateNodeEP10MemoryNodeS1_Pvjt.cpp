//cpp
// @symbol _ZN22ExpandingHeapAllocator12AllocateNodeEP10MemoryNodeS1_Pvjt
#include "ExpandingHeapAllocator.h"

/* ExpandingHeapAllocator::AllocateNode(MemoryNode*, MemoryNode*, void*, u32, u16) at
 * 0x0204e690 -- STATIC. Five declared parameters, five body arguments, no room for a
 * `this`.
 *
 * Carves one allocation out of a free block. The block is unlinked, split into up to
 * three pieces -- leading gap, the allocation, trailing gap -- and each surviving piece
 * gets a fresh header. A gap smaller than a header (0x10) is not worth a node, so it is
 * collapsed into the allocation instead. 'FR' (0x4652) tags the free remnants, 'UD'
 * (0x5544) the allocated one.
 *
 * THE LAST PARAMETER IS A u16 AND THE ROM SAYS SO. The imported symbol ended `Pvjj`,
 * making both trailing parameters u32; declared that way the member cannot reproduce
 * the function. At +0x10c the ROM reads its stack-passed fifth argument with
 *
 *     ldrh r3, [sp, #0x38]
 *
 * a halfword load, which is the width mwccarm picks from the parameter's DECLARED type
 * -- not from what the body does with the value, and not something a cast at the use
 * site can reach (measured: no effect). #1221 and the MemoryNode slice recorded that
 * divergence and three failed attempts to reproduce it from a u32 declaration: a
 * truncating local (999 words -- it re-homes the argument), `*(u16 *)&param` (5 words --
 * taking the address re-homes it), and use-site casts (0 effect). All three were trying
 * to make a u32 parameter behave like a u16 one. Declared `u16`, the member emits the
 * ROM's own instruction and the whole function matches at 2004/b56.
 *
 * The name was the thing that was wrong. `Pvjj` came verbatim from the imported
 * symbols.x, whose only backing is a hand-written declaration for CALLING this function
 * from mod code -- and a caller cannot detect an over-wide integer parameter, because
 * AAPCS widens it to a word on the way in either way and the callee just ignores the
 * high half. The declaration was never checked against the callee, and the callee is
 * where the width is observable. See the same correction applied to DeallocateAll.
 *
 * `Extent` is the recovered body's own view of MemoryNode::Target -- kept, and cast at
 * the call, for the reason CreateNode states: rewriting a recovered body's local views
 * into the reconstructed type is a body change, and this class has already produced a
 * three-word divergence from exactly that kind of tidying (SetNodeID, #1221). Only the
 * declaration moved.
 *
 * `c` is typed MemoryNode* because the ROM's mangled name says so, but callers pass the
 * allocator's embedded node-list, not a real node -- the same sentinel-node convention
 * LinkNode and UnlinkNode document.
 */
struct Extent { int start; int end; };

extern "C" {
void  _ZN10MemoryNode6TargetC1EP10MemoryNode(struct Extent* thiz, MemoryNode* node);
void* _ZN22ExpandingHeapAllocator10UnlinkNodeEP10MemoryNodeS1_(MemoryNode* c, MemoryNode* node);
void* _ZN22ExpandingHeapAllocator10CreateNodeEPN10MemoryNode6TargetEt(struct Extent* t, unsigned short tt);
void* _ZN22ExpandingHeapAllocator8LinkNodeEP10MemoryNodeS1_S1_(void* c, void* node, void* link);
void  MultiStore_Int(int val, int* dst, int len);
}

void* ExpandingHeapAllocator::AllocateNode(MemoryNode* c, MemoryNode* node, void* target,
                                           u32 size, u16 z)
{
    struct Extent t0;
    struct Extent t1;
    struct Extent t2;
    void* link;
    int header;
    _ZN10MemoryNode6TargetC1EP10MemoryNode(&t0, node);

    int oldLimit = t0.end;
    header = (int)target - 0x10;
    int backStart = (int)size + (int)target;
    t0.end = header;
    t1.end = oldLimit;
    t1.start = backStart;

    link = _ZN22ExpandingHeapAllocator10UnlinkNodeEP10MemoryNodeS1_(c, node);

    unsigned int frontGap = t0.end - t0.start;
    if (frontGap < 0x10) {
        t0.end = t0.start;
    } else {
        void* newFront = _ZN22ExpandingHeapAllocator10CreateNodeEPN10MemoryNode6TargetEt(&t0, 0x4652);
        link = _ZN22ExpandingHeapAllocator8LinkNodeEP10MemoryNodeS1_S1_(c, newFront, link);
    }

    unsigned int backGap = t1.end - t1.start;
    if (backGap < 0x10) {
        t1.start = t1.end;
    } else {
        void* newBack = _ZN22ExpandingHeapAllocator10CreateNodeEPN10MemoryNode6TargetEt(&t1, 0x4652);
        link = _ZN22ExpandingHeapAllocator8LinkNodeEP10MemoryNodeS1_S1_(c, newBack, link);
    }

    int flagsWord = *(int*)((char*)c - 4);
    int* dst = (int*)t0.end;
    int len = t1.start - t0.end;
    if (((unsigned short)(flagsWord & 0xff)) & 1) {
        volatile int zero = 0;
        MultiStore_Int(zero, dst, len);
    }

    t2.start = header;
    t2.end = t1.start;
    void* allocNode = _ZN22ExpandingHeapAllocator10CreateNodeEPN10MemoryNode6TargetEt(&t2, 0x5544);

    unsigned short* flagsPtr = (unsigned short*)((char*)allocNode + 2);
    void* usedList = (char*)c + 8;
    *flagsPtr &= ~0x8000;
    *flagsPtr |= (z & 1) << 15;
    int t0e = t0.end;
    *flagsPtr &= ~0x7f00;
    *flagsPtr |= (((unsigned short)((int)allocNode - t0e)) & 0x7f) << 8;
    unsigned int cRaw = *(unsigned short*)((char*)c + 0x10);
    unsigned int cValue = cRaw & 0xff;
    *flagsPtr &= ~0xff;
    *flagsPtr |= cValue;

    void* usedTail = (void*)(*(int*)((char*)c + 0xc));
    _ZN22ExpandingHeapAllocator8LinkNodeEP10MemoryNodeS1_S1_(usedList, allocNode, usedTail);

    return target;
}
