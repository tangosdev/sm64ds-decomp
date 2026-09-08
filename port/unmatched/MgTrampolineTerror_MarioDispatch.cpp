/* PORT_HOST_ABI.  The 3D-MARIO record's member-pointer wall -- the SECOND
 * field-held pmf in dScMgTrampoline2_c's closure, and the one a `::*` sweep
 * finds.  Run mg11, lane TTE.  actor id 0x181 = scene 385.
 *
 * ---- 1. WHERE IT SITS ----------------------------------------------------
 *
 * The factory builds TEN records of 0x1d0 bytes at this+0x5ddc (element ctor
 * func_ov006_021248a8, element dtor func_ov006_021227c8 -- the ROM's own r3 and
 * [sp] columns, see unmatched/MgTrampolineTerror_Factory.cpp).  Each record
 * carries an mwcc member pointer at OFFSET 0, and two TUs dispatch it:
 *
 *     src/func_ov006_020cf758.cpp   struct C { PMF pmf; };  (c->*(c->pmf))();
 *     src/func_ov006_020cea2c.cpp   PMF *pp = (PMF *)c;  (((C *)c)->**pp)();
 *
 * Both are real `T::*` declarations, so the prescribed `::*` sweep finds them.
 * THIS LANE RAN THAT SWEEP OVER ALL 216 TUs of its closure -- the 204 slice
 * lines PLUS the 12 TUs it host-copies or repairs, which are not slice lines and
 * would otherwise never be swept -- and it returns exactly THREE files: these
 * two and src/func_ov006_02123340.cpp (the class's own vtable slot 6,
 * unmatched/MgTrampolineTerror_StateDispatch.cpp).
 *
 * THE FOURTH WALL IS NOT ONE OF THEM, and saying it was is the error the mg11
 * review caught. src/func_ov006_020c8f20.cpp declares no `T::*` anywhere; it
 * open-codes the decode in plain ints, which is precisely why
 * unmatched/MgTrampolineTerror_SubDispatch.cpp exists and why a RUN was needed
 * to convict it. The lane's sweep tool reports PMF-SHAPED files -- its `::*` arm
 * and its open-coded arm added together -- and reading that total as the `::*`
 * answer put a file inside the detector it is invisible to. FOUR WALLS, THREE
 * FOUND BY THE SWEEP, ONE FOUND BY A RUN.
 *
 * IT WAS FOUND BY A RUN BEFORE THE SWEEP WAS RUN, which is the honest order and
 * the reason the sweep is now written down.  A 3000-frame RENDERED boot of
 * scene 385 faulted with
 *
 *     FAULT code c0000005 at +0x00093129 accessing 3083e230
 *       func_ov006_020cea2c
 *       func_ov006_020cedf0        (walks data_ov006_0214082c, stride 0x1d0)
 *       func_ov006_02123cb4        (this class's PLAY state)
 *       func_ov006_02123340 / tte_beh
 *
 * -- MSVC's four-byte member pointer read half of one eight-byte record and
 * dispatched on it.
 *
 * ---- 2. THE PAIR UNIVERSE, BOUNDED FROM THE ROM'S OWN RUN -----------------
 *
 * Sixteen writers name their source and every one of them is a named mwcc pair
 * in ONE CONTIGUOUS RUN of ov006 .data, 0x0213b31c .. 0x0213b39c, read out of
 * extracted/overlays/overlay_0006.bin at base 0x020bfec0.  A sample of the
 * writers, each `*(struct S *)c = <pair>`:
 *
 *     src/func_ov006_020cdad0.c  data_ov006_0213b31c   {0x020cd9b0, 0}
 *     src/func_ov006_020cd7b8.c  data_ov006_0213b32c   {0x020cd744, 0}
 *     src/func_ov006_020cdf20.c  data_ov006_0213b34c   {0x020cdf1c, 0}
 *     src/func_ov006_020cdeec.c  data_ov006_0213b354   {0x020cdea0, 0}
 *     src/func_ov006_020cde7c.c  data_ov006_0213b35c   {0x020cde4c, 0}
 *     src/func_ov006_020cfc58.c  data_ov006_0213b364   {0x020cfa44, 0}
 *     src/func_ov006_020cd98c.c  data_ov006_0213b36c   {0x020cd864, 0}
 *     src/func_ov006_020cfa28.c  data_ov006_0213b374   {0x020cf820, 0}
 *     src/func_ov006_020cf804.c  data_ov006_0213b37c   {0x020cf790, 0}
 *     src/func_ov006_020cdce4.c  data_ov006_0213b384   {0x020cdc8c, 0}
 *     src/func_ov006_020cdc68.c  data_ov006_0213b38c   {0x020cdc38, 0}
 *     src/func_ov006_020cdc14.c  data_ov006_0213b394   {0x020cdaec, 0}
 *     src/func_ov006_020cde28.c  data_ov006_0213b39c   {0x020cdd08, 0}
 *
 * The run's ENDS are where it stops being pairs: 0x0213b314 is the ASCII word
 * 0x676e6972 ("ring") and 0x0213b3a4 is a five-word record ({0x1e, 0x0212e05c,
 * 0x0212e054, 0x0212e130, 1, 0x0212e094}) that is plainly a table of pointers
 * and counts.  Every adjustment word inside the run is ZERO.
 *
 * FOURTEEN DISTINCT CODE WORDS, all with a matched src TU and a delinks block;
 * all fourteen are switched below and their TUs are slice lines.  Anything not
 * covered falls through to the framework, which prints the address on an
 * UNHANDLED line and counts it -- a run with zero UNHANDLED is the evidence,
 * this header is not.
 *
 * ---- 3. THE STORED PAIR IS NOT REWRITTEN ---------------------------------
 *
 * Routing happens at the two dispatch SITES.  The pairs stay the ROM's own
 * words, for the reason unmatched/MgMemory2_FieldPmf.cpp section 3 gives and
 * port/mg_fanout_costs.txt section 4 derives from the flower: sixteen writers
 * copy these records by value and at least one consumer in this family compares
 * a stored pair word for word (src/func_ov006_020c94e0.cpp does it one record
 * class over), so host addresses in the .data would make a comparison
 * permanently false while the dispatch itself looked fine.
 *
 * ---- 4. THE TWO HOST COPIES ARE THEIR src TUs WITH THE DISPATCH ROUTED ----
 *
 * func_ov006_020cf758 is one statement and the statement is the dispatch.
 * func_ov006_020cea2c keeps every other line verbatim -- the AddVec3 on
 * c+8 / c+0x2c, the Animation::Advance at c+0x194 and the nine field copies, in
 * src's order and at src's offsets.  Both src TUs are OUT of
 * port/slice_tte.txt: listing either would be an LNK2005.
 */

