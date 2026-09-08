/* CUTSCENE_OBJECT's TEN pointer-to-member STATE tables: THE SIXTY-NINE SOURCE
 * RECORDS ARE SEATED AND THE TEN READERS ARE THE DECOMP'S OWN -- run link100,
 * lane PMFB6, gate 1.
 *
 * WHAT THIS FILE USED TO BE. Lane TAIL2 wrote it in wave 1 as an ADDRESS
 * SWITCH: it hosted all ten readers, read the ROM's own {function,
 * this-adjustment} record, checked the function word against the address the
 * host body was compiled from and called the host body. Ten matched TUs stood
 * behind it, unlinked.
 *
 * WHY IT IS NOT THAT ANY MORE, AND THE MEASUREMENT THAT SETTLES IT. TAIL2's
 * stated reason was that MSVC forms a pointer-to-member over an INCOMPLETE
 * class as the four-word GENERAL representation, so `arr[i]` would read
 * sixteen bytes out of an eight-byte record. That was true when it was
 * written and it is not true now: port/CMakeLists.txt block R8 turns /vmg
 * /vmm on for every C++ source in the target, and this box's own cl
 * 19.44.35228 lays a forward-declared-class PMF out in EIGHT bytes with that
 * pair. Re-measured for this gate on all ten readers, off each one's own
 * /FAsc listing under the port's own flags
 * (runs/link100/out/PMFB6/emit_gate1_out.txt):
 *
 *     reader                 table               emitted operand   arity
 *     func_ov002_020f3310    data_ov002_02110e24 [eax*8],[eax*8+4]   1
 *     func_ov002_020f3d98    data_ov002_02110e34 [eax*8],[eax*8+4]   1
 *     func_ov002_020f37a0    data_ov002_02110e4c [eax*8],[eax*8+4]   1
 *     func_ov002_020f5848    data_ov002_02110e64 [eax*8],[eax*8+4]   1
 *     func_ov002_020f5990    data_ov002_02110e7c [eax*8],[eax*8+4]   1
 *     func_ov002_020f3740    data_ov002_02110e9c [eax*8],[eax*8+4]   1
 *     func_ov002_020f2dd4    data_ov002_02110ebc [eax*8],[eax*8+4]   1
 *     func_ov002_020f562c    data_ov002_02110eec [eax*8],[eax*8+4]   1
 *     func_ov002_020f5dd8    data_ov002_02110f34 [eax*8],[eax*8+4]   0
 *     func_ov002_020f5f0c    data_ov002_02110f9c [edi*8],[edi*8+4]   0
 *
 * Ten for ten the operand is the two words of ONE ROM record. /Zp4 was
 * compiled on every one of the ten and each pair of listings came back
 * identical outside the TITLE line naming the .obj: 0 diff lines. It is not
 * claimed on this gate's CMake block.
 *
 * THE ARITY IS READ PER ROW, not inherited. Eight of the ten dispatch as
 *     push <index> / mov ecx,TAB[i*8+4] / mov eax,TAB[i*8] / add ecx,this /
 *     call eax
 * with no `add esp` after the call, which is __thiscall's callee-popped stack
 * half; their faces are __fastcall(self, dead_edx, int) and pop that word the
 * same way. data_ov002_02110f34 and data_ov002_02110f9c push NOTHING, so their
 * faces are __fastcall(self, dead_edx). func_ov002_020f5dd8 TAIL JUMPS
 * (`jmp eax`) rather than calling, and a face is right there too: a tail jump
 * leaves ecx set exactly as the call does and the face's own `ret 0` returns
 * to the reader's caller.
 *
 * ---- WHAT THE ROM ACTUALLY HAS ---------------------------------------------
 *
 * data_ov002_0210b650..0x0210b8b8 are NOT vtables. They are eight-byte mwcc
 * pointer-to-member cells, {function, this-adjustment}, and
 * __sinit_ov002_02107370 (matched, linked, slice_gate10.txt:447) copies
 * SIXTY-NINE of them into ov002 bss as ten State arrays that tile
 * 0x02110e24..0x0211104c with no gap:
 *
 *     0x02110e24   2 cells   func_ov002_020f3310    (C::*)(int)
 *     0x02110e34   3 cells   func_ov002_020f3d98    (C::*)(int)
 *     0x02110e4c   3 cells   func_ov002_020f37a0    (C::*)(int)
 *     0x02110e64   3 cells   func_ov002_020f5848    (C::*)(int)
 *     0x02110e7c   4 cells   func_ov002_020f5990    (C::*)(int)
 *     0x02110e9c   4 cells   func_ov002_020f3740    (C::*)(int)
 *     0x02110ebc   6 cells   func_ov002_020f2dd4    (C::*)(int)
 *     0x02110eec   9 cells   func_ov002_020f562c    (C::*)(int)
 *     0x02110f34  13 cells   func_ov002_020f5dd8    (C::*)()
 *     0x02110f9c  22 cells   func_ov002_020f5f0c    (C::*)()
 *
 * port/tools/tail2_cells.py refuses any cell whose image word and relocation
 * disagree, whose adjustment is non-zero, whose +4 carries a relocation of its
 * own, or whose array has a hole. All of them pass on this tree with DEFECTS 0
 * (runs/link100/out/PMFB6/rom_gate1.txt). The sinit's own S16 record is TWO
 * eight-byte cells written field by field, `.x` at flat index 2i and `.y` at
 * 2i+1, and five of the ten arrays end on an `.x` with no `.y` after it -- but
 * every one of those unwritten `.y` slots is the FIRST CELL OF THE NEXT ARRAY,
 * which that array's own `[0].x` fills. There is no cell in the run without a
 * cartridge word, which is the precondition lane PMFB3 wrote for taking these
 * rows at all.
 *
 * ---- WHY THE SEAT IS ON THE SOURCE RECORDS AND NOT ON THE TABLES -----------
 *
 * The sinit is linked and runs, so a seat written into the bss tables would be
 * overwritten by the ROM's own copy loop. Seating the sixty-nine records the
 * loop copies FROM leaves that loop doing the ROM's own work and cannot be
 * undone by it. Each record is checked against its cartridge word before it is
 * written, so a mount holding bytes that are not the ROM's is loud here rather
 * than fatal later.
 *
 * NOTHING ELSE IN THE CARTRIDGE READS THOSE RECORDS. A relocation sweep over
 * arm9 and all 103 overlay reloc files for this gate: all sixty-nine records
 * are referenced only from the sinit's own literal pool 0x02107e4c..0x02107f84,
 * each of the ten tables is referenced exactly twice (its own reader's pool and
 * the sinit's), and no relocation points into a table anywhere but at its base.
 * So there is no by-value comparison sentinel here of the kind lane MGWRITER
 * found at data_ov004_020bc254, and none of the sixty-nine has to move with a
 * partner. (ov006 carries three relocations into 0x02110e28 and 0x0211102c;
 * those addresses are ov006's OWN .text -- ov002 .bss and ov006 .text share the
 * load window -- and are not these tables.) port_ov002_patch, the generated
 * pointer rebase, writes 0 of the sixty-nine.
 *
 * ---- ONE FACE PER CELL, AND THREE OF THEM ABORT ---------------------------
 *
 * The faces are per CELL and not per body, which is lane PMFB5's rule: cells
 * 5..8 and 9..12 and 13..16 and 17..20 of data_ov002_02110f9c carry the same
 * four code words four times each and stay distinguishable that way, and the
 * census below can name the cell the game entered instead of the body.
 *
 * data_ov002_02110eec[2], [6] and [8] name func_ov002_020f5010,
 * func_ov002_020f43cc and func_ov002_020f3de4, for which there is NO src file
 * anywhere in the tree -- they are not guess-marked, they are simply not
 * decompiled. Those three cells get an ABORTING face that names itself. A cell
 * with no matched TU is never given a neighbour's body or an invented one.
 *
 * PORT_HOST_ABI: the ROM's own {code, adjust} records hold DS code addresses,
 * so the port seats them with the host addresses of the faces above; the
 * dispatch itself is the matched TU's, through MSVC's own eight-byte
 * pointer to member.
 */
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>

