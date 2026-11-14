//cpp
/* Heap -- abstract base of the ROM's mHeap::Heap_t family (RTTI at
   0x02099ce4 spells the cartridge name; SolidHeap and ExpandingHeap derive
   from it). TU claims .text 0x0203c1b4..0x0203cc28, and .data
   0x02099d28..0x02099e18 -- SolidHeap_t's type-name string and the three
   vtables; the rest of the family's type_info, below it, is emitted here too
   and checked against the ROM's copy. Memory's five wrappers sit at the
   low end; the class's own contiguous run in delinks order: the game-heap
   bootstrap, the four tail-call veneers, the scratch-heap push/pop pair and
   the default-heap setter; then the dispatch run, where each public Heap
   forwarder follows the SolidHeap and ExpandingHeap overrides of the slot it
   calls (ResizeToFit through Destroy); then the three factories,
   CreateSolidHeap, CreateExpandingHeap and CreateRootHeap; then the three
   classes' destructors and constructors; then the root-heap bootstrap,
   InitializeRootHeap and SetupRootHeap; and last the global allocation
   entry points over the default heap -- the global operator new, new[],
   delete and delete[], and the two unnamed bodies they forward to. Written back-to-front: mwccarm emits
   .text in reverse source order under default deferred codegen.
 *
 * leftovers:
 * - The Heap* forms are defined first. Deferred codegen emits definitions in
 *   reverse source order, and the convenience overloads call those forms.
 *   A null heap selects defaultHeapPtr.
 * - Only the structors the cartridge kept have a ROM home. One empty
 *   destructor per class makes mwccarm emit D0, D1 and D2, and one constructor
 *   emits C1 and C2; nothing derives from SolidHeap_t or ExpHeap_t, so their D2
 *   and C2 never reached the cartridge, and Heap_t is only ever constructed as
 *   a base, so its C1 did not either. The manifest deadstrips those five.
 * - ~Heap_t is the family's key function, so this TU emits all three vtables
 *   and their type_info. That is only correct under the cartridge's mHeap
 *   names: the strings are the ROM's own `N5mHeap6Heap_tE' and siblings. */
#include "Heap.h"
#include "decl_common.h"
#include "SolidHeap.h"
#include "ExpandingHeap.h"
#include "nnsys/fnd.h"
#include "Memory.h"

/* Placement new, as <new> declares it. The factories construct each heap
   in a block they have already carved out, and the null test the ROM
   makes before every one of those constructor calls is this operator's:
   a non-throwing allocation function may return null, so the compiler
   checks before it constructs. Spelled out, that test emitted twice. */
inline void* operator new(size_t, void* p) { return p; }

/* 0x020a0eac, already named in config/arm9/symbols.txt. */
extern "C" Heap* GAME_HEAP_PTR;
/* 0x020a0ea8 -- where SetupSolidHeapAsDefault parks the outgoing default heap.
   Unnamed in config/arm9/symbols.txt; left as the address symbol rather than
   minted here, since naming it is a claim of its own. */
extern "C" Heap* data_020a0ea8;

extern "C" {
/* `extern' is load-bearing on both of these, and specifically because they
       sit inside a BRACED linkage block. Written `extern "C" u32 X;' on its own
       line the extern specifier is implied and the line is a declaration.
       Written `extern "C" { u32 X; }' mwccarm emits a DEFINITION, which
       collides with the ROM's own bss:

         mwldarm.exe: Multiply-defined: "data_020a0e9c" in ...SetupRootHeapEv.o
         mwldarm.exe: Previously defined in _dsd_gap@main_40.o

       build_pin.verify compiles a single file and never links, so it returned
       (True, '2004/b56') on the broken spelling. Only rombuild.py caught it. */
extern u32 ROOT_HEAP_ARENA_ID; /* 0x020a0ea4 */
extern Heap *data_020a0e9c;    /* the root heap; unnamed */

unsigned int OS_GetArenaLo(s32 arena);
unsigned int OS_GetArenaHi(s32 arena);
void *OS_InitAlloc(s32 arena, void *lo, void *hi, int maxHeaps);
void OS_SetArenaLo(s32 arena, unsigned int lo);
void *OS_AllocFromArenaLo(s32 arena, unsigned int size, unsigned int align);
}

/* The global allocation entry points, highest addresses first: two bodies
   over the default heap, then the four global operators as three-word
   tail-call veneers to them (`ldr ip,[pc] / bx ip / .word'). The ROM's call
   sites settle which operator is which: every deleting destructor calls
   0x0203cbcc, so that is operator delete; new[] buffers are released through
   0x0203cbc0, which __cxa_vec_delete and __cxa_vec_new's cleanup call as the
   array deallocator; 0x0203cbd8 is what __cxa_vec_new allocates the array
   with. The two bodies are reached only through the veneers (two loads each)
   and keep their address names.

   mwccarm's size_t is unsigned long, so the two allocators mangle to _Znwm
   and _Znam. An explicit call is spelled `::operator new': inside a member
   of an fBase_c-derived class, plain `operator new' finds fBase_c's class
   allocator first. The library declarations of the deallocators carry
   `throw()', so the definitions do as well.

   THE HOST KEEPS ITS OWN GLOBAL OPERATORS. Defining them here would replace
   the CRT's for the whole PC port, so on the host every new and delete,
   array or not, is the CRT's. Nothing here reaches MSVC. */
#ifndef _MSC_VER

/* func_0203cc0c at 0x0203cc0c -- allocate from the default heap with the
 * default alignment. The body of operator new and operator new[]. */
void *func_0203cc0c(u32 size) {
    return Memory::defaultHeapPtr->Allocate(size);
}

/* func_0203cbf0 at 0x0203cbf0 -- hand a block back to the default heap
 * through its _Deallocate veneer. The body of operator delete and
 * operator delete[]. */
void func_0203cbf0(void *ptr) {
    Memory::defaultHeapPtr->_Deallocate(ptr);
}

