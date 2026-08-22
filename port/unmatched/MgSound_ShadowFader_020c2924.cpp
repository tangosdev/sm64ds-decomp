/* HOST COPY of src/func_ov006_020c2924.cpp -- the sub-object's "re-seat the
 * animation and clear the offsets" call, reached from vtable slot 0
 * (InitResources) through src/func_ov006_0211c478.c, and from slot 18's reset
 * through the same body.
 * Run mg9, lane BOX. actor id 0x16f = scene 367.
 *
 * ONE DEFECT, AND IT IS A RECEIVER DROP AT A HOST C++ VIRTUAL. It is the third
 * distinct shape of the same disease this class carries -- the other two are
 * port/unmatched/MgSound_ShadowSlot35.cpp (a raw vtable-word read typed
 * `int (*)(void)`) and port/unmatched/MgSound_ModelRender_020c29dc.cpp (a
 * shadow class at the wrong host slot).
 *
 * ---- WHAT src DOES AND WHAT THE ROM DOES ---------------------------------
 *
 * src/func_ov006_020c2924.cpp:
 *
 *     void* obj = data_0209f5bc;
 *     int (**vt)(void*) = *(int(***)(void*))obj;
 *     if (vt[6](obj) == 0) return;
 *
 * The ROM at 0x020c2930, read out of extracted/overlays/overlay_0006.bin at
 * base 0x020bfec0:
 *
 *     ldr r0,[pc,#0x48]     pool 0x020c2980 = 0209F5BC
 *     ldr r0,[r0]           the installed fader
 *     ldr r1,[r0]           its vptr
 *     ldr r1,[r1,#0x18]     BYTE +0x18, which is slot 6
 *     blx r1                r0 IS the object, so the receiver is passed
 *     cmp r0,#0 / popeq / bxeq
 *
 * THE SLOT NUMBER IS RIGHT AND THE CALLING CONVENTION IS NOT.
 * `int (**vt)(void*)` is a __cdecl pointer taking the object as a STACK
 * argument, so MSVC pushes it and never sets ecx. data_0209f5bc is a HOST
 * object -- hal/fader_wipes.cpp's `void *data_0209f5bc = &hal_wipes[0]` over
 * `struct HalFaderWipe`, whose virtuals are laid out in ROM BYTE ORDER on
 * purpose (that file annotates every slot with its ROM byte: 0x00 D1, 0x04 D0,
 * 0x08 AdvanceFade, 0x0c, 0x10, 0x14 IsAtStart, 0x18 IsAtEnd ...). So byte
 * +0x18 is HalFaderWipe::IsAtEnd, which is __thiscall and reads `this` out of
 * ecx. The stack still BALANCES -- a __cdecl caller cleans its own push and a
 * zero-parameter __thiscall callee cleans nothing -- so nothing faults and
 * nothing in abicheck or aritycheck can see it. What is wrong is only that
 * IsAtEnd reads currInterp off whatever ecx happened to hold.
 *
 * THAT IS THE WORSE KIND OF WRONG, for section 14's reason: a once-per-boot
 * gate that silently decides something is harder to catch than a crash. Its
 * answer decides whether the seven zeroing stores below run at all.
 *
 * ITS OWN SIBLING ONE FUNCTION UP GETS THIS RIGHT, which is where the fix
 * comes from rather than from invention. src/func_ov006_020c2848.cpp -- called
 * on the line above, on the same object, at the same byte -- declares a local
 * `struct VObj` of SEVEN virtuals and calls `((VObj*)data_0209f5bc)->v6()`.
 * That is a real C++ virtual call: MSVC's slot 6 is byte +0x18, and `this`
 * rides ecx. This file is src/func_ov006_020c2924.cpp with the raw table read
 * replaced by exactly that spelling and NOTHING else changed.
 *
 * THE DISPATCH STAYS VIRTUAL rather than qualified, for
 * MgFlower_ModelRender_020c3bf4.cpp's reason: the ROM's is
 * (ldr vptr / ldr [vptr,#0x18] / blx) and a qualified call would devirtualise
 * whichever fader is installed.
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
 * IT DEFINES func_ov006_020c2924, so src/func_ov006_020c2924.cpp is OUT of
 * port/slice_box.txt -- listing both would be an LNK2005.
 *
 * HOW IT WAS FOUND, because the method is the transferable part. Neither
 * prescribed detector sees this: there is no unresolved symbol and no `::*`.
 * A source grep for the shadow shape does not see it either, because the
 * source has no shadow class -- that is the whole defect. It came out of a
 * WORD-BY-WORD sweep of this class's and its sub-object's code for `blx Rm`
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
