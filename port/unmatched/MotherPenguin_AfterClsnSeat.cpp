/* MOTHER_PENGUIN's state table seat, the SoundObject/Cap_StateDispatch
 * treatment applied to a plain pointer-to-member table instead of a PMF
 * dispatch.
 *
 * THE TABLE, RE-READ FROM THE ROM (gate mpg; the gate-191 reading this
 * replaces was right about the bytes and wrong about the SHAPE).
 * __sinit_ov018_02112c80 copies six 8-byte records (data_ov018_0211394c..
 * 02113974, static const in the overlay's own .rodata) verbatim into the
 * mutable instance table data_ov018_02113c4c, which the module's own
 * symbols.txt sizes 0x02113c4c..0x02113c7c = 48 bytes. The ROM's own store
 * offsets are 0, 8, 0x10, 0x18, 0x20, 0x28 (disassembled at 0x02112d28..
 * 0x02112d8c), so the six records are contiguous and 8 bytes each.
 *
 * They are NOT six independent cells. func_ov018_021123d0(self, i) is
 *     add r1, r2, r1, lsl #4      ; r2 = 0x02113c4c
 *     str r1, [r0, #0x370]
 *     bx  ip                      ; ip = 0x02112398
 * -- a SIXTEEN-byte stride. The table is THREE 16-byte entries of TWO
 * pointer-to-member records each, and the class has three states:
 *
 *   entry  offset  record 0 (ENTER, dispatched by func_ov018_02112398)
 *                  record 1 (TICK,  dispatched by func_ov018_0211235c, +8)
 *   ------------------------------------------------------------------
 *     0    0x00    enter func_ov018_021122ec   sets self+0x37c = 0
 *          0x08    tick  func_ov018_02112234
 *     1    0x10    enter func_ov018_021121dc   sets self+0x37c = 1
 *          0x18    tick  func_ov018_02111fac
 *     2    0x20    enter func_ov018_02111f1c   sets self+0x37c = 2
 *          0x28    tick  func_ov018_02111e28
 *
 * THE UNIVERSE IS CLOSED BY THE ROM, not by a count. Every one of the eight
 * arm_call relocations into func_ov018_021123d0 sets r1 immediately before
 * the branch, and the eight values are 0,0,0,0,0,1,1,2 (0x0211258c,
 * 0x02112170, 0x02112010, 0x02111f44, 0x02111f04 = 0; 0x021119b4 and
 * 0x02111a38 = 1; 0x02111bdc = 2). i never exceeds 2, so the three entries
 * above are the whole table and no record is unaccounted for.
 *
 * THE FACE IS RAW __cdecl, and MSVC's own output says so. Both dispatchers
 * compile to a TAIL JUMP:
 *     func_ov018_02112398:  mov eax,[ebp+8] / mov edx,[eax+370h]
 *                           mov ecx,[edx+4] / add ecx,eax
 *                           mov eax,[edx]   / pop ebp / jmp eax
 *     func_ov018_0211235c:  ... mov ecx,[edx+0Ch] / add ecx,eax
 *                           mov eax,[edx+8] / pop ebp / jmp eax
 * so at the jump the dispatcher's own frame is gone and [esp+4] still holds
 * the `self` argument the dispatcher was called with -- exactly where a
 * __cdecl `int f(char *self)` reads it. That is PMFB7's rule ("the enter
 * cells stay untouched because their dispatchers are jmp edx tail jumps")
 * with the tick half measured the same way instead of assumed. No
 * __fastcall face is needed for either half. The records are {code, delta}
 * with delta 0 in all six, and /vmg /vmm makes MSVC's own representation the
 * same 8-byte {code, delta} pair, so the seat writes only the code word.
 *
 * WHAT IS SEATED ARE COUNTING TRAMPOLINES, not the bare bodies -- the
 * dScStarSel_c "tick witness" treatment (hal/scene_boot.cpp's g_ss_hits).
 * A class that is constructed is not a class that RUNS, and from outside the
 * two look identical. Each trampoline is the __cdecl callee of the tail
 * jump, counts, and then calls the ROM body with an ordinary call, so the
 * ROM's own reference graph reaches every one of the six bodies and a run
 * can say how many times.
 *
 * WHAT THIS GATE HAD TO FIX BEFORE THE SEAT COULD BE HONEST (all three are
 * port-side defects the ROM byte gate cannot see):
 *
 *  1. THE TICK DISPATCHER WAS NEVER CALLED. MotherPenguin::Behavior's first
 *     instruction pair is `mov r4,r0; bl 0x0211235c`, and ov018's own
 *     relocs.txt says `from:0x02112488 kind:arm_call to:0x0211235c
 *     module:overlay(18)`. src/_ZN7SkiLift8BehaviorEv.cpp spells that call
 *     `_ZN13RacingPenguin16OnPendingDestroyEv()`, which is ov019's OWN
 *     4-byte `bx lr` at the SAME window address (ov018 and ov019 share load
 *     base 0x021111a0 and are never co-resident). The port linked ov019's
 *     empty body, so the mother penguin's per-frame state tick never ran at
 *     all. Fixed in the Behavior host copy beside this file; the matched src
 *     stays byte-locked and the crossing is reported decomp-side.
 *  2. src/func_ov018_02111e28.cpp declares func_ov018_021123d0 with ONE
 *     parameter and calls it with one. The ROM sets r1 = 0 at 0x02111ef8
 *     (a `mov r1,#0` the source spells as the neighbouring
 *     `*(int*)(s+0x374) = 0`, which is why the file still byte-matches on
 *     ARM). On x86 the second argument lives on the stack, the callee reads
 *     [ebp+0Ch], shifts it left by four and adds it to the table base -- an
 *     uninitialised index and a wild pointer parked in self+0x370. The CMake
 *     gate block renames that one call to port_mpg_set_state_zero below,
 *     which supplies the ROM's own 0.
 *  3. src/func_ov018_021121dc.cpp names its animation file record
 *     data_ov027_02113bf0. The literal at 0x02112230 is 0x02113bf0, it
 *     carries NO relocation row (a module-internal reference), and ov018's
 *     own bss has data_ov018_02113bf0 there -- the Animation SharedFilePtr
 *     __sinit_ov018_02112c80 constructs with file id 0x406 and that
 *     func_ov018_021122ec already reads as data_ov018_02113bf0[1]. The port
 *     defines data_ov027_02113bf0 too, in ov027's own mount, so the link
 *     would have SUCCEEDED against the wrong storage. The CMake gate block
 *     renames it; the same #1308-#1310 family main fixed for InitResources
 *     and for 021122ec, one file it missed.
 *
 * Cell 0's target func_ov018_021122ec ("AfterClsn" per its own recovered-
 * name comment, itself carrying the SAME dsd-era class-identity mislabel as
 * the eight _ZN7SkiLift* files -- it is MotherPenguin's own body) is carried
 * byte-identical from main HEAD, and its own data_ov027/ov030 spellings were
 * fixed there to data_ov018_02113be8/02113bf0 -- the exact cells this gate's
 * mount already provides.
 */
