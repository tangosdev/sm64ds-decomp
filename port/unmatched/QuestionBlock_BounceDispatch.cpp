/* HOST COPY of src/func_ov102_021498e0.cpp -- the QUESTION_BLOCK's state-1
 * main half, the block bouncing after Mario hits it (daObjHatenaBlock_c,
 * actor 20, ov102), plus the seat for the two pointer-to-member CONTENT tables
 * it dispatches through.
 *
 * WHY A HOST COPY. The matched TU forms `struct C; typedef void (C::*PMF)();`
 * and then reads `data_ov102_0214e870[content].pmf[ch]` /
 * `data_ov102_0214e8c0[content].pmf[ch]` while C is INCOMPLETE. mwcc answers
 * an incomplete-class PMF with the ROM's 8-byte {fn, delta} record; MSVC
 * widens it to its 16-byte general representation, so compiling that dispatch
 * on the host reads the tables at the wrong stride and calls through garbage.
 * The Door / Painting treatment: transcribe the body for the host with the PMF
 * dispatch respelled to read the ROM-layout table explicitly. The CONTENT
 * bodies are NOT copied -- they are matched src TUs, sliced and linked as-is
 * (port/slice_gate180.txt); only this PMF-dispatching frame gets hosted.
 *
 * THE TABLE LAYOUT (verified against the overlay image + the matched
 * __sinit_ov102_0214d908). data_ov102_0214e870 (0x20) and
 * data_ov102_0214e8c0 (0x100) are BSS -- the overlay image ends at 0x0214e6e0,
 * they carry no static relocs, and __sinit_ov102_0214d908 (gate 23) fills them
 * at boot by copying source .data statics. Each content row is 32 bytes --
 * FOUR 8-byte {fn, delta} records, one per ch -- straight from the ROM's own
 * dispatch math at 0x021498e0+0x2a8: `add r0, r1, r0, lsl #5` (base +
 * content*32) then `add r3, r0, r4, lsl #3` (+ ch*8). Flat pair index =
 * content*4 + ch; 0214e8c0 is 8 content rows exactly (0x100/32), 0214e870 is
 * one. The record is mwcc's PMF: word0 = fn (or the vtable offset), word1 =
 * (delta<<1)|virtual-bit. Every record these tables ship is {fn, 0} --
 * non-virtual, zero delta -- so each pair is just a function pointer.
 *
 * THE SEATED STATICS. The .fn word each source static ships is the overlay
 * image's own DS CODE ADDRESS (the ovdata contract, e.g. 0214e310 -> 021492d4,
 * confirmed against config/arm9/overlays/ov102/relocs.txt). Copied into the
 * BSS and called, that jumps into unbuilt DS memory. port_..._content_seat()
 * rewrites the 36 source statics that feed the two content tables to the host
 * addresses of the linked bodies BEFORE __sinit copies them -- the WaterBomb /
 * Cap reading, one less mapping to get wrong than reseating the runtime table
 * after. Each is checked against the ROM address its host body was compiled
 * from, so a mount pointing at the wrong bytes says so instead of calling into
 * the overlay image.
 *
 * THE CASE-0 TABLE-CHOICE FIX is preserved exactly: switch case 0 reads
 * data_ov102_0214e8c0 (NOT 0214e870); only case 2 reads 0214e870. This was a
 * real bug fixed during matching and the relocs confirm it.
 */
#include <cstdio>
#include <cstdlib>

extern "C" {

struct PortPmf { unsigned fn; int delta; };

/* the two runtime content tables __sinit_ov102_0214d908 fills (BSS). Read here
 * as flat {fn,delta} pairs: row `content` at +content*32, pmf[ch] at +ch*8. */
extern PortPmf data_ov102_0214e870[];   /* 1 row  x 4 pmfs = 4 pairs  (0x20)  */
extern PortPmf data_ov102_0214e8c0[];   /* 8 rows x 4 pmfs = 32 pairs (0x100) */

/* the 36 source statics __sinit_ov102_0214d908 copies into those two tables.
 * ov102 relocs ship each as {DS-code-addr, 0}; reseated to host bodies below. */
extern PortPmf
    data_ov102_0214e388[], data_ov102_0214e380[], data_ov102_0214e378[],
    data_ov102_0214e370[], data_ov102_0214e368[], data_ov102_0214e360[],
    data_ov102_0214e358[], data_ov102_0214e350[], data_ov102_0214e348[],
    data_ov102_0214e340[], data_ov102_0214e320[], data_ov102_0214e288[],
    data_ov102_0214e338[], data_ov102_0214e330[], data_ov102_0214e280[],
    data_ov102_0214e318[], data_ov102_0214e2a8[], data_ov102_0214e2b0[],
    data_ov102_0214e270[], data_ov102_0214e298[], data_ov102_0214e2a0[],
    data_ov102_0214e2f8[], data_ov102_0214e2f0[], data_ov102_0214e2e8[],
    data_ov102_0214e390[], data_ov102_0214e2d8[], data_ov102_0214e2d0[],
    data_ov102_0214e2c8[], data_ov102_0214e2c0[], data_ov102_0214e2b8[],
    data_ov102_0214e2e0[], data_ov102_0214e328[],
    data_ov102_0214e310[], data_ov102_0214e308[], data_ov102_0214e300[],
    data_ov102_0214e290[];

/* --- the CONTENT bodies the two tables reach (matched src, linked as-is) --- */
void  func_ov102_02149684(int *dst, void *src);
void *func_ov102_02149220(void *c);
void  func_ov102_02149288(void *c);
void  func_ov102_021492d4(void *c);
void *func_ov102_02149384(void *c);
void  func_ov102_021493dc(void *c);
void  func_ov102_02149428(void *c);
void  func_ov102_02149478(void *c);
int   func_ov102_021494cc(void *c);
void  func_ov102_0214953c(void *c, int p1, int p2);

/* --- the rest of the matched body's closure (already linked) --- */
int  DecIfAbove0_Short(short *p);
void func_ov002_020f0438(void *a);
void _ZN5Sound9PlayBank3EjRK7Vector3(int bank, void *pos);
void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned id, int x, int y, int z);
int  _ZN8SaveData16HasPlayerLostCapEv(void);
void func_ov102_02149da8(void *c, int i);

