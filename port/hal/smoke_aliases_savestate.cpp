// port/hal/smoke_aliases_savestate.cpp -- the __cdecl, static and data half of
// smoke_savestate's link, as /alternatename rows copied verbatim from
// port/hal/cxx_aliases.cpp.
//
// Lane SMOKELINK4B, run link100 wave 10 round 4.
//
// Read port/hal/smoke_aliases_heap.cpp's header for the whole argument and
// port/hal/smoke_aliases_roots.cpp's for the decoration-by-decoration check
// this file's rows were put through too.  In one line: hal/cxx_aliases.cpp
// holds 1328 rows AND real definitions, so putting it on a smoke harness drags
// the window build's scaffolding in behind it, while a file of nothing but
// linker directives costs a target exactly the rows it carries.
//
// THE THREE GATE-9 HARNESSES SHARE ONE NAME SET, so this file and its two
// siblings carry the same twenty-eight rows; port/faces_sync_smoke_savestate.txt's
// header has the measurement.
//
// WHY AN ALIAS IS HONEST FOR EVERY ROW HERE.  port/faces_sync.txt's header
// states the rule: an /alternatename is a NAME bridge and never an ABI bridge,
// so a row may only be closed this way when the two sides already agree about
// where the receiver and the arguments live.  Each row below was checked by its
// own decoration before it was copied -- SA is a static and __cdecl, YA is a
// free function and __cdecl, 3 is plain data, and an AE-to-AE pair inside one
// class is the non-virtual against the virtual spelling of one body with the
// receiver in ECX on both sides.  Anything else on these targets is a face in
// port/faces_sync_smoke_savestate.txt, not a row here.
//
// Every row is copied CHARACTER FOR CHARACTER from hal/cxx_aliases.cpp at the
// line named beside it (one row from hal/bob_enemy_bridges.cpp, also named), so
// a grep proves the two files agree and neither is a fresh adjudication.  This
// file is on the smoke targets ONLY, so no row here can collide with
// cxx_aliases.cpp's copy on a window target.
//
// NOT HERE, AND THAT IS THE DIFFERENCE FROM THE TEN LEDGERS BEFORE THIS ONE:
// the heap-teardown seam's four rows (the reversed operator_delete2, __ZdlPv,
// and NestedHeapIterator::FindNested).  These three targets already compile
// hal/cxxname_bridge.cpp, which is the file the window targets get the flat
// __ZN6Memory16operator_delete2EPv from, so the seam is already joined here and
// the reversed row would be a second definition rather than a bridge.  cstd::abs
// IS here, once, in the verbatim block: it arrives with
// ExpandingHeapAllocator::MaxAllocatableSize exactly as it does on smoke_roots.

/* THE NINE SAME-CLASS THISCALL SPELLINGS, the rows every smoke ledger from
   smoke_roots on carries.  Both sides AE, __thiscall, same class, same
   parameters: the receiver is in ECX on both, Q against U is the non-virtual
   against the virtual spelling of one body, and H against PAX is int against
   void* in EAX.  hal/heap_vtable.cpp's slot thunks spell the Q form and the
   src/ TUs define the U form. */
/* hal/cxx_aliases.cpp:2977 */
#pragma comment(linker, "/alternatename:?VAllocate@ExpandingHeap@@QAEPAXIH@Z=?VAllocate@ExpandingHeap@@UAEPAXIH@Z")
/* hal/cxx_aliases.cpp:2978 */
#pragma comment(linker, "/alternatename:?VReallocate@ExpandingHeap@@QAEPAXPAXI@Z=?VReallocate@ExpandingHeap@@UAEIPAXI@Z")
/* hal/cxx_aliases.cpp:2979 */
#pragma comment(linker, "/alternatename:?VSizeof@ExpandingHeap@@QAEIPAX@Z=?VSizeof@ExpandingHeap@@UAEIPAX@Z")
/* hal/cxx_aliases.cpp:2980 */
#pragma comment(linker, "/alternatename:?VIntact@ExpandingHeap@@QAE_NXZ=?VIntact@ExpandingHeap@@UAE_NXZ")
/* hal/cxx_aliases.cpp:2981 */
#pragma comment(linker, "/alternatename:?VRescue@ExpandingHeap@@QAEXXZ=?VRescue@ExpandingHeap@@UAEXXZ")
/* hal/cxx_aliases.cpp:2982 */
#pragma comment(linker, "/alternatename:?VResizeToFit@ExpandingHeap@@QAEIXZ=?VResizeToFit@ExpandingHeap@@UAEIXZ")
/* hal/cxx_aliases.cpp:2983 */
#pragma comment(linker, "/alternatename:?VGetNodeID@ExpandingHeap@@QAEIXZ=?VGetNodeID@ExpandingHeap@@UAEIXZ")
/* hal/cxx_aliases.cpp:2984 */
#pragma comment(linker, "/alternatename:?VSetNodeID@ExpandingHeap@@QAEXI@Z=?VSetNodeID@ExpandingHeap@@UAEII@Z")
/* hal/cxx_aliases.cpp:2985 */
#pragma comment(linker, "/alternatename:?Allocate@Heap@@QAEHIH@Z=?Allocate@Heap@@QAEPAXIH@Z")

