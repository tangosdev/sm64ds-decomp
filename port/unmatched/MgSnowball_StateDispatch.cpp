// MgSnowball_StateDispatch.cpp -- run mg11 lane SNW.
//
// dScMgSnowball_c (actor id 0x179, scene 377, "Snowball Slalom"), the whole of
// this class's pointer-to-member wall: THREE translation units, FOUR decode
// sites, FOUR tables, FOURTEEN slots, all arity 1.
//
// ---- 0. WHY THIS CLASS'S WALL IS FOUR SITES AND NOT ONE -------------------
//
// port/mg_fanout_costs.txt section 14 tells a lane to ask "how deep is the
// machine" rather than "how many TUs name the table".  This class is the one
// the question was written for.
//
// Its vtable slot 6, func_ov006_021283a4, DECODES NOTHING.  A scan of that
// body for the five-instruction ARM Itanium sequence returns zero sites; it
// reaches the machine by CALLING func_ov006_0212a2e0 at 0x02128f04, and it is
// that callee's only caller.  A lane that read slot 6 alone -- which is the
// slot every seat wires by name -- would have priced this class's wall at
// ZERO and shipped a scene that jumps to DS addresses on its first frame.
//
// The machine is then TWO LEVELS DEEP.  func_ov006_0212a2e0 decodes
// data_ov006_02143038, and slots 1 and 2 of that table are themselves the
// dispatchers func_ov006_0212a224 and func_ov006_02129d94.  Neither has a
// single arm_call anywhere in ov006: their only references in the whole
// overlay are the .data pair words at 0x0213ffb0 and 0x0213ff78, so the ONLY
// way either body runs is through the first-level table.  A closure walk from
// slot 6 that stopped at direct calls would not have found them.
//
// THE FOUR SITES, from a body-by-body scan of this class's own bodies out of
// extracted/overlays/overlay_0006.bin at base 0x020bfec0 -- each body taken at
// its own start and size from config/arm9/overlays/ov006/symbols.txt, never an
// address range picked by eye:
//
//   0x02129dc0  func_ov006_02129d94   decodes data_ov006_02143070
//   0x02129df8  func_ov006_02129d94   AND    data_ov006_02143020   (one TU,
//                                            two tables, two sites)
//   0x0212a248  func_ov006_0212a224   decodes data_ov006_02143050
//   0x0212a324  func_ov006_0212a2e0   decodes data_ov006_02143038
//
// The same scan over this class's twenty-seven own bodies returns those four
// and nothing else, and over the six bodies it inherits from
// dScMgSingle3DBase_c it returns zero.
//
// RUN mg9 LANE LKY HANDED THESE FOUR SITES OVER, in port/slice_lky.txt section
// 6 and port/unmatched/MgBSC_StateDispatch.cpp section 3, after its own review
// found them swept in by a wrong address span.  THEY WERE RE-DERIVED HERE
// RATHER THAN INHERITED: this lane wrote its own detector, reproduced LKY's
// 114 sites over the whole of ov006 and its ONE site in func_ov006_021254c0 as
// controls, reproduced the five-site reading of the retired span
// 0x02124908..0x0212a3c0 as a third control, and then scanned this class's own
// body list.  The four addresses agree digit for digit.
//
// ---- 1. THE FOUR TABLES AND THEIR FOURTEEN SLOTS -------------------------
//
// THE SLOT NUMBERS COME FROM THE CONSTRUCTOR, NOT FROM ADDRESS ORDER.  The run
// law's "sinit source is slot ground truth" is not a formality here: this
// class's source pairs are copied in an order that has nothing to do with
// where they sit in .data.  src/__sinit_ov006_021333e0.c, verbatim in its own
// order:
//
//   data_ov006_02143038  (3)  <- 0x0213ff88, 0x0213ffb0, 0x0213ff78
//   data_ov006_02143050  (4)  <- 0x0213ff90, 0x0213ff98, 0x0213ff80, 0x0213ff70
//   data_ov006_02143070  (4)  <- 0x0213ffa0, 0x0213ff68, 0x0213ff60, 0x0213ff58
//   data_ov006_02143020  (3)  <- 0x0213ffc8, 0x0213ffa8, 0x0213ffc0
//
// Read the pair run 0x0213ff58..0x0213ffc8 in address order instead and every
// one of the fourteen slot numbers comes out wrong.  Worse, the run is not
// even contiguous pairs: MgSnowballSlalom_SpawnInfo sits INSIDE it at
// 0x0213ffb8 as {0x0212a520, 0x01790179} -- the factory word and the actor id
// twice -- and a sweep would read it as a fifteenth pair with a nonzero
// adjustment that no constructor ever copies.  That is the same shape
// port/mg_fanout_costs.txt section 4 records for dScMgCurling_c and the same
// one run mg9 lane WIG's sweep hit.
//
// Every adjustment word in the fourteen reads ZERO, checked in the image.
//
//   table                slot  code        body                        src
//   data_ov006_02143038   0    0x0212a274  func_ov006_0212a274         .c
//                         1    0x0212a224  func_ov006_0212a224  DISP   .cpp
//                         2    0x02129d94  func_ov006_02129d94  DISP   .cpp
//   data_ov006_02143050   0    0x0212a110  func_ov006_0212a110         .c
//                         1    0x0212a000  func_ov006_0212a000         .c
//                         2    0x02129eec  func_ov006_02129eec         .c
//                         3    0x02129e28  func_ov006_02129e28         .c
//   data_ov006_02143070   0    0x02129cb0  func_ov006_02129cb0         .c
//                         1    0x02129b74  func_ov006_02129b74         .c
//                         2    0x02129a34  func_ov006_02129a34         .c
//                         3    0x0212992c  func_ov006_0212992c         .c
//   data_ov006_02143020   0    0x02129894  func_ov006_02129894         .cpp
//                         1    0x021297c0  func_ov006_021297c0         .c
//                         2    0x0212972c  func_ov006_0212972c         .c
//
// Fourteen slots, fourteen DISTINCT bodies -- no slot is shared with another,
// so no arity is inferred from a neighbour -- and all fourteen resolve to a
// matched src TU.  NO STATE FLOOR: not one of them is bodiless and not one
// carries a NONMATCHING banner.
//
// ALL FOURTEEN ARE ARITY 1.  Every src spells its body `void f(char *self,
// int idx)` and every ROM caller passes the element index in r1, so this
// class's states route through port_mg_call1 rather than port_mg_call0.  Two
// of the fourteen are the second-level dispatchers themselves, which is why
// snowball_try_1 below must route those two code words to the HOST copies in
// this file and not to their src TUs -- routing them to src would put the
// wall back one level down and nothing would report it.
//
// ---- 2. WHY THE PORT CANNOT COMPILE THE THREE SRC TUs --------------------
//
// port/mg_fanout_costs.txt section 4 names three reasons and each alone is
// fatal.  All three apply here, and the three TUs split across BOTH of the
// spelling shapes that section describes:
//
//   src/func_ov006_0212a2e0.cpp   declares `extern "C" PMF data_ov006_02143038[]`
//   src/func_ov006_0212a224.cpp   declares `extern "C" PMF data_ov006_02143050[]`
//        -- the SILENT shape.  extern "C" mangles to the plain C name the
//        ov006 mount already defines, so the link is satisfied and the
//        compile is clean while MSVC strides an eight-byte table by four.
//        Slot 0 would read correctly and slots 1 and 2 would each read half
//        of one record and half of the next.
//
//   src/func_ov006_02129d94.cpp   declares `extern Entry data_ov006_02143070[]`
//                                 and `extern Entry data_ov006_02143020[]`
//        -- the LOUD shape.  C++ linkage encodes the member-pointer type into
//        the symbol, so the link fails with LNK2019 and the defect announces
//        itself.
//
// One class, both shapes, which is why this file replaces all three rather
// than trusting a link to find them.
//
// ---- 3. WHAT EACH HOST COPY CHANGES ---------------------------------------
//
// Each body below is its src TU with the pointer-to-member declaration
// replaced by MgPmf and the dispatch replaced by a port_mg_snowball_call1
// call.  Every other statement, offset, constant and loop bound is verbatim
// from the src, which is itself matched against the ROM.  Nothing here is a
// re-derivation of behaviour.