/* The ten readers are the decomp's own TUs now, and each asks the linker for
 * its table under the DECORATED spelling MSVC gives a C++-linkage declaration
 * of an array of pointer-to-member. The mount publishes the plain C name. Each
 * directive below was read off that reader's own /FAsc EXTRN line. */
#pragma comment(linker, "/alternatename:?data_ov002_02110e24@@3PAP8C@@AEXH@ZA=_data_ov002_02110e24")
#pragma comment(linker, "/alternatename:?data_ov002_02110e34@@3PAP8C@@AEXH@ZA=_data_ov002_02110e34")
#pragma comment(linker, "/alternatename:?data_ov002_02110e4c@@3PAUEntry@@A=_data_ov002_02110e4c")
#pragma comment(linker, "/alternatename:?data_ov002_02110e64@@3PAP8C@@AEXH@ZA=_data_ov002_02110e64")
#pragma comment(linker, "/alternatename:?data_ov002_02110e7c@@3PAP8C@@AEXH@ZA=_data_ov002_02110e7c")
#pragma comment(linker, "/alternatename:?data_ov002_02110e9c@@3PAP8C@@AEXH@ZA=_data_ov002_02110e9c")
#pragma comment(linker, "/alternatename:?data_ov002_02110ebc@@3PAUEntry@@A=_data_ov002_02110ebc")
#pragma comment(linker, "/alternatename:?data_ov002_02110eec@@3PAP8C@@AEXH@ZA=_data_ov002_02110eec")
#pragma comment(linker, "/alternatename:?data_ov002_02110f34@@3PAP8C@@AEXXZA=_data_ov002_02110f34")
#pragma comment(linker, "/alternatename:?data_ov002_02110f9c@@3PAP8C@@AEXXZA=_data_ov002_02110f9c")