/* fBase_c's TWO VIRTUAL SPELLINGS, the same shape as the nine above and new to
   this batch.  Both sides AE, one class, the same two integer parameters: U is
   the virtual spelling the fBase_c vtable fills ask for and Q is the
   non-virtual spelling src/_ZN7fBase_c9Virtual34Ejj.cpp's sibling TUs emit.
   Nothing about the receiver moves. */
/* hal/cxx_aliases.cpp:2988 */
#pragma comment(linker, "/alternatename:?Virtual34@fBase_c@@UAEHII@Z=?Virtual34@fBase_c@@QAEHII@Z")
/* hal/cxx_aliases.cpp:2989 */
#pragma comment(linker, "/alternatename:?Virtual38@fBase_c@@UAEHII@Z=?Virtual38@fBase_c@@QAEHII@Z")

/* THE STATICS AND FREE FUNCTIONS.  SA is a static member and __cdecl, YA is a
   namespace-level free function and __cdecl: caller and callee agree about
   every argument's place, so the name hop is the whole difference.  The three
   ExpandingHeapAllocator rows carry include/ExpandingHeapAllocator.h:89-91's
   reason ("no room for a `this'"); Memory::Allocate, Memory::Deallocate,
   cstd::abs, cstd::sqrt, cstd::fdiv_result and GX::LoadTexPltt are free
   functions in their namespaces, which is what the YA says; dBgCh::
   ShouldPassThroughImpl is a static. */
/* hal/cxx_aliases.cpp:3284 */
#pragma comment(linker, "/alternatename:__ZN22ExpandingHeapAllocator10UnlinkNodeEP10MemoryNodeS1_=?UnlinkNode@ExpandingHeapAllocator@@SAPAXPAUMemoryNode@@0@Z")
/* hal/cxx_aliases.cpp:3285 */
#pragma comment(linker, "/alternatename:__ZN22ExpandingHeapAllocator10CreateNodeEPN10MemoryNode6TargetEt=?CreateNode@ExpandingHeapAllocator@@SAPAXPAUTarget@MemoryNode@@G@Z")
/* hal/cxx_aliases.cpp:3286 */
#pragma comment(linker, "/alternatename:__ZN22ExpandingHeapAllocator8LinkNodeEP10MemoryNodeS1_S1_=?LinkNode@ExpandingHeapAllocator@@SAPAXPAUMemoryNode@@00@Z")
/* hal/cxx_aliases.cpp:3245 */
#pragma comment(linker, "/alternatename:__ZN2GX11LoadTexPlttEPKvjj=?LoadTexPltt@GX@@YAXPBXII@Z")
/* hal/cxx_aliases.cpp:3287 */
#pragma comment(linker, "/alternatename:__ZN4cstd3absEi=?abs@cstd@@YAHH@Z")
/* hal/cxx_aliases.cpp:3264 */
#pragma comment(linker, "/alternatename:__ZN4cstd4sqrtEy=?sqrt@cstd@@YAH_K@Z")
/* hal/cxx_aliases.cpp:3290 */
#pragma comment(linker, "/alternatename:__ZN4cstd11fdiv_resultEv=?fdiv_result@cstd@@YAHXZ")
/* hal/cxx_aliases.cpp:3288 */
#pragma comment(linker, "/alternatename:__ZN5dBgCh21ShouldPassThroughImplEPvRK4CLPSRKS_b=?ShouldPassThroughImpl@dBgCh@@SA_NPAXABUCLPS@@ABU1@_N@Z")
/* hal/cxx_aliases.cpp:3289 */
#pragma comment(linker, "/alternatename:__ZN6Memory8AllocateEjiP4Heap=?Allocate@Memory@@YAPAXIHPAVHeap@@@Z")
/* hal/cxx_aliases.cpp:1491 */
#pragma comment(linker, "/alternatename:__ZN6Memory10DeallocateEPvP4Heap=?Deallocate@Memory@@YAXPAXPAVHeap@@@Z")
/* hal/cxx_aliases.cpp:3339 */
#pragma comment(linker, "/alternatename:__ZN4Heap18InitializeGameHeapEjPS_=?InitializeGameHeap@Heap@@SAXIPAU1@@Z")

