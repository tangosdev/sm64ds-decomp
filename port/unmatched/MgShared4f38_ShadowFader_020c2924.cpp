/* HOST COPY of src/func_ov006_020c2924.cpp -- the +0x4f38 sub-object's
 * "re-seat the animation and clear the offsets" call, host-copied ONCE for both
 * classes that mount it.
 *
 * Run mg9, adjudicated at the merge. Lane BOX (dScMgSound_c, actor 0x16f,
 * scene 367) found and repaired the defect; lane CUP (dScMgCup_c, actor 0x169,
 * scene 361) kept the DEFECTIVE src TU live on port/slice_cup.txt because
 * nothing in that lane's instruments could see it. THE RULING IS THAT THE src
 * TU IS DEFECTIVE ON THE HOST FOR BOTH CLASSES, so this host copy is the
 * single definition and both slices drop the src line. Attribution and the
 * behaviour-delta this creates for scene 361 are in
 * runs/mg9/out/ADJUDICATE/canonical.md.
 *
 * THE BODY IS SHARED, AND THAT IS A ROM FACT. relocs.txt carries EXACTLY TWO
 * arm_calls to 0x020c2924, one per class, and both are once-per-boot slots:
 *
 *     from:0x020e0018  inside func_ov006_020dfeec   dScMgCup_c   slot 18
 *                                                   (the state reset)
 *     from:0x0211c5a4  inside func_ov006_0211c478   dScMgSound_c slot 0
 *                                                   (InitResources)
 *
 * ONE DEFECT, AND IT IS A RECEIVER DROP AT A HOST C++ VIRTUAL. It is the third
 * distinct shape of the same disease dScMgSound_c carries -- the other two are
 * port/unmatched/MgSound_ShadowSlot35.cpp (a raw vtable-word read typed
 * `int (*)(void)`) and port/unmatched/MgShared4f38_ModelRender_020c29dc.cpp
 * (a shadow class at the wrong host slot).
 *
 * ---- WHAT src DOES AND WHAT THE ROM DOES ---------------------------------
 *
 * src/func_ov006_020c2924.cpp:
 *
 *     void* obj = data_0209f5bc;
 *     int (**vt)(void*) = *(int(***)(void*))obj;
 *     if (vt[6](obj) == 0) return;
 *
 * The ROM at 0x020c2924, read out of extracted/overlays/overlay_0006.bin at
 * base 0x020bfec0 and re-read first-hand at adjudication:
 *
 *     020c2924  e92d4010  push  {r4, lr}
 *     020c2928  e1a04000  mov   r4, r0
 *     020c292c  ebffffc5  bl    #0x20c2848
 *     020c2930  e59f0048  ldr   r0, [pc, #0x48]   pool 0x020c2980 = 0209F5BC
 *     020c2934  e5900000  ldr   r0, [r0]          the installed fader
 *     020c2938  e5901000  ldr   r1, [r0]          its vptr
 *     020c293c  e5911018  ldr   r1, [r1, #0x18]   BYTE +0x18, which is slot 6
 *     020c2940  e12fff31  blx   r1                r0 IS the object
 *     020c2944  e3500000  cmp   r0, #0
 *     020c2948  08bd4010  popeq {r4, lr}
 *     020c294c  012fff1e  bxeq  lr
 *
 * THE SLOT NUMBER IS RIGHT AND THE CALLING CONVENTION IS NOT.
 * `int (**vt)(void*)` is a __cdecl pointer taking the object as a STACK
 * argument, so MSVC pushes it and never sets ecx. data_0209f5bc is a HOST
 * object -- hal/fader_wipes.cpp's `void *data_0209f5bc = &hal_wipes[0]` over
 * `struct HalFaderWipe`, whose virtuals are laid out in ROM BYTE ORDER on
 * purpose (that file annotates every slot with its ROM byte: 0x00 D1, 0x04 D0,
 * 0x08 AdvanceFade, 0x0c SetBackwardTime, 0x10 SetForwardTime, 0x14 IsAtStart,
 * 0x18 IsAtEnd ...). So byte +0x18 is HalFaderWipe::IsAtEnd, which is
 * __thiscall and reads `this` out of ecx. The stack still BALANCES -- a __cdecl
 * caller cleans its own push and a zero-parameter __thiscall callee cleans
 * nothing -- so nothing faults and nothing in abicheck or aritycheck can see
 * it. What is wrong is only that IsAtEnd reads currInterp off whatever ecx
 * happened to hold.
 *
 * THAT IS THE WORSE KIND OF WRONG, for section 14's reason: a once-per-boot
 * gate that silently decides something is harder to catch than a crash. Its
 * answer decides whether the SEVEN zeroing stores below run at all, and the ROM
 * puts all seven at 0x020c2950..0x020c2974 in exactly the order the src spells:
 *
 *     020c2950  mov r1,#0
 *     020c2954  str r1,[r4,#0x190]     020c2968  str r0,[r4,#0x194]
 *     020c2958  ldr r0,[r4,#0x190]     020c296c  str r1,[r4,#0x60]
 *     020c295c  str r0,[r4,#0x18c]     020c2970  str r1,[r4,#0xd0]
 *     020c2960  str r1,[r4,#0x198]     020c2974  str r1,[r4,#0xe4]
 *     020c2964  ldr r0,[r4,#0x198]
 *
 * ITS OWN SIBLING ONE FUNCTION UP GETS THIS RIGHT, which is where the fix
 * comes from rather than from invention. src/func_ov006_020c2848.cpp -- called
 * on the line above, on the same object, at the same byte (ROM 0x020c2878) --
 * declares a local `struct VObj` of SEVEN virtuals and calls
 * `((VObj*)data_0209f5bc)->v6()`. That is a real C++ virtual call: MSVC's slot
 * 6 is byte +0x18, and `this` rides ecx. This file is
 * src/func_ov006_020c2924.cpp with the raw table read replaced by exactly that
 * spelling and NOTHING else changed.
 *
 * THE DISPATCH STAYS VIRTUAL rather than qualified, for
 * MgFlower_ModelRender_020c3bf4.cpp's reason: the ROM's is
 * (ldr vptr / ldr [vptr,#0x18] / blx) and a qualified call would devirtualise
 * whichever fader is installed.
 *
 * ---- WHAT THE REPAIR CHANGES FOR SCENE 361, STATED UP FRONT ---------------
 *
 * hal/fader_wipes.cpp's HalFaderWipe::IsAtEnd is `currInterp == 0x1000`, and
 * `data_0209f5bc = &hal_wipes[0]` is documented there as sitting at
 * currInterp = 0, which is what "no fade in progress" looks like. So a CORRECT
 * gate normally answers FALSE and the seven stores DO NOT run, where the
 * defective gate answered from garbage and ran them or not at random.
 *
 * Scene 367 has always had the repaired body (lane BOX shipped it) and boots
 * clean at 300/900/1200 frames. Scene 361 has been running the DEFECTIVE src
 * TU up to this merge, so the box merge is the first time 361 gets the correct
 * gate. The blast radius is one call per boot -- func_ov006_020dfeec is vtable
 * slot 18 and lane CUP's own census reports "state-reset 1" -- and it cannot
 * touch the dispatch counters, which come from func_ov006_020c2b8c. See
 * wiring.md for what the executor must re-measure and what is allowed to move.
 *
 * ---- WHAT CHANGED FROM src, so it can be checked line by line -------------
 *
 *   1. `int (**vt)(void*) = ...; if (vt[6](obj) == 0) return;` becomes
 *      `if (((HalFaderVObj *)data_0209f5bc)->v6() == 0) return;`
 *
 * Nothing else. The func_ov006_020c2848(c) call that opens it and all seven
 * stores -- +0x190 = 0, +0x18c = [+0x190], +0x198 = 0, +0x194 = [+0x198],
 * +0x60 = 0, +0xd0 = 0, +0xe4 = 0, in the ROM's own order at 0x020c2950
 * onwards -- are src's, transcribed unchanged, including the read-back through
 * the just-stored word that src spells and the ROM performs.
 *
 * IT DEFINES func_ov006_020c2924, so src/func_ov006_020c2924.cpp is out of
 * port/slice_box.txt (already) AND out of port/slice_cup.txt (this merge).
 * Listing it in either would be an LNK2005 -- and that LNK2005 is exactly how
 * the mg9 merge found the overlap.
 *
 * HOW IT WAS FOUND, because the method is the transferable part. Neither
 * prescribed detector sees this: there is no unresolved symbol and no `::*`.
 * A source grep for the shadow shape does not see it either, because the
 * source has no shadow class -- that is the whole defect. It came out of a
 * WORD-BY-WORD sweep of dScMgSound_c's and this sub-object's code for `blx Rm`
 * with the register loaded by an `ldr Rm,[Rn,#imm]` whose Rn is neither pc nor
 * sp: 23 sites, of which 15 are the pointer-to-member pairs, 3 are vtable slot
 * 35, 1 is the field dispatch, 2 are the model draw and TWO are this byte
 * +0x18 pair. Word by word rather than by disassembling linearly, which stops
 * at the first literal pool.
 *
 * THE OTHER ONE OF THAT PAIR IS 0x020c2878, INSIDE func_ov006_020c2848, AND IT
 * IS CORRECT -- it is the `->v6()` this file borrows. It is left alone.
 *
 * THE DECOMP-SIDE FIX IS NOT ROUTED, because there is nothing wrong with the
 * decomp: the raw table read produces the ROM's own instruction sequence. The
 * defect is the HOST's, which is what makes a port host copy the whole remedy.
 */
