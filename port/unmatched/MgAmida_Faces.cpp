/* PORT_HOST_ABI. THREE NAMED TRAPS for dScMgAmida_c, actor id 0x173 (scene
 * 371) -- two in ov006, found by a delinks join over the callees, and one in
 * ov004, found by the link.
 * Run mg9, lane S371.
 *
 * THE PRECEDENT IS func_ov006_020e1854, dScMgCurling_c's twenty-fifth state:
 * an address the ROM reaches, no delink block, no src TU in either extension,
 * and therefore no body this tree may write. What goes in its place is a trap
 * that SAYS WHICH ADDRESS WAS ENTERED and does nothing else. A plausible body
 * would be worse than a missing one, because a plausible body cannot be told
 * apart from a real one by anything downstream, and
 * port/tools/inferred_stub_guard.py exists to refuse exactly that.
 *
 * ---- WHY THIS CLASS HAS FLOORS WHEN ITS COST ROW SAYS nosrc 0 -------------
 *
 * port/mg_fanout_costs.txt section 3's nosrc column counts VTABLE BODIES. All
 * eleven of this class's override bodies have src. These two are CALLEES, and
 * finding them is section 13 CORRECTION 3's method rather than section 4's: a
 * delinks join over every callee of every override, not over the state
 * addresses. That section's own words are "the check that finds it is a delink
 * join over every callee of every override", and this class is the second one
 * where that check pays.
 *
 *   func_ov006_020d27dc   0xe48 = 3656 bytes, the largest single hole in any
 *       seated minigame. config/arm9/overlays/ov006/symbols.txt names it;
 *       delinks.txt runs `src/func_ov006_020d25fc.c .text start:0x020d25fc
 *       end:0x020d27dc` and the next block starts elsewhere, so the 0xe48
 *       bytes from 0x020d27dc are covered by nothing. ONE caller in the whole
 *       overlay: `from:0x020d4d78 kind:arm_call to:0x020d27dc`, which is inside
 *       vtable slot 6, the Behavior.
 *
 *   func_ov006_020d36a4   0x4fc. Same shape -- named, `end:0x020d36a4` is where
 *       src/func_ov006_020d3668.cpp's block stops, and no block resumes until
 *       past it. TWO callers: `from:0x020d3da0` (inside func_ov006_020d3ba0,
 *       which vtable slot 0 and vtable slot 18 both call as their last act) and
 *       `from:0x020d4bd4` (inside slot 6). So this one is reached on the INIT
 *       path as well as the tick path.
 *
 * THE MODULE-RESIDENCY TRAP WAS CHECKED AND IS NOT WHAT THIS IS. Section 13
 * records src/func_ov002_020f5b98.c standing next to an ov006 hole at the same
 * address and a name-shaped lookup calling the floor closed. There is no
 * src/func_ov00X_020d27dc.* and no src/func_ov00X_020d36a4.* for ANY X in this
 * tree, in either extension.
 *
 * ---- THE SIGNATURES ARE READ OUT OF THE ROM AND OUT OF THE CALLERS ---------
 *
 * Both take self in r0 and return nothing, and both halves are measured:
 *
 *   0x020d27dc  push {r4,r5,r6,r7,r8,sb,sl,fp,lr} / sub sp,sp,#0x64 /
 *               mov sb,r0, and its SINGLE exit is
 *                   020d35dc  add sp, sp, #0x64
 *                   020d35e0  pop {r4, r5, r6, r7, r8, sb, sl, fp, lr}
 *                   020d35e4  bx  lr
 *               with no `mov r0,#N` in front of it.
 *   0x020d36a4  push {r4,r5,r6,r7,r8,sb,sl,fp,lr} / sub sp,sp,#0x1c /
 *               mov sb,r0, and its single exit is
 *                   020d3b80  pop {r4, r5, r6, r7, r8, sb, sl, fp, lr}
 *                   020d3b84  bx  lr
 *               likewise.
 *
 * and both decompiled callers agree: src/func_ov006_020d4b7c.c declares both as
 * `void f(char *c)` and src/func_ov006_020d3ba0.c independently declares
 * 020d36a4 as `void func_ov006_020d36a4(void *sb)`. This file repeats those
 * declarations rather than inventing one. A trap that returned a value would be
 * asserting something the ROM does not say; these assert only that they were
 * entered.
 *
 * ---- WHAT THE HOLES COST THE BOOT, SAID PLAINLY ---------------------------
 *
 * Both bodies dispatch vtable slot 36 three times each -- 0x020d2924,
 * 0x020d2abc, 0x020d2d8c and 0x020d36c0, 0x020d3ab0, 0x020d3b20 -- so six of
 * this class's twenty-five slot-36 dispatch sites are inside code the port
 * cannot run. 0x020d36a4 reads it in its own fourth instruction, before it does
 * anything else, which is what a per-mode branch at the top of a function looks
 * like. Whatever these two do, they do it differently in "Mario's Slides" and
 * in "Connect the Characters", and this seat reaches neither version.
 *
 * The counters below are the honest measure of that. A run whose Behavior
 * counter climbs while 020d27dc's counter climbs with it has ticked the class
 * INTO a hole every one of those frames, and the seat prints both numbers side
 * by side rather than only the green one.
 */

