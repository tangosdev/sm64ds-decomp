/* HOST COPIES of the UNAGI's (Unagi / eel, actor 242, ov016) state dispatch,
 * and the seat of the ten PMF halves the five dispatch cells read.
 *
 * Unagi drives a five-cell state machine through the bss cells
 * data_ov016_02114d8c / d9c / dac / dbc / d7c. Each cell is 16 bytes: an
 * {ENTER, MAIN} pair of mwcc { fn, delta } PMFs. func_ov016_02111bf0(c, &cell)
 * stores &cell at c+0x34c and dispatches the ENTER half (cell+0) once, on
 * SetState; Unagi::Behavior reads the stored pointer back and dispatches the
 * MAIN half (cell+8) every frame. Both are the Scuttlebug __thiscall/stride
 * trap and must be host copies:
 *
 *   - func_ov016_02111bf0 forms the ROM's `add r0,r0,r1,asr#1` PMF this-adjust
 *     + virtual-bit test over cell[0]; on MSVC the recovered `(c->**q)()` reads
 *     the wrong stride. Read cell+0 as a plain { fn, 0 } and call fn(this).
 *   - Unagi::Behavior inlines the same dispatch over cell+8 (the MAIN half,
 *     ROM offset +8 confirmed by disassembly: ldr r0,[r1,#8]; add r3,r1,#8;
 *     ldreq r1,[r3]; blx r1). So the whole Behavior is host-copied here (the
 *     LavaBubble/Amilift-Behavior treatment) with the dispatch respelled.
 *
 * All ten .delta halves are 0 (non-virtual complete-class form, per the overlay
 * bytes), so the host dispatch is just "read the fn word, call it with this".
 *
 * THE SEAT rewrites each of the ten SOURCE records' fn word with its host body
 * BEFORE __sinit_ov016_021136ec copies the ten 8-byte source values into the
 * five cells (the MontyMole/Scuttlebug order: seat the source, before the copy).
 * ALL TEN are matched src (func_ov016_02111bac / 19ec / 1860 / 1758 / 1994 /
 * 18b4 / 1718 / 15c0 / 1534, and BookSwitch_Spawn). The tenth, cell d7c's
 * ENTER, is the tiny 7-instruction path-arrival state-enter at 0x021115a4 that
 * dsd mis-symbolized as "BookSwitch_Spawn" -- it is NOT a factory, it sets a
 * timer and a speed and returns 1.
 *
 * 09-19, run link100 wave 14, lane SEAT14B: that tenth body was a hand
 * transcription here (unagi_state_arrive); src/BookSwitch_Spawn.c is the same
 * seven instructions and now takes the seat, off port/slice_small7.txt. Read
 * out of extracted/overlays/overlay_0016.bin at base 0x021111a0, the body is a
 * real standalone leaf entry -- ADD r1,r0,#0x100 / MOV r2,#100 / STRH r2,[r1] /
 * MOV r1,#0x14000 / STR r1,[r0,#0x98] / MOV r0,#1 / BX lr -- so it is not an
 * interior address of a larger function. The dispatch discards the return
 * value, which is the only thing the retired transcription left out.
 *
 * Each source's ROM fn is checked against the mount before it is overwritten, so
 * a mount pointing at the wrong bytes aborts instead of seating a stale address.
 */
#include "hal/pmf_seat5.h"
#include <cstdio>
#include <cstdlib>

