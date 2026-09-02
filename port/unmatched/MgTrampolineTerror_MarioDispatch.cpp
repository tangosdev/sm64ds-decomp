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

/* src/func_ov006_020cf758.cpp, whose whole body is the dispatch. */
// PORT_HOST_ABI: mwcc field pointer-to-member dispatch, the 8-byte pair at record offset 0 MSVC's 4-byte pmf reads half of
extern "C" void func_ov006_020cf758(void *c)
{
    const MgPmf *p = (const MgPmf *)c;
    port_mg_tte_mario_call(c, p->code, p->adj);
}

/* src/func_ov006_020cea2c.cpp, verbatim except the first statement. */
// PORT_HOST_ABI: mwcc field pointer-to-member dispatch, the 8-byte pair at record offset 0 MSVC's 4-byte pmf reads half of
extern "C" void func_ov006_020cea2c(char *c)
{
    {
        const MgPmf *p = (const MgPmf *)c;
        port_mg_tte_mario_call(c, p->code, p->adj);
    }
    AddVec3(c + 8, c + 0x2c, c + 8);
    _ZN9Animation7AdvanceEv(c + 0x194);
    *(int *)(c + 0x14) = *(int *)(c + 8);
    *(int *)(c + 0x18) = *(int *)(c + 0xc);
    *(int *)(c + 0x1c) = *(int *)(c + 0x10);
    *(int *)(c + 0x50) = *(int *)(c + 0x38);
    *(int *)(c + 0x54) = *(int *)(c + 0x3c);
    *(int *)(c + 0x58) = *(int *)(c + 0x40);
    *(int *)(c + 0x5c) = *(int *)(c + 0x44);
    *(int *)(c + 0x60) = *(int *)(c + 0x48);
    *(int *)(c + 0x64) = *(int *)(c + 0x4c);
}
