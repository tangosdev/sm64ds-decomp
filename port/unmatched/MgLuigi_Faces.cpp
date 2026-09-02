// PORT_HOST_ABI. dScMgLuigi_c's own faces and its one unmatched-body trap.
// Run mg5, lane LUI. Actor id 0x16e = scene 366, "Wanted!".
//
// SEVEN ROWS, ALL OF THEM NAMED BY WAVE 1 OF THIS LANE'S LINK and none of them
// generated. port/tools/facegen.py is the generator hal/scene_mg_faces_gen.cpp
// comes from; it is not used here because six of the seven are one-line
// name-spelling rows whose target this lane read out of the ROM anyway, and
// the seventh is a trap rather than a face. The wall for THIS class is its
// six pointer-to-member dispatchers, and those are host copies in
// unmatched/MgLuigi_StateDispatch.cpp rather than aliases -- section 4's
// rulebook corollary, and the reason facegen's PAU blind spot (tool finding 1)
// cannot bite this lane: nothing here aliases a pair table.
//
// THIS FILE LIVES IN unmatched/ RATHER THAN hal/ because run mg5 has five
// lanes editing hal/scene_mg_faces.cpp's neighbourhood at once, and a new file
// under this lane's own MgLuigi_* name concatenates where an edit conflicts.
//
// EVERY LHS BELOW IS UNDEFINED EVERYWHERE ELSE IN THE BUILD, which is the
// condition port/tools/alternatename_guard.py checks post-link: a defined LHS
// defeats its alias silently. The guard runs on every build and covers these.

#include <cstdio>

/* ---- 1. the class vtable's Itanium name ----------------------------------
 *
 * src/MgWanted_Spawn.c writes `p[0] = (int)_ZTV12dScMgLuigi_c;`. That is the
 * ROM's own class name -- the RTTI string at 0x0213ce60 reads
 * "12dScMgLuigi_c" -- and it is not a config symbol name, so it needs a face
 * onto the mounted table. The address is settled by the ROM twice over:
 *   config/arm9/overlays/ov006/relocs.txt
 *   from:0x020f3830 kind:load to:0x0213cf10 module:overlay(6)
 * and 0x020f3830 is inside MgWanted_Spawn (0x020f3800, 0x34 bytes); and slot
 * 17's own literal pool at 0x020efc60 stores the same word into the object.
 * This is exactly the shape hal/scene_mg_faces.cpp section 2 carries for
 * _ZTV14dScMgCurling_c. */
#pragma comment(linker, "/alternatename:__ZTV12dScMgLuigi_c=_data_ov006_0213cf10")

/* ---- 2. three ov004 DATA symbols spelled with a func_ prefix -------------
 *
 * The decomp names these three by address with a `func_` stem even though all
 * three are data, and no config anywhere defines that spelling. Each target
 * below is read off the referencing body's OWN literal pool rather than
 * guessed from the name:
 *
 *   func_020beb68  src/func_ov006_020f3294.c (slot 18) and
 *                  src/func_ov006_020f0274.c. Pool at 0x020f33b4 = 020BEB68.
 *                  This is the symbol port/mg_fanout_costs.txt section 4 calls
 *                  "the one symbol in the set with no LNK2019 at all"; here it
 *                  emits one, because this lane's first referencing object is
 *                  a different one.
 *   func_020bc864  src/func_ov006_020f3460.c (slot 0). Pool at 0x020f37fc.
 *   func_020bc888  src/func_ov006_020f3460.c (slot 0). Pool at 0x020f37f8.
 *
 * ov004's config names all three as data_ov004_* at those addresses, and the
 * ov006 relocations that reach them say module overlays(3,4) or overlay(4),
 * with ov004 the co-resident answer on the minigame path. */
#pragma comment(linker, "/alternatename:_func_020beb68=_data_ov004_020beb68")
#pragma comment(linker, "/alternatename:_func_020bc864=_data_ov004_020bc864")
#pragma comment(linker, "/alternatename:_func_020bc888=_data_ov004_020bc888")

/* ---- 3. two IRQ methods declared at C++ linkage --------------------------
 *
 * src/func_ov006_020f00a4.cpp declares
 *     namespace IRQ { void SetIRQHandler(unsigned int, IRQHandler);
 *                     void EnableIRQs(unsigned int); }
 * OUTSIDE its extern "C" block, so MSVC mangles both and the Itanium names the
 * build defines do not satisfy them. Both targets are __cdecl free functions
 * in namespace IRQ with the same arguments, so a plain alias is correct and no
 * argument-landing face is needed -- this is the case facegen's tool finding 4
 * records it misclassifies as a reverse face.
 *
 * BOTH TARGETS ARE ntr/runtime.cpp's, not src/'s, and that is deliberate on
 * the port's part rather than an accident this lane should route around: the
 * baseline map defines __ZN3IRQ13SetIRQHandlerEjPFvvE and
 * __ZN3IRQ10EnableIRQsEj in ntr_2x:runtime.cpp.obj, which is where the host
 * models the DS interrupt controller. src/_ZN3IRQ10EnableIRQsEj.c exists and
 * is not in the link.
 *
 * THE NAME LENGTHS DIFFER AND THAT IS THE ITANIUM ENCODING, not a typo:
 * "SetIRQHandler" is 13 characters and "EnableIRQs" is 10.
 *
 * NOTE the sibling that needs NO face: src/func_ov006_020efc68.c (slot 5)
 * calls _ZN3IRQ13SetIRQHandlerEjPFvvE by its Itanium name inside extern "C",
 * so it resolves directly. The same function, two spellings, one of which
 * costs a row. */