extern "C" {

/* the five 16-byte dispatch cells (bss, filled by __sinit_ov016_021136ec) */
struct PortPmf { unsigned fn; int delta; };
extern PortPmf data_ov016_02114d8c[2], data_ov016_02114d9c[2],
    data_ov016_02114dac[2], data_ov016_02114dbc[2], data_ov016_02114d7c[2];

/* the ten SOURCE records the sinit copies from (each { fn, 0 }); nine matched
   src, the tenth the mis-named 021115a4 path-arrival enter. Named for the seat
   check; addresses are the ov016 mount's own. */
extern PortPmf data_ov016_021148a0, data_ov016_021148a8;  /* d8c enter/main */
extern PortPmf data_ov016_021148b8, data_ov016_021148c0;  /* d9c enter/main */
extern PortPmf data_ov016_02114888, data_ov016_02114880;  /* dac enter/main */
extern PortPmf data_ov016_021148b0, data_ov016_02114890;  /* dbc enter/main */
extern PortPmf BookSwitch_SpawnInfo, data_ov016_02114878; /* d7c enter/main */

/* the ten matched state bodies (nine on slice_gate188.txt, the d7c ENTER on
   slice_small7.txt) */
int BookSwitch_Spawn(void *c);       /* d7c enter, ov016 0x021115a4 */
void func_ov016_02111bac(void *c);   /* d8c enter */
void func_ov016_021119ec(void *c);   /* d8c main  */
void func_ov016_02111860(void *c);   /* d9c enter */
void func_ov016_02111758(void *c);   /* d9c main  */
void func_ov016_02111994(void *c);   /* dac enter */
void func_ov016_021118b4(void *c);   /* dac main  */
void func_ov016_02111718(void *c);   /* dbc enter */
void func_ov016_021115c0(void *c);   /* dbc main  */
void func_ov016_02111534(void *c);   /* d7c main  */

/* the whole-Behavior call graph (everything except the inlined PMF dispatch) */
unsigned short DecIfAbove0_Short(unsigned short *p);
void _ZN8dActor_c9UpdatePosEP5dCc_c(void *self, void *cc);
void *_ZN8dActor_c10FindWithIDEj(unsigned id);
void *_ZN8dActor_c13ClosestPlayerEv(void *self);
int Vec3_Dist(const void *a, const void *b);
void _ZN7fBase_c18MarkForDestructionEv(void *self);
void *_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(unsigned a, unsigned b,
        const void *pos, const void *rot, int e, int f);
void _ZN5dCc_c5ClearEv(void *self);
void _ZN5dCc_c6UpdateEv(void *self);
void _ZN14BlendModelAnim7AdvanceEv(void *self);
void func_ov016_02111c40(void *c);
void func_ov016_02111284(void *c);
extern unsigned char data_0209f220;

}  /* extern "C" */

/* The d7c ENTER (0x021115a4, size 0x1c) is src/BookSwitch_Spawn.c now. The
 * hand transcription that stood here read, from the overlay bytes:
 *   add r1,r0,#0x100; mov r2,#0x64; strh r2,[r1]   -> *(u16*)(this+0x100)=100
 *   mov r1,#0x14000;  str r1,[r0,#0x98]             -> *(int*)(this+0x98)=0x14000
 *   mov r0,#1; bx lr                                -> return 1
 * and the matched TU is those same three statements with the return kept. The
 * cast below is the one the MAIN seats already use: both sides are cdecl with
 * one pointer argument, so the discarded int return costs nothing.
 */

/* PORT_HOST_ABI: SetState + dispatch the ENTER half. The matched
 * func_ov016_02111bf0 forms the ROM PMF adjust over cell[0]; here cell+0 is a
 * plain { fn, 0 } and fn is called with `this`. Also stands in for
 * func_ov018_02111bf0, the cross-overlay alias typo in the recovered src,
 * bridged onto this body by port/unmatched/Jrb_Aliases.cpp's own
 * /alternatename pragma (forward-declared on the next line, matching
 * Jrb_Aliases.cpp's own (void*,void*) signature, so this tag documents
 * that bridged name too). */
extern "C" int func_ov018_02111bf0(void *c, void *cell);
/* func_ov016_02111bf0 IS NOT A HOST COPY ANY MORE. Run link100 lane PMF2 put
   src/func_ov016_02111bf0.cpp back on port/slice_pmf2.txt (batch 2): with /vmg /vmm
   global MSVC's pointer-to-member IS the ROM's 8-byte {function, delta}
   pair, the matched TU compiles to the same tail jump this body was, and
   the seat in this file aborts the binary on a nonzero delta so the two
   agree word for word. The reading above is kept as the derivation. */
