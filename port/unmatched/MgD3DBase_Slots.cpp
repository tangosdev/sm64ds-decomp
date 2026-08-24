/* PORT_HOST_ABI. FOUR ARM ride-throughs in dScMgD3DBase_c's vtable that the
 * host cannot reproduce -- slots 2, 10, 27 and 28.
 *
 * SHARED FILE, NOT A LANE FILE, and it is shared in the strongest sense: lanes
 * BNP (0x174, scene 372) and BNT (0x175, scene 373) of run mg11 arrived at
 * these four repairs INDEPENDENTLY, from two different scenes, and converged on
 * the same four bodies with the same four spellings. The file keeps BNT's
 * filename and BNT's four symbol names so the two lanes' trees collide in ONE
 * visible place instead of producing four duplicate symbols; the measurements
 * below are BNP's own. Lanes seating 0x180 and 0x181 should call these four
 * rather than add a fifth copy.
 *
 * THE TWO COPIES ARE NOT BYTE-IDENTICAL, so the merge resolves an ADD/ADD
 * conflict here by picking one rather than getting a free dedupe. The only code
 * difference is a vestigial `#include <cstdio>` in BNT's copy that nothing in
 * the file uses; the four bodies are the same. As of e9f6b9b36 lane TTI carried
 * this file byte-identical; this header has since been edited, so the two now
 * differ in prose only.
 *
 * THAT SENTENCE IS PINNED TO A COMMIT ON PURPOSE, and the reason is the bug it
 * replaces. The earlier wording claimed, in this file, that another lane's copy
 * of THIS FILE was byte-identical to it -- and writing the claim changed the
 * bytes, so the sentence falsified itself the moment it was saved. A claim
 * about a concurrent lane's file state is volatile by construction: pin it to a
 * commit, or state it from OUTSIDE the compared files, or do not write it.
 *
 * All four slots hold ONE body each in all four subclasses' tables, so a repair
 * here is a repair for the whole family.
 *
 * ---- SLOT 2, AfterInitResources: THE ONE THAT DECIDES WHETHER THE SCENE LIVES
 *
 * src/_ZN17MgBounceAndPounce18AfterInitResourcesEj.cpp calls
 * func_ov004_020b08f0 with ONE argument because the ROM never names the second:
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
 * The framework's second argument arrives in r1 and rides through for free. On
 * the host it is simply not pushed, and the callee reads whatever the stack
 * holds. func_ov004_020b08f0's tail is Scene::AfterInitResources(this, flags),
 * where vfSuccess == 1 MARKS THE ACTOR FOR DESTRUCTION -- so a garbage flags is
 * a coin flip on whether the minigame survives frame 0.
 * port/unmatched/MgFlower_Slot2.cpp records the same defect for
 * dScMgSingle3DBase_c at offset +0x471c; this base's offset is +0x47e4.
 *
 * BNP MEASURED IT COMING UP DESTROY, EVERY RUN. Scene 372 with the src TU wired
 * straight to the vtable face:
 *
 *   BEFORE  slot hits: init 1, cleanup 1, behavior 0, render 0, D2 1, slot18 1
 *           36-slot table, 2 total slot entries; framework slots entered: 12(x1)
 *   CONTROL scene 386, whose class keeps the FRAMEWORK's slot 2: behavior 253,
 *           cleanup 0, on the same binary
 *   AFTER   slot hits: init 1, cleanup 0, behavior 253, render 0, D2 0, slot18 1
 *
 * The scene booted clean, exited 0 and did nothing. The return value is
 * forwarded too -- the ROM's own `pop / bx lr` right after the bl returns
 * Initialise's r0 -- so this body returns it rather than inventing a 1.
 *
 * ---- SLOT 10, BeforeRender: a dropped receiver the tail-jump trick misses ---
 *
 * src/_ZN17MgBounceAndPounce12BeforeRenderEv.cpp declares func_ov004_020b04f4
 * with NO parameter and calls it with none. The ROM at 0x020e7040 never touches
 * r0 before the bl, so the receiver rides through; and the callee REALLY READS
 * IT -- 0x020b04fc is `mov r4,r0`, 0x020b0518 reads self+0x4628, 0x020b0540
 * reads self+0xf0, 0x020b05bc passes self+0xcc on. port/tools/aritycheck.py
 * names the disagreement:
 *
 *   DROPS  func_ov004_020b04f4
 *     declared 0 param(s) at src/_ZN17MgBounceAndPounce12BeforeRenderEv.cpp:4
 *     DEFINED  1 param(s) at src/func_ov004_020b04f4.cpp:21
 *
 * THE TAIL-JUMP MECHANISM DOES NOT COVER THIS ONE, which is why a host body is
 * owed rather than a tailjump_guard row: the port's ~fifty forwarders are
 * correct only because MSVC compiles a ONE-CALL forwarder as a `jmp` that
 * reuses the caller's cdecl frame. This body tests the result, returns early
 * and then makes a SECOND call, so MSVC builds a real prologue and `self` is
 * not where func_ov004_020b04f4 looks.
 *
 * ---- SLOTS 27 AND 28: the same shape, one instruction long ------------------
 *
 * In the ROM each is `ldr ip,[pc]; bx ip` onto an ov004 body that DOES read r0
 * (func_ov004_020af27c reads self+0x4630 at 0x020af284; func_ov004_020af04c
 * reads self+0xf4 at 0x020af060), and each src TU declares its target with no
 * parameter. These two ARE one-call forwarders, so the tail-jump mechanism
 * COULD carry them -- but BNP measured that port/tools/tailjump_guard.py does
 * NOT have them in its assertion set (35 frames / 22 veneer derived, unchanged
 * from base with these two TUs in the slice), so nothing would notice if the
 * form ever stopped holding. Naming the argument is the cheaper guarantee, and
 * it is what BNT chose independently.
 *
 * ---- WHAT CHANGED FROM src, so it can be checked line by line --------------
 *
 * Each body is its src TU verbatim with one argument named and forwarded, and
 * nothing else. The four carry port_ names and do NOT define the decomp
 * symbols, which is MgFlower_Slot2.cpp's rule: the decomp has real matched
 * bodies for all four, this file is the host's calling convention being placed
 * by hand, so those four src TUs stay OUT of the slices and the port's map
 * never claims a decompilation that does not exist.
 */

