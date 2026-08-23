/* PORT_HOST_ABI. _ZN17MgBounceAndPounce18AfterInitResourcesEj, address
 * 0x020e70c0, vtable slot 2 (AfterInitResources) of dScMgD3DBase_c and of
 * every ov006 class under it -- dScMgJump_c (0x174), dScMgJump2_c (0x175) and
 * ids 0x180 and 0x181 all hold this word in slot 2 of their own tables.
 * Run mg11, lane BNP.
 *
 * SHARED FILE, NOT A LANE FILE, and it is the SECOND instance of a defect lane
 * FLW already diagnosed and fixed once. port/unmatched/MgFlower_Slot2.cpp is
 * the same repair for dScMgSingle3DBase_c's copy of the same glue
 * (func_ov006_0210a6e4, offset +0x471c); this file is dScMgD3DBase_c's copy
 * (offset +0x47e4). Its header ends by telling the next lane to call it rather
 * than write a second, and this file obeys that instruction for a DIFFERENT
 * base class rather than duplicating the flower's -- the two bodies are
 * genuinely different ROM functions at different offsets, so one cannot serve
 * both. The next of the four dScMgD3DBase_c seats should call THIS.
 *
 * WHAT IS WRONG WITH THE MATCHED TU, and it is an ARM ride-through rather than
 * a bad body. src/_ZN17MgBounceAndPounce18AfterInitResourcesEj.cpp is a
 * FAITHFUL DECOMP: it was disassembled out of
 * extracted/overlays/overlay_0006.bin at base 0x020bfec0 and every instruction
 * agrees. The ROM is eight instructions and one pool word:
 *
 *     020e70c0  push {r4,lr}
 *     020e70c4  mov  r4,r0
 *     020e70c8  bl   0x20b08f0          <- r1 IS NEVER WRITTEN
 *     020e70cc  ldr  r0,[pc,#0xc]       ; 020e70e0 = 0x000047e4
 *     020e70d0  add  r0,r4,r0
 *     020e70d4  bl   0x2022f40          <- Particle::SysTracker::Initialise
 *     020e70d8  pop  {r4,lr}
 *     020e70dc  bx   lr
 *
 * The framework's SECOND argument arrives in r1 and is never touched, so it
 * rides through into func_ov004_020b08f0 for free. src spells that call with
 * ONE argument, which is the only way to spell it in C when the value is never
 * named -- and on the host that means the second argument is simply not
 * pushed, and the callee reads whatever the stack happens to hold.
 *
 * WHY IT MATTERS, AND THIS LANE MEASURED IT RATHER THAN INHERITING THE CLAIM.
 * func_ov004_020b08f0's tail is Scene::AfterInitResources(this, flags), and
 * hal/scene_mg_faces.cpp's argument-landing face records what flags decides:
 * vfSuccess == 1 marks the actor for destruction. The first proof run of scene
 * 372 with the src TU wired straight to the vtable face reported
 *
 *     dScMgJump_c slot hits: init 1, cleanup 1, behavior 0, render 0, D2 1,
 *                            D0 0, slot18-FLOOR 1, slot19 0
 *     36-slot table, 2 total slot entries; framework slots entered: 12(x1)
 *
 * -- InitResources ran, AfterInitResources ran, and then the scene was torn
 * down on frame 0 with the framework's pending-destroy slot entered once and
 * not one Behavior tick. A control on the same binary, scene 386, whose class
 * keeps the FRAMEWORK's slot 2 rather than this one, reported behavior 253 and
 * cleanup 0. So the coin flip FLW's header describes is not hypothetical here:
 * it came up destroy, every run, and the only symptom was a scene that booted
 * clean and did nothing.
 *
 * WHY A FACE CANNOT FIX IT. The vtable face can pass two arguments to
 * 0x020e70c0 all it likes; the argument is dropped INSIDE that function, at
 * its own call site. The only place to land it is this body.
 *
 * WHAT CHANGED FROM src, stated so it can be checked line by line:
 *
 *     src:   int _ZN17MgBounceAndPounce18AfterInitResourcesEj(self)
 *              func_ov004_020b08f0((void*)self);
 *     here:  int port_mg_d3dbase_after_init(void *c, unsigned f)
 *              func_ov004_020b08f0(c, f);
 *
 * and nothing else. The Particle::SysTracker::Initialise call, the +0x47e4
 * offset and the return of its result are src's, verbatim.
 *
 * IT CARRIES A port_ NAME AND DOES NOT DEFINE THE DECOMP SYMBOL, which is
 * FLW's rule and is why the src TU stays out of port/slice_bnp.txt: listing
 * both would be an LNK2005, the symbol stays absent from the port's map, and
 * nothing in this tree claims a decompilation that does not exist.
 */

extern "C" {

void func_ov004_020b08f0(void *c, unsigned f);
int  _ZN8Particle10SysTracker10InitialiseEv(void *);

int port_mg_d3dbase_after_init(void *c, unsigned f)
{
    func_ov004_020b08f0(c, f);
    return _ZN8Particle10SysTracker10InitialiseEv((char *)c + 0x47e4);
}

}  /* extern "C" */