extern "C" {
/* THE MATCHED STATE BODIES, each declared with ITS OWN parameter list read out
   of its matched TU for this gate (not carried from the retired host file's
   comment column). A body in an arity-1 table takes (self, int); a body in an
   arity-0 table takes (self) or nothing. Every one was checked against the
   arity its array dispatches with, and a disagreement would have stopped this
   seat rather than been cast around. */
void func_ov002_020f2a78(void *, int);         /* src/func_ov002_020f2a78.c */
void func_ov002_020f2a88(void *, int);         /* src/func_ov002_020f2a88.c */
void func_ov002_020f2aec(void *, int);         /* src/func_ov002_020f2aec.c */
void func_ov002_020f2bf4(void *, int);         /* src/func_ov002_020f2bf4.c */
void func_ov002_020f2d0c(void *, int);         /* src/func_ov002_020f2d0c.c */
void func_ov002_020f2d70(void *, int);         /* src/func_ov002_020f2d70.c */
void func_ov002_020f30f8(void *, int);         /* src/func_ov002_020f30f8.c */
void func_ov002_020f32e4(void *, int);         /* src/func_ov002_020f32e4.c */
void func_ov002_020f3310(void *, int);         /* src/func_ov002_020f3310.cpp */
void func_ov002_020f335c(void *, int);         /* src/func_ov002_020f335c.c */
void func_ov002_020f340c(void *, int);         /* src/func_ov002_020f340c.c */
void func_ov002_020f34bc(void *, int);         /* src/func_ov002_020f34bc.c */
void func_ov002_020f35e4(void *, int);         /* src/func_ov002_020f35e4.c */
void func_ov002_020f3740(void *, int);         /* src/func_ov002_020f3740.cpp */
void func_ov002_020f378c(void *, int);         /* src/func_ov002_020f378c.c */
void func_ov002_020f3ae8(void *, int);         /* src/func_ov002_020f3ae8.c */
void func_ov002_020f3ba0(void *, int);         /* src/func_ov002_020f3ba0.c */
void func_ov002_020f3d38(void *, int);         /* src/func_ov002_020f3d38.c */
void func_ov002_020f3d98(void *, int);         /* src/func_ov002_020f3d98.cpp */
void func_ov002_020f40fc(void *, int);         /* src/func_ov002_020f40fc.c */
void func_ov002_020f4710(void *, int);         /* src/func_ov002_020f4710.c */
void func_ov002_020f4a2c(void *, int);         /* src/func_ov002_020f4a2c.c */
void func_ov002_020f4d70(void *, int);         /* src/func_ov002_020f4d70.c */
void func_ov002_020f5328(void *, int);         /* src/func_ov002_020f5328.c */
void func_ov002_020f55b4(void *, int);         /* src/func_ov002_020f55b4.c */
void func_ov002_020f562c(void *, int);         /* src/func_ov002_020f562c.cpp */
void func_ov002_020f5678(void *, int);         /* src/func_ov002_020f5678.c */
void func_ov002_020f569c(void *, int);         /* src/func_ov002_020f569c.c */
void func_ov002_020f57c0(void *, int);         /* src/func_ov002_020f57c0.c */
void func_ov002_020f5848(void *, int);         /* src/func_ov002_020f5848.cpp */
void func_ov002_020f5894(void *, int);         /* src/func_ov002_020f5894.c */
void func_ov002_020f5b24(void *);              /* src/func_ov002_020f5b24.c */
void func_ov002_020f5b7c(void);                /* src/func_ov002_020f5b7c.c */
void func_ov002_020f5b80(void);                /* src/func_ov002_020f5b80.c */
void func_ov002_020f5b84(void);                /* src/func_ov002_020f5b84.c */
void func_ov002_020f5b88(void);                /* src/func_ov002_020f5b88.c */
void func_ov002_020f5b8c(void *);              /* src/func_ov002_020f5b8c.c */
void func_ov002_020f5b98(void *);              /* src/func_ov002_020f5b98.c */
void func_ov002_020f5bcc(void *);              /* src/func_ov002_020f5bcc.c */
void func_ov002_020f5bf4(void *);              /* src/func_ov002_020f5bf4.c */
void func_ov002_020f5c40(void *);              /* src/func_ov002_020f5c40.c */
void func_ov002_020f5c88(void *);              /* src/func_ov002_020f5c88.c */
void func_ov002_020f5cd0(void *);              /* src/func_ov002_020f5cd0.c */
void func_ov002_020f5d34(void *);              /* src/func_ov002_020f5d34.c */
void func_ov002_020f5e18(void *);              /* src/func_ov002_020f5e18.c */
void func_ov002_020f5e38(void *);              /* src/func_ov002_020f5e38.c */
void func_ov002_020f5e58(void *);              /* src/func_ov002_020f5e58.c */
void func_ov002_020f5e78(void *);              /* src/func_ov002_020f5e78.c */
void func_ov002_020f5e98(void *);              /* src/func_ov002_020f5e98.c */
void func_ov002_020f5eb8(void *);              /* src/func_ov002_020f5eb8.c */
void func_ov002_020f5ed8(void *);              /* src/func_ov002_020f5ed8.c */
void func_ov002_020f5ee4(void *);              /* src/func_ov002_020f5ee4.c */
void func_ov002_020f5ef0(void *);              /* src/func_ov002_020f5ef0.c */
void func_ov002_020f5efc(void);                /* src/func_ov002_020f5efc.c */

/* The sixty-nine SOURCE RECORDS in ov002 .data, as the generated mount
   publishes them: eight bytes apiece, {function, this-adjustment}. */
extern unsigned char data_ov002_0210b770[];
extern unsigned char data_ov002_0210b760[];
extern unsigned char data_ov002_0210b810[];
extern unsigned char data_ov002_0210b6d8[];
extern unsigned char data_ov002_0210b800[];
extern unsigned char data_ov002_0210b7d8[];
extern unsigned char data_ov002_0210b7d0[];
extern unsigned char data_ov002_0210b6c0[];
extern unsigned char data_ov002_0210b788[];
extern unsigned char data_ov002_0210b780[];
extern unsigned char data_ov002_0210b778[];
extern unsigned char data_ov002_0210b668[];
extern unsigned char data_ov002_0210b7a8[];
extern unsigned char data_ov002_0210b848[];
extern unsigned char data_ov002_0210b700[];
extern unsigned char data_ov002_0210b6b0[];
extern unsigned char data_ov002_0210b7b0[];
extern unsigned char data_ov002_0210b7a0[];
extern unsigned char data_ov002_0210b798[];
extern unsigned char data_ov002_0210b790[];
extern unsigned char data_ov002_0210b878[];
extern unsigned char data_ov002_0210b870[];
extern unsigned char data_ov002_0210b6f8[];
extern unsigned char data_ov002_0210b6f0[];
extern unsigned char data_ov002_0210b6e0[];
extern unsigned char data_ov002_0210b758[];
extern unsigned char data_ov002_0210b710[];
extern unsigned char data_ov002_0210b748[];
extern unsigned char data_ov002_0210b740[];
extern unsigned char data_ov002_0210b738[];
extern unsigned char data_ov002_0210b730[];
extern unsigned char data_ov002_0210b8b0[];
extern unsigned char data_ov002_0210b728[];
extern unsigned char data_ov002_0210b890[];
extern unsigned char data_ov002_0210b7f8[];
extern unsigned char data_ov002_0210b6b8[];
extern unsigned char data_ov002_0210b7e8[];
extern unsigned char data_ov002_0210b680[];
extern unsigned char data_ov002_0210b720[];
extern unsigned char data_ov002_0210b6c8[];
extern unsigned char data_ov002_0210b6d0[];
extern unsigned char data_ov002_0210b650[];
extern unsigned char data_ov002_0210b7c8[];
extern unsigned char data_ov002_0210b818[];
extern unsigned char data_ov002_0210b820[];
extern unsigned char data_ov002_0210b7c0[];
extern unsigned char data_ov002_0210b7b8[];
extern unsigned char data_ov002_0210b718[];
extern unsigned char data_ov002_0210b690[];
extern unsigned char data_ov002_0210b6a0[];
extern unsigned char data_ov002_0210b850[];
extern unsigned char data_ov002_0210b7f0[];
extern unsigned char data_ov002_0210b898[];
extern unsigned char data_ov002_0210b688[];
extern unsigned char data_ov002_0210b698[];
extern unsigned char data_ov002_0210b830[];
extern unsigned char data_ov002_0210b880[];
extern unsigned char data_ov002_0210b8a0[];
extern unsigned char data_ov002_0210b840[];
extern unsigned char data_ov002_0210b860[];
extern unsigned char data_ov002_0210b8b8[];
extern unsigned char data_ov002_0210b660[];
extern unsigned char data_ov002_0210b858[];
extern unsigned char data_ov002_0210b670[];
extern unsigned char data_ov002_0210b7e0[];
extern unsigned char data_ov002_0210b828[];
extern unsigned char data_ov002_0210b8a8[];
extern unsigned char data_ov002_0210b808[];
extern unsigned char data_ov002_0210b838[];
}  /* extern "C" */

