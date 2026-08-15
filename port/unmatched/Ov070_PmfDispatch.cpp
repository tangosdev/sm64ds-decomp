/* RUN LINKW WAVE 17 (lane w17): the two ov070 RENDER bodies, hosted -- and the
 * written record of the mwcc POINTER-TO-MEMBER bug that sits in front of them,
 * whose fix is a compile option in port/CMakeLists.txt (R8) rather than code.
 *
 * WHY THIS FILE EXISTS. Mounting level 27 (Tick Tock Clock) put the first AMP
 * (266) and FLAME_CHOMP (270) instances in front of a mounted level's tick
 * loop. Both took an access violation on frame 0 -- the fault run linkw wave 8
 * recorded against levels 16/21/25/27/28 and filed as "DATA-dependent, not a
 * dead vtable slot". It is neither. It is TWO independent, entirely general
 * bugs, one behind the other, and both fire on every instance of both classes
 * on every level.
 *
 * ============================================================================
 * BUG ONE: the mwcc pointer-to-member width. FIXED BY /vmg /vmm, NOT HERE.
 * ============================================================================
 *
 * MEASURED. SM64DS_LEVEL=27 SM64DS_FAULTS_FATAL=1:
 *
 *   FAULT c0000005 at +0xffc00000 (EIP = 0) accessing 00000000
 *     actor id 0x10a (AMP), eax=00000000 ecx=<this> ebx=_ZTV3Amp
 *     stack[00] = Amp::Behavior +0xa
 *
 * and the same shape one class over with id 0x10e (FLAME_CHOMP), stack[00] =
 * FlameChomp::Behavior +0x9. Both call sites are the FIRST call in their
 * Behavior and both callees tail-jump through a null.
 *
 * src/func_ov070_02120d34.cpp is
 *
 *     struct C; typedef void (C::*PMF)();
 *     struct C { char pad[0x41c]; PMF *pp; };
 *     extern "C" void func_ov070_02120d34(C *c) { PMF *p = c->pp + 1; (c->**p)(); }
 *
 * and the ROM body it decompiles (overlay_0070.bin at 0x02120d34) is
 *
 *     ldr  r1, [r0, #0x41c]      pp, the current State object
 *     add  r3, r1, #8            &pp[1]  -- the mwcc PMF stride is EIGHT
 *     ldr  r1, [r3, #4]          adj
 *     add  r0, r0, r1, asr #1    this += adj >> 1
 *     ands r1, r1, #1            odd adj = virtual PMF
 *     ldrne r2, [r0] / ldrne r1, [r3] / ldrne r1, [r2, r1]
 *     ldreq r1, [r3]             even adj = the function itself
 *     blx  r1
 *
 * mwcc's pointer-to-member over an incomplete class is the Itanium 8-byte
 * {fn, adj} record. MSVC's DEFAULT for the same declaration is the 4-byte
 * single-inheritance form, so `c->pp + 1` strides four and lands on the adj
 * word of record 0 -- which every one of these records holds as zero. The call
 * is literally `call 0`, which is exactly the EIP the fault reports.
 *
 * THE STRIDE IS PINNED BY THE SETTER, not inferred. func_ov070_02120da8 (Amp)
 * and func_ov070_02121880 (FlameChomp) both compute the State pointer as
 * `table + (state << 4)` -- sixteen bytes per state, two 8-byte PMFs per
 * State, pp[0] the enter handler and pp[1] the per-frame tick. The record
 * counts agree: ov70_seat_state_pmfs seats SIX source records for Amp (three
 * states) and EIGHT for FlameChomp (four).
 *
 * THE FIX IS A COMPILE OPTION, and it is in port/CMakeLists.txt under R8:
 * /vmg /vmm on those two TUs makes MSVC use the multiple-inheritance
 * representation, {void *fn; int delta;} -- eight bytes, the ROM's own record,
 * with a call that adds delta to `this` and calls fn. `c->pp + 1` then strides
 * eight and the dispatch IS the ROM's even-adj branch. Both matched TUs keep
 * their own job, nothing is renamed and nothing goes dead. (The ROM stores adj
 * and shifts right by one where MSVC stores delta directly; every ov070 record
 * carries adj 0, so the two agree word for word here. A record with an odd adj
 * would need the virtual branch, and none exists.)
 *
 * WHY ONLY TWO OF ov070's FOUR dispatchers get the option. func_ov070_02120d70
 * and func_ov070_02121848 read pp[0] rather than pp[1], and at index 0 the
 * wrong stride cannot bite: MSVC's 4-byte PMF at offset 0 IS the fn word and
 * every seated record carries adj 0, so the compiled `call [p]` with an
 * unadjusted `this` is already the ROM's even-adj branch. They are correct BY
 * LUCK, not by design, and the luck is worth writing down.
 *
 * THE WIDER FIX, which is NOT this lane's. `grep -rl "c->pp + 1" src/` finds
 * TWENTY-THREE TUs with this exact shape:
 *
 *   _ZN10KingBobOmb8BehaviorEv  _ZN10MrBlizzard8BehaviorEv
 *   _ZN11ChiefChilly8BehaviorEv func_ov018_0211235c func_ov019_02112268
 *   func_ov027_02111cfc func_ov030_02114134 func_ov070_02120d34
 *   func_ov070_0212180c func_ov070_02121fd0 func_ov071_02120278
 *   func_ov071_021215c0 func_ov072_0211fc3c func_ov072_02120560
 *   func_ov072_02121cdc func_ov077_02124718 func_ov077_02125e20
 *   func_ov080_021250c8 func_ov081_02127708 func_ov085_0212a430
 *   func_ov085_0212dbdc func_ov085_0212de5c func_ov096_021368b4
 *
 * Every one of them is the same `call 0` the moment its class is both
 * registered and ticked, and /vmg /vmm is the same one-line remedy for each.
 * Two are turned on here because they are what stands between level 27 and a
 * green battery; the other twenty-one are a lane of their own, and each wants
 * its own measured before/after rather than a blanket flag.
 *
 * ============================================================================
 * BUG TWO: the ModelAnim slot-5 collision. FIXED HERE.
 * ============================================================================
 *
 * With the PMF dispatch fixed both classes faulted one step later, in RENDER:
 *
 *   FAULT c0000005 accessing 00000000
 *     Model::Virtual10 +0xc  <- ModelAnim::Virtual10 +0x25
 *                            <- ModelAnim::Virtual18 +0xe
 *                            <- Amp::Render +0x14
 *
 * signature for signature the one port/unmatched/ModelAnim_Renders.cpp
 * measured on Butterfly. Both Renders dispatch through a LOCAL SIX-VIRTUAL
 * SHADOW over the ModelAnim at +0xd4, so their slot 5 is the ROM's
 * ModelAnim::Render while the host _ZTV9ModelAnim's slot 5 is
 * ModelAnim::Virtual18 -- a two-argument method that reads its scale off the
 * stack. _ZTV5Model can be dual-filled and is; _ZTV9ModelAnim cannot, because
 * Virtual18 really is there. (This is also why wave 8's SNUFIT note reads
 * Model::Virtual10 +0xc: the same collision on a third class, still open.)
 *
 * The two bodies are transcribed below from the ROM listings and dispatched
 * from ov070's two vtable fills by their port_ names. THE COST IS TWO LINKED
 * TUs and it should be read plainly: src/_ZN3Amp6RenderEv.cpp and
 * src/_ZN10FlameChomp6RenderEv.cpp are C++ METHODS whose only reference was
 * the C face in hal/actor_classes_ov070.cpp, so routing the fills past those
 * faces leaves both methods unreferenced and /OPT:REF strips them. That is the
 * same trade the Butterfly/Whomp/Seaweed copies made when their sources came
 * off slice_gate33/slice_gate64; there is no way to keep a body linked and
 * also not call it.
 */

