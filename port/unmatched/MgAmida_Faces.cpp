/* PORT_HOST_ABI. TWO NAMED TRAPS for dScMgAmida_c, actor id 0x173 (scene 371).
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

unsigned port_mg_amida_floor_27dc(void) { return g_amida_floor_27dc; }
unsigned port_mg_amida_floor_36a4(void) { return g_amida_floor_36a4; }

}  /* extern "C" */