#include <cstdio>
#include <cstdlib>

/* ---- defect 4, the C++-LINKAGE SPELLING of two mounted records -----------
   src/func_ov018_02111f1c.cpp declares its two file records OUTSIDE its own
   `extern "C"` block:
       extern void *data_ov018_02113c08[];
       extern void *data_ov018_02113bf8[];
   so MSVC emits them mangled (?data_ov018_02113c08@@3PAPAXA) while
   port/ov018_syms.txt publishes them at C linkage. dumpbin /symbols on the
   TU before the link lists exactly those two as WOULD NOT RESOLVE. Both are
   routed with the standard directive, the same
   ?data_ovNNN_ADDR@@3PAPAXA=_data_ovNNN_ADDR shape twelve other actor
   classes already use (actor_classes_ov032.cpp:214,
   actor_classes_ov064.cpp:175, ...). The LHS has no definition anywhere, so
   the alias FIRES and alternatename_guard sees the healthy same-address
   shape. The matched src is not touched. */
#pragma comment(linker, "/alternatename:?data_ov018_02113c08@@3PAPAXA=_data_ov018_02113c08")
#pragma comment(linker, "/alternatename:?data_ov018_02113bf8@@3PAPAXA=_data_ov018_02113bf8")

extern "C" {
struct PortCodePair { unsigned fn; int delta; };
extern PortCodePair data_ov018_02113c4c[6];   /* three entries, two records */

/* the six ROM bodies, all matched src, all on port/slice_mpg.txt */
int func_ov018_021122ec(char *self);          /* entry 0 enter -- AfterClsn */
int func_ov018_02112234(char *self);          /* entry 0 tick               */
int func_ov018_021121dc(char *self);          /* entry 1 enter              */
int func_ov018_02111fac(char *self);          /* entry 1 tick  -- the talk  */
int func_ov018_02111f1c(char *self);          /* entry 2 enter              */
int func_ov018_02111e28(char *self);          /* entry 2 tick  -- the chase */

void func_ov018_021123d0(char *self, int i);  /* the state setter           */
}

enum { PORT_MPG_RECORDS = 6 };

typedef int (*PortMpgFn)(char *);

