/* THREE HAND-RULED ALIAS ROWS for dScMgSmartball_c. Run mg5, lane SMB.
 *
 * All three were put to port/tools/facegen.py first and it REFUSED all three,
 * which is the tool behaving correctly rather than a gap. Its refusals are
 * quoted verbatim below beside the ruling that answers each, so a reader can
 * check the ruling instead of trusting it -- the same shape
 * hal/scene_mg_faces.cpp section 2 uses.
 *
 * ---------------------------------------------------------------------------
 * 1. Memory::Deallocate, wanted by func_ov006_0210d7e0, vtable slot 17.
 *
 *      unresolved external symbol
 *      "void __cdecl Memory::Deallocate(void *,struct Heap *)"
 *      (?Deallocate@Memory@@YAXPAXPAUHeap@@@Z)
 *      referenced in function _func_ov006_0210d7e0
 *
 *    facegen: "free function at NAMESPACE scope (Memory::Deallocate, the @@Y
 *    spelling): not a method and not face material, and ALIAS_FN joins
 *    global-scope names only -- a namespace member joined onto a bare C name
 *    could land on a different function; rule it by hand"
 *
 *    THE REFUSAL IS RIGHT AND THE ANSWER IS A PLAIN ALIAS, which is the exact
 *    situation port/mg_fanout_costs.txt section 10's fourth tool finding
 *    records for ?Allocate@Memory@@YAPAXIH@Z: "no face is needed, both sides
 *    are __cdecl with the same two arguments, and a plain alias is correct".
 *    The MSVC mangle says @@YAX -- Y for a free function, A for __cdecl, X for
 *    void return -- over (PAX, PAUHeap) = (void *, Heap *). The Itanium
 *    spelling _ZN6Memory10DeallocateEPvP4Heap is the same function with the
 *    same two pointer arguments, it is DEFINED in this binary out of
 *    src/, and eleven port/hal files already call it under that name. Nothing
 *    is being converted: an alias here changes a name and not a convention.
 *
 *    WHY THE SYMBOL APPEARS AT ALL. src/func_ov006_0210d7e0.cpp is one of the
 *    ".cpp with a recovered header" bodies and it declares the callee as real
 *    C++ -- `namespace Memory { void Deallocate(void*, Heap*); }` -- so MSVC
 *    emits the namespace-qualified mangle where every C-spelled caller emits
 *    the Itanium name. dScMgCurling_c's own D0 does not hit this because
 *    src/func_ov006_020e065c spells the same call as a C name.
 *
 * ---------------------------------------------------------------------------
 * 2 and 3. Two ov004 DATA words spelled with func_ names, wanted by
 *          func_ov006_02118488, vtable slot 6, Behavior.
 *
 *      unresolved external symbol _func_020bc864 referenced in
 *      function _func_ov006_02118488
 *      unresolved external symbol _func_020bc888 referenced in
 *      function _func_ov006_02118488
 *
 *    facegen: "plain C name: a face cannot supply it, host or mount it"
 *
 *    THE REFUSAL IS RIGHT AND THE ANSWER IS "MOUNT IT", not "host it", and the
 *    difference is the whole ruling. These are not missing storage. They are
 *    the SAME name-spelling defect port/mg_fanout_costs.txt section 6 records
 *    as its fourth, smaller item -- "src/func_ov006_020e3578.c spells ov004's
 *    func_ov004_020adc74 as bare func_020adc74, a name that exists in no
 *    config" -- in its data form:
 *
 *      include/decl_common.h:1357   extern int func_020bc864;
 *      include/decl_common.h:1362   extern int func_020bc888;
 *      src/func_ov006_02118488.c:161  func_020bc888 = 0x80;
 *      src/func_ov006_02118488.c:162  func_020bc864 = -0x30;
 *
 *    Both are DATA and both live in ov004: config/arm9/overlays/ov004/
 *    symbols.txt names them data_ov004_020bc864 and data_ov004_020bc888, and
 *    nothing anywhere in the ROM has a FUNCTION at either address. The ROM
 *    agrees word for word -- at 0x0211899c the body does
 *
 *        0211899c  ldr r1, [pc, #0xd8]   ; =0x020bc888
 *        021189a0  mov r3, #0x80
 *        021189a4  ldr r0, [pc, #0xd4]   ; =0x020bc864
 *        021189a8  mvn r2, #0x2f          ; -0x30
 *        021189ac  str r3, [r1]
 *        021189b0  str r2, [r0]
 *
 *    which is two stores through two pooled addresses, exactly what an
 *    `extern int` assignment compiles to.
 *
 *    SO THE ALIAS IS ONTO THE MOUNT AND NOT ONTO NEW STORAGE, which is what
 *    makes it correct rather than merely linkable. port/hal's generated ov004
 *    mount already defines _data_ov004_020bc864 and _data_ov004_020bc888 out
 *    of the overlay image (ov004_syms.c.obj in the map), and other ov004 code
 *    in this binary reads the same two words under those names. A hosted
 *    stand-in would have given this one TU a private copy that the rest of
 *    ov004 could not see, which is the failure the section-6 note is about.
 *
 *    WHAT THE TWO WORDS ARE, for the record: they are written only inside the
 *    branch that ends the game -- state 1, after the 300-frame wait, when the
 *    two data_020a0de8/data_020a0de9 flags are both set and the actor's own
 *    parameter byte at self+8 is zero -- immediately before
 *    func_ov004_020b0a54(0x10) and the move to state 2. This lane does not
 *    claim to know what they mean; it claims they are ov004's and not this
 *    TU's, which is all an alias needs.
 *
 *    THE DECOMP-SIDE FIX IS ROUTED, NOT TAKEN: include/decl_common.h should
 *    spell both as data_ov004_* and src/func_ov006_02118488.c should follow.
 *    That is a byte-gated-tree change and this file leaves src/ and include/
 *    alone.
 */