/* THE DATA ROWS.  A 3 in the decoration is plain data, and for data a name
   bridge IS the whole binding: one object, two names.  _ROOT_HEAP_ARENA_ID and
   _GAME_HEAP_PTR are the port's role names for the two heap words
   hal/os_arena.cpp and hal/heap_globals.cpp own; ?IDENTITY_MATRIX4X3 is the
   identity Matrix4x3 at arm9 0x02082128, which each of these targets' generated
   host-src/romdata.c defines as data_02082128 (copied from
   hal/bob_enemy_bridges.cpp:440, which is where the window targets get this
   exact row -- out/SMOKELINK2/route.md called it NEW because cxx_aliases.cpp
   does not carry it, and it is not new, it is one file over). */
/* hal/cxx_aliases.cpp:3826 */
#pragma comment(linker, "/alternatename:?data_020a4d34@@3HA=__ZN6Memory25isRootHeapIterInitializedE")
/* hal/cxx_aliases.cpp:3825 */
#pragma comment(linker, "/alternatename:?data_020a4d38@@3UNestedHeapIterator@@A=__ZN6Memory16rootHeapIteratorE")
/* hal/cxx_aliases.cpp:2830 */
#pragma comment(linker, "/alternatename:?data_020a60b0@@3IA=_data_020a60b0")
/* hal/cxx_aliases.cpp:3392 */
#pragma comment(linker, "/alternatename:_ROOT_HEAP_ARENA_ID=_data_020a0ea4")   /* 0x020a0ea4, os_arena.cpp */
/* hal/cxx_aliases.cpp:3409 */
#pragma comment(linker, "/alternatename:_GAME_HEAP_PTR=_data_020a0eac_c")   /* 0x020a0eac, heap_globals.cpp */
/* hal/bob_enemy_bridges.cpp:440 */
#pragma comment(linker, "/alternatename:?IDENTITY_MATRIX4X3@@3UMatrix4x3@@A=_data_02082128")

/* THE SECOND-ORDER DATA ROWS.  Every row above was named by the link at
   454383169; these seven were named by the NEXT link, once the source rows
   this batch adds were on the target and their own references resolved.  All
   seven are plain data (a 3 in the decoration), and for data a name bridge IS
   the whole binding: one object, two names.  Five come from
   hal/actor_classes_star.cpp rather than hal/cxx_aliases.cpp, which is simply
   where the window build keeps them, and hal/actor_vtables.cpp -- already on
   these three targets -- is the file that DEFINES all five flat names inside
   its extern "C" block. */
/* hal/actor_classes_star.cpp:303 */
#pragma comment(linker, "/alternatename:?data_0209b468@@3PAXA=_data_0209b468")
/* hal/actor_classes_star.cpp:304 */
#pragma comment(linker, "/alternatename:?data_0209b460@@3PAXA=_data_0209b460")
/* hal/actor_classes_star.cpp:305 */
#pragma comment(linker, "/alternatename:?data_0209b45c@@3PAXA=_data_0209b45c")
/* hal/actor_classes_star.cpp:306 */
#pragma comment(linker, "/alternatename:?data_0208e378@@3FA=_data_0208e378")
/* hal/actor_classes_star.cpp:307 */
#pragma comment(linker, "/alternatename:?data_020a4bb8@@3PAXA=_data_020a4bb8")
/* hal/cxx_aliases.cpp:706 */
#pragma comment(linker, "/alternatename:?data_0209f2d8@@3EA=_data_0209f2d8")
/* hal/cxx_aliases.cpp:846 */
#pragma comment(linker, "/alternatename:?data_020a0c80@@3PAPAXA=_data_020a0c80")

/* THE GLOBAL DEALLOCATOR SEAM, arriving in round 3 with src/_ZdlPv.cpp.  The
   flat __ZdlPv is what port/unmatched/func_02073244_hostcopy.c and the model
   family's host destructors call; src/_ZdlPv.cpp emits ??3@YAXPAX@Z since the
   sync.  Both sides YA, __cdecl, one pointer: no receiver is at stake.  Copied
   verbatim from hal/cxx_aliases.cpp, the same row
   port/hal/smoke_aliases_roots.cpp and every smoke alias file after it carry. */
/* hal/cxx_aliases.cpp:3665 */
#pragma comment(linker, "/alternatename:__ZdlPv=??3@YAXPAX@Z")
