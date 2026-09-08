/* HOST COPY of src/_ZN5Enemy11UpdateDeathER12WithMeshClsn.cpp -- the mwcc
 * pointer-to-member dispatch, for the eighth time in this port and the first
 * one that belongs to a BASE class rather than to a single actor.
 *
 * The matched source is
 *
 *     typedef int (Enemy::*PMF)(WithMeshClsn&);
 *     extern PMF data_ov002_0210dbc0[];
 *     ...
 *     ret = (thiz->*data_ov002_0210dbc0[deathType - 1])(*clsn);
 *
 * and the typedef is formed while `Enemy` is still INCOMPLETE (its shape is
 * declared four lines further down). MSVC answers an incomplete class with the
 * four-word GENERAL representation, so the array it indexes gets a 16-byte
 * stride where the ROM's is 8, and entry 3 of an eight-entry table reads out
 * of the middle of entry 1. mwccarm's is {function, this-delta}, two words.
 *
 * So the table is read here as what it is: eight {fn, delta} pairs, the ROM's
 * own bytes, with the same virtual-bit test the Butterfly's matched source
 * spells out for itself. Every delta in the ROM's copy is 0 and no entry has
 * the virtual bit; the seat below asserts both rather than assuming them.
 *
 * ---- and the eight statics have to be seated ------------------------------
 *
 * __sinit_ov002_02100938 (matched, in slice_gate10) copies eight {fn, delta}
 * statics out of ov002 .data into data_ov002_0210dbc0. Those statics are the
 * ROM's own words -- DS code addresses -- so the host bodies go over the
 * SOURCE side before the sinit copies them, and the sinit propagates host
 * addresses wherever the ROM propagates DS ones. This is the Rabbit's
 * treatment (hal/actor_overlays.cpp) applied to ov002.
 *
 * ---- and the SECOND table -------------------------------------------------
 *
 * func_ov002_020aea30 is the same shape one table along: it dispatches
 * data_ov002_0210db80[deathType - 1] with two int arguments, and that is the
 * ENTER-DEATH half where 0210dbc0 is the per-frame half. Same sinit, same
 * incomplete-class typedef, same treatment. Its eight bodies are matched too.
 *
 * DEATH TYPE, one line each, in the order the table stores them:
 *   1  020ae64c  shrink while the timer runs, then the shared tail
 *   2  020ae608  wait for the ground, spawn the coin, kill and track
 *   3  020ae608  the same body (four of the eight entries share it)
 *   4  020ae4cc  the particle burst, then the coin
 *   5  020ae608
 *   6  020ae608
 *   7  020ae454  the sink-and-fade
 *   8  020aea24  the plain kill
 */
#include <cstdio>
#include <cstdlib>

extern "C" {

void DecIfAbove0_Short(short *p);
int _ZN5Actor9UpdatePosEP12CylinderClsn(void *self, void *clsn);
int _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(void *self, void *clsn,
                                             unsigned sel);

/* the eight ROM statics the sinit copies, and the bss table it copies into */
struct PortEnemyPmf { unsigned fn; int delta; };
extern PortEnemyPmf data_ov002_021081b0[], data_ov002_021081a8[],
    data_ov002_02108148[], data_ov002_02108198[], data_ov002_02108190[],
    data_ov002_02108188[], data_ov002_02108180[], data_ov002_02108178[];
extern PortEnemyPmf data_ov002_0210dbc0[];

/* ...and the enter-death table's eight statics and its own bss copy */
extern PortEnemyPmf data_ov002_021081b8[], data_ov002_02108150[],
    data_ov002_02108168[], data_ov002_02108160[], data_ov002_02108158[],
    data_ov002_02108170[], data_ov002_02108140[], data_ov002_021081a0[];
extern PortEnemyPmf data_ov002_0210db80[];

void func_ov002_020ae9f8(void *self, int a, int b);
void func_ov002_020ae954(void *self, int a, int b);
void func_ov002_020ae890(void *self, int a, int b);
void func_ov002_020ae87c(void *self, int a, int b);
void func_ov002_020ae844(void *self, int a, int b);
void func_ov002_020ae80c(void *self, int a, int b);
void func_ov002_020ae73c(void *self, int a, int b);
void func_ov002_020aea2c(void *self, int a, int b);

int func_ov002_020ae64c(void *self, void *clsn);
int func_ov002_020ae608(void *self, void *clsn);
int func_ov002_020ae4cc(void *self, void *clsn);
int func_ov002_020ae454(void *self, void *clsn);
int func_ov002_020aea24(void *self, void *clsn);

}  /* extern "C" */

typedef int (*PortEnemyDeathFn)(void *, void *);

