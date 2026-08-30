// PORT_HOST_ABI. The mwcc POINTER-TO-MEMBER WALL, ov027's one customer:
// daPgDfdr_c (actor id 258, Snowman's Land) and its TWO dispatching TUs.
// Run rel0215 wave 2, lane cast-ov027.
//
// Read port/unmatched/MgBase_StateDispatch.cpp's header first. It carries the
// ROM disassembly of the Itanium ARM dispatch sequence, the measurement that
// the adjustment word decides virtual vs non-virtual, and the finding that the
// overlay constructors already copy the pairs correctly. None of that is
// repeated here.
//
// ---- WHY A HOST COPY AT ALL ------------------------------------------------
// mwcc's member pointer is EIGHT bytes {code, adjustment} and MSVC's is four;
// the code word is a raw DS address; and MSVC cannot deliver `this` in ecx
// onto the plain cdecl bodies src/ carries. So the two dispatch sites get a
// host copy with the pointer-to-member call replaced by an address switch, and
// the byte-matched originals STAY in src/ as proof, dropped from
// port/slice_ov027cast.txt.
//
// ---- port/tools/stategen.py REFUSED THIS TABLE, AND THE REFUSAL IS CORRECT --
// stategen generates exactly this file from the constructor plus the raw
// image. Run on src/__sinit_ov027_02112df8.cpp it says:
//
//   "no two-scalar pair struct among the 2 struct(s) declared here (D, S2) --
//    this parser keys slot offsets off the {code, adjustment} pair type and
//    will not guess which of these is it. Teach it the spelling rather than
//    picking one"
//
// which is right: this constructor spells the pair as `struct S2 { int w[2]; }`
// -- ONE array field, not two scalars -- and `struct D { S2 a, b, c, d; }` for
// the destination. Every table stategen has seen spells the pair as two scalar
// fields. Teaching it the array spelling is a change to a SHARED generator
// that twenty-nine other lanes read, and it needs its own fixture; this lane
// is not the place for that mid-wave. FOUR ROWS AND TWO SITES ARE WRITTEN BY
// HAND HERE, the MgCurling precedent, and the finding is handed to the
// coordinator so a tool owner can add the spelling and its fixture.
//
// ---- THE FOUR ADDRESSES, AND WHERE THEY COME FROM --------------------------
// Not guessed and not read out of a running table. src/__sinit_ov027_02112df8.cpp
// names, in source order, which .data pair fills which .bss slot, and the pairs
// were read out of extracted/overlays/overlay_0027.bin at (addr - 0x021111a0)
// -- never extracted/dsd/arm9_overlays, trap T4.
//
//   dest offset  filled from            pair reads      body
//   +0x00        data_ov027_021139d4    {02111ca8, 0}   func_ov027_02111ca8
//   +0x08        data_ov027_021139cc    {02111c48, 0}   func_ov027_02111c48
//   +0x10        data_ov027_021139dc    {02111b2c, 0}   func_ov027_02111b2c
//   +0x18        data_ov027_021139c4    {02111a28, 0}   func_ov027_02111a28
//
// All four adjustment words are ZERO, so all four are the NON-VIRTUAL arm of
// the dispatch and the code word is called directly. Nothing here needs the
// virtual arm, and the switch says so rather than modelling a path the ROM
// never takes on this class.
//
// All four bodies have a matched src TU and all four are in
// port/slice_ov027cast.txt. There is no hole.
//
// ---- THE TWO SITES, DISASSEMBLED -------------------------------------------
// The state POINTER lives at +0x3cc and is set by func_ov027_02111d70, which
// the ROM encodes as `add r1, r2, r1, lsl #4` -- the element stride is SIXTEEN
// bytes, two pairs per element:
//
//   02111d70  ldr r2,[pc]        -> 02113ce4   the table base
//   02111d74  ldr r12,[pc]       -> 02111d38   func_ov027_02111d38
//   02111d78  add r1, r2, r1, lsl #4           r1 = base + idx*16
//   02111d7c  str r1,[r0,#0x3cc]
//   02111d80  bx  r12                          tail-jump into the first site
//
//   func_ov027_02111d38   ldr r3,[r0,#0x3cc]              p = pp
//   func_ov027_02111cfc   ldr r1,[r0,#0x3cc]; add r3,r1,#8  p = pp + 8
//
// then both run the identical five-instruction Itanium tail:
//   ldr r1,[r3,#4] / add r0,r0,r1,asr #1 / ands r1,r1,#1 /
//   ldrne r2,[r0] ; ldrne r1,[r3] ; ldrne r1,[r2,r1] / ldreq r1,[r3] / blx r1
//
// So site A dispatches the pair at pp+0 and site B the pair at pp+8, and with
// the 16-byte element stride the two live states are:
//
//   idx 0  (pp = +0x00)   site A -> func_ov027_02111ca8   site B -> func_ov027_02111c48
//   idx 1  (pp = +0x10)   site A -> func_ov027_02111b2c   site B -> func_ov027_02111a28
//
// The transitions in the bodies agree: func_ov027_02111a28 calls
// func_ov027_02111d70(c, 0) and func_ov027_02111c48 calls it with 1.
//
// ---- WHY THE SWITCH IS ON THE CODE WORD, NOT ON THE OFFSET -----------------
// The MgCurling shape. Switching on the stored DS address rather than on
// (pp - table) means the switch is also a CHECK that the constructor's copy
// landed where this file says it did: a sinit that ran in a different order,
// or a mount that sized data_ov027_02113ce4 at dsd's inferred 8 bytes instead
// of the pinned 0x20, produces a code word that matches no case and the
// default arm says so loudly instead of running the wrong state.
//
// ---- TRAP T2, AND WHY THIS FILE NAMES THE FOUR SOURCES ---------------------
// data_ov027_021139c4 / _021139cc / _021139d4 / _021139dc are mounted romdata
// whose bytes include a RELOCATED word -- a DS code address, which is exactly
// what port/tools/ptr_audit.py sweeps for. Nothing in the port ever CALLS
// through those words (this switch calls host bodies by name), but the audit's
// rule is that a host TU must NAME the symbol, and a comment is not a name.
// port_dapgdfdr_states_check() below reads all four and compares them against
// the four cells the constructor filled, so the references are load-bearing:
// they are the check, not a gesture at the audit.