/* HOST COPY RETIRED, run link100 lane SEAT5, census batch B5 (PMF-SINGLES).
   src/_ZN5Unagi8BehaviorEv.cpp dispatches its own field now: with /vmg /vmm
   (block R8) MSVC's pointer to member IS the ROM's eight-byte {code, adjust}
   pair, so the widening this file's banner was written for does not happen.
   The TU's own /FAsc listing under the port's flags is

       mov eax,DWORD PTR [ecx+8]    the MAIN half's code word, ROM offset +8
       test eax,eax / je            the ROM's own null guard
       mov ecx,DWORD PTR [ecx+12]   the adjust word at cell+0xc
       add ecx,edi                  receiver = this + adjust
       call eax                     a CALL, receiver in ecx, NOTHING pushed

   -- ARITY ZERO, /Zp4 changes 0 listing lines outside the TITLE (the member
   sits at offset 8 of its holder, 8 mod 8 = 0). So each of the FIVE MAIN
   records now holds a zero-argument __fastcall face that forwards the receiver
   as the one cdecl argument the ROM's own state body takes.

   THE ENTER RECORDS DO NOT CHANGE. They are reached by func_ov016_02111bf0,
   itself a matched TU (port/slice_pmf2.txt) that MSVC compiles as a one-call
   forwarder ending in jmp, so the caller's own cdecl frame is reused and a
   plain cdecl body is right there.

   THE UNIVERSE IS CLOSED. ovblock.py over Unagi's own class code block
   (021111a0..02112588, derived from _ZTV5Unagi at 02114958) gives TEN {code,0}
   pairs in ONE SOLID run 02114878..021148c8, ten distinct code words, and every
   reference into that run is __sinit_ov016_021136ec -- the overlay's own sinit,
   which copies them PAIRWISE into the five 16-byte bss cells named above (read
   off the sinit at 02113764..0211383c). Nothing else in ov016 can hand this
   field an address. NO BY-VALUE SENTINEL: the one comparison Behavior makes,
   `*(void**)(this+0x34c) != &data_ov016_02114dbc`, is on the CELL ADDRESS, not
   on a stored pair, and the seat does not move any cell. Measurements in
   runs/link100/out/SEAT5/ (sweep_unagi.txt, dis_unagi_sinit.txt,
   emit_seat5_out.txt) and port/slice_seat5.txt.

   The flat C name _ZN5Unagi8BehaviorEv the actor class table calls is defined
   in port/hal/pmf_seat5.cpp: the matched TU is a real C++ member and publishes
   ?Behavior@Unagi@@QAEHXZ. */
static void __fastcall seat5_unagi_main_021119ec(void *self, void *dead_edx)
{ (void)dead_edx; port_seat5_count(0); func_ov016_021119ec(self); }  /* d8c MAIN */
static void __fastcall seat5_unagi_main_02111758(void *self, void *dead_edx)
{ (void)dead_edx; port_seat5_count(1); func_ov016_02111758(self); }  /* d9c MAIN */
static void __fastcall seat5_unagi_main_021118b4(void *self, void *dead_edx)
{ (void)dead_edx; port_seat5_count(2); func_ov016_021118b4(self); }  /* dac MAIN */
static void __fastcall seat5_unagi_main_021115c0(void *self, void *dead_edx)
{ (void)dead_edx; port_seat5_count(3); func_ov016_021115c0(self); }  /* dbc MAIN */
static void __fastcall seat5_unagi_main_02111534(void *self, void *dead_edx)
{ (void)dead_edx; port_seat5_count(4); func_ov016_02111534(self); }  /* d7c MAIN */

static const struct { PortPmf *rec; unsigned rom; void (*host)(void *); }
g_unagi_sources[] = {
    {&data_ov016_021148a0, 0x02111bac, func_ov016_02111bac},  /* d8c enter */
    {&data_ov016_021148a8, 0x021119ec,
     (void (*)(void *))(void *)seat5_unagi_main_021119ec},  /* d8c main  */
    {&data_ov016_021148b8, 0x02111860, func_ov016_02111860},  /* d9c enter */
    {&data_ov016_021148c0, 0x02111758,
     (void (*)(void *))(void *)seat5_unagi_main_02111758},  /* d9c main  */
    {&data_ov016_02114888, 0x02111994, func_ov016_02111994},  /* dac enter */
    {&data_ov016_02114880, 0x021118b4,
     (void (*)(void *))(void *)seat5_unagi_main_021118b4},  /* dac main  */
    {&data_ov016_021148b0, 0x02111718, func_ov016_02111718},  /* dbc enter */
    {&data_ov016_02114890, 0x021115c0,
     (void (*)(void *))(void *)seat5_unagi_main_021115c0},  /* dbc main  */
    {&BookSwitch_SpawnInfo, 0x021115a4,
     (void (*)(void *))(void *)BookSwitch_Spawn},  /* d7c enter */
    {&data_ov016_02114878, 0x02111534,
     (void (*)(void *))(void *)seat5_unagi_main_02111534},  /* d7c main  */
};

extern "C" void port_unagi_states_seat(void)
{
    static int done;
    if (done)
        return;
    done = 1;
    for (unsigned i = 0; i < sizeof g_unagi_sources /
                             sizeof g_unagi_sources[0]; ++i) {
        PortPmf *p = g_unagi_sources[i].rec;
        if (p->fn != g_unagi_sources[i].rom || p->delta != 0) {
            std::fprintf(stderr, "FATAL: Unagi source %u: the mount holds "
                         "%08x/%d, the ROM's own table says %08x/0 -- WRONG "
                         "BYTES\n", i, p->fn, p->delta, g_unagi_sources[i].rom);
            std::abort();
        }
        p->fn = (unsigned)(size_t)g_unagi_sources[i].host;
    }
}