#include <cstdio>
#include <cstdlib>   /* run link100 lane FWD gate 3: the seat aborts on wrong bytes */

struct MgPmf { unsigned code; int adj; };

extern "C" {

void port_mg_call0(void *self, unsigned code, int adj);

/* the fourteen distinct code words in 0x0213b31c..0x0213b39c */
void func_ov006_020cd744(void *c);
void func_ov006_020cd864(void *c);
void func_ov006_020cd9b0(void *c);
void func_ov006_020cdaec(void *c);
void func_ov006_020cdc38(void *c);
void func_ov006_020cdc8c(void *c);
void func_ov006_020cdd08(void *c);
void func_ov006_020cde4c(void *c);
void func_ov006_020cdea0(void *c);
void func_ov006_020cdf1c(void *c);
void func_ov006_020cdf3c(void *c);
void func_ov006_020cf790(void *c);
void func_ov006_020cf820(void *c);
void func_ov006_020cfa44(void *c);

/* src/func_ov006_020cea2c.cpp's own callees, spelled as that file spells them */
void AddVec3(void *a, void *b, void *c);
void _ZN9Animation7AdvanceEv(void *anim);

/* run link100 lane FWD gate 3: both are src TUs on port/slice_fwd.txt now,
   not definitions in this file. The declarations stay because the class's
   census and its factory both name them. */
void func_ov006_020cea2c(char *c);
void func_ov006_020cf758(void *c);

}  /* extern "C" */

static unsigned g_tte_mario_calls, g_tte_mario_routed;