/* ---- THE FACES (run link100, lane PMFB2) --------------------------------
   Both dispatchers over these two tables are retired onto their matched TUs,
   and both dispatch a real pointer-to-member, which MSVC emits as
   `call <reg>` with the receiver in ECX and the arguments PUSHED:

     src/_ZN5Enemy11UpdateDeathER12WithMeshClsn.cpp
         push DWORD PTR _clsn$[ebp]
         mov  ecx, <table 0210dbc0>[eax*8-4]     the adjust word
         mov  eax, <table 0210dbc0>[eax*8-8]     the code word (index-1 folded
                                                  into the displacement)
         add  ecx, ebx  /  call eax
     src/func_ov002_020aea30.c   (a //cpp-marked .c, compiled as C++)
         push DWORD PTR _b$[ebp]  /  push DWORD PTR _a$[ebp]
         mov  ecx, <table 0210db80>[edx*8-4]
         mov  eax, <table 0210db80>[edx*8-8]
         add  ecx, esi  /  call eax

   THE ARITY IS NOT THE ZERO-ARGUMENT SHAPE and is taken from each row's OWN
   listing, which is the standing trap: a wrong-arity raw cast smashes the
   stack on first exercise. One face takes the WithMeshClsn& the death states
   read, the other takes the two ints the enter states read; __fastcall lands
   the receiver in ecx, ignores edx and leaves those arguments exactly where a
   cdecl body reads them.

   BOTH THE ROM STATICS AND THE BSS TABLES take the face, because the seat
   below writes both -- leaving the statics on the raw body would let a second
   run of the sinit quietly reinstate the wrong convention.

   WHAT THE RETIRED COPIES CHECKED THAT NOTHING CHECKS NOW: each tested
   `m->fn & 1` and aborted, calling it a virtual member pointer. That test
   reads the CODE word; the mwcc virtual flag lives in bit 0 of the ADJUST
   word, and all sixteen source pairs read {code, 0} in
   extracted/overlays/overlay_0002.bin, so the flag is never set and the test
   could never have fired. It is not being replaced because it was inert. */
static int __fastcall pmf_face_func_ov002_020ae64c(void *self, void *dead_edx, void *clsn)
{ return func_ov002_020ae64c(self, clsn); }
static int __fastcall pmf_face_func_ov002_020ae608(void *self, void *dead_edx, void *clsn)
{ return func_ov002_020ae608(self, clsn); }
static int __fastcall pmf_face_func_ov002_020ae4cc(void *self, void *dead_edx, void *clsn)
{ return func_ov002_020ae4cc(self, clsn); }
static int __fastcall pmf_face_func_ov002_020ae454(void *self, void *dead_edx, void *clsn)
{ return func_ov002_020ae454(self, clsn); }
static int __fastcall pmf_face_func_ov002_020aea24(void *self, void *dead_edx, void *clsn)
{ return func_ov002_020aea24(self, clsn); }

#define ENTER_FACE(sym)                                                       \
    static void __fastcall pmf_face_##sym(void *self, void *dead_edx,         \
                                          int a, int b)                       \
    { sym(self, a, b); }
ENTER_FACE(func_ov002_020ae9f8)
ENTER_FACE(func_ov002_020ae954)
ENTER_FACE(func_ov002_020ae890)
ENTER_FACE(func_ov002_020ae87c)
ENTER_FACE(func_ov002_020ae844)
ENTER_FACE(func_ov002_020ae80c)
ENTER_FACE(func_ov002_020ae73c)
ENTER_FACE(func_ov002_020aea2c)
#undef ENTER_FACE

/* THE TWO ALIASES. Each matched TU declares its table outside its extern "C"
   block, so each names it at C++ linkage while the ov002 mount defines the
   plain C name. The death table's directive already exists in
   hal/bob_enemy_bridges.cpp:66 and is not repeated here; the enter table's is
   this lane's and lives with the seat that owns it. alternatename_guard's rule
   holds: the LHS is never DEFINED anywhere, only referenced. */
#pragma comment(linker, "/alternatename:?data_ov002_0210db80@@3PAP8C@@AEXHH@ZA=_data_ov002_0210db80")

#define DF(sym) (PortEnemyDeathFn)pmf_face_##sym
static const struct { PortEnemyPmf *slot; unsigned rom; PortEnemyDeathFn host; }
g_enemy_death[] = {
    {data_ov002_021081b0, 0x020ae64c, DF(func_ov002_020ae64c)},
    {data_ov002_021081a8, 0x020ae608, DF(func_ov002_020ae608)},
    {data_ov002_02108148, 0x020ae608, DF(func_ov002_020ae608)},
    {data_ov002_02108198, 0x020ae4cc, DF(func_ov002_020ae4cc)},
    {data_ov002_02108190, 0x020ae608, DF(func_ov002_020ae608)},
    {data_ov002_02108188, 0x020ae608, DF(func_ov002_020ae608)},
    {data_ov002_02108180, 0x020ae454, DF(func_ov002_020ae454)},
    {data_ov002_02108178, 0x020aea24, DF(func_ov002_020aea24)},
};
#undef DF