#include "Model.h"
#include "ModelAnim.h"

extern "C" {

int _ZN15TextureSequence6UpdateER15ModelComponents(void *seq, void *comp);
int _ZN18TextureTransformer6UpdateER15ModelComponents(void *tr, void *comp);

/* PORT_HOST_ABI: ROM-order ModelAnim slot-5 dispatch, the Whomp/Butterfly
   case -- the host _ZTV9ModelAnim slot 5 is Virtual18. Amp::Render, ROM
   0x02120e24 line for line:
       add r0,r4,#0xd4 / ldr r2,[r0] / mov r1,#0 / ldr r2,[r2,#0x14] / blx r2
       ldr r0,[r4,#0x420] ; return 1 if it is 0 or 2
       TextureSequence::Update(this+0x188, this+0x140)
       TextureTransformer::Update(this+0x19c, this+0x140)
       add r0,r4,#0x138 / ldr r2,[r0] / add r1,r4,#0x80 / ldr r2,[r2,#0x14]
   The second dispatch is Model's slot 5, which cxxname_bridge DOES dual-fill,
   so only the first one was ever broken; it is spelled qualified anyway so the
   file reads the same way twice. */
int port_ov070_amp_render(void *selfv)
{
    char *c = (char *)selfv;
    ((ModelAnim *)(c + 0xd4))->ModelAnim::Render(0);
    int s = *(int *)(c + 0x420);
    if (s != 0 && s != 2) {
        _ZN15TextureSequence6UpdateER15ModelComponents(c + 0x188, c + 0x140);
        _ZN18TextureTransformer6UpdateER15ModelComponents(c + 0x19c, c + 0x140);
        ((Model *)(c + 0x138))->Model::Render((const Vector3 *)(c + 0x80));
    }
    return 1;
}

/* PORT_HOST_ABI: ROM-order ModelAnim slot-5 dispatch, the same ruling.
   FlameChomp::Render, ROM 0x021218c4, four instructions of body:
       add r0,r1,#0xd4 / ldr r2,[r0] / add r1,r1,#0x80 / ldr r2,[r2,#0x14]
   -- one draw at the actor's own scale Vector3, then return 1. */
int port_ov070_flamechomp_render(void *selfv)
{
    char *c = (char *)selfv;
    ((ModelAnim *)(c + 0xd4))->ModelAnim::Render((const Vector3 *)(c + 0x80));
    return 1;
}

}