static int tte_mario_try(void *self, unsigned code)
{
    switch (code) {
    case 0x020cd744u: func_ov006_020cd744(self); return 1;
    case 0x020cd864u: func_ov006_020cd864(self); return 1;
    case 0x020cd9b0u: func_ov006_020cd9b0(self); return 1;
    case 0x020cdaecu: func_ov006_020cdaec(self); return 1;
    case 0x020cdc38u: func_ov006_020cdc38(self); return 1;
    case 0x020cdc8cu: func_ov006_020cdc8c(self); return 1;
    case 0x020cdd08u: func_ov006_020cdd08(self); return 1;
    case 0x020cde4cu: func_ov006_020cde4c(self); return 1;
    case 0x020cdea0u: func_ov006_020cdea0(self); return 1;
    case 0x020cdf1cu: func_ov006_020cdf1c(self); return 1;
    case 0x020cdf3cu: func_ov006_020cdf3c(self); return 1;
    case 0x020cf790u: func_ov006_020cf790(self); return 1;
    case 0x020cf820u: func_ov006_020cf820(self); return 1;
    case 0x020cfa44u: func_ov006_020cfa44(self); return 1;
    default:                                     return 0;
    }
}

extern "C" void port_mg_tte_mario_call(void *self, unsigned code, int adj)
{
    ++g_tte_mario_calls;
    if (code != 0 && adj == 0 && tte_mario_try(self, code)) {
        ++g_tte_mario_routed;
        return;
    }
    port_mg_call0(self, code, adj);
}

extern "C" void port_mg_tte_mario_counts(unsigned *calls, unsigned *routed)
{
    if (calls)  *calls  = g_tte_mario_calls;
    if (routed) *routed = g_tte_mario_routed;
}


/* ==== RUN link100, LANE FWD gate 3: THE SEVENTEEN PAIRS ARE SEATED ========
 *
 * The two host copies below are retired and src/func_ov006_020cf758.cpp and
 * src/func_ov006_020cea2c.cpp dispatch the field themselves. Both emit
 *     mov ecx, [obj+4] / mov eax, [obj] / add ecx, obj / jmp-or-call eax
 * -- receiver in ecx, ARITY ZERO, nothing pushed (read off their own /FAsc
 * listings, runs/link100/out/FWD/emit_gate3_out.txt; 020cf758 tail-JUMPS and
 * 020cea2c CALLS, and one set of zero-argument __fastcall faces is right for
 * both). So the field has to hold a host address, which means the .data pairs
 * the sixteen writers copy from have to hold one.
 *
 * THE UNIVERSE IS BOUNDED, which is what lane MGWRITER left open. Over this
 * class's own code block 020cd744..020d1008 -- containing all fourteen code
 * words, both dispatch sites (020cea2c, 020cf758) and both record walkers
 * (020cedf0 at stride 0x1d0, 020d0ac0 at stride 0x32c) -- every `load`
 * relocation in ov006 landing in the block is followed by a zero word in
 * exactly SEVENTEEN places, and all seventeen are the run 0213b31c..0213b39c.
 * Nothing else in ov006 can hand either field an address in this class.
 * runs/link100/out/FWD/sweep_tramp_narrow.txt is the sweep; the wider sweep
 * from 020cc198 in sweep_tramp.txt shows the sibling class's own run next door
 * and is kept as the control.
 *
 * ONE FACE PER CODE WORD, NOT PER PAIR. Three of the seventeen are BY-VALUE
 * COMPARISON SENTINELS -- src/func_ov006_020ce674.c compares the object's
 * stored pair against data_ov006_0213b324 and _0213b334 word for word, and
 * src/func_ov006_020ce108.cpp against _0213b33c -- and each holds the same code
 * word as a WRITTEN pair (b324 and b344 both hold 020cdf3c; b334, b33c and b32c
 * all hold 020cd744). A face per pair would make those comparisons permanently
 * false while the dispatch still looked fine, which is section 3's own warning
 * come true. A face per code word moves both sides of every comparison
 * together. That is the reverse of the rule PMFB5 wrote for TABLES, where two
 * cells holding one word had to stay distinguishable; the difference is that
 * nothing compares a table cell by value.
 *
 * THE COUNTERS MOVE WITH THE DISPATCH. g_tte_mario_calls and
 * g_tte_mario_routed are what hal/scene_mg_trampoline2.cpp:721 prints, and the
 * switch below is no longer reached from inside this file, so each face bumps
 * both. A counter left behind a seat reads zero forever and nothing looks
 * wrong -- lane PMFB3's tapped-pad trap.
 * ======================================================================== */