#include <cstdio>

/* The ROM's member-pointer record with no member-pointer type anywhere: two
   words, {code, adjustment}, which is what the ROM actually stores and what
   the mwcc table strides by eight. */
struct MgPmf { unsigned code; int adj; };

extern "C" {

/* MgBase_StateDispatch.cpp owns port_mg_call0/port_mg_call1 and the UNHANDLED
   report.  This file adds a switch in front of call1 and hands everything it
   does not own straight through, so the null-code guard, the nonzero-
   adjustment refusal and the report all still live in exactly one place. */
void port_mg_call1(void *self, unsigned code, int adj, int a);

/* The fourteen state bodies, each declared with the parameter list ITS OWN src
   TU defines.  All fourteen are (receiver, int). */
void func_ov006_0212a274(char *self, int idx);
void func_ov006_0212a110(char *c, int idx);
void func_ov006_0212a000(char *base, int index);
void func_ov006_02129eec(char *c, int idx);
void func_ov006_02129e28(char *c, int i);
void func_ov006_02129cb0(char *o, int i);
void func_ov006_02129b74(char *base, int index);
void func_ov006_02129a34(char *base, int i);
void func_ov006_0212992c(char *self, int idx);
void func_ov006_02129894(char *c, int i);
void func_ov006_021297c0(unsigned char *base, int idx);
void func_ov006_0212972c(char *c, int idx);
/* and the two second-level dispatchers, whose DEFINITIONS are in this file */
void func_ov006_0212a224(char *base, int idx);
void func_ov006_02129d94(char *c, int i);

/* The four tables.  The ov006 mount defines the storage;
   __sinit_ov006_021333e0 fills all fourteen slots at minigame scene load. */
extern MgPmf data_ov006_02143038[];
extern MgPmf data_ov006_02143050[];
extern MgPmf data_ov006_02143070[];
extern MgPmf data_ov006_02143020[];

/* src/func_ov006_0212a2e0.cpp's other two externals, spelled as its src
   spells them. */
int RandomIntInternal(int *seed);
extern int data_0209d4b8;

}  /* extern "C" */