/* operator new(size_t) at 0x0203cbe4. Every new-expression in src/ resolves
 * here. */
void *operator new(size_t size) {
    return func_0203cc0c(size);
}

/* operator new[](size_t) at 0x0203cbd8. */
void *operator new[](size_t size) {
    return func_0203cc0c(size);
}

/* operator delete(void*) at 0x0203cbcc. */
void operator delete(void *ptr) throw() {
    func_0203cbf0(ptr);
}

/* operator delete[](void*) at 0x0203cbc0. */
void operator delete[](void *ptr) throw() {
    func_0203cbf0(ptr);
}
#endif /* !_MSC_VER */

namespace mHeap {

/* Heap::SetupRootHeap() at 0x0203cb04 -- take the whole of the NitroSDK arena
 * selected by ROOT_HEAP_ARENA_ID and turn it into the root heap.
 *
 * The sequence is the standard OS_*Arena dance: read lo and hi, round lo up to
 * a 32-byte boundary and publish it back so nothing else claims the space, then
 * allocate everything from there to hi. `sz' is deliberately computed from the
 * RE-READ hi rather than from the one captured earlier.
 *
 * ROOT_HEAP_ARENA_ID is an arena id, not a pointer: InitializeRootHeap stores 0
 * into it to select OS_ARENA_MAIN, and it is passed by value as the first
 * argument to each OS entry point.
 *
 * OS_InitAlloc's last argument is a heap count, not an alignment: it reserves
 * descriptors for four OS heaps at lo and returns the 32-aligned memory after
 * them (src/engine/os/OS_Alloc.cpp). */
Heap_t* Heap_t::SetupRootHeap()
{
    int lo = OS_GetArenaLo(ROOT_HEAP_ARENA_ID);
    int hi = OS_GetArenaHi(ROOT_HEAP_ARENA_ID);

    lo = (int)OS_InitAlloc(ROOT_HEAP_ARENA_ID, (void *)lo, (void *)hi, 4);
    OS_SetArenaLo(ROOT_HEAP_ARENA_ID, lo);

    int top = OS_GetArenaHi(ROOT_HEAP_ARENA_ID);
    int sz = top - ((lo + 0x1f) & ~0x1f);

    void *mem = OS_AllocFromArenaLo(ROOT_HEAP_ARENA_ID, sz, 0x20);
    /* The failure return stays last. The ROM branches (`beq') to a tail block
       holding its own `mov r0,#0 / pop / bx'; an early `if (mem == 0) return 0;'
       is predicated in place instead (`moveq/popeq/bxeq'), four bytes short. */
    if (mem != 0) {
        Heap* heap = CreateRootHeap(mem, sz);
        if (heap != 0)
        {
            data_020a0e9c = heap;
            Memory::defaultHeapPtr = heap;
        }
        return heap;
    }
    return 0;
}

/* Heap::InitializeRootHeap() at 0x0203cae8 -- clears ROOT_HEAP_ARENA_ID then
 * tail-calls SetupRootHeap(). The word at 0x020a0ea4 is the NitroSDK arena id
 * the root heap is carved from; zeroing it selects OS_ARENA_MAIN. */
void Heap_t::InitializeRootHeap()
{
    ROOT_HEAP_ARENA_ID = 0;
    SetupRootHeap();
}

/* The three constructors. Each initializer list is the ROM body: Heap_t's sets
   the four fields and then raises the fail-fast bit; the derived two construct
   Heap_t, let CodeWarrior install their own vptr, then store the allocator. */
Heap_t::Heap_t(void* start, u32 size, Heap_t* root)
    : heapStart(start), heapSize(size), parentHeap(root), flags(0)
{
    flags = 0x4000;
}

ExpHeap_t::ExpHeap_t(void *start, u32 size, Heap_t *root, NNSFndHeapHandle allocator_)
    : Heap_t(start, size, root), allocator(allocator_) {}

SolidHeap_t::SolidHeap_t(void *start, u32 size, Heap_t *root, NNSFndHeapHandle allocator_)
    : Heap_t(start, size, root), allocator(allocator_) {}

/* The destructors are empty: CodeWarrior supplies each vptr transition, the
   base teardown and, in D0, the inline operator delete. ~Heap_t is the
   family's key function -- the first virtual not defined inline -- so these
   three definitions are what make this TU emit the vtables and type_info. */
Heap_t::~Heap_t()
{
}

ExpHeap_t::~ExpHeap_t()
{
}

SolidHeap_t::~SolidHeap_t()
{
}

/* Heap::CreateRootHeap(void*, u32) at 0x0203c8e8 -- build the one heap that has
 * no parent, directly on a block the caller already owns.
 *
 * Unlike CreateSolidHeap and CreateExpandingHeap this allocates nothing: it is
 * handed raw memory by Heap::SetupRootHeap, which took it from the OS arena, and
 * lays an ExpandingHeap over the front of it. So the 0x18 appears twice for the
 * same reason as in those two -- it is the size of the derived object -- but
 * here it is subtracted from what the caller gave rather than added to what is
 * requested. `parentHeap' comes out NULL, which is what makes this the root.
 *
 * THIS FUNCTION IS WHY include/Heap.h HAS NO FIELD AT 0x14. It writes one, and
 * it is static, so the evidence pass attributed the write to Heap through a
 * pointer that is really an ExpandingHeap under construction -- a single witness
 * against the two derived constructors, which both write 0x14 only after calling
 * Heap's constructor. That is now expressible: the object is an ExpandingHeap*
 * here, and the field belongs to it.
 *
 * The shadow this replaces typed the second parameter `void* end' and called
 * 0x08 `heapEnd'. The mangled name says `j' -- a u32 -- and both values are
 * sizes. */

Heap_t* Heap_t::CreateRootHeap(void* mem, u32 size)
{
    ExpandingHeap* heap = (ExpandingHeap*)mem;

    heap->heapStart = heap + 1;
    heap->allocator = NNS_FndCreateExpHeapEx(heap->heapStart, size - 0x18, 3);
    if (heap->allocator != 0)
    {
        heap->heapSize = size - 0x18;
        new (heap) ExpHeap_t(heap->heapStart, heap->heapSize, 0, heap->allocator);
        return heap;
    }

    heap->heapSize = 0;
    return 0;
}

/* Heap::CreateExpandingHeap(u32, Heap*, int) at 0x0203c844 -- the expanding twin
 * of Heap::CreateSolidHeap, instruction for instruction bar the allocator it
 * builds and the constructor it calls. See CreateSolidHeap for the 0x18 and for why
 * neither could be migrated until ExpandingHeap became a real type.
 *
 * Same correction: 0x08 is `heapSize' and always held a size, not an end
 * address. */

Heap_t* Heap_t::CreateExpandingHeap(u32 size, Heap_t* root, int align)
{
    Heap* parent = root;
    if (root == 0)
        parent = Memory::defaultHeapPtr;

    ExpandingHeap* heap = (ExpandingHeap*)parent->Allocate(size + 0x18, align);
    if (heap != 0)
    {
        heap->heapStart = heap + 1;
        heap->allocator = NNS_FndCreateExpHeapEx(heap->heapStart, size, 3);
        if (heap->allocator == 0)
        {
            parent->Deallocate(heap);
            heap = 0;
        }
        else
        {
            heap->heapSize = size;
            new (heap) ExpHeap_t(heap->heapStart, heap->heapSize, parent, heap->allocator);
        }
    }
    return heap;
}

/* Heap::CreateSolidHeap(u32, Heap*, int) at 0x0203c7a0 -- allocate a block out
 * of `root' and build a SolidHeap in the first 0x18 bytes of it, with the
 * remaining `size' bytes as the new heap's arena.
 *
 * The 0x18 is the derived object: Heap is 0x14 and SolidHeap adds `allocator' at
 * 0x14, so the header sits in [heap, heap+0x18) and heapStart is the first byte
 * past it. That is why the request is `size + 0x18' but the arena handed to the
 * allocator is `size'.
 *
 * A NULL `root' means the current default heap.
 *
 * This could not be migrated with the rest of Heap's methods: it writes
 * heapStart, heapSize and allocator on a raw block BEFORE the constructor runs,
 * so it needs SolidHeap to be a real type with real members. Until this slice,
 * include/SolidHeap.h modelled the base as `u8 pad_000[0x14]' and this file
 * carried a local shadow instead -- one that called 0x08 `heapEnd', stored
 * `(void*)size' into it, then read it back and cast it to u32 to pass on. The
 * field is `heapSize' and the value was a size the whole way through.
 *
 * The ROM tests `heap' a second time just before the constructor, inside a
 * branch that already established it. That second test is placement new's
 * own; see the operator at the top of this file. */

Heap_t* Heap_t::CreateSolidHeap(u32 size, Heap_t* root, int align)
{
    Heap* parent = root;
    if (root == 0)
        parent = Memory::defaultHeapPtr;

    SolidHeap* heap = (SolidHeap*)parent->Allocate(size + 0x18, align);
    if (heap != 0)
    {
        heap->heapStart = heap + 1;
        heap->allocator = NNS_FndCreateFrmHeapEx(heap->heapStart, size, 3);
        if (heap->allocator == 0)
        {
            parent->Deallocate(heap);
            heap = 0;
        }
        else
        {
            heap->heapSize = size;
            new (heap) SolidHeap_t(heap->heapStart, heap->heapSize, parent, heap->allocator);
        }
    }
    return heap;
}

/* Heap::Destroy() at 0x0203c758 -- tears the heap down: let the concrete heap
 * release its own bookkeeping (slot 2), forget the arena, then hand the block
 * back to the heap this one was carved out of.
 *
 * Was a cast of `this' to a local `struct Base' for the slot-2 call, plus
 * address-cast writes through `&unk_004' / `&unk_008' / `&unk_00c' -- the
 * casts existed because the header declared those fields but nothing would
 * admit what they were. They are named now, so the writes are plain. */

void Heap_t::Destroy()
{
    VDestroy();

    heapStart = 0;
    heapSize = 0;

    /* A root heap has no parent to return the block to. */
    Heap* parent = parentHeap;
    if (parent == 0)
        return;

    parent->Deallocate(this);
    parentHeap = 0;
}

/* Heap::_Destroy() at 0x0203c74c -- a three-word tail-call veneer to
 * Heap::Destroy (0x0203c758): `ldr ip,[pc] / bx ip / .word 0x0203c758'.
 *
 * PROBE: can a veneer be a real method? It used to be spelled as an
 * argument-less extern "C" function calling another argument-less extern "C"
 * function, both by mangled name -- which reproduces the bytes because a tail
 * call never touches the arguments, and which is also why the shape survived
 * unexamined. Written as a real member forwarding real arguments it should emit
 * the same three words. */

void Heap_t::_Destroy()
{
    Destroy();
}

/* ExpandingHeap::VDestroy() at 0x0203c72c -- Heap vtable slot 2. Tear the
 * allocator down and forget it; Heap::Destroy calls this before returning the
 * block to the parent heap.
 *
 * The allocator is a NitroSystem expanded heap; NNS_FndDestroyExpHeap
 * unlinks it from the heap that contains it. */

void ExpHeap_t::VDestroy()
{
    NNS_FndDestroyExpHeap(allocator);
    allocator = 0;
}

/* SolidHeap::VDestroy() at 0x0203c70c -- Heap vtable slot 2. Tear the allocator
 * down and forget it; Heap::Destroy calls this before returning the block to
 * the parent heap.
 *
 * The .c file this replaces was a copy of ExpandingHeap's and never finished
 * being adapted: its comments named ExpandingHeap::VDestroy, quoted
 * ExpandingHeap's address 0x0203c72c instead of this one, and its local struct
 * was called `ExpandingHeap'. Only the symbol on the definition made it a
 * SolidHeap function at all.
 *
 * The allocator is a NitroSystem frame heap; NNS_FndDestroyFrmHeap unlinks
 * it from the heap that contains it. */

void SolidHeap_t::VDestroy()
{
    NNS_FndDestroyFrmHeap(allocator);
    allocator = 0;
}

/* Heap::Allocate(u32, int) at 0x0203c6cc -- the allocation entry point: call
 * slot 3, and if it comes back empty on a fail-fast heap, do not return the
 * NULL to the caller, OS_Terminate() instead.
 *
 * RETURN TYPE: void*, from the definition. Its old shard declared `int
 * Heap::Allocate(...)' while the sibling overload in this file, which
 * forwards to it, declared the same function `void*'. Two sources
 * disagreeing about one signature is exactly the debt the shadow structs
 * create. ExpandingHeap::VAllocate and SolidHeap::VAllocate both return void*,
 * so void* it is, and both definitions now agree because they share a header.
 *
 * Was a cast of `this' to a local `struct Base' with three anonymous virtuals
 * padding `m' out to index 3, plus an address-cast read of `&unk_010'. */

void* Heap_t::Allocate(u32 size, int align)
{
    void* block = VAllocate(size, align);
    if (block == 0 && (flags & 0x4000))
        OS_Terminate();
    return block;
}

/* ExpandingHeap::VAllocate(u32, int) at 0x0203c6bc -- Heap vtable slot 3. Four
 * instructions: load the allocator from this+0x14 and tail-call it, passing
 * r1/r2 through untouched.
 *
 * This override always had the right spelling -- `Eji', (u32, int). SolidHeap's
 * claimed `Ejj' for the same slot, and the ROM agreed with this one; see
 * include/SolidHeap.h for the signed `blt' that settles it. */

void* ExpHeap_t::VAllocate(u32 size, int align)
{
    return NNS_FndAllocFromExpHeapEx(allocator, size, align);
}

/* SolidHeap::VAllocate(u32, int) at 0x0203c6ac -- Heap vtable slot 3. Four
 * instructions: load the allocator from this+0x14 and tail-call it, passing
 * r1 and r2 through untouched.
 *
 * THE SYMBOL WAS SolidHeap::VAllocate(u32, u32) AND THAT WAS WRONG. `Ejj' claims
 * (u32, u32); ExpandingHeap's override of the SAME SLOT is
 * mHeap::ExpHeap_t::VAllocate(u32, int), which claims (u32, int). Two overrides of
 * one slot cannot have different parameter types, so one import was wrong, and
 * nothing in the tree could tell which -- a caller cannot observe a parameter's
 * signedness through a virtual call, and until SolidHeap actually derived from
 * Heap nothing compiled the two declarations against each other.
 *
 * The ROM decides it. This function forwards r2 straight into
 * NNS_FndAllocFromFrmHeapEx (0x0204eb70), which does:
 *
 *     cmp r2, #0
 *     blt 0x0204eba4        <- SIGNED branch
 *     ...
 *     0x0204eba4: rsb r2, r2, #0    <- negate, then allocate backwards
 *
 * `blt' is signed. Declared u32 the compiler emits `bcc'/`blo' and the
 * negate-and-allocate-backwards path becomes unreachable, so the alignment
 * argument is an `int' whose sign selects the direction. Every sibling in the
 * family already spelled it that way: Heap::Allocate,
 * NNS_FndAllocFromFrmHeapEx and NNS_FndAllocFromExpHeapEx all take an int
 * alignment. The symbol is renamed in config/arm9/symbols.txt to match, and this
 * file with it.
 *
 * It also carried a local `class SolidHeap { u32 unk00; ... }' rather than
 * including the header. */

void* SolidHeap_t::VAllocate(u32 size, int align)
{
    return NNS_FndAllocFromFrmHeapEx(allocator, size, align);
}

/* Heap::Intact() at 0x0203c664. Asks the concrete heap whether its bookkeeping
 * is still consistent (slot 6), and latches a global the first time one says no.
 *
 * This file used to reach the vtable by casting `this' to a locally declared
 * `struct Base { virtual void v0(); ... virtual bool m(); }' -- six anonymous
 * slots of padding so that `m' landed on index 6. The cast is gone: Heap now
 * declares VIntact as its own slot-6 pure virtual, so this is an ordinary
 * virtual call and the slot index is the header's business, not this file's. */

bool Heap_t::Intact()
{
    bool intact = VIntact();
    if (intact)
        return intact;

    /* Latched once and never cleared -- the first corrupted heap in the run is
       the one worth reporting. */
    if (!data_020a0e98)
        data_020a0e98 = 1;
    return intact;
}

/* ExpandingHeap::VIntact() at 0x0203c65c -- Heap vtable slot 6. Eight bytes:
 * `mov r0,#1 / bx lr'.
 *
 * It answers "yes" unconditionally, exactly as SolidHeap's does, so despite the
 * node list there is no consistency check here either. Heap::Intact latches a
 * global the first time any heap reports damage; nothing in this family can
 * ever trip it. */

bool ExpHeap_t::VIntact()
{
    return 1;
}

/* SolidHeap::VIntact() at 0x0203c654 -- Heap vtable slot 6. Eight bytes:
 * `mov r0,#1 / bx lr'.
 *
 * It answers "yes" unconditionally, so it is not a corruption check -- a linear
 * allocator has no free list to walk and nothing to find inconsistent. Note
 * what that costs the caller: Heap::Intact latches a global the first time any
 * heap reports damage, and a solid heap can never trip it. */

bool SolidHeap_t::VIntact()
{
    return 1;
}

/* Heap::Rescue() at 0x0203c634 -- plain forward to slot 7, the concrete heap's
 * last-ditch recovery. Called from Heap::ResizeToFit just before OS_Terminate().
 *
 * RETURN TYPE: void, from the definitions. The old shadow here declared its
 * fake slot as `int m()' and returned it, which is unobservable for a pure
 * forwarder -- r0 simply carries whatever the callee left -- so it byte-matched
 * while claiming a result that does not exist. Both overrides are void
 * (ExpandingHeap::VRescue, SolidHeap::VRescue), and the reconstruction that was
 * already written for Heap::ResizeToFit said `void Rescue()' too. */

void Heap_t::Rescue()
{
    VRescue();
}

/* ExpandingHeap::VRescue() at 0x0203c630 -- Heap vtable slot 7. Four bytes:
 * `bx lr'. No recovery is attempted.
 *
 * The previous comment guessed that the name "appears to be called just
 * before/by OS_Terminate, perhaps to rescue heap data before a fatal exception". The
 * caller is known and it is Heap::ResizeToFit, which calls Rescue() and then
 * OS_Terminate() when a fail-fast heap cannot shrink -- so the guess had the order
 * right and the direction wrong: Rescue runs before OS_Terminate because OS_Terminate is what
 * follows a failure, not because OS_Terminate calls it. */

void ExpHeap_t::VRescue()
{
}

/* SolidHeap::VRescue() at 0x0203c62c -- Heap vtable slot 7. Four bytes: `bx lr'.
 * There is no recovery to attempt on a linear allocator. */

void SolidHeap_t::VRescue()
{
}

/* Heap::MaxAllocationUnitSize() at 0x0203c60c -- plain forward to slot 10.
 *
 * Was a cast of `this' to a local `struct Base' with ten anonymous virtuals
 * padding `m' out to index 10. */

u32 Heap_t::MaxAllocationUnitSize()
{
    return VMaxAllocationUnitSize();
}

/* ExpandingHeap::VMaxAllocationUnitSize() at 0x0203c5f8 -- Heap vtable slot 10.
 * Same call as slot 11 on this heap: the largest allocatable block and the
 * largest allocation unit are both "the biggest free node". */

u32 ExpHeap_t::VMaxAllocationUnitSize()
{
    return NNS_FndGetAllocatableSizeForExpHeapEx(allocator, 4);
}

/* SolidHeap::VMaxAllocationUnitSize() at 0x0203c5e4 -- Heap vtable slot 10.
 * Identical to slots 11 and 12 on a linear allocator; see VMemoryLeft. */

u32 SolidHeap_t::VMaxAllocationUnitSize()
{
    return NNS_FndGetAllocatableSizeForFrmHeapEx(allocator, 4);
}

/* ExpandingHeap::VMaxAllocatableSize() at 0x0203c5d0 -- Heap vtable slot 11.
 * The LARGEST SINGLE free node at 4-byte alignment, not the total; see
 * VMemoryLeft for why the distinction matters here and not on a solid heap.
 *
 * Reached `*(void**)((char*)&unk_014)' through an address cast before the
 * allocator was a named, typed member. */

u32 ExpHeap_t::VMaxAllocatableSize()
{
    return NNS_FndGetAllocatableSizeForExpHeapEx(allocator, 4);
}

/* SolidHeap::VMaxAllocatableSize() at 0x0203c5bc -- Heap vtable slot 11.
 * Identical to slots 10 and 12 on a linear allocator; see VMemoryLeft. */

u32 SolidHeap_t::VMaxAllocatableSize()
{
    return NNS_FndGetAllocatableSizeForFrmHeapEx(allocator, 4);
}

/* ExpandingHeap::VMemoryLeft() at 0x0203c5ac -- Heap vtable slot 12. The SUM of
 * every free node's size.
 *
 * Slots 10, 11 and 12 are three different numbers on an expanding heap and one
 * number on a solid one. Here 12 sums the free list while 10 and 11 ask for the
 * largest single node -- so a fragmented heap can report plenty of memory left
 * and still refuse an allocation. */

u32 ExpHeap_t::VMemoryLeft()
{
    return NNS_FndGetTotalFreeSizeForExpHeap(allocator);
}

/* SolidHeap::VMemoryLeft() at 0x0203c598 -- Heap vtable slot 12.
 *
 * Slots 10, 11 and 12 all make the same
 * NNS_FndGetAllocatableSizeForFrmHeapEx(allocator, 4) call on a solid heap:
 * the allocator is linear, so the memory left, the largest allocatable block
 * and the largest single unit are all the same number. On an expanding heap
 * they differ, which is why the three slots exist at all. */

u32 SolidHeap_t::VMemoryLeft()
{
    return NNS_FndGetAllocatableSizeForFrmHeapEx(allocator, 4);
}

/* Heap::Reallocate(void*, u32) at 0x0203c578 -- forwards to slot 8 and hands
 * back the resulting SIZE, or 0 if the block could not be resized.
 *
 * RETURN TYPE: u32, and it took a caller to establish that it returns anything
 * at all. This was declared
 * `void' when the only evidence was its own body -- a pure forward, where the
 * return is unobservable because r0 just carries whatever the callee left.
 * SolidHeap::VResizeToFit branches on the result (`if (!Reallocate(...))'),
 * which a void declaration cannot compile at all. A forwarder's return type is
 * unobservable at its definition and observable at its callers; only one of
 * those was available before.
 *
 * It is a SIZE and not a moved pointer. Both allocators say so in their own
 * bodies: NNS_FndResizeForMBlockFrmHeap returns `size' or 0, and
 * NNS_FndResizeForMBlockExpHeap returns `size', 0 or the block's old size. The
 * `void*' this once claimed came from a local shadow in
 * ExpandingHeap::VReallocate's old shard that declared the allocator method
 * itself as pointer-returning -- a guess nothing ever checked, because a
 * forwarder cannot disagree with its callee about a value it merely passes on.
 *
 * Was a cast of `this' to a local `struct Base' with eight anonymous virtuals
 * padding `m' out to index 8. */

u32 Heap_t::Reallocate(void* ptr, u32 size)
{
    return VReallocate(ptr, size);
}

/* ExpandingHeap::VReallocate(void*, u32) at 0x0203c568 -- Heap vtable slot 8.
 * Four instructions: load the allocator from this+0x14 and tail-call it.
 *
 * RETURN TYPE: u32, changed from void*. THIS FILE IS WHERE THE void* CAME FROM.
 * It declared its own allocator class with a `void* Reallocate(void*, u32)',
 * and that local guess propagated outwards until Heap vtable slot 8
 * was documented as returning a moved pointer. The real definition,
 * NNS_FndResizeForMBlockExpHeap, returns `size', 0 or the block's old size --
 * a size every time. Nothing could catch it,
 * because a tail-call forwarder cannot disagree with its callee about a value it
 * merely passes on. */

u32 ExpHeap_t::VReallocate(void* ptr, u32 size)
{
    return NNS_FndResizeForMBlockExpHeap(allocator, ptr, size);
}

/* SolidHeap::VReallocate(void*, u32) at 0x0203c558 -- Heap vtable slot 8.
 * Four instructions: load the allocator from this+0x14 and tail-call it.
 *
 * Returns the new SIZE, not a moved pointer -- NNS_FndResizeForMBlockFrmHeap
 * resizes in place and returns `size' or 0. This file used to declare its own
 * `class SolidHeap' with members named unk00..unk10 plus an allocator class
 * whose Reallocate it declared pointer-returning, which is
 * where the tree's `void*' for this slot came from. */

u32 SolidHeap_t::VReallocate(void* ptr, u32 size)
{
    return NNS_FndResizeForMBlockFrmHeap(allocator, ptr, size);
}

/* Heap::Deallocate(void*) at 0x0203c538 -- forwards to the concrete heap's
 * slot 4. No guard: freeing is the one operation with nothing to fail at.
 *
 * Was a cast of `this' to a local `struct Base' with four anonymous virtuals
 * padding `m' out to index 4; VDeallocate is now Heap's own slot-4 pure
 * virtual. */

void Heap_t::Deallocate(void* ptr)
{
    VDeallocate(ptr);
}

/* ExpandingHeap::VDeallocate(void*) at 0x0203c50c -- Heap vtable slot 4. Free
 * one block back to the node list.
 *
 * NULL is tolerated silently, which is what lets callers deallocate
 * unconditionally. This is the slot where the two heaps differ most plainly:
 * SolidHeap's override crashes on any non-null pointer, because a linear
 * allocator cannot return a single block. */

void ExpHeap_t::VDeallocate(void* ptr)
{
    if (!ptr)
        return;

    NNS_FndFreeToExpHeap(allocator, ptr);
}

/* SolidHeap::VDeallocate(void*) at 0x0203c4e4 -- Heap vtable slot 4.
 *
 * Freeing a single block from a linear allocator is not supported, so a
 * non-null pointer is a programming error and crashes. Freeing NULL is
 * tolerated silently, which is what lets callers deallocate unconditionally.
 * VDeallocateAll is the only real free a solid heap has. */

void SolidHeap_t::VDeallocate(void* ptr)
{
    if (ptr == 0)
        return;
    OS_Terminate();
}

} /* namespace mHeap */