/* ---- the per-cell census ---------------------------------------------------
   SM64DS_CUTSCENE_STATES=1: a per-cell census, printed once at exit. Inert
   unset (one getenv, cached), and it is a MEASUREMENT rather than an
   assertion -- without it a table nothing dispatches and a table that runs
   every frame produce exactly the same silence, which are opposite findings.
   THE COUNT MOVED WITH THE DISPATCH. It used to be bumped inside the host
   readers; the readers are the decomp's now, so each FACE bumps it as it is
   entered, which counts the same event one call deeper. The line format is
   unchanged because port/tools/tail2_states_proof.py joins it against
   build/tail2_cells.tsv. */
static struct { unsigned arr; unsigned idx; unsigned long n; } g_st_seen[128];
static int g_st_nseen;
static int g_st_on = -1;

static void st_report(void)
{
    std::printf("  [cutscene-states] %d distinct cells entered\n", g_st_nseen);
    for (int i = 0; i < g_st_nseen; ++i)
        std::printf("  [cutscene-states] %08x[%u] x%lu\n", g_st_seen[i].arr,
                    g_st_seen[i].idx, g_st_seen[i].n);
    std::fflush(stdout);
}

static void st_note(unsigned arr, unsigned idx)
{
    if (g_st_on < 0) {
        g_st_on = std::getenv("SM64DS_CUTSCENE_STATES") != 0;
        if (g_st_on) std::atexit(st_report);
    }
    if (!g_st_on) return;
    for (int i = 0; i < g_st_nseen; ++i)
        if (g_st_seen[i].arr == arr && g_st_seen[i].idx == idx) {
            ++g_st_seen[i].n;
            return;
        }
    if (g_st_nseen < (int)(sizeof g_st_seen / sizeof g_st_seen[0])) {
        g_st_seen[g_st_nseen].arr = arr;
        g_st_seen[g_st_nseen].idx = idx;
        g_st_seen[g_st_nseen].n = 1;
        ++g_st_nseen;
    }
}

/* A cell whose body has no matched TU anywhere in src/. It is reached only if
   the game actually indexes that cell, and then it says which one and stops
   instead of running something this file invented. */
static void st_unhosted(unsigned arr, unsigned idx, unsigned rom)
{
    std::fprintf(stderr, "UNHOSTED: data_ov002_%08x[%u] is func_ov002_%08x, "
                 "which has no matched TU anywhere in src/ -- see the header "
                 "of hal/cutscene_states_link100.cpp\n", arr, idx, rom);
    std::abort();
}