/* 1. */
#pragma comment(linker, "/alternatename:?Deallocate@Memory@@YAXPAXPAUHeap@@@Z=__ZN6Memory10DeallocateEPvP4Heap")

/* 2 and 3. */
#pragma comment(linker, "/alternatename:_func_020bc864=_data_ov004_020bc864")
#pragma comment(linker, "/alternatename:_func_020bc888=_data_ov004_020bc888")

/* 4. Run mg5, lane INTEG. dScMgSmartball_c::InitResources (vtable slot 0) was
 * recovered on branch decomp/smb-bodies as the plain-C body
 * src/_ZN16dScMgSmartball_c13InitResourcesEv.c (NONMATCHING, the flagged weak
 * point). hal/scene_mg.cpp's slot-0 dispatch calls it by its ROM address name
 * func_ov006_02118b70, so the flat name is aliased onto the mangled one. Both
 * sides are __cdecl taking one pointer and returning s32 -- exactly the
 * situation row 1 and section 10's ?Allocate finding describe -- so this
 * changes a name and not a convention. The func_ov006_02118b70 trap is removed
 * from MgSmartball_Traps.cpp so the flat name is undefined for the alias to
 * bind. The Render body at 0x021173c8 stays trapped: its only recovered form on
 * main is the __thiscall member dScMgSmartball_c::Render, and an alias cannot
 * cross the calling convention the way this cdecl-to-cdecl one can. */
#pragma comment(linker, "/alternatename:_func_ov006_02118b70=__ZN16dScMgSmartball_c13InitResourcesEv")