extern "C" {
extern int  func_ov006_020c2848(void *c);
extern void *data_0209f5bc;
void func_ov006_020c2924(char *c);
}

/* Seven virtuals so the seventh is MSVC slot 6, which is byte +0x18 -- the
   byte the ROM loads. Identical in shape to src/func_ov006_020c2848.cpp's own
   local `struct VObj`, and named differently only so the two do not collide at
   C++ linkage. */
struct HalFaderVObj {
    virtual int v0();
    virtual int v1();
    virtual int v2();
    virtual int v3();
    virtual int v4();
    virtual int v5();
    virtual int v6();
};

// PORT_HOST_ABI: a __cdecl-typed raw vtable read against a __thiscall host
// virtual -- the stack balances and the receiver does not arrive.
void func_ov006_020c2924(char *c)
{
    func_ov006_020c2848(c);
    if (((HalFaderVObj *)data_0209f5bc)->v6() == 0)          /* DELTA 1 */
        return;
    *(int *)(c + 0x190) = 0;
    *(int *)(c + 0x18c) = *(int *)(c + 0x190);
    *(int *)(c + 0x198) = 0;
    *(int *)(c + 0x194) = *(int *)(c + 0x198);
    *(int *)(c + 0x60)  = 0;
    *(int *)(c + 0xd0)  = 0;
    *(int *)(c + 0xe4)  = 0;
}
