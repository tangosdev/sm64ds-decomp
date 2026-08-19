/* PORT_HOST_ABI. func_ov006_0210a6e4, vtable slot 2 (AfterInitResources) of
 * dScMgSingle3DBase_c and of every ov006 class under it -- dScMgFlower_c
 * (0x186), dScMgCup_c (0x169) and dScMgMemory2_c (0x16b) all hold this word.
 * Run mg5, lane FLW.
 *
 * WHAT IS WRONG WITH THE MATCHED TU, and it is an ARM ride-through rather
 * than a bad body. src/func_ov006_0210a6e4.cpp is a FAITHFUL DECOMP: it was
 * disassembled out of extracted/overlays/overlay_0006.bin at base 0x020bfec0
 * and ruled REAL_DECOMP three times over now (lane MGA, lane MGB and this
 * lane), and no constant, offset, branch target or instruction count differs.
 * The ROM is eight instructions:
 *
 *     0210a6e4  push {r4,lr}
 *     0210a6e8  mov  r4,r0
 *     0210a6ec  bl   0x20b08f0          <- r1 IS NEVER WRITTEN
 *     0210a6f0  ldr  r0,[pc,#0xc]       ; = 0x0000471c
 *     0210a6f4  add  r0,r4,r0
 *     0210a6f8  bl   0x2022f40          <- Particle::SysTracker::Initialise
 *     0210a6fc  pop  {r4,lr}
 *     0210a700  bx   lr
 *
 * The framework's SECOND argument arrives in r1 and is never touched, so it
 * rides through into func_ov004_020b08f0 for free. src spells that call with
 * ONE argument, which is the only way to spell it in C when the value is
 * never named -- and on the host that means the second argument is simply not
 * pushed, and the callee reads whatever the stack happens to hold.
 *
 * WHY IT MATTERS RATHER THAN BEING TIDY. func_ov004_020b08f0's tail is
 * Scene::AfterInitResources(this, flags), and hal/scene_mg_faces.cpp's own
 * argument-landing face records what flags decides: "vfSuccess == 1 marks the
 * actor for destruction, so a garbage argument decides on the first frame
 * whether the minigame scene survives". A stack-garbage flags is therefore a
 * coin flip on whether the scene lives, made before frame 0 and reported by
 * nothing.
 *
 * WHY A FACE CANNOT FIX IT. Lane MGA filed this as "the face must pass both",
 * and that is not where the loss happens. The vtable face can pass two
 * arguments to func_ov006_0210a6e4 all it likes; the argument is dropped
 * INSIDE that function, at its own call site. The only place to land it is
 * this body.
 *
 * WHAT CHANGED FROM src, stated so it can be checked line by line:
 *
 *     src:   int func_ov006_0210a6e4(void* c)
 *              func_ov004_020b08f0(c);
 *     here:  int port_mg_flower_after_init(void* c, unsigned f)
 *              func_ov004_020b08f0(c, f);
 *
 * and nothing else. The Particle::SysTracker::Initialise call, the +0x471c
 * offset and the return of its result are src's, verbatim.
 *
 * IT CARRIES A port_ NAME AND DOES NOT DEFINE func_ov006_0210a6e4. The decomp
 * has a real matched body for that symbol and this file is not a stand-in for
 * it -- it is the host's calling convention being placed by hand. So
 * src/func_ov006_0210a6e4.cpp stays out of port/slice_flw.txt (listing both
 * would be an LNK2005), the symbol stays absent from the port's map, and
 * nothing in this tree claims a decompilation that does not exist.
 *
 * THE NEXT LANE TO SEAT 361 OR 363 SHOULD CALL THIS RATHER THAN WRITE A
 * SECOND. The body is dScMgSingle3DBase_c's shared glue, not dScMgFlower_c's,
 * and the file is named for the lane that paid for it rather than for the
 * class that owns it.
 */

extern "C" {

void func_ov004_020b08f0(void *c, unsigned f);
int  _ZN8Particle10SysTracker10InitialiseEv(void *);

int port_mg_flower_after_init(void *c, unsigned f)
{
    func_ov004_020b08f0(c, f);
    return _ZN8Particle10SysTracker10InitialiseEv((char *)c + 0x471c);
}

}  /* extern "C" */