#pragma comment(linker, "/alternatename:?SetIRQHandler@IRQ@@YAXIP6AXXZ@Z=__ZN3IRQ13SetIRQHandlerEjPFvvE")
#pragma comment(linker, "/alternatename:?EnableIRQs@IRQ@@YAXI@Z=__ZN3IRQ10EnableIRQsEj")

/* ---- 4. ONE ov004 BODY IN THE CLOSURE HAS NO SOURCE ----------------------
 *
 * func_ov004_020b0e84, 0x66c bytes. config/arm9/overlays/ov004/symbols.txt
 * names it, and config/arm9/overlays/ov004/delinks.txt does NOT cover it --
 * the blocks run 0x020b0de0..0x020b0e84 and then jump to 0x020b14f0 -- and no
 * src file defines it. A name-shaped check would call it decompiled; the
 * delink join is what says otherwise.
 *
 * IT IS ON THIS CLASS'S RENDER PATH, and that is the honest cost of the trap
 * rather than a footnote. dScMgLuigi_c slot 9 (func_ov006_020f33c0) opens with
 * func_ov004_020b1e34(c, 0xe0, 0x14, 1), and src/func_ov004_020b1e34.c is a
 * two-line veneer -- `ldr r1,[r0,#0xb4]; b func_ov004_020b0e84` in the ROM --
 * whose whole body is a call to this. So the first thing this class's Render
 * does is enter a trap.
 *
 * AND THE VENEER LOSES TWO ARGUMENTS ON THE HOST, which is recorded here
 * because it is invisible to the byte gate and would be a real defect the day
 * a body lands. The ROM sets r1, r2 and r3 at the call site (mov r1,#0xe0; mov
 * r2,#0x14; mov r3,#1 at 0x020f33c8..0x020f33d0), the veneer overwrites r1 and
 * rides r2 and r3 through, and src/func_ov004_020b1e34.c declares one
 * parameter and calls func_ov004_020b0e84(a, a->b4) with two. So 0xe0, 0x14
 * and 1 reach the ROM's body and do not reach the host's. Nothing in the tree
 * diagnoses that. This lane does not repair it: src/ is read-only here and the
 * callee is a trap, so there is nothing yet for the arguments to be wrong AT.
 * The repair is a PORT_HOST_ABI host copy of the veneer, and it belongs to
 * whoever decompiles 0x020b0e84.
 *
 * The trap takes the two parameters its matched caller declares and passes, so
 * the call site's stack is right whatever happens inside. */

/* Run mg5, lane INTEG: func_ov004_020b0e84 (the "Wanted!" score HUD, 0x66c) was
   copied across from origin/main as src/func_ov004_020b0e84.cpp with its Obj
   vtable header include/private/ov004_obj_vtbl.h, and is now in
   port/slice_lui.txt, so the trap that stood in on dScMgLuigi_c's Render path is
   gone and the real body runs. The veneer func_ov004_020b1e34 still drops the
   0xe0/0x14/1 arguments the ROM rides through r2/r3 (see section 4 above); that
   is a separate PORT_HOST_ABI veneer repair, not this seat. The counter is kept
   because hal/scene_mg.cpp prints port_mg_luigi_ov004_trap_hits(); it now stays
   0, the honest report that the score HUD is no longer stubbed. */