#include <cstdio>
#include <cstring>

extern "C" {

/* the .bss dispatch storage, pinned :0x20 in port/ov027_syms.txt */
extern unsigned char data_ov027_02113ce4[];

/* the four .data {code, adjustment} pair sources, 8 bytes each */
extern unsigned char data_ov027_021139c4[];
extern unsigned char data_ov027_021139cc[];
extern unsigned char data_ov027_021139d4[];
extern unsigned char data_ov027_021139dc[];

/* the four states, all matched, all in port/slice_ov027cast.txt */
int func_ov027_02111a28(char *self);
void func_ov027_02111b2c(char *self);
int func_ov027_02111c48(char *self);
int func_ov027_02111ca8(char *self);

void port_actor_slot_decline(const char *what);

void func_ov027_02111d38(void *self);
void func_ov027_02111cfc(void *self);
void port_dapgdfdr_states_check(void);

}

namespace {

const unsigned kCodeEnter0 = 0x02111ca8u;
const unsigned kCodeTick0  = 0x02111c48u;
const unsigned kCodeEnter1 = 0x02111b2cu;
const unsigned kCodeTick1  = 0x02111a28u;

unsigned pair_code(const unsigned char *p)
{
    unsigned v;
    std::memcpy(&v, p, sizeof v);
    return v;
}

unsigned pair_adj(const unsigned char *p)
{
    unsigned v;
    std::memcpy(&v, p + 4, sizeof v);
    return v;
}

void decline(const char *site, unsigned code, unsigned adj)
{
    static char msg[192];
    std::snprintf(msg, sizeof msg,
                  "daPgDfdr_c state dispatch (%s): pair {0x%08x, 0x%08x} is "
                  "not one of this class's four", site, code, adj);
    std::fprintf(stderr, "UNHOSTED: %s\n", msg);
    port_actor_slot_decline(msg);
}

/* The one address switch. Both sites share it; they differ only in which of
   the element's two pairs they hand it. */
void dispatch(const unsigned char *pair, char *self, const char *site)
{
    unsigned adj = pair_adj(pair);
    if (adj & 1u) {
        /* the VIRTUAL arm. Every one of this class's four pairs carries a zero
           adjustment, so the ROM never takes it here. Say that rather than
           model a path no measurement covers. */
        decline(site, pair_code(pair), adj);
        return;
    }
    switch (pair_code(pair)) {
    case kCodeEnter0: func_ov027_02111ca8(self); return;
    case kCodeTick0:  func_ov027_02111c48(self); return;
    case kCodeEnter1: func_ov027_02111b2c(self); return;
    case kCodeTick1:  func_ov027_02111a28(self); return;
    default: break;
    }
    decline(site, pair_code(pair), adj);
}

} /* namespace */

/* The constructor's copy, checked once. Called from
   hal/actor_classes_ov027.cpp's fill, after the sinits have run. */
extern "C" void port_dapgdfdr_states_check(void)
{
    static int done;
    if (done)
        return;
    done = 1;

    struct Row { const unsigned char *src; unsigned off; unsigned code; };
    const Row rows[4] = {
        { data_ov027_021139d4, 0x00u, kCodeEnter0 },
        { data_ov027_021139cc, 0x08u, kCodeTick0  },
        { data_ov027_021139dc, 0x10u, kCodeEnter1 },
        { data_ov027_021139c4, 0x18u, kCodeTick1  },
    };
    for (int i = 0; i < 4; ++i) {
        unsigned s = pair_code(rows[i].src);
        unsigned d = pair_code(data_ov027_02113ce4 + rows[i].off);
        if (s == rows[i].code && d == rows[i].code)
            continue;
        static char msg[192];
        std::snprintf(msg, sizeof msg,
                      "daPgDfdr_c state table row %d: source reads 0x%08x, "
                      "dest +0x%02x reads 0x%08x, expected 0x%08x",
                      i, s, rows[i].off, d, rows[i].code);
        std::fprintf(stderr, "UNHOSTED: %s\n", msg);
        port_actor_slot_decline(msg);
    }
}

/* ---- the two host copies -------------------------------------------------
   Signatures are the ROM's: plain cdecl, one argument, the receiver. The two
   src TUs that these replace are src/func_ov027_02111d38.cpp and
   src/func_ov027_02111cfc.cpp, both byte-matched and both dropped from the
   slice. Every other ov027 body still calls these two by name. */

extern "C" void func_ov027_02111d38(void *self)
{
    char *c = (char *)self;
    const unsigned char *pp = *(const unsigned char **)(c + 0x3cc);
    if (!pp) {
        decline("02111d38", 0u, 0u);
        return;
    }
    dispatch(pp, c, "02111d38");
}

extern "C" void func_ov027_02111cfc(void *self)
{
    char *c = (char *)self;
    const unsigned char *pp = *(const unsigned char **)(c + 0x3cc);
    if (!pp) {
        decline("02111cfc", 0u, 0u);
        return;
    }
    dispatch(pp + 8, c, "02111cfc");
}