#define TTE_FACE(sym)                                                     \
    static void __fastcall tte_face_##sym(void *self, void *dead_edx)     \
    {                                                                     \
        (void)dead_edx;                                                   \
        ++g_tte_mario_calls;                                              \
        ++g_tte_mario_routed;                                             \
        sym(self);                                                        \
    }

TTE_FACE(func_ov006_020cd744)
TTE_FACE(func_ov006_020cd864)
TTE_FACE(func_ov006_020cd9b0)
TTE_FACE(func_ov006_020cdaec)
TTE_FACE(func_ov006_020cdc38)
TTE_FACE(func_ov006_020cdc8c)
TTE_FACE(func_ov006_020cdd08)
TTE_FACE(func_ov006_020cde4c)
TTE_FACE(func_ov006_020cdea0)
TTE_FACE(func_ov006_020cdf1c)
TTE_FACE(func_ov006_020cdf3c)
TTE_FACE(func_ov006_020cf790)
TTE_FACE(func_ov006_020cf820)
TTE_FACE(func_ov006_020cfa44)

extern "C" {
/* the seventeen source pairs, mounted from ov006 .data */
extern MgPmf data_ov006_0213b31c;   /* written  src/func_ov006_020cdad0.c */
extern MgPmf data_ov006_0213b324;   /* SENTINEL src/func_ov006_020ce674.c */
extern MgPmf data_ov006_0213b32c;   /* written  src/func_ov006_020cd7b8.c */
extern MgPmf data_ov006_0213b334;   /* SENTINEL src/func_ov006_020ce674.c */
extern MgPmf data_ov006_0213b33c;   /* SENTINEL src/func_ov006_020ce108.cpp */
extern MgPmf data_ov006_0213b344;   /* written  src/func_ov006_020ce0ac.cpp */
extern MgPmf data_ov006_0213b34c;   /* written  src/func_ov006_020cdf20.c */
extern MgPmf data_ov006_0213b354;   /* written  src/func_ov006_020cdeec.c */
extern MgPmf data_ov006_0213b35c;   /* written  src/func_ov006_020cde7c.c */
extern MgPmf data_ov006_0213b364;   /* written  src/func_ov006_020cfc58.c */
extern MgPmf data_ov006_0213b36c;   /* written  src/func_ov006_020cd98c.c */
extern MgPmf data_ov006_0213b374;   /* written  src/func_ov006_020cfa28.c */
extern MgPmf data_ov006_0213b37c;   /* written  src/func_ov006_020cf804.c */
extern MgPmf data_ov006_0213b384;   /* written  src/func_ov006_020cdce4.c */
extern MgPmf data_ov006_0213b38c;   /* written  src/func_ov006_020cdc68.c */
extern MgPmf data_ov006_0213b394;   /* written  src/func_ov006_020cdc14.c */
extern MgPmf data_ov006_0213b39c;   /* written  src/func_ov006_020cde28.c */
}

