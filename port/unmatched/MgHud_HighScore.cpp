/* PORT_HOST_ABI. func_ov004_020b19f0, the minigame HUD's HIGH SCORE drawer,
 * plus func_ov006_020fba48, dScMgPachinko_c's call into it.
 * Run mg5, lane HSDISP.
 *
 * WHY THIS FILE EXISTS: the owner reported "HIGH SCORE 999999" on scene 368 on
 * a build where the STORED high score, and both getters, measure zero:
 *
 *     [score:postctor] +0x4650 HIGH SCORE = 0 (0x00000000)
 *     [score:postctor] getters: func_ov004_020adc1c()=0 func_ov004_020adbc0()=0
 *
 * so the number on screen is not the number in memory and the defect is
 * between the getter and the glyphs. It is a dropped argument, the same family
 * as port/unmatched/MgPachinko_Award.cpp, MgBase_Slot7.cpp and
 * MgSmartball_Slot18.cpp, and port/tools/aritycheck.py reports it as a row.
 *
 * ---- THE ROM, DISASSEMBLED --------------------------------------------
 *
 * extracted/overlays/overlay_0006.bin at base 0x020bfec0. NOT the dsd export:
 * extracted/dsd/arm9_overlays/ov006.bin is the compressed payload and
 * disassembles into plausible-looking ARM at a drifting offset, which is how
 * a reader can convince himself he has the wrong function.
 *
 *     020FBA48  stmdb sp!, {lr}          func_ov006_020fba48
 *     020FBA4C  sub   sp, sp, #4
 *     020FBA50  bl    0x20adc1c          ; r0 = *(u32*)(scene + 0x4650)
 *     020FBA54  bl    0x20b19f0          ; r0 RIDES THROUGH, all 32 bits
 *     020FBA58  add   sp, sp, #4
 *     020FBA5C  ldm   sp!, {lr}
 *     020FBA60  bx    lr
 *
 * and extracted/overlays/overlay_0004.bin at base 0x020ad660:
 *
 *     020ADC1C  ldr   r0, [pc, #0x14]     func_ov004_020adc1c, NO parameters
 *     020ADC20  ldr   r0, [r0]
 *     020ADC24  cmp   r0, #0
 *     020ADC28  addne r0, r0, #0x4000
 *     020ADC2C  ldrne r0, [r0, #0x650]    ; scene + 0x4650, a full u32
 *     020ADC30  moveq r0, #0
 *     020ADC34  bx    lr
 *
 *     020B19F0  push  {r4, lr}            func_ov004_020b19f0
 *     020B19F4  sub   sp, sp, #0x10
 *     020B19F8  mov   r4, r0              ; <-- ONE parameter, the value
 *     020B19FC  bl    0x20ad674           ; GetGameLanguage
 *     ...
 *     020B1A2C  mov   r2, r4              ; the value, third argument
 *     020B1A38  mov   r0, #0x48           ; x
 *     020B1A3C  mov   r1, #0xc            ; y
 *     020B1A40  mov   r3, #1
 *     020B1A48  bl    0x20b1ea4           ; func_ov004_020b1ea4
 *
 * So the chain is (int)(void) -> (int) and the value crossing both calls is a
 * full 32-bit word.
 *
 * ---- WHAT src SPELLS --------------------------------------------------
 *
 * src/func_ov006_020fba48.c is a recovered decompile that got the whole shape
 * wrong -- its banner even calls the function "converts a sublevel ID to a
 * course ID, then opens the cannon", which is not what 0x020fba48 does:
 *
 *     typedef signed char s8;
 *     extern s8   func_ov004_020adc1c(s8 levelID);
 *     extern void func_ov004_020b19f0(s8 courseID);
 *     void func_ov006_020fba48(s8 levelID)
 *     {
 *         s8 courseID = func_ov004_020adc1c(levelID);
 *         func_ov004_020b19f0(courseID);
 *     }
 *
 * Three disagreements with the ROM, in one TU:
 *
 *   1. func_ov004_020adc1c is declared to RETURN s8. Its definition, and the
 *      ROM, return a 32-bit score. Everything above 0xff is thrown away here.
 *   2. func_ov004_020b19f0 is declared to TAKE s8. The ROM takes the 32-bit
 *      value in r0 and src/func_ov004_020b19f0.c defines the parameter as a
 *      full word. The caller writes one byte of the outgoing argument.
 *   3. func_ov006_020fba48 is declared to take a parameter at all. The ROM
 *      takes none, and src/func_ov006_020fedc4.c passes the scene pointer.
 *
 * On ARM (2) costs nothing that shows: mwccarm still moves a whole register.
 * On the host it is the whole defect, and MSVC's own output for the src TU is
 * the proof (dumpbin /disasm:nobytes of func_ov006_020fba48.c.obj, 32-bit
 * Release, this tree):
 *
 *     00000003  push  ecx                  ; 4 bytes for the s8 local, NOT
 *                                          ;   initialised
 *     00000004  push  dword ptr [ebp+8]    ; the invented parameter
 *     00000007  call  _func_ov004_020adc1c
 *     0000000C  mov   byte ptr [ebp-4],al  ; ONE byte of the slot is written
 *     0000000F  push  dword ptr [ebp-4]    ; ALL FOUR are handed over
 *     00000012  call  _func_ov004_020b19f0
 *
 * The drawer reads all thirty-two bits, so it receives the low byte of the
 * score with three bytes of stack litter above it. Both outcomes the owner and
 * this lane saw come from that one word:
 *
 *   * litter that lands POSITIVE and above the cap paints 999999, because
 *     func_ov004_020b1ea4 opens with `if (val >= 0xf423f) val = 0xf423f`.
 *     That is the owner's screen.
 *   * litter that lands NEGATIVE misses the clamp entirely -- the test is
 *     signed -- so no digit loop runs, d[0] stays hugely negative, and
 *     data_ov006_02137cd8[d[0] + fb] indexes megabytes outside the glyph
 *     table. That is the "garbage glyph pointer out of data_ov006_02137cd8"
 *     an earlier lane saw as a nondeterministic fault and left alone. The
 *     glyph table is not mis-seated; its INDEX is garbage.
 *
 * The stored word never moves in either case, which is why the trace reads
 * zero while the screen reads nines.
 *
 * MEASURED HERE, scene 368, SM64DS_MG_SCORE_TRACE=1, SM64DS_FAULTS_FATAL=1:
 *
 *     before   [hud:hiscore] draw1: drawer received 2180799488 (0x81fc5c00)
 *                            func_ov004_020adc1c() = 0
 *              and the run died at 0xC0000005 on that first draw
 *     after    [hud:hiscore] draw1: drawer received 0 (0x00000000)
 *                            func_ov004_020adc1c() = 0
 *              and 400 frames run clean
 *
 * The low byte of the before value is 0x00 -- the score that WAS delivered.
 * 0x81fc5c is the litter, and it is the whole difference between the two runs.
 *
 * ---- WHAT THIS FILE IS ------------------------------------------------
 *
 * ONE body now, lane LINKMG. The file used to carry a second, a copy of
 * src/func_ov004_020b19f0.c with the parameter retyped from void * to int.
 * That retype was never load-bearing on the host: the src TU is plain C, its
 * one parameter and func_ov004_020b1ea4's third argument are both 4-byte
 * cdecl slots whether spelled void * or int, and MSVC compiles the src TU
 * as-is with no diagnostic that changes code. The defect this file exists for
 * was always in the CALLER below, so the drawer is back on port/slice_mg1.txt
 * as the matched TU and only the caller stays here.
 *
 *   func_ov006_020fba48   src/func_ov006_020fba48.c verbatim except the two
 *                         declarations and its own signature take the widths
 *                         the ROM takes. Out of port/slice_pch.txt for that
 *                         reason. The SM64DS_MG_SCORE_TRACE witness moved
 *                         here from the drawer copy; it prints the same two
 *                         numbers at the same instant.
 *
 * src/ and include/ are untouched; the byte gate never sees this file.
 *
 * THE OTHER THREE CALLERS OF 0x020b19f0 HAVE THE SAME BREAK and are NOT fixed
 * here, because they belong to classes this lane does not own. All four ROM
 * sites are the identical `bl 0x20adc1c; bl 0x20b19f0` r0 ride-through:
 *
 *     0x020e34ec   src/func_ov006_020e34ec.c    declares (int), passes the
 *                                               getter's result -- CORRECT
 *     0x020e67f0   src/func_ov006_020e67f0.c    declares (), calls with none
 *     0x021004c0   src/func_ov006_021004c0.c    declares (void), calls with
 *                                               none
 *
 * The last two draw whatever the outgoing slot happened to hold. They are
 * recorded in the lane report rather than changed under this branch.
 */

