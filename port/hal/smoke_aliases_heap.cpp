// port/hal/smoke_aliases_heap.cpp -- the __cdecl half of the gate-2 smoke's
// link, as /alternatename rows copied verbatim from port/hal/cxx_aliases.cpp.
//
// Lane SMOKELINK2, run link100 wave 10 round 2.
//
// WHY THIS FILE EXISTS.  hal/cxx_aliases.cpp holds 1328 alias rows and the
// whole port's C-linkage adjudication, and the smoke harnesses do not link it:
// it also carries real definitions (the DSSTATE block, MultiCopyHalf, the
// matrix builders, the ppu audit), so putting it on a 26-object harness would
// drag the window build's scaffolding in behind it.  What the harness needs is
// the five rows below and nothing else.  A file of nothing but linker
// directives compiles to an object with no code, no data and no external
// reference, so it costs a target exactly the rows it carries.
//
// WHY AN ALIAS IS HONEST FOR THESE FIVE AND FOR NOTHING ELSE HERE.
// port/faces_sync.txt's header states the rule: an /alternatename is a NAME
// bridge and never an ABI bridge, so a row whose right hand side is a
// non-static member cannot be closed with one -- the alias links, the receiver
// is garbage and nothing says so.  Every right hand side below is either
// __cdecl or data:
//
//   ?UnlinkNode@ExpandingHeapAllocator@@SAPAXPAUMemoryNode@@0@Z   SA = static
//   ?CreateNode@ExpandingHeapAllocator@@SAPAXPAUTarget@MemoryNode@@G@Z  SA
//   ?LinkNode@ExpandingHeapAllocator@@SAPAXPAUMemoryNode@@00@Z    SA = static
//   __ZN6Memory16rootHeapIteratorE                                data
//   __ZN6Memory25isRootHeapIterInitializedE                       data
//
// include/ExpandingHeapAllocator.h:89-91 declares the three as static and says
// why ("STATIC.  Two declared parameters, two body arguments, no room for a
// `this`"), and src/_ZN22ExpandingHeapAllocator10UnlinkNodeEP10MemoryNodeS1_.cpp
// repeats the measurement at its own ROM address.  Caller and callee are both
// __cdecl, so the name hop is the whole difference.
//
// The two data rows are the same C-linkage flip one level down:
// port/hal/heap_globals.cpp owns the single real storage at 0x020a4d34 and
// 0x020a4d38 and already publishes three spellings of it (its lines 43 to 45);
// src/func_0204df54.cpp asks for two more, the ones a C++ TU produces when it
// declares the globals with their recovered types.  The storage is unchanged;
// these are two more names for the same bytes.
//
// EVERY ROW IS COPIED CHARACTER FOR CHARACTER from hal/cxx_aliases.cpp, at the
// lines named beside it, so a grep proves the two files agree and neither is a
// fresh adjudication.  This file is on the smoke targets ONLY, so no row here
// can collide with the copy in cxx_aliases.cpp on a window target.

/* hal/cxx_aliases.cpp:3284 */
#pragma comment(linker, "/alternatename:__ZN22ExpandingHeapAllocator10UnlinkNodeEP10MemoryNodeS1_=?UnlinkNode@ExpandingHeapAllocator@@SAPAXPAUMemoryNode@@0@Z")
/* hal/cxx_aliases.cpp:3285 */
#pragma comment(linker, "/alternatename:__ZN22ExpandingHeapAllocator10CreateNodeEPN10MemoryNode6TargetEt=?CreateNode@ExpandingHeapAllocator@@SAPAXPAUTarget@MemoryNode@@G@Z")
/* hal/cxx_aliases.cpp:3286 */
#pragma comment(linker, "/alternatename:__ZN22ExpandingHeapAllocator8LinkNodeEP10MemoryNodeS1_S1_=?LinkNode@ExpandingHeapAllocator@@SAPAXPAUMemoryNode@@00@Z")
/* hal/cxx_aliases.cpp:3825 */
#pragma comment(linker, "/alternatename:?data_020a4d38@@3UNestedHeapIterator@@A=__ZN6Memory16rootHeapIteratorE")
/* hal/cxx_aliases.cpp:3826 */
#pragma comment(linker, "/alternatename:?data_020a4d34@@3HA=__ZN6Memory25isRootHeapIterInitializedE")