/* FreeVisitedBlock(void*, NNSFndHeapHandle, u32) at 0x0203c4cc -- the visitor
 * ExpHeap_t::VDeallocateAll hands to NNS_FndVisitAllocatedForExpHeap. It has
 * the NNSFndHeapVisitor shape (block, heap, user parameter), ignores the user
 * parameter, and frees the block. The ROM keeps no name for it; this one is
 * coined. It used to be spelled as a static member of a coined allocator
 * class, ExpandingHeapAllocator::InvokeDeallocate; the NitroSystem heap
 * underneath was never a class. */
void FreeVisitedBlock(void *memBlock, NNSFndHeapHandle heap, u32 userParam) {
    NNS_FndFreeToExpHeap(heap, memBlock);
}

namespace mHeap {

/* ExpandingHeap::VDeallocateAll() at 0x0203c4b0 -- Heap vtable slot 5. Visits
 * every allocated block of the expanded heap with FreeVisitedBlock, which
 * frees each one.
 *
 * An earlier copy of the allocator declared in this file took the visitor as
 * a pointer to a function pointer, and the call carried a cast to fit it.
 * Byte-identical either way -- an address is an address -- which is why
 * nothing caught it while the declaration lived here. */

void ExpHeap_t::VDeallocateAll()
{
    NNS_FndVisitAllocatedForExpHeap(allocator, FreeVisitedBlock, 0);
}

/* SolidHeap::VDeallocateAll() at 0x0203c49c -- Heap vtable slot 5. Throws the
 * whole arena away at once, which for a linear allocator is the only kind of
 * free that works: individual blocks cannot be returned (VDeallocate calls
 * OS_Terminate), so everything goes together or nothing does. */

void SolidHeap_t::VDeallocateAll()
{
    NNS_FndFreeToFrmHeap(allocator, NNS_FND_FRMHEAP_FREE_ALL);
}

/* Heap::Sizeof(void*) at 0x0203c454 -- asks slot 9 how large an allocated block
 * is, and treats -1 as the failure report.
 *
 * The result is held in an `int' because that is what this function returns;
 * slot 9 is declared u32, which is what the overrides return. The choice is
 * BYTE-UNOBSERVABLE, and that was measured, not assumed: compiling this with
 * `u32 size' and `size == (u32)-1' is byte-identical under 2004/b56. An earlier
 * revision of this comment claimed the u32 spelling "would turn that into an
 * unsigned compare and change the instruction", which is simply false -- the
 * guard is an equality, so it lowers to cmp/bne on the Z flag and there is no
 * signed or unsigned variant of that to pick between. Runbook section 2 asks
 * for "byte-unobservable", not "verified"; the earlier wording was the exact
 * overclaim it warns about.
 *
 * Was a cast of `this' to a local `struct Base' with nine anonymous virtuals
 * padding `m' out to index 9. */

int Heap_t::Sizeof(void* ptr)
{
    int size = VSizeof(ptr);
    if (size == -1 && (flags & 0x4000))
        OS_Terminate();
    return size;
}

/* ExpandingHeap::VSizeof(void*) at 0x0203c444 -- Heap vtable slot 9. Node
 * headers carry their own size, so this ignores `this' entirely and forwards to
 * NNS_FndGetSizeForMBlockExpHeap, which takes the block alone -- which is why
 * the ROM body
 * is `mov r0, r1' followed by a tail call: it moves the pointer argument into
 * r0 over the incoming `this'.
 *
 * SolidHeap's override crashes instead, because a linear allocator records no
 * sizes to look up. */

u32 ExpHeap_t::VSizeof(void* ptr)
{
    return NNS_FndGetSizeForMBlockExpHeap(ptr);
}

/* SolidHeap::VSizeof(void*) at 0x0203c428 -- Heap vtable slot 9.
 *
 * A linear allocator does not record block sizes, so asking is a programming
 * error: it calls OS_Terminate() and, if that ever returns, reports -1 -- the failure
 * value Heap::Sizeof tests for.
 *
 * Return type widened from int to u32 to match the slot. Byte-unobservable:
 * both spellings lower the -1 to `mvn r0,#0'. It is a consistency change, not
 * evidence of a width. */

u32 SolidHeap_t::VSizeof(void* ptr)
{
    OS_Terminate();
    return -1;
}

/* Heap::SetNodeID(u32) at 0x0203c408 -- plain forward to slot 13. The node id
 * is the tag the allocator stamps on blocks it hands out, so a later sweep can
 * free everything belonging to one id (see NNS_FndVisitAllocatedForExpHeap).
 *
 * Slot 13 returns u32 -- SolidHeap's override is eight bytes of `mov r0,#0 /
 * bx lr', which a void declaration could not emit. This wrapper drops it, so
 * the call is still a plain tail call and the bytes do not care.
 *
 * The old shadow declared its slot-13 entry as `v13(void*)' and converted the
 * incoming id to a pointer to call it -- an invented cast that existed only to
 * satisfy the invented declaration. */

void Heap_t::SetNodeID(u32 id)
{
    VSetNodeID(id);
}

/* ExpandingHeap::VSetNodeID(u32) at 0x0203c3f8 -- Heap vtable slot 13. Four
 * instructions: load the allocator from this+0x14 and tail-call it.
 *
 * RETURN TYPE: u32, changed from void. The slot returns the PREVIOUS id --
 * NNS_FndSetGroupIDForExpHeap returns it, and
 * SolidHeap's override materializes `mov r0,#0' in eight bytes, which void
 * cannot emit. This one is a pure tail call, so it byte-matches either way and
 * could never have settled the question itself; it just has to agree. */

u32 ExpHeap_t::VSetNodeID(u32 id)
{
    return NNS_FndSetGroupIDForExpHeap(allocator, id);
}

/* SolidHeap::VSetNodeID(u32) at 0x0203c3f0 -- Heap vtable slot 13. Eight bytes:
 * `mov r0,#0 / bx lr'. The id is accepted and dropped; a linear allocator has
 * nothing to stamp it on.
 *
 * THIS FUNCTION IS WHY SLOT 13 RETURNS u32 rather than void. The `mov r0,#0'
 * is a materialized return value, and a void declaration cannot emit it --
 * which makes this the one place in the family where the slot's return type is
 * byte-observable. ExpandingHeap's override is a pure forward and would match
 * under either spelling, so it could not settle the question; this one does.
 *
 * The previous text opened with `u32 id = (u32)id_;', converting the parameter
 * to its own type and then never using it. */

u32 SolidHeap_t::VSetNodeID(u32 id)
{
    return 0;
}

/* ExpandingHeap::VGetNodeID() at 0x0203c3e0 -- Heap vtable slot 14. Forwards to
 * the allocator, which really does keep an id: it stamps every node it hands
 * out, so a visitor can later free just one id's worth. SolidHeap's
 * override returns a constant 0 because a linear allocator has nothing to
 * stamp. */

u32 ExpHeap_t::VGetNodeID()
{
    return NNS_FndGetGroupIDForExpHeap(allocator);
}

/* SolidHeap::VGetNodeID() at 0x0203c3d8 -- Heap vtable slot 14. Eight bytes:
 * `mov r0,#0 / bx lr'. A linear allocator does not tag blocks, so there is no
 * node id to report. */

u32 SolidHeap_t::VGetNodeID()
{
    return 0;
}

/* Heap::ResizeToFit() at 0x0203c390 -- hand the arena's unused tail back (slot
 * 15) and, on a fail-fast heap, treat a refusal as fatal.
 *
 * This file already held a correct C++ method. What it did not have was a
 * header: the whole class -- every field and all sixteen vtable slots -- was
 * declared locally, which is why the reconstruction that eventually became
 * include/Heap.h could sit here agreeing with the ROM and help nobody. It is
 * the same class now, from the shared header.
 *
 * The local copy did get two things wrong, which is the argument for having
 * one copy rather than eleven: it declared `bool VDeallocate' and `unsigned int
 * VSetNodeID', where both overrides return void. Neither error was reachable
 * from this file, so nothing could ever have caught them. */

u32 Heap_t::ResizeToFit()
{
    u32 shrunk = VResizeToFit();
    if (!shrunk)
    {
        if (flags & 0x4000)
        {
            Rescue();
            OS_Terminate();
        }
    }
    return shrunk;
}

/* ExpandingHeap::VResizeToFit() at 0x0203c388 -- Heap vtable slot 15. Eight
 * bytes: `mov r0,#0 / bx lr'.
 *
 * Always fails, and 0 is the failure value Heap::ResizeToFit tests. This is the
 * one slot where the two heaps genuinely invert: SolidHeap hands its unused tail
 * back to the parent, while an expanding heap's free space is scattered across a
 * node list and there is no tail to give. */

u32 ExpHeap_t::VResizeToFit()
{
    return 0;
}

/* SolidHeap::VResizeToFit() at 0x0203c33c -- Heap vtable slot 15. Give back the
 * arena's unused tail.
 *
 * Two steps, and the second can fail after the first has succeeded. First the
 * allocator reports how much it actually needs; then this heap shrinks its own
 * block in the PARENT heap to that much plus 0x18 -- the SolidHeap object
 * itself, which lives at the front of the block. Only once the parent has
 * agreed is heapSize updated.
 *
 * Returns the new total block size, or 0 if either step refused.
 *
 * Its old shard re-declared `struct Heap' and `struct SolidHeap' locally and
 * reach the base through `thiz->base.parentHeap'. It is the same layout, said
 * once. Its call to Heap::Reallocate is also what proved that method returns a
 * value at all -- see Heap::Reallocate. */

u32 SolidHeap_t::VResizeToFit()
{
    u32 used = NNS_FndAdjustFrmHeap(allocator);
    if (!used)
        return 0;

    u32 total = used + 0x18;
    if (!parentHeap->Reallocate(this, total))
        return 0;

    heapSize = used;
    return total;
}

/* Install this heap as the process-wide default and hand back the one it
   replaced, so a caller can put it back. */
Heap_t* Heap_t::SetDefault()
{
    Heap* previous = Memory::defaultHeapPtr;
    Memory::defaultHeapPtr = this;
    return previous;
}

/* Push a solid scratch heap in front of the current default, remembering the
   old one so RestoreFromTemporary can put it back. One level deep only: a
   second call overwrites the saved pointer and the first default is lost.

   Note the order. The outgoing default is read straight out of the global
   rather than from SetDefault's return value, even though SetDefault returns
   exactly that -- so the save happens before the swap, not after it. */
void* Heap_t::SetupSolidHeapAsDefault(u32 size, Heap_t* root, int align)
{
    Heap* heap = CreateSolidHeap(size, root, align);
    if (!heap)
        return 0;

    data_020a0ea8 = Memory::defaultHeapPtr;
    heap->SetDefault();
    return heap;
}

/* Three-word tail-call veneer to SetupSolidHeapAsDefault above:
   `ldr ip,[pc] / bx ip / .word 0x0203c2e4'. Static, by the same argument-count
   test as its target: three declared parameters, three arguments in the body,
   no room for a `this'. The veneer sits twelve bytes BEFORE what it jumps to
   -- the pair is adjacent and the thunk is still a long-form ldr/bx, which is
   what these veneers look like throughout: they are not distance-driven. */
void* Heap_t::InitializeSolidHeapAsDefault(u32 size, Heap_t* root, int align)
{
    return SetupSolidHeapAsDefault(size, root, align);
}

/* Undoes SetupSolidHeapAsDefault: reinstate the heap that was default before
   the scratch heap was pushed, and drop the saved pointer. */
void Heap_t::RestoreFromTemporary()
{
    data_020a0ea8->SetDefault();
    data_020a0ea8 = 0;
}

/* Three-word tail-call veneer to Allocate(u32, int) (0x0203c6cc). */
void* Heap_t::_Allocate(u32 size, int align)
{
    return Allocate(size, align);
}

/* Convenience overload that forwards to Allocate(u32, int) with the default
   alignment of 4. */
void* Heap_t::Allocate(u32 size)
{
    return Allocate(size, 4);
}

/* Three-word tail-call veneer to Deallocate (0x0203c538). */
void Heap_t::_Deallocate(void* ptr)
{
    Deallocate(ptr);
}

/* Three-word tail-call veneer to Sizeof (0x0203c454). */
int Heap_t::_Sizeof(void* ptr)
{
    return Sizeof(ptr);
}

/* Carve the game heap out of `root' and publish it. Alignment 4 is hard-coded
   here; callers do not get a say. */
void Heap_t::InitializeGameHeap(u32 size, Heap_t* root)
{
    GAME_HEAP_PTR = CreateExpandingHeap(size, root, 4);
}

/* Memory::Allocate / Deallocate, 0x0203c1b4..0x0203c24c.
 * The Heap*-taking forms come first so reverse emission puts them
 * above the one-argument wrappers, which call them. */
} /* namespace mHeap */

namespace Memory {

void *Allocate(u32 size, int align, Heap *heap)
{
    if (!heap)
        heap = defaultHeapPtr;
    return heap->Allocate(size, align);
}

void Deallocate(void *ptr, Heap *heap)
{
    if (!heap)
        heap = defaultHeapPtr;
    heap->Deallocate(ptr);
}

void *Allocate(u32 size, int align)
{
    return Allocate(size, align, 0);
}

void *Allocate(u32 size)
{
    return Allocate(size, 4, 0);
}

void Deallocate(void *ptr)
{
    Deallocate(ptr, 0);
}

}

/* Zero Vector3 immediately after GAME_HEAP_PTR. Vector3(x, y, z) scalarizes,
 * and a type derived from Vector3 would emit ~Vector3 into this object, so
 * the stores are a file-local type. Its destructor is aliased to ~Vector3. */
#ifndef _MSC_VER
struct VecOff {
    int x, y, z;
    VecOff(int px, int py, int pz) {
        x = px;
        y = py;
        z = pz;
    }
    ~VecOff();
};
typedef char VecOff_size_must_be_0xc[sizeof(VecOff) == 0xc ? 1 : -1];

VecOff data_020a0ebc(0, 0, 0);
#endif
