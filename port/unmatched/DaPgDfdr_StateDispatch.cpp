/* THE SEAT for daPgDfdr_c's state table (actor id 258, Snowman's Land, ov027).
 * Run link100, lane UNMATCH. Until this lane the file held HOST COPIES of the
 * two dispatchers as well; they are retired and the ROM's own two TUs carry
 * them (port/slice_unmatch2.txt).
 *
 * Read port/unmatched/MgBase_StateDispatch.cpp's header first. It carries the
 * ROM disassembly of the Itanium ARM dispatch sequence and the measurement
 * that the adjustment word decides virtual vs non-virtual. None of that is
 * repeated here.
 *
 * ---- WHY THE HOST COPIES ARE GONE -----------------------------------------
 * The old header said "MSVC's member pointer is four bytes". That was written
 * before the port built with /vmg /vmm, and it is no longer what this compiler
 * does. MEASURED on 2026-09-07 with the port's own flags out of build.ninja
 * (/O2 /Ob2 -std:c++17 -MT /Oy- /vmg /vmm), src/func_ov027_02111d38.cpp:
 *
 *     mov  eax, DWORD PTR _c$[ebp]
 *     mov  edx, DWORD PTR [eax+972]        ; c->pp, the ROM's +0x3cc
 *     mov  ecx, DWORD PTR [edx+4]          ; the ROM's adjustment word
 *     add  ecx, eax
 *     mov  eax, DWORD PTR [edx]            ; the ROM's code word
 *     pop  ebp
 *     jmp  eax
 *
 * and src/func_ov027_02111cfc.cpp is the same body reading [edx+8]/[edx+12] --
 * the second pair of the 16-byte element, which is what `c->pp + 1` is over an
 * EIGHT-byte member pointer. So MSVC reproduces the ROM's own record layout
 * word for word, and the dispatch is a TAIL JUMP: it pops its frame and jumps,
 * so the callee's [esp+4] is still this function's own `c` argument. A plain
 * cdecl ov027 state body reads its receiver from exactly there. The receiver
 * is ALSO left in ecx, which is what makes this shape serve both conventions.
 *
 * The one thing MSVC cannot do is invent a host address for a word the ROM
 * filled with a DS one. That is what this file is now: the seat.
 *
 * ---- THE FOUR ADDRESSES, AND WHERE THEY COME FROM --------------------------
 * Not guessed and not read out of a running table. src/__sinit_ov027_02112df8.cpp
 * names, in source order, which .data pair fills which .bss slot, and the pairs
 * were re-read for this lane out of extracted/overlays/overlay_0027.bin at
 * (addr - 0x021111a0) -- never extracted/dsd/arm9_overlays, trap T4:
 *
 *   dest offset  filled from            pair reads      body
 *   +0x00        data_ov027_021139d4    {02111ca8, 0}   func_ov027_02111ca8
 *   +0x08        data_ov027_021139cc    {02111c48, 0}   func_ov027_02111c48
 *   +0x10        data_ov027_021139dc    {02111b2c, 0}   func_ov027_02111b2c
 *   +0x18        data_ov027_021139c4    {02111a28, 0}   func_ov027_02111a28
 *
 * All four adjustment words are ZERO, so all four are the NON-VIRTUAL arm of
 * the dispatch and the code word is called directly. All four bodies have a
 * matched src TU and all four are in port/slice_ov027cast.txt. There is no
 * hole.
 *
 * ---- THE SEAT IS IDEMPOTENT, AND THAT IS DELIBERATE ------------------------
 * The other seats in this tree carry a `static int done` and abort if a word is
 * not the ROM's. Both halves of that are a hazard here: .dsstate rolls the
 * mount back on a save-state restore, so a seat that refuses to run twice
 * leaves DS addresses in the table afterwards, while a seat that aborts on an
 * already-seated word turns its own second run into a crash. So each word is
 * matched three ways: the ROM's address (seat it), the host address already
 * written (nothing to do), anything else (abort by name). Re-running is free,
 * and re-running is what a rollback needs.
 *
 * ---- TRAP T2, AND WHY THIS FILE NAMES THE FOUR SOURCES ---------------------
 * data_ov027_021139c4 / _021139cc / _021139d4 / _021139dc are mounted romdata
 * whose bytes include a RELOCATED word -- a DS code address, which is exactly
 * what port/tools/ptr_audit.py sweeps for. The audit's rule is that a host TU
 * must NAME the symbol, and a comment is not a name. The seat below writes all
 * four, so the references are load-bearing.
 */