/* Both sides are seated, and that is deliberate. The sinit runs from the
   window's own boot and the registry runs later, so by the time the first
   Enemy-family class asks for this the COPY has already happened -- seating
   only the statics would leave the bss table full of DS addresses. Seating
   only the bss table would break if the sinit were ever run again. Each slot
   is checked against the ROM address it is supposed to hold and left alone if
   it is already the host body, so the order stops mattering. */
static void port_enemy_death_seat_one(PortEnemyPmf *p, unsigned rom,
                                      PortEnemyDeathFn host, const char *where,
                                      unsigned i)
{
    if (p->fn == (unsigned)(size_t)host)
        return;
    if (p->fn != rom || p->delta != 0) {
        std::fprintf(stderr, "FATAL: Enemy death state %u (%s): the mount holds "
                     "%08x/%d, the ROM's own table says %08x/0 -- WRONG BYTES\n",
                     i, where, p->fn, p->delta, rom);
        std::abort();
    }
    p->fn = (unsigned)(size_t)host;
}

typedef void (*PortEnemyEnterFn)(void *, int, int);

#define EF(sym) (PortEnemyEnterFn)pmf_face_##sym
static const struct { PortEnemyPmf *slot; unsigned rom; PortEnemyEnterFn host; }
g_enemy_enter[] = {
    {data_ov002_021081b8, 0x020ae9f8, EF(func_ov002_020ae9f8)},
    {data_ov002_02108150, 0x020ae954, EF(func_ov002_020ae954)},
    {data_ov002_02108168, 0x020ae890, EF(func_ov002_020ae890)},
    {data_ov002_02108160, 0x020ae87c, EF(func_ov002_020ae87c)},
    {data_ov002_02108158, 0x020ae844, EF(func_ov002_020ae844)},
    {data_ov002_02108170, 0x020ae80c, EF(func_ov002_020ae80c)},
    {data_ov002_02108140, 0x020ae73c, EF(func_ov002_020ae73c)},
    {data_ov002_021081a0, 0x020aea2c, EF(func_ov002_020aea2c)},
};
#undef EF

extern "C" void port_enemy_death_states_seat(void)
{
    static int done;
    if (done)
        return;
    done = 1;
    for (unsigned i = 0; i < sizeof g_enemy_death / sizeof g_enemy_death[0];
         ++i) {
        port_enemy_death_seat_one(g_enemy_death[i].slot, g_enemy_death[i].rom,
                                  g_enemy_death[i].host, "static", i);
        port_enemy_death_seat_one(&data_ov002_0210dbc0[i], g_enemy_death[i].rom,
                                  g_enemy_death[i].host, "table", i);
        port_enemy_death_seat_one(g_enemy_enter[i].slot, g_enemy_enter[i].rom,
                                  (PortEnemyDeathFn)g_enemy_enter[i].host,
                                  "enter static", i);
        port_enemy_death_seat_one(&data_ov002_0210db80[i], g_enemy_enter[i].rom,
                                  (PortEnemyDeathFn)g_enemy_enter[i].host,
                                  "enter table", i);
    }
}

/* BOTH HOST COPIES RETIRED (run link100, lane PMFB2).
   func_ov002_020aea30 and _ZN5Enemy11UpdateDeathER12WithMeshClsn are on
   port/slice_pmfb2.txt and compile from src/func_ov002_020aea30.c (a
   //cpp-marked .c, given LANGUAGE CXX in CMake block R10a) and
   src/_ZN5Enemy11UpdateDeathER12WithMeshClsn.cpp. Both define the Itanium C
   name themselves, so neither needs a face for its own symbol.

   MEASURED FOR THAT LANE, both ways round:
     RECORD STRIDE. The ROM strides both tables by eight --
     `add r3, r1, r0, lsl #3` at 0x020ae6e0 and `add lr, r3, r0, lsl #3` at
     0x020aea70 -- and both matched TUs emit [reg*8] off the raw index, with
     the index-1 folded into the displacement (-8 and -4). Neither record is a
     struct containing a pointer-to-member, so /Zp4 does not move either and
     neither is on the option list.
     ADJUST WORDS. All sixteen source pairs -- 0x02108140..0x021081b8, the
     eight the seat above names per table -- read {code, 0} in
     extracted/overlays/overlay_0002.bin, so `this` is never adjusted and the
     ROM takes its non-virtual arm.
     CALL SHAPE AND ARITY. `call <reg>` with the receiver in ecx and the
     arguments PUSHED: one WithMeshClsn& for the death half, two ints for the
     enter half. The faces above carry exactly those arities, each from its own
     listing.
   The seat is unchanged in what it CHECKS; only the address it writes is now
   the face's rather than the body's. */