extern short       data_02082214[];
extern signed char data_0209f2f8;

}  /* extern "C" */

/* ------------------------------------------------------------------------- */
/* Seat: rewrite the 36 content-table source statics to host bodies, then let */
/* __sinit_ov102_0214d908 copy them into the BSS tables. Runs BEFORE that     */
/* __sinit from hal/actor_overlays.cpp.                                       */
/* ------------------------------------------------------------------------- */
/* HOST COPY RETIRED, run link100 wave 7 lane SEAT5, census batch B5.
   src/func_ov102_021498e0.cpp dispatches the two content tables itself now.
   THE WIDTH CLAIM IN THIS FILE'S BANNER IS DEAD: block R8 turns on /vmg AND
   /vmm for every C++ source in this build, and /vmm picks the multiple-
   inheritance worst case, which is EIGHT bytes -- the ROM's own {fn, delta}
   pair. The TU's own /FAsc listing under the port's flags reads the tables at
   the ROM's own stride, at all three indirect sites
   (runs/link100/out/SEAT5/emit_seat5_out.txt):

       movzx eax,BYTE PTR [esi+1011]      content, the +0x3f3 byte
       shl   eax,5                        content*32, the ROM's own lsl #5
       mov   ecx,DWORD PTR tbl[eax+4]     the adjust word
       mov   eax,DWORD PTR tbl[eax]       the code word
       add   ecx,esi                      receiver = this + adjust
       call  eax                          a CALL, receiver in ecx, NOTHING pushed

   and the ch-indexed sites read tbl[eax*8] / tbl[eax*8+4] with eax =
   content*4 + ch -- the ROM's own lsl #3. ARITY ZERO; /Zp4 changes 0 listing
   lines outside the TITLE. So every content record holds a zero-argument
   __fastcall face that forwards the receiver as the one cdecl argument the
   matched content body takes.

   THE UNIVERSE IS CLOSED AND IS THIS ROW'S OWN. The two tables are bss (the
   overlay image ends at 0x0214e6e0), __sinit_ov102_0214d908 fills them from
   exactly the THIRTY-SIX source statics listed below, every one {fn, 0} checked
   against its own reloc destination, and the tables are 0x20 and 0x100 bytes =
   ONE and EIGHT content rows of four records. THE ONLY READER IS THIS ROW: the
   only two code-side `load` relocations that reach either table are the pool
   words at 0x02149c6c and 0x02149c70, and both sit inside func_ov102_021498e0
   itself (0x021498e0 + 0x398 = 0x02149c78); the sinit's own three are the fill.
   NO BY-VALUE SENTINEL: nothing in ov102 compares a stored pair against a
   constant. Measurements in runs/link100/out/SEAT5/ and port/slice_seat5.txt.

   ONE FACE PER CODE WORD, not per record: with no by-value sentinel the two are
   equivalent, and the eight distinct bodies the thirty-six records name need
   eight faces. */
static void __fastcall seat5_qb_021494cc(void *s, void *e)
{ (void)e; func_ov102_021494cc(s); }
static void __fastcall seat5_qb_02149478(void *s, void *e)
{ (void)e; func_ov102_02149478(s); }
static void __fastcall seat5_qb_02149428(void *s, void *e)
{ (void)e; func_ov102_02149428(s); }
static void __fastcall seat5_qb_02149384(void *s, void *e)
{ (void)e; func_ov102_02149384(s); }
static void __fastcall seat5_qb_021493dc(void *s, void *e)
{ (void)e; func_ov102_021493dc(s); }
static void __fastcall seat5_qb_021492d4(void *s, void *e)
{ (void)e; func_ov102_021492d4(s); }
static void __fastcall seat5_qb_02149288(void *s, void *e)
{ (void)e; func_ov102_02149288(s); }
static void __fastcall seat5_qb_02149220(void *s, void *e)
{ (void)e; func_ov102_02149220(s); }