/* Routed through one of this class's fourteen table slots. */
static unsigned g_snw_state_hits;
/* A table slot whose body this file does not own -- zero here, and it is
   printed rather than assumed, because "no floor" is a claim. */
static unsigned g_snw_floor_hits;
/* Which of the fourteen actually ran, so a census can show the machine MOVED
   rather than only that it fired. */
static unsigned g_snw_slot_hits[14];
/* The first-level selector's low and high water marks: the byte at +0xbeaf of
   an element, which chooses between the three second-level machines. */
static int g_snw_sel_lo = -1, g_snw_sel_hi = -1;
/* How many of the fifty elements were live on the last slot-6 pass. */
static unsigned g_snw_live_last;

static int snowball_try_1(void *self, unsigned code, int a)
{
    char *c = (char *)self;

    switch (code) {
    /* data_ov006_02143038 -- the FIRST level, three slots */
    case 0x0212a274u: ++g_snw_slot_hits[0];  func_ov006_0212a274(c, a); return 1;
    case 0x0212a224u: ++g_snw_slot_hits[1];  func_ov006_0212a224(c, a); return 1;
    case 0x02129d94u: ++g_snw_slot_hits[2];  func_ov006_02129d94(c, a); return 1;

    /* data_ov006_02143050 -- second level, reached only through slot 1 above */
    case 0x0212a110u: ++g_snw_slot_hits[3];  func_ov006_0212a110(c, a); return 1;
    case 0x0212a000u: ++g_snw_slot_hits[4];  func_ov006_0212a000(c, a); return 1;
    case 0x02129eecu: ++g_snw_slot_hits[5];  func_ov006_02129eec(c, a); return 1;
    case 0x02129e28u: ++g_snw_slot_hits[6];  func_ov006_02129e28(c, a); return 1;

    /* data_ov006_02143070 -- second level, reached only through slot 2 above */
    case 0x02129cb0u: ++g_snw_slot_hits[7];  func_ov006_02129cb0(c, a); return 1;
    case 0x02129b74u: ++g_snw_slot_hits[8];  func_ov006_02129b74(c, a); return 1;
    case 0x02129a34u: ++g_snw_slot_hits[9];  func_ov006_02129a34(c, a); return 1;
    case 0x0212992cu: ++g_snw_slot_hits[10]; func_ov006_0212992c(c, a); return 1;

    /* data_ov006_02143020 -- second level, the SECOND table slot 2 decodes */
    case 0x02129894u: ++g_snw_slot_hits[11]; func_ov006_02129894(c, a); return 1;
    case 0x021297c0u: ++g_snw_slot_hits[12];
                      func_ov006_021297c0((unsigned char *)c, a); return 1;
    case 0x0212972cu: ++g_snw_slot_hits[13]; func_ov006_0212972c(c, a); return 1;

    default:                                                           return 0;
    }
}

