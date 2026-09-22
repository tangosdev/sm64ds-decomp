// port/hal/smoke_aliases_modelanim.cpp -- the __cdecl, static and data half of
// smoke_modelanim's link, as /alternatename rows copied verbatim from
// port/hal/cxx_aliases.cpp.
//
// Lane SMOKELINK3, run link100 wave 10 round 2.
//
// Read port/hal/smoke_aliases_heap.cpp's header for the whole argument and
// port/hal/smoke_aliases_roots.cpp's for the decoration-by-decoration check
// this file's rows were put through too.  In one line: hal/cxx_aliases.cpp
// holds 1328 rows AND real definitions, so putting it on a smoke harness drags
// the window build's scaffolding in behind it, while a file of nothing but
// linker directives costs a target exactly the rows it carries.
//
// WHY AN ALIAS IS HONEST FOR EVERY ROW HERE.  port/faces_sync.txt's header
// states the rule: an /alternatename is a NAME bridge and never an ABI bridge,
// so a row may only be closed this way when the two sides already agree about
// where the receiver and the arguments live.  Each row below was checked by its
// own decoration before it was copied -- SA is a static and __cdecl, YA is a
// free function and __cdecl, 3 is plain data, and an AE-to-AE pair inside one
// class is the non-virtual against the virtual spelling of one body with the
// receiver in ECX on both sides.  Anything else on this target is a face in
// port/faces_sync_smoke_modelanim.txt, not a row here.
//
// Every row is copied CHARACTER FOR CHARACTER from hal/cxx_aliases.cpp at the
// line named beside it, so a grep proves the two files agree and neither is a
// fresh adjudication.  This file is on the smoke targets ONLY, so no row here
// can collide with cxx_aliases.cpp's copy on a window target.

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
/* hal/cxx_aliases.cpp:2830 */
#pragma comment(linker, "/alternatename:?data_020a60b0@@3IA=_data_020a60b0")
/* hal/cxx_aliases.cpp:3392 */
#pragma comment(linker, "/alternatename:_ROOT_HEAP_ARENA_ID=_data_020a0ea4")   /* 0x020a0ea4, os_arena.cpp */
/* hal/cxx_aliases.cpp:3285 */
#pragma comment(linker, "/alternatename:__ZN22ExpandingHeapAllocator10CreateNodeEPN10MemoryNode6TargetEt=?CreateNode@ExpandingHeapAllocator@@SAPAXPAUTarget@MemoryNode@@G@Z")
/* hal/cxx_aliases.cpp:3284 */
#pragma comment(linker, "/alternatename:__ZN22ExpandingHeapAllocator10UnlinkNodeEP10MemoryNodeS1_=?UnlinkNode@ExpandingHeapAllocator@@SAPAXPAUMemoryNode@@0@Z")
/* hal/cxx_aliases.cpp:3286 */
#pragma comment(linker, "/alternatename:__ZN22ExpandingHeapAllocator8LinkNodeEP10MemoryNodeS1_S1_=?LinkNode@ExpandingHeapAllocator@@SAPAXPAUMemoryNode@@00@Z")
/* hal/cxx_aliases.cpp:3245 */
#pragma comment(linker, "/alternatename:__ZN2GX11LoadTexPlttEPKvjj=?LoadTexPltt@GX@@YAXPBXII@Z")
/* hal/cxx_aliases.cpp:3287 */
#pragma comment(linker, "/alternatename:__ZN4cstd3absEi=?abs@cstd@@YAHH@Z")
/* hal/cxx_aliases.cpp:3665 */
#pragma comment(linker, "/alternatename:__ZdlPv=??3@YAXPAX@Z")

/* THE HEAP-TEARDOWN SEAM, the same four rows port/hal/smoke_aliases_roots.cpp
   carries and for the same reason.  This target hosts the same root-heap layer,
   so the same source rows join its link and the same four names come with them.
   Read that file's tail for the full derivation of each; in short:

     __ZN6Memory16operator_delete2EPv   the flat name the window targets get
        from hal/cxxname_bridge.cpp:682 and this target gets from the ROM's own
        matched veneer at 0x0203cbcc, so cxx_aliases.cpp:2936 is used REVERSED.
        Both sides YA, __cdecl, one pointer: no receiver is at stake.
     __ZdlPv                            its next hop, joined to src/_ZdlPv.cpp's
        ??3@YAXPAX@Z exactly as cxx_aliases.cpp:3665 joins them on the window
        targets, so the deallocator that runs is the ROM's and not the CRT's.
     __ZN4cstd3absEi                    ExpandingHeapAllocator::MaxAllocatableSize
        calls it; src/_ZN4cstd3absEi.cpp emits the C++ spelling.  __cdecl both
        sides.
     __ZN18NestedHeapIterator10FindNestedEPv   HeapAllocator::Remove calls it;
        include/NestedHeapIterator.h:26 declares it STATIC, which is why it is a
        row here while its non-static sibling Remove is a face in
        port/faces_sync_smoke_modelanim.txt. */
/* hal/cxx_aliases.cpp:2936, REVERSED: see hal/smoke_aliases_roots.cpp's tail */
#pragma comment(linker, "/alternatename:__ZN6Memory16operator_delete2EPv=?operator_delete2@Memory@@YAXPAX@Z")
/* hal/cxx_aliases.cpp:3299 */
#pragma comment(linker, "/alternatename:__ZN18NestedHeapIterator10FindNestedEPv=?FindNested@NestedHeapIterator@@SAPAU1@PAX@Z")

/* THE MODEL-FAMILY SEAM, the same two rows port/hal/smoke_aliases_model.cpp
   carries and for the same reason.  This target hosts Model and ModelBase, so
   the same source rows join its link and the same two data names come with
   them.  Read that file's tail for each derivation; in short:

     ?IDENTITY_MATRIX4X3@@3UMatrix4x3@@A   the C++ spelling of the identity
        Matrix4x3 at arm9 0x02082128, which this target's generated
        host-src/romdata.c defines on its line 4 as data_02082128 and
        src/_ZN5ModelC1Ev.cpp asks for under its recovered type.  Copied
        verbatim from port/hal/bob_enemy_bridges.cpp:440, which is the file the
        window targets get this exact spelling from.  out/SMOKELINK2/route.md
        classified it NEW because hal/cxx_aliases.cpp does not carry it; it is
        not new, it is one file over.
     _data_0208e87c                        ModelBase's vtable under the port's
        old name, which port/unmatched/ModelBaseModel_Dtors_HostCopy.c's
        ModelBase bodies store into word 0, against config's _ZTV9ModelBase.
        Plain data on both sides: one table, two names.
   Both are data rows, so a name bridge IS an ABI bridge here. */
/* hal/bob_enemy_bridges.cpp:440 */
#pragma comment(linker, "/alternatename:?IDENTITY_MATRIX4X3@@3UMatrix4x3@@A=_data_02082128")
/* hal/cxx_aliases.cpp:3684 */
#pragma comment(linker, "/alternatename:_data_0208e87c=__ZTV9ModelBase")
/* cstd::abs is not repeated here: the verbatim block above already
   carries hal/cxx_aliases.cpp:3287, which is the same row. */
