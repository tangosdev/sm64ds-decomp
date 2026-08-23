// PORT_HOST_ABI. A RECEIVER RIDE-THROUGH the host cannot reproduce, in
// dScMgD3DBase_c's vtable slot 10. Run mg11, lane BNP.
//
// SHARED FILE, NOT A LANE FILE. dScMgD3DBase_c is the base of FOUR minigame
// classes -- 0x174 dScMgJump_c, 0x175 dScMgJump2_c, 0x180 and 0x181, all four
// pointing at the type_info at 0x0213c5c8 -- and all four hold 0x020e7040 in
// slot 10 of their own tables. The repair below is therefore named for the
// BASE and not for a lane, so that the second, third and fourth seats reuse it
// instead of adding a second definition of the same symbol. A lane arriving
// here with its own copy should delete its copy, not rename this one.
//
// ---- WHAT IS WRONG WITH THE SRC TU ----------------------------------------
//
// src/_ZN17MgBounceAndPounce12BeforeRenderEv.cpp is a faithful transcription of
// the ROM and is unusable on the host for exactly that reason. The ROM body at
// 0x020e7040 is
//
//     020e7040  stmdb  sp!, {lr}
//     020e7044  sub    sp, sp, #4
//     020e7048  bl     #0x20b04f4          <-- r0 is never touched
//     020e704c  cmp    r0, #0
//     020e7050  addeq  sp, sp, #4
//     020e7054  moveq  r0, #0
//     020e7058  ldmeq  sp!, {lr}
//     020e705c  bxeq   lr
//     020e7060  bl     #0x2022ee0
//     020e7064  mov    r0, #1
//     ...
//
// -- it never names its receiver, because on ARM r0 simply survives the
// prologue and arrives at the callee. The src transcribes that honestly:
//
//     extern int func_ov004_020b04f4();
//     if (func_ov004_020b04f4() == 0) return 0;
//
// AND func_ov004_020b04f4 DOES READ IT. src/func_ov004_020b04f4.cpp defines it
// `(void *c)`, and the ROM body proves the parameter is real rather than
// decorative: 0x020b04fc is `mov r4,r0`, 0x020b0518 reads self+0x4628,
// 0x020b0540 reads self+0xf0 and 0x020b05bc passes self+0xcc to
// func_ov004_020b8714. port/tools/aritycheck.py names the disagreement:
//
//     DROPS  func_ov004_020b04f4
//       declared 0 param(s) at src/_ZN17MgBounceAndPounce12BeforeRenderEv.cpp:4
//       DEFINED  1 param(s) at src/func_ov004_020b04f4.cpp:21
//
// THE TAIL-JUMP MECHANISM DOES NOT SAVE THIS ONE, which is the whole reason a
// host copy is owed rather than a tailjump_guard row. Roughly fifty forwarders
// in this port are correct only because MSVC compiles a ONE-CALL forwarder as a
// `jmp`, which reuses the caller's own cdecl frame so the argument the
// forwarder never names is still where its target reads it. This body is not a
// one-call forwarder: it tests the result, conditionally returns 0, and then
// makes a SECOND call. MSVC builds a real prologue, `self` is not on the stack
// where func_ov004_020b04f4 looks, and what it reads instead is this frame's
// own saved state -- the dropped-receiver shape that presents as a soft lock or
// a wild write rather than as a crash at the call.
//
// SO THE COPY THREADS THE RECEIVER AND CHANGES NOTHING ELSE. Same two calls in
// the same order, the same early return, the same literal 1. The symbol keeps C
// linkage and the Itanium spelling the src TU gave it, so the linker sees one
// definition and port/slice_bnp.txt leaves the src TU out.

extern "C" {

int  func_ov004_020b04f4(void *self);   /* dScMgBase_c::BeforeRender */
int  _ZN8Particle9RenderAllEv(void);    /* Particle::RenderAll, a static */

int  _ZN17MgBounceAndPounce12BeforeRenderEv(void *self);

int _ZN17MgBounceAndPounce12BeforeRenderEv(void *self)
{
    if (func_ov004_020b04f4(self) == 0)
        return 0;
    _ZN8Particle9RenderAllEv();
    return 1;
}

}  /* extern "C" */
