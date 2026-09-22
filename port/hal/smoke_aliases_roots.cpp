// port/hal/smoke_aliases_roots.cpp -- the __cdecl and data half of the gate-3a
// smoke's link, as /alternatename rows copied verbatim from
// port/hal/cxx_aliases.cpp.
//
// Lane SMOKELINK3, run link100 wave 10 round 2, continuing SMOKELINK2's lever.
//
// Read port/hal/smoke_aliases_heap.cpp's header first; it states the whole
// argument and nothing about it changes here.  In one line: cxx_aliases.cpp
// holds 1328 rows AND real definitions (the DSSTATE block, MultiCopyHalf, the
// matrix builders, the ppu audit), so putting it on a 54-object harness drags
// the window build's scaffolding in behind it, while a file of nothing but
// linker directives costs a target exactly the rows it carries.
//
// WHY AN ALIAS IS HONEST FOR EVERY ROW HERE.  port/faces_sync.txt's header
// states the rule: an /alternatename is a NAME bridge and never an ABI bridge,
// so a row may only be closed this way when the two sides already agree about
// where the receiver and the arguments live.  Each row below was checked, by
// its decoration, before it was copied:
//
//   nine   ?V*@ExpandingHeap@@QAE...  ->  ?V*@ExpandingHeap@@UAE...
//          and ?Allocate@Heap@@QAEHIH@Z -> ?Allocate@Heap@@QAEPAXIH@Z.
//          Both sides are AE, __thiscall, same class, same parameter list:
//          receiver in ECX on both.  Q against U is the non-virtual against
//          virtual spelling of one body and H against PAX is int against void*
//          in EAX, which is the same register.  hal/heap_vtable.cpp's slot
//          thunks spell the Q form and the src/ TUs define the U form; these
//          nine rows are how the window targets have joined them since before
//          the sync.
//   three  __ZN22ExpandingHeapAllocator{Create,Link,Unlink}Node...  ->  SA.
//          include/ExpandingHeapAllocator.h:89-91 declares all three static
//          and says why ("no room for a `this'"), so caller and callee are
//          both __cdecl and the name hop is the whole difference.
//   three  plain data: _ROOT_HEAP_ARENA_ID, data_020a4d34, data_020a4d38.
//          port/hal/heap_globals.cpp and port/hal/os_arena.cpp own the single
//          real storage; these are more names for the same bytes.
//
// Every one of the fifteen is copied CHARACTER FOR CHARACTER from
// hal/cxx_aliases.cpp at the line named beside it, so a grep proves the two
// files agree and neither is a fresh adjudication.  The two rows in the last
// block are the exception and say so in their own note.  This file is on the
// smoke targets ONLY, so no row here can collide with cxx_aliases.cpp's copy
// on a window target.

/* hal/cxx_aliases.cpp:2985 */
#pragma comment(linker, "/alternatename:?Allocate@Heap@@QAEHIH@Z=?Allocate@Heap@@QAEPAXIH@Z")
/* hal/cxx_aliases.cpp:2977 */
#pragma comment(linker, "/alternatename:?VAllocate@ExpandingHeap@@QAEPAXIH@Z=?VAllocate@ExpandingHeap@@UAEPAXIH@Z")
/* hal/cxx_aliases.cpp:2983 */
#pragma comment(linker, "/alternatename:?VGetNodeID@ExpandingHeap@@QAEIXZ=?VGetNodeID@ExpandingHeap@@UAEIXZ")
/* hal/cxx_aliases.cpp:2980 */
#pragma comment(linker, "/alternatename:?VIntact@ExpandingHeap@@QAE_NXZ=?VIntact@ExpandingHeap@@UAE_NXZ")
/* hal/cxx_aliases.cpp:2978 */
#pragma comment(linker, "/alternatename:?VReallocate@ExpandingHeap@@QAEPAXPAXI@Z=?VReallocate@ExpandingHeap@@UAEIPAXI@Z")
/* hal/cxx_aliases.cpp:2981 */
#pragma comment(linker, "/alternatename:?VRescue@ExpandingHeap@@QAEXXZ=?VRescue@ExpandingHeap@@UAEXXZ")
/* hal/cxx_aliases.cpp:2982 */
#pragma comment(linker, "/alternatename:?VResizeToFit@ExpandingHeap@@QAEIXZ=?VResizeToFit@ExpandingHeap@@UAEIXZ")
/* hal/cxx_aliases.cpp:2984 */
#pragma comment(linker, "/alternatename:?VSetNodeID@ExpandingHeap@@QAEXI@Z=?VSetNodeID@ExpandingHeap@@UAEII@Z")
/* hal/cxx_aliases.cpp:2979 */
#pragma comment(linker, "/alternatename:?VSizeof@ExpandingHeap@@QAEIPAX@Z=?VSizeof@ExpandingHeap@@UAEIPAX@Z")
/* hal/cxx_aliases.cpp:3826 */
#pragma comment(linker, "/alternatename:?data_020a4d34@@3HA=__ZN6Memory25isRootHeapIterInitializedE")
/* hal/cxx_aliases.cpp:3825 */
#pragma comment(linker, "/alternatename:?data_020a4d38@@3UNestedHeapIterator@@A=__ZN6Memory16rootHeapIteratorE")
/* hal/cxx_aliases.cpp:3392 */
#pragma comment(linker, "/alternatename:_ROOT_HEAP_ARENA_ID=_data_020a0ea4")   /* 0x020a0ea4, os_arena.cpp */
/* hal/cxx_aliases.cpp:3285 */
#pragma comment(linker, "/alternatename:__ZN22ExpandingHeapAllocator10CreateNodeEPN10MemoryNode6TargetEt=?CreateNode@ExpandingHeapAllocator@@SAPAXPAUTarget@MemoryNode@@G@Z")
/* hal/cxx_aliases.cpp:3284 */
#pragma comment(linker, "/alternatename:__ZN22ExpandingHeapAllocator10UnlinkNodeEP10MemoryNodeS1_=?UnlinkNode@ExpandingHeapAllocator@@SAPAXPAUMemoryNode@@0@Z")
/* hal/cxx_aliases.cpp:3286 */
#pragma comment(linker, "/alternatename:__ZN22ExpandingHeapAllocator8LinkNodeEP10MemoryNodeS1_S1_=?LinkNode@ExpandingHeapAllocator@@SAPAXPAUMemoryNode@@00@Z")