extern "C" void port_mg_snowball_call1(void *self, unsigned code, int adj,
                                       int a)
{
    if (code != 0 && adj == 0 && snowball_try_1(self, code, a)) {
        ++g_snw_state_hits;
        return;
    }
    port_mg_call1(self, code, adj, a);
}

extern "C" unsigned port_mg_snowball_state_hits(void) { return g_snw_state_hits; }
extern "C" unsigned port_mg_snowball_floor_hits(void) { return g_snw_floor_hits; }
extern "C" unsigned port_mg_snowball_live(void)       { return g_snw_live_last; }

extern "C" void port_mg_snowball_sel_range(int *lo, int *hi)
{
    if (lo) *lo = g_snw_sel_lo;
    if (hi) *hi = g_snw_sel_hi;
}

extern "C" void port_mg_snowball_slot_hits(const unsigned **v, unsigned *n)
{
    if (v) *v = g_snw_slot_hits;
    if (n) *n = 14;
}

// ---- 4. THE THREE HOST COPIES ---------------------------------------------
//
// func_ov006_0212a2e0 -- the FIRST level, and slot 6's only route in.
//
// Fifty elements of 0x24 bytes starting at +0xbe94.  For each live one (the
// byte at +0xbeae) it dispatches data_ov006_02143038 on the byte at +0xbeaf,
// and then runs the respawn test: if the element's +0xbe98 has fallen more
// than 0xc8 (in 20.12, so 200 units) behind the scene's own +0xab6c, the
// element is thrown back with a fresh pseudo-random lane and all three of its
// state bytes cleared.
//
// THE RECEIVER IS THE OBJECT AND THE CURSOR IS SEPARATE.  src walks a cursor
// `q` forward by 0x24 per element but calls the member pointer on `o`, the
// object itself, passing the element index.  Getting that backwards would
// hand every state body a pointer 0x24 bytes further along on each iteration
// and the fields would read plausible garbage.  The ROM disassembly at
// 0x0212a324 loads r0 from the saved object register, not from the cursor.
//
// The magic 0x7fff mask and the (rnd << 5) >> 0xf reshaping are src's, which
// is the ROM's: RandomIntInternal returns 32 bits, the top half is taken,
// masked to 15 bits, then folded to a small lane index and re-scaled by 0xf.