static const struct { PortPmf *slot; unsigned rom; void *host; }
g_qblock_content_statics[] = {
    /* --- 0214e8c0 feeders. Which table offset each lands at is the matched
       __sinit's business, not the seat's: the seat only rewrites each source
       static's fn word, checked against its own reloc destination. --- */
    { data_ov102_0214e388, 0x021494cc, (void *)seat5_qb_021494cc },
    { data_ov102_0214e380, 0x021494cc, (void *)seat5_qb_021494cc },
    { data_ov102_0214e378, 0x021494cc, (void *)seat5_qb_021494cc },
    { data_ov102_0214e370, 0x021494cc, (void *)seat5_qb_021494cc },
    { data_ov102_0214e368, 0x02149478, (void *)seat5_qb_02149478 },
    { data_ov102_0214e360, 0x02149478, (void *)seat5_qb_02149478 },
    { data_ov102_0214e358, 0x02149478, (void *)seat5_qb_02149478 },
    { data_ov102_0214e350, 0x02149478, (void *)seat5_qb_02149478 },
    { data_ov102_0214e348, 0x02149428, (void *)seat5_qb_02149428 },
    { data_ov102_0214e340, 0x02149428, (void *)seat5_qb_02149428 },
    { data_ov102_0214e320, 0x02149428, (void *)seat5_qb_02149428 },
    { data_ov102_0214e288, 0x02149428, (void *)seat5_qb_02149428 },
    { data_ov102_0214e338, 0x02149384, (void *)seat5_qb_02149384 },
    { data_ov102_0214e330, 0x02149384, (void *)seat5_qb_02149384 },
    { data_ov102_0214e280, 0x02149384, (void *)seat5_qb_02149384 },
    { data_ov102_0214e318, 0x02149384, (void *)seat5_qb_02149384 },
    { data_ov102_0214e2a8, 0x021493dc, (void *)seat5_qb_021493dc },
    { data_ov102_0214e2b0, 0x021493dc, (void *)seat5_qb_021493dc },
    { data_ov102_0214e270, 0x021493dc, (void *)seat5_qb_021493dc },
    { data_ov102_0214e298, 0x021493dc, (void *)seat5_qb_021493dc },
    { data_ov102_0214e2a0, 0x021492d4, (void *)seat5_qb_021492d4 },
    { data_ov102_0214e2f8, 0x02149288, (void *)seat5_qb_02149288 },
    { data_ov102_0214e2f0, 0x02149288, (void *)seat5_qb_02149288 },
    { data_ov102_0214e2e8, 0x02149288, (void *)seat5_qb_02149288 },
    { data_ov102_0214e390, 0x02149288, (void *)seat5_qb_02149288 },
    { data_ov102_0214e2d8, 0x02149288, (void *)seat5_qb_02149288 },
    { data_ov102_0214e2d0, 0x02149288, (void *)seat5_qb_02149288 },
    { data_ov102_0214e2c8, 0x02149288, (void *)seat5_qb_02149288 },
    { data_ov102_0214e2c0, 0x02149220, (void *)seat5_qb_02149220 },
    { data_ov102_0214e2b8, 0x02149288, (void *)seat5_qb_02149288 },
    { data_ov102_0214e2e0, 0x02149288, (void *)seat5_qb_02149288 },
    { data_ov102_0214e328, 0x02149288, (void *)seat5_qb_02149288 },
    /* --- 0214e870 feeders --- */
    { data_ov102_0214e310, 0x021492d4, (void *)seat5_qb_021492d4 },
    { data_ov102_0214e308, 0x021492d4, (void *)seat5_qb_021492d4 },
    { data_ov102_0214e300, 0x021492d4, (void *)seat5_qb_021492d4 },
    { data_ov102_0214e290, 0x021492d4, (void *)seat5_qb_021492d4 },
};

extern "C" void port_question_block_content_seat(void)
{
    static int done;
    if (done)
        return;
    done = 1;
    for (unsigned i = 0; i < sizeof g_qblock_content_statics /
                             sizeof g_qblock_content_statics[0]; ++i) {
        PortPmf *p = g_qblock_content_statics[i].slot;
        if (p->fn != g_qblock_content_statics[i].rom || p->delta != 0) {
            std::fprintf(stderr, "FATAL: QuestionBlock content static %u: the "
                         "mount holds %08x/%d, the ROM's own table says "
                         "%08x/0 -- WRONG BYTES\n", i, p->fn, p->delta,
                         g_qblock_content_statics[i].rom);
            std::abort();
        }
        p->fn = (unsigned)(size_t)g_qblock_content_statics[i].host;
    }
}