/* ---- 4a. THE DROPPED ARGUMENTS ARE CLOSED: ALL THREE ARE DEAD -------------
 *
 * Run mg5, lane WTIMER. Section 4 above says the veneer's dropped 0xe0/0x14/1
 * "would be a real defect the day a body lands", and the body landed with the
 * INTEG seat, so the day arrived and this is the adjudication. The answer is
 * that all three are dead, so the host veneer's arity costs nothing and the
 * PORT_HOST_ABI repair section 4 reserves is NOT needed.
 *
 * WHY IT WAS WORTH CHECKING RATHER THAN ASSUMING. This project has found six
 * separate cases of the ROM riding a value through a register a C prototype
 * drops, and one of them -- unmatched/MgHud_HighScore.cpp -- is on this exact
 * HUD path and put 999999 on this game family's high score. An earlier ruling
 * covering two of the registers is not a ruling about the third.
 *
 * THE EVIDENCE, read off a capstone listing of extracted/overlays/
 * overlay_0004.bin at base 0x020AD660 (the same base and the same file the
 * sibling seat in unmatched/MgHud_ScaledNumber.cpp proves by prologue scan):
 *
 *   r1 = 0xe0 never reaches the callee IN THE ROM EITHER. The veneer's own
 *   second instruction is `ldr r1,[r0,#0xb4]` at 0x020b1e38, which overwrites
 *   it before the `bx ip` tail call. The host veneer passing a->b4 as the
 *   second argument is therefore not a drop at all; it is the same value.
 *
 *   r2 = 0x14 is clobbered before it is read. func_ov004_020b0e84's fourth
 *   instruction, `ldr r2,[sl,#0x50]` at 0x020b0e90, writes r2, and nothing
 *   between the prologue and it reads r2.
 *
 *   r3 = 1 is never read at all. Over the whole 0x66c body the first
 *   instruction that touches r3 is `smull r3, r0, r2, r1` at 0x020b0f54, and
 *   that is a WRITE. There is no read of r3 before it on any path.
 *
 *   AND THERE ARE NO STACK ARGUMENTS EITHER, which is the check that closes
 *   the gap a register-only sweep would leave: the body contains no
 *   sp-relative load at or above the incoming-argument boundary (sp+0x30,
 *   after `push {r4-r8,sb,sl,fp,lr}` and `sub sp,sp,#0xc`). Its only
 *   sp-relative traffic is its own locals.
 *
 * SO THE ROM PASSES THREE VALUES ITS OWN CALLEE IGNORES, and the host dropping
 * them is faithful by accident rather than by design. Recorded rather than
 * deleted, because "we checked and it is dead" and "nobody has looked" are
 * different states and only one of them is finished. */
static unsigned g_luigi_ov004_trap;

extern "C" unsigned port_mg_luigi_ov004_trap_hits(void)
{
    return g_luigi_ov004_trap;
}

/* ---- 5. THE IRQ 2 HANDLER NAMES AN MMIO REGISTER AS A C SYMBOL -----------
 *
 * src/func_ov006_020efcf8.c is dScMgLuigi_c's HBlank handler -- the one slot 5
 * tears down and src/func_ov006_020f00a4.cpp installs -- and it declares
 *
 *     extern volatile u16 data_04000006;
 *
 * which is VCOUNT. The port maps the DS address space one-to-one in the host
 * process (port/ntr/io.cpp VirtualAllocs it), so every other MMIO access in
 * the tree is a DEREF of a literal address and works unchanged; port/ntr's
 * README describes hostgen rewriting exactly that shape into NTR_MMIO. A
 * NAMED SYMBOL is the one form that cannot work: the linker has no way to
 * place a C object at an absolute address, and defining one would silently
 * send the read to host storage that no scanline ever updates.
 *
 * data_04000006 IS THE ONLY OCCURRENCE OF THAT SPELLING IN THE WHOLE TREE, so
 * this is a one-off in the decomp rather than a convention the port should
 * grow a mechanism for. The same TU derefs 0x4000004 (DISPSTAT) literally two
 * lines later, which is what makes it a slip rather than a style.
 *
 * SO THE TU IS HOST-COPIED HERE, verbatim but for that one read, and its src
 * line is commented out of port/slice_lui.txt. Nothing else changes: the
 * 0x4000004 test, the 0x4000040 source address, the >= 0xc0 / <= 0xc0 pair of
 * comparisons (which the ROM really does write that way) and the
 * data_023c0000 + 0x3ff8 bit set are the src's.
 *
 * WHAT IT DOES, so a reader knows what is at stake if it is wrong: on each
 * HBlank it captures the four window registers at 0x04000040 into row v of
 * data_0209f648, which is the per-scanline window table the "Wanted!" screen
 * uses. port/irq2_map.txt is where the host's HBlank edge comes from.
 */
extern "C" {
extern char data_023c0000[];
extern int data_0209f608;
extern int data_0209f648[][192];
void func_ov006_020efdac(void);
void MultiCopy_Int(int *dst, int *src, int len);
}

// PORT_HOST_ABI: src names the MMIO register VCOUNT as a C symbol (data_04000006) the linker cannot place at its absolute address; host copy derefs the literal address 0x4000006 like every other MMIO access
extern "C" void func_ov006_020efcf8(void)
{
    int v;
    *(int *)(((int)(size_t)data_023c0000 + 0x3ff8)) |= 2;
    v = *(volatile unsigned short *)0x4000006 + 1;
    if (v >= 0xc0) {
        func_ov006_020efdac();
    } else if (v <= 0xc0) {
        if (*(volatile unsigned short *)0x4000004 & 2) {
            MultiCopy_Int(&data_0209f648[data_0209f608][v],
                          (int *)0x4000040, 4);
        }
    }
}