extern "C" void port_mg_tte_pairs_seat(void)
{
    static int done;
    if (done)
        return;
    done = 1;

    static const struct { MgPmf *p; unsigned rom; void *face;
                         const char *what; } seats[] = {
        {&data_ov006_0213b31c, 0x020cd9b0u, (void *)tte_face_func_ov006_020cd9b0,
         "0213b31c written  src/func_ov006_020cdad0.c"},
        {&data_ov006_0213b324, 0x020cdf3cu, (void *)tte_face_func_ov006_020cdf3c,
         "0213b324 SENTINEL src/func_ov006_020ce674.c"},
        {&data_ov006_0213b32c, 0x020cd744u, (void *)tte_face_func_ov006_020cd744,
         "0213b32c written  src/func_ov006_020cd7b8.c"},
        {&data_ov006_0213b334, 0x020cd744u, (void *)tte_face_func_ov006_020cd744,
         "0213b334 SENTINEL src/func_ov006_020ce674.c"},
        {&data_ov006_0213b33c, 0x020cd744u, (void *)tte_face_func_ov006_020cd744,
         "0213b33c SENTINEL src/func_ov006_020ce108.cpp"},
        {&data_ov006_0213b344, 0x020cdf3cu, (void *)tte_face_func_ov006_020cdf3c,
         "0213b344 written  src/func_ov006_020ce0ac.cpp"},
        {&data_ov006_0213b34c, 0x020cdf1cu, (void *)tte_face_func_ov006_020cdf1c,
         "0213b34c written  src/func_ov006_020cdf20.c"},
        {&data_ov006_0213b354, 0x020cdea0u, (void *)tte_face_func_ov006_020cdea0,
         "0213b354 written  src/func_ov006_020cdeec.c"},
        {&data_ov006_0213b35c, 0x020cde4cu, (void *)tte_face_func_ov006_020cde4c,
         "0213b35c written  src/func_ov006_020cde7c.c"},
        {&data_ov006_0213b364, 0x020cfa44u, (void *)tte_face_func_ov006_020cfa44,
         "0213b364 written  src/func_ov006_020cfc58.c"},
        {&data_ov006_0213b36c, 0x020cd864u, (void *)tte_face_func_ov006_020cd864,
         "0213b36c written  src/func_ov006_020cd98c.c"},
        {&data_ov006_0213b374, 0x020cf820u, (void *)tte_face_func_ov006_020cf820,
         "0213b374 written  src/func_ov006_020cfa28.c"},
        {&data_ov006_0213b37c, 0x020cf790u, (void *)tte_face_func_ov006_020cf790,
         "0213b37c written  src/func_ov006_020cf804.c"},
        {&data_ov006_0213b384, 0x020cdc8cu, (void *)tte_face_func_ov006_020cdc8c,
         "0213b384 written  src/func_ov006_020cdce4.c"},
        {&data_ov006_0213b38c, 0x020cdc38u, (void *)tte_face_func_ov006_020cdc38,
         "0213b38c written  src/func_ov006_020cdc68.c"},
        {&data_ov006_0213b394, 0x020cdaecu, (void *)tte_face_func_ov006_020cdaec,
         "0213b394 written  src/func_ov006_020cdc14.c"},
        {&data_ov006_0213b39c, 0x020cdd08u, (void *)tte_face_func_ov006_020cdd08,
         "0213b39c written  src/func_ov006_020cde28.c"},
    };

    for (unsigned i = 0; i < sizeof seats / sizeof seats[0]; ++i) {
        if (seats[i].p->code != seats[i].rom || seats[i].p->adj != 0) {
            std::fprintf(stderr, "FATAL: dScMgTrampoline2_c source pair "
                         "%s: the mount holds %08x/%d, the ROM's own "
                         "word says %08x/0 -- WRONG BYTES\n",
                         seats[i].what, seats[i].p->code, seats[i].p->adj,
                         seats[i].rom);
            std::abort();
        }
        seats[i].p->code = (unsigned)(size_t)seats[i].face;
    }
}
/* HOST COPY RETIRED, run link100 lane FWD gate 3.
   src/func_ov006_020cf758.cpp is on port/slice_fwd.txt and dispatches the
   field itself; its one statement IS the dispatch, and with the pairs seated
   the word it jumps to is a host face. */

/* HOST COPY RETIRED, run link100 lane FWD gate 3.
   src/func_ov006_020cea2c.cpp is on port/slice_fwd.txt. The nine field copies,
   the AddVec3 on c+8 / c+0x2c and the Animation::Advance at c+0x194 that this
   copy carried verbatim are the matched TU's own again.

   THE SWITCH ABOVE STAYS AND IS NO LONGER REACHED FROM INSIDE THIS FILE.
   port_mg_tte_mario_call is this class's C-linkage entry point and nothing
   about it moves; the framework's UNHANDLED report still stands behind it. The
   two counters it kept are bumped by the faces now, so
   hal/scene_mg_trampoline2.cpp's census keeps counting the dispatches that
   actually happen. */