extern "C" {

/* dScMgBase_c's own four, exactly as their src TUs declare them */
void func_ov004_020b08f0(void *c, unsigned int flags);
int  func_ov004_020b04f4(void *c);
void func_ov004_020af27c(void *c);
void func_ov004_020af04c(void *c);

/* the two Particle entry points the two repaired bodies keep */
int  _ZN8Particle10SysTracker10InitialiseEv(void *p);
int  _ZN8Particle9RenderAllEv(void);

}  /* extern "C" */

/* slot 2. src/_ZN17MgBounceAndPounce18AfterInitResourcesEj.cpp verbatim with
   the flags argument named and forwarded. The SysTracker lives at +0x47e4 on
   this base, which is what the ROM's one-word pool at 0x020e70e0 holds. */
extern "C" int port_mg_d3dbase_after_init(void *c, unsigned int flags)
{
    func_ov004_020b08f0(c, flags);
    return _ZN8Particle10SysTracker10InitialiseEv((char *)c + 0x47e4);
}

/* slot 10. src/_ZN17MgBounceAndPounce12BeforeRenderEv.cpp verbatim with the
   receiver named and forwarded. Both early exits are src's. */
extern "C" int port_mg_d3dbase_before_render(void *c)
{
    if (func_ov004_020b04f4(c) == 0)
        return 0;
    _ZN8Particle9RenderAllEv();
    return 1;
}

/* slots 27 and 28. The two veneers, with the receiver the ROM rides through in
   r0 spelled as the argument it is. */
extern "C" void port_mg_d3dbase_slot27(void *c) { func_ov004_020af27c(c); }
extern "C" void port_mg_d3dbase_slot28(void *c) { func_ov004_020af04c(c); }
