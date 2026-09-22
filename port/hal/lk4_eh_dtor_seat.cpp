// Seat ExpandingHeap's dtor chain and its two remaining allocator forwarders,
// closing the class: slots 0/1/2/5/12 of _ZTV13ExpandingHeap were the last
// traps while src/ carried the matched bodies. The audited closure
// (Heap::D2, HeapAllocator::Destroy/Remove, ExpandingHeapAllocator::
// DeallocateAll/InvokeDeallocate) is enrolled alongside; every deeper callee
// (Heap::Deallocate, the NestedHeapIterator pair, Memory::operator_delete2,
// data_02099d90) was already linked.
//
//   [ 0] ~ExpandingHeap D1   0x0203c9f0  flat .c, ecx->arg adapter
//   [ 1] ~ExpandingHeap D0   0x0203c9c0  flat .c, ecx->arg adapter
//   [ 2] VDestroy            0x0203c72c  flat .c, ecx->arg adapter
//   [ 5] VDeallocateAll      0x0203c4b0  MSVC method, qualified dispatch
//   [12] VMemoryLeft         0x0203c5ac  MSVC method, qualified dispatch
//
// TWO NAME SPACES, the batch-2 mechanism: VDeallocateAll and VMemoryLeft are
// real MSVC methods whose callee references are MSVC-mangled, while the
// allocator bodies they reach are flat-C definitions
// (_ZN22ExpandingHeapAllocator13DeallocateAllEPFvPvPS_jEj,
// _ZN22ExpandingHeapAllocator16InvokeDeallocateEPvPS_j, and the
// already-linked _ZN22ExpandingHeapAllocator10MemoryLeftEv). The
// /alternatename bridges below carry the decorated spellings verbatim from
// the link errors, the heap_globals law (the linker is the authority on
// decoration).
//
// The slots are written at boot (level_boot's port_stage_a2_seat) for the
// walk_window family only, the lk1_eh_vmax_seat treatment: the D0/D1 closure
// needs Memory::operator_delete2, which rides slice_gate16, and the minimal
// root-heap targets do not link it. Those targets keep the traps, which they
// never dispatch (nothing there destroys the root heap).
//
// LATER (run linkw, lane l2): the same file now also closes the BASE class,
// which is the other half of the same object. _ZTV4Heap (data_02099d90) gets
// its two matched destructors and the pure-virtual trap for the fourteen
// slots the ROM leaves null, and the flat face gate 4b calls for
// Heap::Allocate(unsigned) now forwards into that method's matched body
// instead of a host copy. Both blocks carry their own evidence below.

#include <stdio.h>
#include <stdlib.h>
#include "types.h"

// Flat C name -> the MSVC-mangled reference the matched method TUs emit.
//
// DeallocateAll USED TO BE THE FIRST LINE OF THIS BLOCK AND IT WAS WRONG, and
// Remove used to be the last one and was wrong the other way round. Both were
// receiver crossings -- an /alternatename is a NAME bridge and never an ABI
// bridge -- and both are replaced by receiver-bridging faces further down this
// file, each with its measured evidence in the header there. The two survivors
// below are NOT crossings and are left exactly as they were:
//   InvokeDeallocate  SA is a static member. No receiver on either side.
//   MemoryLeft        a crossing on paper, adjudicated INERT in
//                     port/tools/aliascheck_baseline.txt: the LHS is DEFINED
//                     by the face at hal/heap_vtable.cpp:96, so the directive
//                     never fires (it is row 68 of alternatename_baseline.txt
//                     for that reason).
/* RETIRED at ALIAS2 (wave 8, the main -> port sync). DEAD RHS and an UNREFERENCED left hand side: nothing in the build defines __ZN22ExpandingHeapAllocator16InvokeDeallocateEPvPS_j, and nothing references ?InvokeDeallocate@ExpandingHeapAllocator@@SAXPAXPAV1@I@Z, so the row can never fire and nothing wants it to. */
// #pragma comment(linker, "/alternatename:?InvokeDeallocate@ExpandingHeapAllocator@@SAXPAXPAV1@I@Z=__ZN22ExpandingHeapAllocator16InvokeDeallocateEPvPS_j")
#pragma comment(linker, "/alternatename:?MemoryLeft@ExpandingHeapAllocator@@QAEIXZ=__ZN22ExpandingHeapAllocator10MemoryLeftEv")

/* the matched methods, dispatched qualified through a local shadow (the
   heap_vtable slot_intact mechanism; both sides are MSVC C++ on the same
   class name, so the references resolve with no alias machinery) */