#include <cstdio>

extern "C" {

static unsigned g_amida_floor_27dc;
static unsigned g_amida_floor_36a4;
static unsigned g_amida_floor_ae5c4;

/* THE HOLE IN THE BEHAVIOR'S OWN CALL LIST. 0xe48 bytes, one caller. */
void func_ov006_020d27dc(char *c);
void func_ov006_020d27dc(char *c)
{
    (void)c;
    ++g_amida_floor_27dc;
}

/* THE HOLE ON THE INIT PATH AND THE TICK PATH BOTH. 0x4fc bytes, two callers. */
void func_ov006_020d36a4(void *sb);
void func_ov006_020d36a4(void *sb)
{
    (void)sb;
    ++g_amida_floor_36a4;
}

/* ---- THE THIRD FLOOR, AND IT IS ov004's ---------------------------------
 *
 * func_ov004_020ae5c4, 0x294. Found by the LINK rather than by the delinks
 * walk over this class's own block, because it is one overlay over:
 * config/arm9/overlays/ov004/symbols.txt names it, delinks.txt stops at
 * `end:0x020ae5c4` and no block resumes over it, and no src file defines it in
 * either extension anywhere in src/.
 *
 * THREE OF THIS CLASS's TUs CALL IT -- src/func_ov006_020d1ba0.c (seven sites),
 * _020d2580.c and _020d3ba0.c -- and all three declare it identically:
 *
 *     int func_ov004_020ae5c4(void *a, int b, int c, int d, int e, int f, int g);
 *
 * SEVEN PARAMETERS, AND THE ROM AGREES. Its prologue pushes nine registers and
 * subtracts 0x14, then reads [sp, #0x3c] at 0x020ae5d8 -- 0x38 past the new sp
 * is the FIRST incoming stack word, so 0x3c is the second of three, which is
 * seven arguments with four in r0..r3. The declaration is repeated here rather
 * than invented, which is also what keeps the plain-name arity gate quiet.
 *
 * IT IS ALSO THE FAMILY's SLOT-34 DISPATCHER, which is worth recording because
 * it is the other half of a defect this lane reports and does not fix. Eight of
 * the ten `ldr Rd,[Rn,#0x88]` sites in the two overlay images are inside this
 * body, each with one word pushed before a `blx` -- five arguments to slot 34 --
 * while hal/scene_mg.cpp's shared mb_v34 thunk is declared (void *, void *) and
 * calls the five-parameter src/func_ov004_020ae3b4.c with one argument. Nothing
 * has ever dispatched slot 34 on any scene because this body has no code and the
 * only other dispatcher, func_ov006_020ce108, is in no slice. So the mismatch is
 * unexercised rather than absent, and this trap is why it stays that way here.
 *
 * ITS SINGLE EXIT SETS NO RETURN VALUE (0x020ae84c: add sp / pop / bx lr, with
 * no `mov r0,#N` in front of it), and every one of the ten call sites in this
 * class discards the result, so the trap returns 0 and asserts nothing.
 */
int func_ov004_020ae5c4(void *a, int b, int c, int d, int e, int f, int g);
int func_ov004_020ae5c4(void *a, int b, int c, int d, int e, int f, int g)
{
    (void)a; (void)b; (void)c; (void)d; (void)e; (void)f; (void)g;
    ++g_amida_floor_ae5c4;
    return 0;
}

unsigned port_mg_amida_floor_27dc(void)  { return g_amida_floor_27dc; }
unsigned port_mg_amida_floor_36a4(void)  { return g_amida_floor_36a4; }
unsigned port_mg_amida_floor_ae5c4(void) { return g_amida_floor_ae5c4; }

}  /* extern "C" */