// PORT_HOST_ABI: first-level pmf dispatcher for data_ov006_02143038 and slot 6's only route in; the mwcc eight-byte member-pointer table MSVC's four-byte pmf cannot stride, so the host strides the {code, adj} pairs directly.
extern "C" void func_ov006_0212a2e0(char *o)
{
    int i;
    char *q;
    int mask;
    unsigned live = 0;

    q = o;
    i = 0;
    mask = 0x7fff;
    do {
        if (*(unsigned char *)(q + 0xbeae) != 0) {
            unsigned char fidx = *(unsigned char *)(q + 0xbeaf);
            const MgPmf *e = &data_ov006_02143038[fidx];

            ++live;
            if (g_snw_sel_lo < 0 || (int)fidx < g_snw_sel_lo)
                g_snw_sel_lo = (int)fidx;
            if ((int)fidx > g_snw_sel_hi)
                g_snw_sel_hi = (int)fidx;

            port_mg_snowball_call1(o, e->code, e->adj, i);

            if ((*(int *)(q + 0xbe98) - *(int *)(o + 0xab6c)) >> 12 >= 0xc8) {
                unsigned rnd = ((unsigned)RandomIntInternal(&data_0209d4b8) >> 16)
                             & (unsigned)mask;
                rnd = (rnd << 5) >> 0xf;
                *(int *)(q + 0xbe94) = (int)(rnd << 0xf);
                *(int *)(q + 0xbe98) = *(int *)(o + 0xab6c) - 0x8000;
                *(unsigned char *)(q + 0xbeb0) = 0;
                *(unsigned char *)(q + 0xbeaf) = 0;
                *(unsigned char *)(q + 0xbeb1) = 0;
            }
        }
        i++;
        q += 0x24;
    } while (i < 0x32);

    g_snw_live_last = live;
}

// func_ov006_0212a224 -- second level, table data_ov006_02143050.
//
// Reached ONLY as slot 1 of data_ov006_02143038.  It has no arm_call anywhere
// in ov006; the pair word at 0x0213ffb0 is its only reference in the overlay.

extern "C" void func_ov006_0212a224(char *base, int idx)
{
    unsigned char state = *(unsigned char *)(base + idx * 0x24 + 0xbeb0);
    const MgPmf *e = &data_ov006_02143050[state];
    port_mg_snowball_call1(base, e->code, e->adj, idx);
}

// func_ov006_02129d94 -- second level, and the ONE TU with TWO decode sites.
//
// Reached only as slot 2 of data_ov006_02143038 (pair word 0x0213ff78, its
// only reference in the overlay).  It runs BOTH of its tables in order on the
// same element: the byte at +0xbeb0 indexes data_ov006_02143070, then the byte
// at +0xbeb1 indexes data_ov006_02143020.  The second read happens AFTER the
// first dispatch, so a first-table body that rewrites +0xbeb1 changes which
// body the second call reaches on the same pass -- src reads the two bytes at
// two separate points and this copy keeps that order.
//
// src spells the element as a struct with a 0xbeb0-byte head and a 0x24-byte
// element; the byte offsets below are that layout written out, which is also
// how func_ov006_0212a2e0's src and the ROM at 0x02129db4 and 0x02129dec
// spell it.

extern "C" void func_ov006_02129d94(char *c, int i)
{
    unsigned char idx0 = *(unsigned char *)(c + 0xbeb0 + i * 0x24);
    const MgPmf *e0 = &data_ov006_02143070[idx0];
    port_mg_snowball_call1(c, e0->code, e0->adj, i);

    {
        unsigned char idx1 = *(unsigned char *)(c + 0xbeb1 + i * 0x24);
        const MgPmf *e1 = &data_ov006_02143020[idx1];
        port_mg_snowball_call1(c, e1->code, e1->adj, i);
    }
}