class ExpandingHeap
{
public:
    void VDeallocateAll();
    u32 VMemoryLeft();
};

/* ---- the BASE class, closed the same way --------------------------------
 *
 * _ZTV4Heap is data_02099d90; hal/heap_vtable.cpp hosts it and its comment
 * there carries the reloc evidence for the shape. Two slots, both matched:
 *
 *   [ 0] ~Heap D1  0x0203ca44  src/_ZN4HeapD1Ev.cpp  flat .c, ecx->arg adapter
 *   [ 1] ~Heap D0  0x0203ca20  src/_ZN4HeapD0Ev.cpp  flat .c, ecx->arg adapter
 *
 * and fourteen the ROM leaves null, because Heap declares the rest of the
 * list pure virtual. A null slot is nothing the host can usefully reproduce,
 * so those get one shared trap -- which is exactly what __cxa_pure_virtual
 * is, and it keeps the heap_vtable rule that a slot with no body says so
 * loudly instead of jumping to zero.
 *
 * Nothing dispatches through this table today. Heap is abstract, and
 * Heap::C1 (src/_ZN4HeapC2EPvjPS_.cpp) installs the vptr only for the window
 * between the base constructor and the derived one overwriting it -- the same
 * reading hal/lk2_platform_dtor_seat.cpp records for the Platform base table,
 * and it is why the pair could sit unlinked this long. Seating it is the
 * reference edge that pulls the two matched bodies in, and it makes the
 * hosted table say what the ROM's table says. D1's closure is data_02099d90
 * itself; D0's adds Memory::operator_delete2, already linked.
 */

/* Heap::Allocate(unsigned) at 0x0203c28c, src/_ZN4Heap8AllocateEj.cpp, now the
 * body behind the flat name func_0203cc0c (gate 4b) calls. The matched TU is
 * an MSVC method while the caller spells a __cdecl free function with `this`
 * on the stack, so the face below is a converting forwarder, not an alias --
 * the heap_globals law about which edges may be /alternatename'd and which
 * may not. hal/heap_vtable.cpp keeps the old host body as an alternatename
 * fallback for the ten targets that carry gate 4b without this slice.
 *
 * The one alias that IS safe here: the matched body calls Allocate(size, 4),
 * which it declares returning void*, while the matched two-argument TU
 * (src/_ZN4Heap8AllocateEji.cpp, written against include/Heap.h) defines it
 * returning int. Same __thiscall, same arguments, EAX either way -- the same
 * void/int return bridge hal/lk4_solidheap_seat.cpp takes for Heap::Rescue. */
/* RETIRED at ALIAS2 (wave 8, the main -> port sync). DEFEATED: the left hand side is a real definition in this link now (_ZN4Heap8AllocateEji.cpp.obj), so the directive is inert and alternatename_guard fails on it. */
// #pragma comment(linker, "/alternatename:?Allocate@Heap@@QAEPAXIH@Z=?Allocate@Heap@@QAEHIH@Z")

class Heap
{
public:
    void *Allocate(u32 size);
};

extern "C" {

extern void *_ZTV13ExpandingHeap[16];   /* storage in hal/heap_vtable.cpp */
extern void *data_02099d90[16];         /* _ZTV4Heap, same storage file */

void *_ZN13ExpandingHeapD1Ev(void *self);   /* slot 0 */
void *_ZN13ExpandingHeapD0Ev(void *self);   /* slot 1 */
void _ZN13ExpandingHeap8VDestroyEv(void *self);   /* slot 2 */

void _ZN4HeapD1Ev(void *self);              /* _ZTV4Heap slot 0 */
void *_ZN4HeapD0Ev(void *self);             /* _ZTV4Heap slot 1 */

/* the flat __cdecl face gate 4b's func_0203cc0c calls */
void *_ZN4Heap8AllocateEj(void *self, u32 size)
{ return ((Heap *)self)->Allocate(size); }

}

static void __fastcall eh_d1(void *s, void *)      { _ZN13ExpandingHeapD1Ev(s); }
static void __fastcall eh_d0(void *s, void *)      { _ZN13ExpandingHeapD0Ev(s); }
static void __fastcall eh_vdestroy(void *s, void *){ _ZN13ExpandingHeap8VDestroyEv(s); }
static void __fastcall eh_vdeallall(void *s, void *)
{ ((ExpandingHeap *)s)->VDeallocateAll(); }
static u32 __fastcall eh_vmemleft(void *s, void *)
{ return ((ExpandingHeap *)s)->VMemoryLeft(); }