/* THE GLOBAL DEALLOCATOR SEAM, and the one row here that is NOT a verbatim
   copy.  smoke_roots's thirtieth unresolved name is the flat
   __ZN6Memory16operator_delete2EPv, referenced by Heap::operator delete
   (include/Heap.h:265, whose extern "C" declaration is at Heap.h:141) out of
   src/_ZN4HeapC2EPvjPS_.cpp's scalar deleting destructor and out of
   src/_ZN13ExpandingHeapC1EPvjP4HeapP22ExpandingHeapAllocator.cpp's.

   On the window targets that name is DEFINED, by hal/cxxname_bridge.cpp:682,
   and cxx_aliases.cpp:2936 points the C++ spelling AT it:

     /alternatename:?operator_delete2@Memory@@YAXPAX@Z=__ZN6Memory16operator_delete2EPv

   Here the direction is the other way round, because this target does not
   compile that 16000-line host file and does compile the ROM's own matched
   veneer instead.  src/_ZN6Memory16operator_delete2EPv.cpp is the three-word
   tail call at 0x0203cbcc, it is on this target's source list as of this
   commit, and as a C++ namespace function it emits
   ?operator_delete2@Memory@@YAXPAX@Z.  So the flat name is the one that needs
   a hop, and the row below is cxx_aliases.cpp:2936 reversed.

   It is an ABI bridge and not just a name bridge: YA is __cdecl and both
   sides are free functions taking one pointer, so nothing rides in a
   register that the other side would look for on the stack.  The receiver
   question the faces_sync.txt rule is about does not arise -- there is no
   receiver.

   Taking the ROM's veneer rather than copying the host forwarder is what
   makes this the SRC answer route.md classified it as: the body that runs is
   the cartridge's, byte-matched, and it ends where the cartridge's ends.  Its
   own next hop is the flat _ZdlPv, which src/_ZdlPv.cpp defines as a real
   `operator delete(void *)' and therefore emits as ??3@YAXPAX@Z -- the second
   row below, copied verbatim, is how cxx_aliases.cpp has joined those two
   since it was written.  src/_ZdlPv.cpp joins this target's source list in the
   same commit, so ??3@YAXPAX@Z here is the ROM's deallocator and not the
   CRT's; port/hal/os_arena.cpp's sized-delete router (its own line 305) is
   already on this target and already expects that shape. */
/* hal/cxx_aliases.cpp:2936, REVERSED: see the note above */
#pragma comment(linker, "/alternatename:__ZN6Memory16operator_delete2EPv=?operator_delete2@Memory@@YAXPAX@Z")
/* hal/cxx_aliases.cpp:3665 */
#pragma comment(linker, "/alternatename:__ZdlPv=??3@YAXPAX@Z")

/* cstd::abs.  src/_ZN4cstd3absEi.cpp joins this target's source list in the
   same commit -- ExpandingHeapAllocator::MaxAllocatableSize calls it -- and as
   a C++ namespace function it emits ?abs@cstd@@YAHH@Z, while the ROM's callers
   spell the flat name.  Both sides are YA, __cdecl, one int in and one int
   out, so this is a name hop and nothing else.  Copied verbatim; it is the row
   the window targets have used for the same pair since it was written. */
/* hal/cxx_aliases.cpp:3287 */
#pragma comment(linker, "/alternatename:__ZN4cstd3absEi=?abs@cstd@@YAHH@Z")

/* NestedHeapIterator::FindNested.  HeapAllocator::Remove calls it and the
   sibling Remove on the same object; include/NestedHeapIterator.h:26 declares
   THIS one static and line 28 declares the other non-static, so the two close
   differently on purpose -- Remove is a ledger face in
   port/faces_sync_smoke_roots.txt and FindNested is a name hop between two
   __cdecl free functions.  Copied verbatim. */
/* hal/cxx_aliases.cpp:3299 */
#pragma comment(linker, "/alternatename:__ZN18NestedHeapIterator10FindNestedEPv=?FindNested@NestedHeapIterator@@SAPAU1@PAX@Z")