/* ---- defect 2's shim: the ROM's own r1 = 0, supplied by name -------------
   port/CMakeLists.txt's gate block renames src/func_ov018_02111e28.cpp's ONE
   call site to this. Nothing else may use it: every other caller of
   func_ov018_021123d0 in src/ spells both arguments. */
extern "C" void port_mpg_set_state_zero(char *self)
{
    func_ov018_021123d0(self, 0);
}

/* ---- the tick witness ----------------------------------------------------
   One counter per record, in the ROM's own field order (see the table
   above). SM64DS_MPG_LOG names a file to append one line per process to. */
static unsigned g_mpg_hits[PORT_MPG_RECORDS];

static int __cdecl mpg_r0_enter(char *self)
{ ++g_mpg_hits[0]; return func_ov018_021122ec(self); }
static int __cdecl mpg_r0_tick(char *self)
{ ++g_mpg_hits[1]; return func_ov018_02112234(self); }
static int __cdecl mpg_r1_enter(char *self)
{ ++g_mpg_hits[2]; return func_ov018_021121dc(self); }
static int __cdecl mpg_r1_tick(char *self)
{ ++g_mpg_hits[3]; return func_ov018_02111fac(self); }
static int __cdecl mpg_r2_enter(char *self)
{ ++g_mpg_hits[4]; return func_ov018_02111f1c(self); }
static int __cdecl mpg_r2_tick(char *self)
{ ++g_mpg_hits[5]; return func_ov018_02111e28(self); }

/* {ROM code address the sinit's own source record carries, host entry point}
   -- verified against the ROM's own record before the rewrite, the Cap seat
   shape: a mount pointing at the wrong bytes aborts instead of calling into
   the overlay image.
   __sinit_ov018_02112c80's copy is NOT in source-address order (.a=0211394c,
   .b=02113974, .c=02113954, .d=02113964, .e=0211395c, .f=0211396c), so each
   row here is that source record's own reloc target in the sinit's own field
   order, which is the DESTINATION order. */
static const struct { unsigned rom; PortMpgFn host; const char *what; }
g_mpg_cells[PORT_MPG_RECORDS] = {
    {0x021122ec, mpg_r0_enter, "e0"},   /* +0x00 (.a) */
    {0x02112234, mpg_r0_tick,  "t0"},   /* +0x08 (.b) */
    {0x021121dc, mpg_r1_enter, "e1"},   /* +0x10 (.c) */
    {0x02111fac, mpg_r1_tick,  "t1"},   /* +0x18 (.d) */
    {0x02111f1c, mpg_r2_enter, "e2"},   /* +0x20 (.e) */
    {0x02111e28, mpg_r2_tick,  "t2"},   /* +0x28 (.f) */
};

static void port_mpg_report(void)
{
    const char *path = std::getenv("SM64DS_MPG_LOG");
    if (!path || !path[0])
        return;
    const char *lvl = std::getenv("SM64DS_LEVEL");
    const char *scn = std::getenv("SM64DS_SCENE");
    std::FILE *f = std::fopen(path, "a");
    if (!f)
        return;
    std::fprintf(f, "mpg level=%s scene=%s e0=%u t0=%u e1=%u t1=%u e2=%u "
                 "t2=%u total=%u\n",
                 lvl && lvl[0] ? lvl : "-", scn && scn[0] ? scn : "-",
                 g_mpg_hits[0], g_mpg_hits[1], g_mpg_hits[2], g_mpg_hits[3],
                 g_mpg_hits[4], g_mpg_hits[5],
                 g_mpg_hits[0] + g_mpg_hits[1] + g_mpg_hits[2] +
                 g_mpg_hits[3] + g_mpg_hits[4] + g_mpg_hits[5]);
    std::fclose(f);
}

extern "C" void port_mother_penguin_afterclsn_seat(void)
{
    static int done;
    if (done)
        return;
    done = 1;
    for (int i = 0; i < PORT_MPG_RECORDS; ++i) {
        if (data_ov018_02113c4c[i].fn != g_mpg_cells[i].rom ||
            data_ov018_02113c4c[i].delta != 0) {
            std::fprintf(stderr, "FATAL: MotherPenguin state-table record "
                         "%d (%s): the sinit left %08x/%d, the ROM's own "
                         "record says %08x/0 -- WRONG BYTES\n", i,
                         g_mpg_cells[i].what, data_ov018_02113c4c[i].fn,
                         data_ov018_02113c4c[i].delta, g_mpg_cells[i].rom);
            std::abort();
        }
        data_ov018_02113c4c[i].fn = (unsigned)(size_t)g_mpg_cells[i].host;
    }
    std::atexit(port_mpg_report);
}