static void __fastcall heap_d1(void *s, void *) { _ZN4HeapD1Ev(s); }
static void __fastcall heap_d0(void *s, void *) { _ZN4HeapD0Ev(s); }

/* the fourteen slots the ROM leaves null: Heap's pure virtuals */
static void __fastcall heap_pure_virtual(void *, void *)
{
    fprintf(stderr, "FATAL: _ZTV4Heap pure-virtual slot dispatched -- the base "
                    "table is live and it should not be (see "
                    "hal/lk4_eh_dtor_seat.cpp)\n");
    abort();
}

/* ==== THE TWO RECEIVER-BRIDGING FACES, replacing the two /alternatename
   directives this file used to carry at the top ==========================

   Both were found by port/tools/aliascheck.py's receiver rule on its first
   run (port/abi_checks.txt section 6, defects 1 and 2) and both were then
   confirmed from disassembly by a second reviewer. They run in OPPOSITE
   directions, which is the whole reason the rule is about WHERE THE RECEIVER
   LIVES rather than about pop size:

     DeallocateAll  thiscall LHS, flat cdecl RHS. The caller sets ECX and
                    pushes two arguments; the body reads its FIRST STACK SLOT
                    as `thiz`, so every declared argument shifts by one.
     Remove         flat LHS, thiscall RHS. The callers pass the object as an
                    ordinary argument the ARM way; the body reads ECX.

   ---- 1. ExpandingHeapAllocator::DeallocateAll --------------------------

   The deleted directive was

     ?DeallocateAll@ExpandingHeapAllocator@@QAEXPAP6AXPAXPAV1@I@ZI@Z
       = __ZN22ExpandingHeapAllocator13DeallocateAllEPFvPvPS_jEj

   src/_ZN13ExpandingHeap14VDeallocateAllEv.cpp:30 is

     allocator->DeallocateAll((Visitor *)&InvokeDeallocate, 0);

   a qualified __thiscall call: the ALLOCATOR goes into ECX, and 0 and the
   visitor are pushed. The body,
   src/_ZN22ExpandingHeapAllocator13DeallocateAllEPFvPvPS_jEj.cpp:3, is flat
   cdecl `(void *thiz, Fn fn, void *ctx)` and reads thiz from the first stack
   slot -- which holds the VISITOR, a code pointer. Its very first statement
   dereferences it:

     void *node = *(void **)((char *)thiz + 0x2c);

   so the wrong-object read is UNCONDITIONAL. Whether control then transfers
   to the pushed 0 through `fn` depends only on what happens to sit at
   visitor+0x2c.

   AND IT UNBALANCES THE STACK. The call site is compiled for a callee that
   rets 8; the flat body is __cdecl and pops nothing. Every dispatch leaked
   8 bytes of the caller's ESP. The face below is a real __thiscall method, so
   MSVC emits `ret 8` for it and that leak closes with the receiver.

   REACHABLE through _ZTV13ExpandingHeap slot 5 (eh_vdeallall, seated below)
   and NOT REACHED BY ANYTHING THE PORT CAN DRIVE TODAY, which is measured and
   not assumed. A throwaway fprintf in all four entry points of this file --
   the slot 2 thunk, the slot 5 thunk, and both faces below -- produced ZERO
   lines across level entry -> menu ExitLevel -> destination on levels 4 and 6,
   300-frame selftests on levels 1 and 6, and 120-frame selftests on 29
   mounted levels, from a probe build verified to carry the marker. So the
   heap triple the exit path prints is byte-identical before and after this
   change, and that identity is the expected result rather than a sign the fix
   did nothing: the code was wrong, it is right now, and nothing has reached
   it yet. Do not read the clean drive as coverage.

   ---- 2. HeapAllocator::Remove ------------------------------------------

   The deleted directive was

     __ZN13HeapAllocator6RemoveEv = ?Remove@HeapAllocator@@QAEXXZ

   and this is the ARM veneer ride, which is why the face takes the receiver
   as a STACK ARGUMENT and not in ECX. The ROM path is three frames and two of
   them are pure tail jumps:

     ExpandingHeap::VDestroy 0x0203c72c  r0 = self->allocator; bl 0x0204e3b4
     HeapAllocator::Destroy  0x0204e3b4  ldr ip,[pc]; bx ip; .word 0x0204df38
     func_0204ebb8           0x0204ebb8  ldr ip,[pc]; bx ip; .word 0x0204df38
     HeapAllocator::Remove   0x0204df38  push {r4,lr}; mov r4,r0
                                         bl FindNested; mov r1,r4; bl Remove

   r0 is the allocator at the top and is still the allocator at the bottom,
   because `bx ip` does not touch it. The host reproduces that exactly: the
   two veneers are matched TUs that declare the callee `(void)` and call it
   with nothing, MSVC compiles a one-call void forwarder as a JMP, and a jmp
   reuses the caller's own cdecl argument frame -- so the allocator that
   src/_ZN13ExpandingHeap8VDestroyEv.cpp:23 and src/_ZN9SolidHeap8VDestroyEv.cpp:23
   PUSH is still sitting where this face reads it. That is the same
   ride-through the ov007 slice rests on in about fifty places, and
   port/tools/tailjump_guard.py asserts it; these two frames are added to that
   guard's Class C list in this change, so the ride is checked at link time
   rather than assumed.

   WHAT IT DID BEFORE, and it is worse than "reads garbage". The thunk this
   file seats into slot 2 compiles to

     ?eh_vdestroy@@YIXPAX0@Z:
       push ecx / call __ZN13ExpandingHeap8VDestroyEv / pop ecx / ret

   ECX arrives holding the HEAP, neither VDestroy body writes it, and nothing
   between there and Remove reloads it. So Remove ran with `this` = the Heap,
   deterministically, on every heap teardown. A stable wrong object is the
   worse outcome of the two available: it is a valid pointer, so it never
   faults, and it corrupts identically every time -- FindNested(heap) and then
   NestedHeapIterator::Remove(iter, heap) against a nested-allocator list that
   never contained the heap.

   WHY A FACE AND NOT A HOST COPY OF THE TWO VENEERS. Displacing them into
   port/unmatched would work and would not rest on the tail jump, but it costs
   two matched TUs out of the binary (-2 on linkage.py's headline) and puts two
   more SHADOWS on the replacement queue, to reproduce a ROM shape that IS a
   tail jump. The port's north star is that it BE the decomp, so the faithful
   version wins and the assumption gets a guard instead of a workaround.

   Both directives are DELETED and neither is shadowed, for the reason
   hal/lk4_solidheap_seat.cpp's SetDefault block states: a strongly defined LHS
   beside a live alias is the arrival shape alternatename_guard.py fails the
   build on. */