/* ---- 5. Run mg5, lane SMBSEAT: TWO CDECL FORWARDERS ----------------------
 *
 * Rows 1 to 4 above are all the ordinary /alternatename kind: a reference
 * spelled one way and a definition under another name, same convention on both
 * sides. These two are not. Each stands a __thiscall C++ MEMBER behind a flat
 * cdecl name the dispatch already calls, which an alias cannot do -- the port's
 * documented wall, and the reason row 4's note said Render "stays trapped".
 *
 * THE SHAPE IS port/unmatched/MgPachinko_Faces.cpp SECTION 3, VERBATIM: a
 * cdecl forwarder carrying the flat Itanium name, making the QUALIFIED
 * (non-virtual) call so it reaches this body rather than dispatching through
 * the object's own table, and an /alternatename row pointing the ROM address
 * name onto the forwarder. Both LHS names are undefined everywhere else in the
 * build -- the Render trap is removed from MgSmartball_Traps.cpp and
 * func_ov006_0210f564 never had a body here -- so neither alias is defeated and
 * neither needs a row in tools/alternatename_baseline.txt.
 *
 * ---- 5a. dScMgSmartball_c::Render, vtable slot 9, 0x021173c8 -------------
 *
 * src/_ZN16dScMgSmartball_c6RenderEv.cpp is main's MATCHED body, brought
 * across by address. It is NOT given a delink block in this tree and that is
 * deliberate rather than an oversight: it includes include/dScMgSmartball_c.h,
 * which derives from dScMgBase_c, and this tree's dScMgBase_c is the old flat
 * POD recovery while main's is the migrated dScene_c child. Compiled here with
 * mwccarm 2004/b56 the header's own `sizeof == 0x629c` assert fails
 * ("illegal constant expression"), so a delink block would be a claim this
 * tree cannot honour and the byte gate would reject it the moment it ran.
 * Migrating dScMgBase_c is a byte-gated-tree job and is ROUTED, not taken.
 * The port reaches the body through the host-ABI declaration in
 * MgSmartball_HostAbi.h, which changes no offset the body actually uses --
 * every field it touches is a raw offset off `(char *)this`.
 *
 * ---- 5b. cMgSmartball_board_c::SaveSnapshot, 0x0210f564 ------------------
 *
 * The one sub-object slot with no body in this tree at all: a 0x3b0-byte hole
 * between `.text start:0x0210ef48 end:0x0210f564` and
 * `.text start:0x0210f914 end:0x0210f998`. Brought across from origin/main by
 * address and RE-VERIFIED IN THIS WORKTREE rather than taken on main's word:
 * tools/match.py at mwccarm 2004/b56 with strict relocs, against
 * extracted/overlays/overlay_0006.bin at base 0x020bfec0, reports MATCH for
 * 0x0210f564 size 0x3b0. Its delink block is therefore added to
 * config/arm9/overlays/ov006/delinks.txt -- unlike 5a, this one this tree can
 * honour, because cMgSmartball_object_c is a self-contained root that needs
 * nothing but types.h.
 *
 * ---- 5c. AND ONE MORE PLAIN NAME-SPELLING ALIAS --------------------------
 *
 * 5b's body declares `namespace Sound { void PlayBank2_2D(u32 id); }`, so MSVC
 * emits ?PlayBank2_2D@Sound@@YAXI@Z where every C-spelled caller in this binary
 * emits the Itanium name _ZN5Sound12PlayBank2_2DEj. Identical to row 1's
 * Memory::Deallocate case in every respect that matters: @@YAX is a free
 * function, __cdecl, void return, over one unsigned int; the Itanium spelling
 * is the same function with the same argument and is defined in this binary out
 * of src/. An alias here changes a name and not a convention.
 */
#include "MgSmartball_HostAbi.h"
#include "cMgSmartball_board_c.h"

extern "C" s32 _ZN16dScMgSmartball_c6RenderEv(void *self)
{ return ((dScMgSmartball_c *)self)->dScMgSmartball_c::Render(); }

extern "C" void _ZN20cMgSmartball_board_c12SaveSnapshotEv(void *self)
{ ((cMgSmartball_board_c *)self)->cMgSmartball_board_c::SaveSnapshot(); }

#pragma comment(linker, "/alternatename:_func_ov006_021173c8=__ZN16dScMgSmartball_c6RenderEv")
#pragma comment(linker, "/alternatename:_func_ov006_0210f564=__ZN20cMgSmartball_board_c12SaveSnapshotEv")
#pragma comment(linker, "/alternatename:?PlayBank2_2D@Sound@@YAXI@Z=__ZN5Sound12PlayBank2_2DEj")