#include <cstdio>
#include <cstdlib>

typedef int s32;

extern "C" {

/* The drawer is the matched TU src/func_ov004_020b19f0.c (C linkage, one
 * 4-byte parameter it spells void *); declared here at the width the ROM
 * passes, which is the same slot. */
int  func_ov004_020b19f0(int score);
int  func_ov004_020adc1c(void);

/* Read-only witness for the measurement this file exists to make. Off unless
 * SM64DS_MG_SCORE_TRACE is set, so no battery run changes shape. It prints the
 * value the drawer is HANDED next to the value the getter returns at the same
 * instant: those two numbers are what separate a dropped argument from a
 * mis-seated glyph table, and reading the picture is not allowed to stand in
 * for either of them. */
static bool hud_hiscore_trace(void)
{
    static int on = -1;
    if (on < 0) {
        const char *e = std::getenv("SM64DS_MG_SCORE_TRACE");
        on = (e && e[0] && e[0] != '0') ? 1 : 0;
    }
    return on != 0;
}

/* src/func_ov006_020fba48.c verbatim, with the ROM's widths. The parameter the
 * src TU invented is gone because 0x020fba48 reads none -- exactly as
 * src/func_ov006_020fba28.cpp, the SCORE half of the same HUD, is already
 * spelled (void) against the same (void *) declaration in
 * src/func_ov006_020fedc4.c. The extra argument that declaration pushes is
 * harmless under cdecl: the caller cleans it up. */
/* PORT_HOST_ABI: ARM r0 ride-through; src declares the 32-bit high score as s8 at both ends and invents a parameter the ROM does not take, so MSVC hands the drawer one byte plus stack litter */
void func_ov006_020fba48(void)
{
    int score = func_ov004_020adc1c();
    if (hud_hiscore_trace()) {
        static unsigned n;
        ++n;
        if (n <= 3 || (n % 100) == 0) {
            std::printf("[hud:hiscore] draw%u: drawer handed %u (0x%08x)"
                        "   func_ov004_020adc1c() = %d%s\n",
                        n, (unsigned)score, (unsigned)score, score,
                        (score >= 0xf423f) ? "   <-- OVER THE 999999 CLAMP" : "");
            std::fflush(stdout);
        }
    }
    func_ov004_020b19f0(score);
}

}  /* extern "C" */
