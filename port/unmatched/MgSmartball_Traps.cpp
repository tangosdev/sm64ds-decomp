/* THREE NAMED TRAPS for dScMgSmartball_c, actor id 0x178 (scene 376).
 * Run mg5, lane SMB.
 *
 * THE PRECEDENT IS func_ov006_020e1854, dScMgCurling_c's twenty-fifth state:
 * an address the ROM reaches, no delink block, no src TU in either extension,
 * and therefore no body this tree may write. What goes in its place is a
 * trap that SAYS WHICH ADDRESS WAS ENTERED and does nothing else. A plausible
 * body would be worse than a missing one, because a plausible body cannot be
 * told apart from a real one by anything downstream, and
 * port/tools/inferred_stub_guard exists to refuse exactly that.
 *
 * THE THREE, and each is a genuine hole in config/arm9/overlays/ov006/
 * delinks.txt rather than a name-shaped lookup miss:
 *
 *   func_ov006_02118b70   VTABLE SLOT 0, InitResources.
 *       The delink list runs `src/func_ov006_02118ae4.c .text start:0x02118ae4
 *       end:0x02118b70` and then jumps to `src/func_ov006_0211944c.c .text
 *       start:0x0211944c`. The 0x8dc bytes between are this body and nothing
 *       covers them. config/arm9/overlays/ov006/symbols.txt does not name the
 *       address at all.
 *
 *   func_ov006_021173c8   VTABLE SLOT 9, Render.
 *       Same shape: `src/func_ov006_02115b0c.c .text start:0x02115b0c
 *       end:0x021173c8` then `src/func_ov006_02118488.c .text
 *       start:0x02118488`. The 0x10c0 bytes between are this body.
 *
 *   func_ov006_02114800   NOT a vtable slot, which is why the cost table's
 *       nosrc column says two and this file stands three. symbols.txt DOES
 *       name it -- `func_ov006_02114800 kind:function(arm,size=0x310)` -- and
 *       no delink block covers it, which is the case
 *       port/tools/stategen.py's hole message was corrected to distinguish: a
 *       named symbol is not a decompiled body. It is reached from five call
 *       sites in ov006 (0x0210f7d8, 0x0210f804, 0x0210f848, 0x02111604,
 *       0x02111e6c); the one inside this class's closure is 0x02111e6c, in
 *       src/func_ov006_02111e48.c, so the trap is what keeps that slice line
 *       linkable.
 *
 * THE SIGNATURES ARE READ OUT OF THE ROM AND OUT OF THE CALLERS, NOT CHOSEN.
 *
 *   func_ov006_02114800 is declared `void func_ov006_02114800(int a, void *p,
 *   int c)` by BOTH of its decompiled callers, src/func_ov006_02111e48.c:1 and
 *   src/func_ov006_021115cc.c:1. This file repeats that declaration rather
 *   than inventing one.
 *
 *   The two vtable bodies take `self` in r0 and return 1. Both facts are
 *   measured: 0x02118b70's prologue is `push {r4,r5,r6,r7,lr}` then
 *   `mov r4, r0`, and its SINGLE epilogue is
 *
 *       02119384  mov r0, #1
 *       02119388  add sp, sp, #0x1c
 *       0211938c  pop {r4, r5, r6, r7, lr}
 *       02119390  bx  lr
 *
 *   0x021173c8's prologue is `push {r4..fp,lr}` then `mov sl, r0`, and its
 *   single epilogue is
 *
 *       02118478  mov r0, #1
 *       0211847c  add sp, sp, #0x84
 *       02118480  pop {r4, r5, r6, r7, r8, sb, sl, fp, lr}
 *       02118484  bx  lr
 *
 * RETURNING 1 IS A MEASUREMENT, NOT A GUESS, and the distinction is the whole
 * argument for it. Each body has exactly one exit and that exit's return value
 * is a constant in the image. A trap that returned 0 would be asserting
 * something about these functions that the ROM does not say; a trap that
 * returns the constant the ROM returns asserts only what is in the bytes. What
 * is missing is the BODY, and nothing here pretends otherwise: neither
 * function touches the object, and both report themselves.
 *
 * WHAT THAT COSTS THE BOOT, said plainly because it is this seat's headline
 * result rather than a caveat. Slot 0 is where the class builds every one of
 * its sub-objects, so with slot 0 trapped the object's count fields stay zero
 * and its sub-object pointers stay null. Slot 6 case 0 dereferences
 * self+0x4684 without a null check on its first statement. So this class
 * cannot run its own frames until slot 0 is decompiled, and this file is where
 * a reader finds out why rather than a fault address being the only evidence.
 *
 * DECOMPILING THE THREE IS A BYTE-GATED-TREE JOB. It is routed and not taken
 * here: three bodies of 0x8dc, 0x10c0 and 0x310 bytes, all inside delink holes
 * that would have to be split first, is a decomp lane's work and not a port
 * lane's.
 */

#include <cstdio>

static unsigned g_smb_trap_hits;
static int g_smb_said[3];

static void smb_trap(int which, const char *name, const char *what)
{
    ++g_smb_trap_hits;
    if (!g_smb_said[which]) {
        g_smb_said[which] = 1;
        std::fprintf(stderr, "  [scene] dScMgSmartball_c UNDECOMPILED BODY "
                     "entered: %s -- %s. No delink block and no src TU; "
                     "port/unmatched/MgSmartball_Traps.cpp.\n", name, what);
        std::fflush(stderr);
    }
}

extern "C" {

unsigned port_mg_smartball_trap_hits(void) { return g_smb_trap_hits; }

/* which of the three has been entered, for the run report */
unsigned port_mg_smartball_trap_mask(void)
{
    return (unsigned)(g_smb_said[0] | (g_smb_said[1] << 1) | (g_smb_said[2] << 2));
}

/* Run mg5, lane INTEG: slot 0 InitResources (func_ov006_02118b70) and the aux
   ball-table seeder (func_ov006_02114800) were recovered on branch
   decomp/smb-bodies and are now sliced, so their traps are gone. InitResources
   is reached through the alias in MgSmartball_Faces.cpp; the aux body carries
   its own name. Only the Render trap remains: its recovered form on main is the
   __thiscall member dScMgSmartball_c::Render, which an alias cannot bridge to
   the cdecl func_ov006_021173c8 the dispatch calls. */
int func_ov006_021173c8(void *)
{
    smb_trap(1, "func_ov006_021173c8", "vtable slot 9, Render");
    return 1;
}

}  /* extern "C" */