/* ---- the faces, ONE PER CELL ---------------------------------------------
   ST_FACE1  an arity-1 table: __thiscall pushed one word and the reader does
             not clean up after the call, so the face pops it (__fastcall's own
             stack half is callee-popped) and hands both on as the cdecl
             arguments the ROM's state bodies take.
   ST_FACE0  an arity-0 table: nothing is pushed, so the face takes only the
             two register arguments and passes the receiver on.
   ST_FACE0V the same, for a body whose matched TU takes no argument at all.
   ST_FACEX  a cell with no matched TU. */
#define ST_FACE1(REC, ARR, IDX, SYM)                                          \
    static void __fastcall st_f_##REC(void *self, void *dead_edx, int arg)    \
    {                                                                         \
        (void)dead_edx;                                                       \
        st_note(0x##ARR, IDX);                                                \
        SYM(self, arg);                                                       \
    }
#define ST_FACE0(REC, ARR, IDX, SYM)                                          \
    static void __fastcall st_f_##REC(void *self, void *dead_edx)             \
    {                                                                         \
        (void)dead_edx;                                                       \
        st_note(0x##ARR, IDX);                                                \
        SYM(self);                                                            \
    }
#define ST_FACE0V(REC, ARR, IDX, SYM)                                         \
    static void __fastcall st_f_##REC(void *self, void *dead_edx)             \
    {                                                                         \
        (void)self;                                                           \
        (void)dead_edx;                                                       \
        st_note(0x##ARR, IDX);                                                \
        SYM();                                                                \
    }
#define ST_FACEX(REC, ARR, IDX, ROM)                                          \
    static void __fastcall st_f_##REC(void *self, void *dead_edx, int arg)    \
    {                                                                         \
        (void)self;                                                           \
        (void)dead_edx;                                                       \
        (void)arg;                                                            \
        st_note(0x##ARR, IDX);                                                \
        st_unhosted(0x##ARR, IDX, 0x##ROM);                                   \
    }


/* ---- data_ov002_02110e24: 2 cells, read by func_ov002_020f3310, arity 1 */
ST_FACE1 (0210b770, 02110e24,  0, func_ov002_020f32e4)
ST_FACE1 (0210b760, 02110e24,  1, func_ov002_020f30f8)

/* ---- data_ov002_02110e34: 3 cells, read by func_ov002_020f3d98, arity 1 */
ST_FACE1 (0210b810, 02110e34,  0, func_ov002_020f3d38)
ST_FACE1 (0210b6d8, 02110e34,  1, func_ov002_020f3ba0)
ST_FACE1 (0210b800, 02110e34,  2, func_ov002_020f3ae8)

/* ---- data_ov002_02110e4c: 3 cells, read by func_ov002_020f37a0, arity 1 */
ST_FACE1 (0210b7d8, 02110e4c,  0, func_ov002_020f378c)
ST_FACE1 (0210b7d0, 02110e4c,  1, func_ov002_020f3740)
ST_FACE1 (0210b6c0, 02110e4c,  2, func_ov002_020f3310)

/* ---- data_ov002_02110e64: 3 cells, read by func_ov002_020f5848, arity 1 */
ST_FACE1 (0210b788, 02110e64,  0, func_ov002_020f57c0)
ST_FACE1 (0210b780, 02110e64,  1, func_ov002_020f569c)
ST_FACE1 (0210b778, 02110e64,  2, func_ov002_020f5678)

/* ---- data_ov002_02110e7c: 4 cells, read by func_ov002_020f5990, arity 1 */
ST_FACE1 (0210b668, 02110e7c,  0, func_ov002_020f5894)
ST_FACE1 (0210b7a8, 02110e7c,  1, func_ov002_020f5848)
ST_FACE1 (0210b848, 02110e7c,  2, func_ov002_020f562c)
ST_FACE1 (0210b700, 02110e7c,  3, func_ov002_020f3d98)

/* ---- data_ov002_02110e9c: 4 cells, read by func_ov002_020f3740, arity 1 */
ST_FACE1 (0210b6b0, 02110e9c,  0, func_ov002_020f35e4)
ST_FACE1 (0210b7b0, 02110e9c,  1, func_ov002_020f34bc)
ST_FACE1 (0210b7a0, 02110e9c,  2, func_ov002_020f340c)
ST_FACE1 (0210b798, 02110e9c,  3, func_ov002_020f335c)

/* ---- data_ov002_02110ebc: 6 cells, read by func_ov002_020f2dd4, arity 1 */
ST_FACE1 (0210b790, 02110ebc,  0, func_ov002_020f2d70)
ST_FACE1 (0210b878, 02110ebc,  1, func_ov002_020f2d0c)
ST_FACE1 (0210b870, 02110ebc,  2, func_ov002_020f2bf4)
ST_FACE1 (0210b6f8, 02110ebc,  3, func_ov002_020f2aec)
ST_FACE1 (0210b6f0, 02110ebc,  4, func_ov002_020f2a88)
ST_FACE1 (0210b6e0, 02110ebc,  5, func_ov002_020f2a78)

/* ---- data_ov002_02110eec: 9 cells, read by func_ov002_020f562c, arity 1 */
ST_FACE1 (0210b758, 02110eec,  0, func_ov002_020f55b4)
ST_FACE1 (0210b710, 02110eec,  1, func_ov002_020f5328)
ST_FACEX (0210b748, 02110eec,  2, 020f5010)
ST_FACE1 (0210b740, 02110eec,  3, func_ov002_020f4d70)
ST_FACE1 (0210b738, 02110eec,  4, func_ov002_020f4a2c)
ST_FACE1 (0210b730, 02110eec,  5, func_ov002_020f4710)
ST_FACEX (0210b8b0, 02110eec,  6, 020f43cc)
ST_FACE1 (0210b728, 02110eec,  7, func_ov002_020f40fc)
ST_FACEX (0210b890, 02110eec,  8, 020f3de4)

/* ---- data_ov002_02110f34: 13 cells, read by func_ov002_020f5dd8, arity 0 */
ST_FACE0 (0210b7f8, 02110f34,  0, func_ov002_020f5d34)
ST_FACE0 (0210b6b8, 02110f34,  1, func_ov002_020f5cd0)
ST_FACE0 (0210b7e8, 02110f34,  2, func_ov002_020f5c88)
ST_FACE0 (0210b680, 02110f34,  3, func_ov002_020f5c40)
ST_FACE0 (0210b720, 02110f34,  4, func_ov002_020f5bf4)
ST_FACE0 (0210b6c8, 02110f34,  5, func_ov002_020f5bcc)
ST_FACE0 (0210b6d0, 02110f34,  6, func_ov002_020f5b98)
ST_FACE0 (0210b650, 02110f34,  7, func_ov002_020f5b8c)
ST_FACE0V(0210b7c8, 02110f34,  8, func_ov002_020f5b88)
ST_FACE0V(0210b818, 02110f34,  9, func_ov002_020f5b84)
ST_FACE0V(0210b820, 02110f34, 10, func_ov002_020f5b80)
ST_FACE0V(0210b7c0, 02110f34, 11, func_ov002_020f5b7c)
ST_FACE0 (0210b7b8, 02110f34, 12, func_ov002_020f5b24)

/* ---- data_ov002_02110f9c: 22 cells, read by func_ov002_020f5f0c, arity 0 */
ST_FACE0V(0210b718, 02110f9c,  0, func_ov002_020f5efc)
ST_FACE0 (0210b690, 02110f9c,  1, func_ov002_020f5ef0)
ST_FACE0 (0210b6a0, 02110f9c,  2, func_ov002_020f5ee4)
ST_FACE0 (0210b850, 02110f9c,  3, func_ov002_020f5ed8)
ST_FACE0 (0210b7f0, 02110f9c,  4, func_ov002_020f5eb8)
ST_FACE0 (0210b898, 02110f9c,  5, func_ov002_020f5e98)
ST_FACE0 (0210b688, 02110f9c,  6, func_ov002_020f5e98)
ST_FACE0 (0210b698, 02110f9c,  7, func_ov002_020f5e98)
ST_FACE0 (0210b830, 02110f9c,  8, func_ov002_020f5e98)
ST_FACE0 (0210b880, 02110f9c,  9, func_ov002_020f5e78)
ST_FACE0 (0210b8a0, 02110f9c, 10, func_ov002_020f5e78)
ST_FACE0 (0210b840, 02110f9c, 11, func_ov002_020f5e78)
ST_FACE0 (0210b860, 02110f9c, 12, func_ov002_020f5e78)
ST_FACE0 (0210b8b8, 02110f9c, 13, func_ov002_020f5e58)
ST_FACE0 (0210b660, 02110f9c, 14, func_ov002_020f5e58)
ST_FACE0 (0210b858, 02110f9c, 15, func_ov002_020f5e58)
ST_FACE0 (0210b670, 02110f9c, 16, func_ov002_020f5e58)
ST_FACE0 (0210b7e0, 02110f9c, 17, func_ov002_020f5e38)
ST_FACE0 (0210b828, 02110f9c, 18, func_ov002_020f5e38)
ST_FACE0 (0210b8a8, 02110f9c, 19, func_ov002_020f5e38)
ST_FACE0 (0210b808, 02110f9c, 20, func_ov002_020f5e38)
ST_FACE0 (0210b838, 02110f9c, 21, func_ov002_020f5e18)

/* ---- the seat -------------------------------------------------------------
   One row per cell: the mounted source record, the cartridge word it must be
   holding, and the face that replaces it. The check is the whole point -- a
   record that does not hold {its own ROM word, 0} means the mount is not the
   cartridge's and the seat stops there. */
struct PortStateSeat {
    unsigned char *rec;      /* the mounted eight bytes in ov002 .data */
    unsigned recrom;         /* and that record's own ROM address */
    unsigned rom;            /* the cartridge's own function word */
    unsigned arr;            /* the table the sinit copies it into */
    unsigned idx;            /* and the cell index inside it */
    void *face;
};

static const PortStateSeat g_st_seat[] = {
    { data_ov002_0210b770, 0x0210b770, 0x020f32e4, 0x02110e24,  0, (void *)st_f_0210b770 },
    { data_ov002_0210b760, 0x0210b760, 0x020f30f8, 0x02110e24,  1, (void *)st_f_0210b760 },
    { data_ov002_0210b810, 0x0210b810, 0x020f3d38, 0x02110e34,  0, (void *)st_f_0210b810 },
    { data_ov002_0210b6d8, 0x0210b6d8, 0x020f3ba0, 0x02110e34,  1, (void *)st_f_0210b6d8 },
    { data_ov002_0210b800, 0x0210b800, 0x020f3ae8, 0x02110e34,  2, (void *)st_f_0210b800 },
    { data_ov002_0210b7d8, 0x0210b7d8, 0x020f378c, 0x02110e4c,  0, (void *)st_f_0210b7d8 },
    { data_ov002_0210b7d0, 0x0210b7d0, 0x020f3740, 0x02110e4c,  1, (void *)st_f_0210b7d0 },
    { data_ov002_0210b6c0, 0x0210b6c0, 0x020f3310, 0x02110e4c,  2, (void *)st_f_0210b6c0 },
    { data_ov002_0210b788, 0x0210b788, 0x020f57c0, 0x02110e64,  0, (void *)st_f_0210b788 },
    { data_ov002_0210b780, 0x0210b780, 0x020f569c, 0x02110e64,  1, (void *)st_f_0210b780 },
    { data_ov002_0210b778, 0x0210b778, 0x020f5678, 0x02110e64,  2, (void *)st_f_0210b778 },
    { data_ov002_0210b668, 0x0210b668, 0x020f5894, 0x02110e7c,  0, (void *)st_f_0210b668 },
    { data_ov002_0210b7a8, 0x0210b7a8, 0x020f5848, 0x02110e7c,  1, (void *)st_f_0210b7a8 },
    { data_ov002_0210b848, 0x0210b848, 0x020f562c, 0x02110e7c,  2, (void *)st_f_0210b848 },
    { data_ov002_0210b700, 0x0210b700, 0x020f3d98, 0x02110e7c,  3, (void *)st_f_0210b700 },
    { data_ov002_0210b6b0, 0x0210b6b0, 0x020f35e4, 0x02110e9c,  0, (void *)st_f_0210b6b0 },
    { data_ov002_0210b7b0, 0x0210b7b0, 0x020f34bc, 0x02110e9c,  1, (void *)st_f_0210b7b0 },
    { data_ov002_0210b7a0, 0x0210b7a0, 0x020f340c, 0x02110e9c,  2, (void *)st_f_0210b7a0 },
    { data_ov002_0210b798, 0x0210b798, 0x020f335c, 0x02110e9c,  3, (void *)st_f_0210b798 },
    { data_ov002_0210b790, 0x0210b790, 0x020f2d70, 0x02110ebc,  0, (void *)st_f_0210b790 },
    { data_ov002_0210b878, 0x0210b878, 0x020f2d0c, 0x02110ebc,  1, (void *)st_f_0210b878 },
    { data_ov002_0210b870, 0x0210b870, 0x020f2bf4, 0x02110ebc,  2, (void *)st_f_0210b870 },
    { data_ov002_0210b6f8, 0x0210b6f8, 0x020f2aec, 0x02110ebc,  3, (void *)st_f_0210b6f8 },
    { data_ov002_0210b6f0, 0x0210b6f0, 0x020f2a88, 0x02110ebc,  4, (void *)st_f_0210b6f0 },
    { data_ov002_0210b6e0, 0x0210b6e0, 0x020f2a78, 0x02110ebc,  5, (void *)st_f_0210b6e0 },
    { data_ov002_0210b758, 0x0210b758, 0x020f55b4, 0x02110eec,  0, (void *)st_f_0210b758 },
    { data_ov002_0210b710, 0x0210b710, 0x020f5328, 0x02110eec,  1, (void *)st_f_0210b710 },
    { data_ov002_0210b748, 0x0210b748, 0x020f5010, 0x02110eec,  2, (void *)st_f_0210b748 },
    { data_ov002_0210b740, 0x0210b740, 0x020f4d70, 0x02110eec,  3, (void *)st_f_0210b740 },
    { data_ov002_0210b738, 0x0210b738, 0x020f4a2c, 0x02110eec,  4, (void *)st_f_0210b738 },
    { data_ov002_0210b730, 0x0210b730, 0x020f4710, 0x02110eec,  5, (void *)st_f_0210b730 },
    { data_ov002_0210b8b0, 0x0210b8b0, 0x020f43cc, 0x02110eec,  6, (void *)st_f_0210b8b0 },
    { data_ov002_0210b728, 0x0210b728, 0x020f40fc, 0x02110eec,  7, (void *)st_f_0210b728 },
    { data_ov002_0210b890, 0x0210b890, 0x020f3de4, 0x02110eec,  8, (void *)st_f_0210b890 },
    { data_ov002_0210b7f8, 0x0210b7f8, 0x020f5d34, 0x02110f34,  0, (void *)st_f_0210b7f8 },
    { data_ov002_0210b6b8, 0x0210b6b8, 0x020f5cd0, 0x02110f34,  1, (void *)st_f_0210b6b8 },
    { data_ov002_0210b7e8, 0x0210b7e8, 0x020f5c88, 0x02110f34,  2, (void *)st_f_0210b7e8 },
    { data_ov002_0210b680, 0x0210b680, 0x020f5c40, 0x02110f34,  3, (void *)st_f_0210b680 },
    { data_ov002_0210b720, 0x0210b720, 0x020f5bf4, 0x02110f34,  4, (void *)st_f_0210b720 },
    { data_ov002_0210b6c8, 0x0210b6c8, 0x020f5bcc, 0x02110f34,  5, (void *)st_f_0210b6c8 },
    { data_ov002_0210b6d0, 0x0210b6d0, 0x020f5b98, 0x02110f34,  6, (void *)st_f_0210b6d0 },
    { data_ov002_0210b650, 0x0210b650, 0x020f5b8c, 0x02110f34,  7, (void *)st_f_0210b650 },
    { data_ov002_0210b7c8, 0x0210b7c8, 0x020f5b88, 0x02110f34,  8, (void *)st_f_0210b7c8 },
    { data_ov002_0210b818, 0x0210b818, 0x020f5b84, 0x02110f34,  9, (void *)st_f_0210b818 },
    { data_ov002_0210b820, 0x0210b820, 0x020f5b80, 0x02110f34, 10, (void *)st_f_0210b820 },
    { data_ov002_0210b7c0, 0x0210b7c0, 0x020f5b7c, 0x02110f34, 11, (void *)st_f_0210b7c0 },
    { data_ov002_0210b7b8, 0x0210b7b8, 0x020f5b24, 0x02110f34, 12, (void *)st_f_0210b7b8 },
    { data_ov002_0210b718, 0x0210b718, 0x020f5efc, 0x02110f9c,  0, (void *)st_f_0210b718 },
    { data_ov002_0210b690, 0x0210b690, 0x020f5ef0, 0x02110f9c,  1, (void *)st_f_0210b690 },
    { data_ov002_0210b6a0, 0x0210b6a0, 0x020f5ee4, 0x02110f9c,  2, (void *)st_f_0210b6a0 },
    { data_ov002_0210b850, 0x0210b850, 0x020f5ed8, 0x02110f9c,  3, (void *)st_f_0210b850 },
    { data_ov002_0210b7f0, 0x0210b7f0, 0x020f5eb8, 0x02110f9c,  4, (void *)st_f_0210b7f0 },
    { data_ov002_0210b898, 0x0210b898, 0x020f5e98, 0x02110f9c,  5, (void *)st_f_0210b898 },
    { data_ov002_0210b688, 0x0210b688, 0x020f5e98, 0x02110f9c,  6, (void *)st_f_0210b688 },
    { data_ov002_0210b698, 0x0210b698, 0x020f5e98, 0x02110f9c,  7, (void *)st_f_0210b698 },
    { data_ov002_0210b830, 0x0210b830, 0x020f5e98, 0x02110f9c,  8, (void *)st_f_0210b830 },
    { data_ov002_0210b880, 0x0210b880, 0x020f5e78, 0x02110f9c,  9, (void *)st_f_0210b880 },
    { data_ov002_0210b8a0, 0x0210b8a0, 0x020f5e78, 0x02110f9c, 10, (void *)st_f_0210b8a0 },
    { data_ov002_0210b840, 0x0210b840, 0x020f5e78, 0x02110f9c, 11, (void *)st_f_0210b840 },
    { data_ov002_0210b860, 0x0210b860, 0x020f5e78, 0x02110f9c, 12, (void *)st_f_0210b860 },
    { data_ov002_0210b8b8, 0x0210b8b8, 0x020f5e58, 0x02110f9c, 13, (void *)st_f_0210b8b8 },
    { data_ov002_0210b660, 0x0210b660, 0x020f5e58, 0x02110f9c, 14, (void *)st_f_0210b660 },
    { data_ov002_0210b858, 0x0210b858, 0x020f5e58, 0x02110f9c, 15, (void *)st_f_0210b858 },
    { data_ov002_0210b670, 0x0210b670, 0x020f5e58, 0x02110f9c, 16, (void *)st_f_0210b670 },
    { data_ov002_0210b7e0, 0x0210b7e0, 0x020f5e38, 0x02110f9c, 17, (void *)st_f_0210b7e0 },
    { data_ov002_0210b828, 0x0210b828, 0x020f5e38, 0x02110f9c, 18, (void *)st_f_0210b828 },
    { data_ov002_0210b8a8, 0x0210b8a8, 0x020f5e38, 0x02110f9c, 19, (void *)st_f_0210b8a8 },
    { data_ov002_0210b808, 0x0210b808, 0x020f5e38, 0x02110f9c, 20, (void *)st_f_0210b808 },
    { data_ov002_0210b838, 0x0210b838, 0x020f5e18, 0x02110f9c, 21, (void *)st_f_0210b838 },
};

extern "C" void port_cutscene_states_seat(void)
{
    static int done;
    if (done)
        return;
    done = 1;
    for (unsigned i = 0; i < sizeof g_st_seat / sizeof g_st_seat[0]; ++i) {
        unsigned fn;
        int delta;
        std::memcpy(&fn, g_st_seat[i].rec, 4);
        std::memcpy(&delta, g_st_seat[i].rec + 4, 4);
        if (fn != g_st_seat[i].rom || delta != 0) {
            std::fprintf(stderr, "FATAL: data_ov002_%08x (the source record "
                         "data_ov002_%08x[%u] is copied from) holds %08x/%d, "
                         "the cartridge's own record says %08x/0 -- WRONG "
                         "BYTES\n",
                         g_st_seat[i].recrom,
                         g_st_seat[i].arr, g_st_seat[i].idx, fn, delta,
                         g_st_seat[i].rom);
            std::abort();
        }
        unsigned host = (unsigned)(std::size_t)g_st_seat[i].face;
        delta = 0;
        std::memcpy(g_st_seat[i].rec, &host, 4);
        std::memcpy(g_st_seat[i].rec + 4, &delta, 4);
    }
}