#include <cstdio>
#include <cstdlib>
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

void port_dapgdfdr_states_seat(void);
void port_dapgdfdr_states_check(void);

}

namespace {

struct DfdrRow {
    unsigned char *src;    /* the .data source pair the sinit copies */
    unsigned rom;          /* the ROM's own code word */
    void *host;            /* the host body that replaces it */
    unsigned dest;         /* the .bss offset the sinit copies it to */
};

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

void pair_set_code(unsigned char *p, unsigned v)
{
    std::memcpy(p, &v, sizeof v);
}

DfdrRow *dfdr_rows(void)
{
    static DfdrRow r[4] = {
        { data_ov027_021139d4, 0x02111ca8u, (void *)&func_ov027_02111ca8, 0x00u },
        { data_ov027_021139cc, 0x02111c48u, (void *)&func_ov027_02111c48, 0x08u },
        { data_ov027_021139dc, 0x02111b2cu, (void *)&func_ov027_02111b2c, 0x10u },
        { data_ov027_021139c4, 0x02111a28u, (void *)&func_ov027_02111a28, 0x18u },
    };
    return r;
}

} /* namespace */

/* Called from port_actor_overlays_sinits() in hal/actor_overlays.cpp,
   IMMEDIATELY BEFORE __sinit_ov027_02112df8() copies the four pairs into
   data_ov027_02113ce4 -- the gate-178 ordering exactly. */
extern "C" void port_dapgdfdr_states_seat(void)
{
    DfdrRow *r = dfdr_rows();
    for (int i = 0; i < 4; ++i) {
        unsigned code = pair_code(r[i].src);
        unsigned adj = pair_adj(r[i].src);
        unsigned host = (unsigned)(size_t)r[i].host;

        if (adj != 0u) {
            std::fprintf(stderr, "FATAL: daPgDfdr_c state source %d: adjustment "
                         "word is 0x%08x, the ROM's own table says 0 -- that is "
                         "the VIRTUAL arm and no measurement covers it\n",
                         i, adj);
            std::abort();
        }
        if (code == host)
            continue;               /* already seated; a rollback re-ran us */
        if (code != r[i].rom) {
            std::fprintf(stderr, "FATAL: daPgDfdr_c state source %d: the mount "
                         "holds 0x%08x, the ROM's own table says 0x%08x -- "
                         "WRONG BYTES\n", i, code, r[i].rom);
            std::abort();
        }
        pair_set_code(r[i].src, host);
    }
}

/* The constructor's copy, checked once it has run. Called from
   hal/actor_classes_ov027.cpp's fill. Before this lane it compared the copy
   against the ROM's own addresses; the seat above now replaces those with host
   ones before the sinit reads them, so what this checks is that the seat
   propagated -- source and destination both holding the SAME host body. */
extern "C" void port_dapgdfdr_states_check(void)
{
    static int done;
    if (done)
        return;
    done = 1;

    DfdrRow *r = dfdr_rows();
    for (int i = 0; i < 4; ++i) {
        unsigned host = (unsigned)(size_t)r[i].host;
        unsigned s = pair_code(r[i].src);
        unsigned d = pair_code(data_ov027_02113ce4 + r[i].dest);
        if (s == host && d == host)
            continue;
        static char msg[192];
        std::snprintf(msg, sizeof msg,
                      "daPgDfdr_c state table row %d: source reads 0x%08x, "
                      "dest +0x%02x reads 0x%08x, expected the seated host "
                      "body 0x%08x",
                      i, s, r[i].dest, d, host);
        std::fprintf(stderr, "UNHOSTED: %s\n", msg);
        port_actor_slot_decline(msg);
    }
}
