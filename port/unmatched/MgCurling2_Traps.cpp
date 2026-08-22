/* PORT_HOST_ABI. dScMgCurling2_c's TWO UNDECOMPILED BODIES, actor id 0x177,
 * scene 375. Run mg6, lane S75. The func_ov006_020e1854 precedent, twice.
 *
 * ---- THE RULING ----------------------------------------------------------
 *
 * Both have a config symbol in config/arm9/overlays/ov006/symbols.txt, NO
 * delink block in that overlay's delinks.txt, and no src file anywhere defines
 * them. The delink gap is a single run and it holds exactly these two: the
 * block before ends at 0x020e513c and the next begins at 0x020e59b0, and
 *
 *     func_ov006_020e513c   0x314 bytes, ends 0x020e5450
 *     func_ov006_020e5450   0x560 bytes, ends 0x020e59b0
 *
 * fill it end to end with nothing left over. That is a DECOMP gap, not a port
 * one, and neither gets a plausible body: port/tools/inferred_stub_guard.py
 * exists to refuse exactly the invention that would go here.
 *
 * THEY ARE THE ONLY TWO THINGS THE FIRST LINK WANTED. The wave-1 link of this
 * seat closed to LNK1120 with two unresolved externals and they were these,
 * which is the whole frontier port/slice_s75.txt predicted from the source
 * closure. No other body in the class is missing.
 *
 * ---- THE ARITY IS THE ROM'S AND THE TWO CALLERS DISAGREE ABOUT IT --------
 *
 * Read off the ROM at base 0x020bfec0, both bodies open the same way:
 *
 *     020e513c  push {r4..fp,lr} / sub sp,#0x1c / mov fp,r1 / mov r1,#0x30
 *               mul r1,fp,r1 / str r0,[sp] / ...
 *     020e5450  push {r4..fp,lr} / sub sp,#0x7c / mov sb,r1 / mov r1,#0x30
 *               mul r7,sb,r1 / mov sl,r0 / ...
 *
 * so both take (r0 = the object, r1 = an index) and both stride a 0x30 record
 * array by that index. The traps are declared that way.
 *
 * AND ONE CALLER DROPS BOTH ARGUMENTS, WHICH IS A LANDMINE FOR WHOEVER
 * TRANSCRIBES 0x020e5450 AND IS HARMLESS ONLY WHILE THIS FILE STANDS.
 * src/func_ov006_020e5b70.c is a four-instruction TAIL-CALL VENEER onto
 * 0x020e5450 -- on ARM r0 and r1 ride through a tail jump untouched, so the
 * ROM passes the object and the index without naming them -- and src spells
 * the callee `extern void func_ov006_020e5450(void);` and calls it with no
 * arguments. The other caller, src/func_ov006_020e5b7c.c line 78, spells the
 * same symbol `void func_ov006_020e5450(char *c, int a);` and passes both.
 * Both are cdecl at C linkage so both link against one definition and the
 * caller cleans, and the trap below ignores its parameters, so TODAY the
 * mismatch costs nothing.
 *
 * IT WILL COST SOMETHING THE DAY THE BODY ARRIVES. func_ov006_020e5b70 is slot
 * 2 of state table data_ov006_021419d8 (see unmatched/MgCurling2_StateDispatch
 * .cpp), so the live path is
 *
 *     slot 6 -> func_ov006_020e6354 -> c2_call1(021419d8[2])
 *            -> func_ov006_020e5b70()  <- arguments already gone here
 *            -> func_ov006_020e5450(garbage, garbage)
 *
 * and a real 0x020e5450 would stride a 0x30 array by whatever was on the
 * stack. THE FIX WHEN IT COMES IS THE Ov007_ImplicitR0_020add3c SHAPE: a host
 * copy of the VENEER that forwards its own two arguments, not an edit to
 * src/. It is not taken now because there is nothing to forward them to, and
 * writing a forwarder onto a trap would bury the hazard instead of stating it.
 *
 * ---- WHAT THE SCENE LOSES WHILE THEY ARE TRAPPED -------------------------
 *
 * Stated rather than guessed at, from the call graph alone. Both are reached
 * only from state bodies of table data_ov006_021419d8, the four-slot machine
 * func_ov006_020e6354 runs over the eleven 0x30-stride records at +0x4660:
 *
 *     021419d8 slot 0  func_ov006_020e5e3c  -> calls func_ov006_020e513c
 *     021419d8 slot 1  func_ov006_020e5b7c  -> calls func_ov006_020e5450
 *     021419d8 slot 2  func_ov006_020e5b70  -> tail-jumps func_ov006_020e5450
 *     021419d8 slot 3  func_ov006_020e5a0c  -> neither
 *
 * so three of that table's four states enter a trap and return, and the
 * eleven records they would have advanced stay where slot 0 left them. The
 * class boots, the vtable ticks and the state index advances; the per-record
 * machine those three states drive does not. THE COUNTERS BELOW ARE HOW THAT
 * IS REPORTED rather than inferred from a still screen, and
 * hal/scene_mg_curling2.cpp prints them in the scene's exit census.
 */

#include <cstdio>

extern "C" {
void func_ov006_020e513c(char *c, int idx);
void func_ov006_020e5450(char *c, int idx);
unsigned port_mg_curling2_trap_513c(void);
unsigned port_mg_curling2_trap_5450(void);
}

static unsigned g_t513c, g_t5450;

extern "C" unsigned port_mg_curling2_trap_513c(void) { return g_t513c; }
extern "C" unsigned port_mg_curling2_trap_5450(void) { return g_t5450; }

extern "C" void func_ov006_020e513c(char * /*c*/, int /*idx*/)
{
    static int said;
    ++g_t513c;
    if (!said) {
        said = 1;
        std::fprintf(stderr, "  [scene] func_ov006_020e513c (0x314 bytes, "
                     "config symbol, NO delink block, NO src) entered. The "
                     "record machine it drives did NOT run. "
                     "port/unmatched/MgCurling2_Traps.cpp\n");
        std::fflush(stderr);
    }
}

extern "C" void func_ov006_020e5450(char * /*c*/, int /*idx*/)
{
    static int said;
    ++g_t5450;
    if (!said) {
        said = 1;
        std::fprintf(stderr, "  [scene] func_ov006_020e5450 (0x560 bytes, "
                     "config symbol, NO delink block, NO src) entered. The "
                     "record machine it drives did NOT run, and note that one "
                     "of its two callers reached it through a tail-call veneer "
                     "that drops both arguments. "
                     "port/unmatched/MgCurling2_Traps.cpp\n");
        std::fflush(stderr);
    }
}