class ExpandingHeapAllocator
{
public:
    /* copied verbatim from the calling TU's own declaration
       (src/_ZN13ExpandingHeap14VDeallocateAllEv.cpp), because THAT is what
       decorates to the name the link needs. `class` and not `struct`: the
       parameter encodes as PAV1@, and a struct would mangle PAU1@ and miss. */
    typedef void (*Visitor)(void *, ExpandingHeapAllocator *, u32);
    void DeallocateAll(Visitor *visitor, u32 param);
};

struct HeapAllocator
{
    /* include/HeapAllocator.h declares `struct HeapAllocator` with a public
       `void Remove();`, which decorates ?Remove@HeapAllocator@@QAEXXZ. A wrong
       signature here is an LNK2019 on a name that does not exist, never a
       quiet call to a sibling. */
    void Remove();
};

extern "C" {

/* the flat cdecl bodies the two faces forward into */
void *_ZN22ExpandingHeapAllocator13DeallocateAllEPFvPvPS_jEj(
    void *thiz, void (*fn)(void *, void *, void *), void *ctx);

/* the ARM veneers' stack argument IS the receiver; see the header above */
void _ZN13HeapAllocator6RemoveEv(void *thiz)
{ ((HeapAllocator *)thiz)->HeapAllocator::Remove(); }

}

/* the flat body returns the list node it stopped on and the ROM's method face
   is void, so the value drops here -- the same widening the Rescue and
   SetNodeID bridges take elsewhere in this family. */
void ExpandingHeapAllocator::DeallocateAll(Visitor *visitor, u32 param)
{
    _ZN22ExpandingHeapAllocator13DeallocateAllEPFvPvPS_jEj(
        this, (void (*)(void *, void *, void *))visitor,
        (void *)(size_t)param);
}

extern "C" void hal_seat_expandingheap_dtors(void)
{
    _ZTV13ExpandingHeap[0]  = (void *)eh_d1;
    _ZTV13ExpandingHeap[1]  = (void *)eh_d0;
    _ZTV13ExpandingHeap[2]  = (void *)eh_vdestroy;
    _ZTV13ExpandingHeap[5]  = (void *)eh_vdeallall;
    _ZTV13ExpandingHeap[12] = (void *)eh_vmemleft;

    data_02099d90[0] = (void *)heap_d1;
    data_02099d90[1] = (void *)heap_d0;
    for (int i = 2; i < 16; ++i)
        data_02099d90[i] = (void *)heap_pure_virtual;
}
