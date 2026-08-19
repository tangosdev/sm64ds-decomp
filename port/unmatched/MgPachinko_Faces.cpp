// PORT_HOST_ABI. dScMgPachinko_c's two NAME-SPELLING FACES. Actor id 0x170,
// scene 368. Run mg5, lane PCH.
//
// Neither is a body and neither changes a calling convention. Both are the
// ordinary /alternatename kind: a reference spelled one way and a definition
// that exists under another name, in the same link. They live in this lane's
// own file rather than in hal/scene_mg_faces.cpp so the seat is additive.
//
// THE LHS OF EACH IS UNDEFINED EVERYWHERE ELSE IN THE BUILD, which is the
// condition port/tools/alternatename_guard.py watches: a defined LHS defeats
// the alias silently. Neither needs a row in tools/alternatename_baseline.txt,
// because that file is the allowlist of DELIBERATELY defeated pairs and these
// two are not defeated.
//
// ---- 1. THE CLASS VTABLE SYMBOL ------------------------------------------
//
// src/MgBobOmbSquad_Spawn.c ends with `p[0] = (int)_ZTV15dScMgPachinko_c;` and
// nothing in this tree defines that symbol -- the table is mounted ov006 data
// under its address name, data_ov006_0213d9cc. This is the same shape
// hal/scene_mg_faces.cpp:231 already carries for curling
// (__ZTV14dScMgCurling_c = _data_ov006_0213c304), and it is a REAL identity
// claim rather than bookkeeping: the ROM's factory at 0x020ff3ec loads
// 0x0213d9cc out of its own literal pool, and the RTTI at 0x0213d960 reads
// "15dScMgPachinko_c", so the mangled name and the address are tied together
// by the ROM twice over.
//
// It is NOT a per-TU placeholder guess of the VT0/VT1/VT2 kind. The factory
// spells the Itanium vtable name in full, and the fill has already replaced
// every one of that table's thirty-six words with a host thunk by the time any
// object of this class exists.
//
// ---- 2. AN arm9 CALLEE SPELLED BY ADDRESS --------------------------------
//
// src/func_ov006_020fb97c.c declares and calls
//
//     extern void func_0205a448(unsigned short val, void* dst, int nbytes);
//     func_0205a448(sp, (void*)r, 0x6000);
//
// and no symbol of that name exists anywhere. config/arm9/symbols.txt:2504
// names 0x0205a448 MultiStore16, config/arm9/delinks.txt gives it the block
// `.text start:0x0205a448 end:0x0205a460`, src/MultiStore16.c is its TU, that
// TU is already in port/slice_gate10.txt, and _MultiStore16 is already in the
// walk_window map at 0001:00072ed0. So the body is in the link and only the
// NAME is missing.
//
// This is port/mg_fanout_costs.txt section 6's fourth defect, the one it calls
// "a name-spelling face, the ordinary kind" -- there recorded for
// src/func_ov006_020e3578.c spelling ov004's func_ov004_020adc74 as bare
// func_020adc74. It is a decomp-side spelling question and is ROUTED, not
// taken: the fix in the byte-gated tree is one token in one src TU, and
// whether that still builds byte-identically under mwccarm is that tree's
// question rather than this port's.
//
// The three arguments agree on both sides -- MultiStore16 is a 0x18-byte
// halfword store loop taking (value, dst, bytes) -- so an alias is correct
// here and no argument-landing face is needed.

#pragma comment(linker, "/alternatename:__ZTV15dScMgPachinko_c=_data_ov006_0213d9cc")
#pragma comment(linker, "/alternatename:_func_0205a448=_MultiStore16")

// ---- 3. Run mg5, lane INTEG: THE SLOT-0 InitResources CDECL FACE -----------
//
// dScMgPachinko_c::InitResources (vtable slot 0, the black-screen fix) was
// recovered on origin/main only as the __thiscall C++ member
// src/_ZN15dScMgPachinko_c13InitResourcesEv.cpp. hal/scene_mg.cpp's slot-0
// dispatch (pch_init) calls the ROM address name func_ov006_020fefc0 as a plain
// cdecl (void*). An alias cannot bridge cdecl to __thiscall -- the port's
// documented wall -- so this is the method_faces.cpp shape instead: a cdecl
// forwarder carrying the flat Itanium name, doing the qualified (non-virtual)
// call so it reaches THIS body rather than dispatching through the object's ROM
// vtable, and an alias pointing the ROM address name onto that forwarder. The
// trap for func_ov006_020fefc0 is removed from MgPachinko_Traps.cpp so the LHS
// is undefined for the alias to bind.
#include "dScMgPachinko_c.h"
extern "C" int _ZN15dScMgPachinko_c13InitResourcesEv(void *self)
{ return ((dScMgPachinko_c *)self)->dScMgPachinko_c::InitResources(); }

#pragma comment(linker, "/alternatename:_func_ov006_020fefc0=__ZN15dScMgPachinko_c13InitResourcesEv")

/* MSVC will not emit an object for a translation unit that defines nothing,
   and a dropped object takes its linker directives with it. */
extern "C" int port_mg_pachinko_faces_anchor(void) { return 0x170; }
