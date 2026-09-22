/* HOST COPIES of src/game/actors/d_a_wanwan.cpp and src/game/actors/d_a_wanwan.cpp
 * -- the CHAIN_CHOMP's state machine, and the mwcc pointer-to-member dispatch
 * for the tenth time in this port.
 *
 * The matched pair is
 *
 *     struct C; typedef void (C::*PMF)();
 *     struct Entry { PMF pmf; char rest[12]; };      // 02111ebc, the change
 *     struct Entry { char pad[8]; PMF pmf; ... };    // 02111f08, the run
 *     extern Entry data_ov014_0211476c[];
 *
 * and `C` is INCOMPLETE where both typedefs are formed, so MSVC widens the PMF
 * to its four-word general representation and the two Entry layouts stop
 * describing the ROM's 20-byte record. __sinit_ov014_02113118 is what the
 * record really is: {init PMF, main PMF, one spare word}, six states.
 *
 * The twelve statics are seated on both sides of the sinit, the treatment
 * port/unmatched/Enemy_UpdateDeath.cpp settled on.
 *
 * STATES, in the order the sinit stores them:
 *   0  {02111e74, 02111e14}   chained and idle, watching for the player
 *   1  {02111dc4, 02111ca8}   the lunge
 *   2  {02111b70, 02111af0}   the recoil back onto the chain
 *   3  {02111a6c, 021115ec}   loose, after the post is broken
 *   4  {021115c0, 0211150c}   the jump out of the pit
 *   5  {021114d8, 02111484}   settled
 *
 * All twelve halves are matched src, so this file carries no trap.
 */
#include <cstdio>
#include <cstdlib>

extern "C" {

struct PortChompPmf { unsigned fn; int delta; };
/* the ROM's record: two PMFs and one spare word, 20 bytes */
struct PortChompState { PortChompPmf init, main; int spare; };

extern PortChompPmf data_ov014_021146e0[], data_ov014_021146b8[],
    data_ov014_021146f0[], data_ov014_021146c0[], data_ov014_021146a8[],
    data_ov014_021146a0[], data_ov014_021146f8[], data_ov014_021146b0[],
    data_ov014_021146d8[], data_ov014_021146c8[], data_ov014_021146e8[],
    data_ov014_021146d0[];
extern PortChompState data_ov014_0211476c[];

void func_ov014_02111e74(void *self); void func_ov014_02111e14(void *self);
void func_ov014_02111dc4(void *self); void func_ov014_02111ca8(void *self);
void func_ov014_02111b70(void *self); void func_ov014_02111af0(void *self);
void func_ov014_02111a6c(void *self); void func_ov014_021115ec(void *self);
void func_ov014_021115c0(void *self); void func_ov014_0211150c(void *self);
void func_ov014_021114d8(void *self); void func_ov014_02111484(void *self);

}  /* extern "C" */

typedef void (__fastcall *PortChompFn)(void *);

/* ---- THE RECEIVER ARRIVES IN ECX. Twelve raw cdecl bodies sat in these
   records and read their receiver from [esp+4]. That is right only while the
   PMF call is a tail jump that leaves its caller's own stack argument in
   place, and daWanwan_c::Behavior is not one. Read off this build's image:

     ?Behavior@daWanwan_c@@UAEHXZ +0x84
       mov  eax, dword ptr [esi+0x610]            ; the state index
       push ebx                                   ; a REGISTER SAVE, not an arg
       lea  eax, [eax+eax*4]                      ; the 20-byte stride
       mov  ecx, dword ptr [_data_ov014_0211476c+8+eax*4]  ; the delta
       mov  eax, dword ptr [_data_ov014_0211476c+4+eax*4]  ; the code word
       add  ecx, esi                              ; this + delta
       call eax                                   ; A REAL CALL

   Nothing is pushed for the callee and nothing is taken back after it, so the
   raw body's [ebp+8] is the SAVED EBX one word above the return address.
   Measured on level 6 (Bob-omb Battlefield) under cdb:

     walk_window+0x2d0e45  cmp byte ptr [eax+31Eh],0   ds:0000031e=??
     ebx=esi=edi=00a62a78, and the walker's actor is 3000fbc0
     001af094 006d0bdd  walk_window+0x2d0e45   func_ov014_02111f54+0x15
     001af0a4 006cfbd0  walk_window+0x2d0bdd   func_ov014_02111ca8+0xd
     001af140 005f2893  walk_window+0x2cfbd0   daWanwan_c::Behavior+0xa0

   func_ov014_02111ca8 took 0x00a62a78 (the saved EBX) as its receiver, handed
   it to func_ov014_02111f54, and dActor_c::FindWithID(*(u32 *)(self+0x608))
   read a junk id and returned null. The ROM's own body then dereferences the
   result with no null check, which is what 0x31e is: the matched source's
   `a+0x31e` on a null `a`, not a class four bytes high.

   This is 5ae983797's correction one overlay over, and its reasoning applies
   unchanged: MSVC's PMF sequence puts `this + delta` in ECX before it
   transfers control whichever way it transfers, so a __fastcall thunk is right
   on the tail-jump path too and a raw body is right only by luck. One register
   argument, so each thunk returns with a bare `ret`, which is what a caller
   that pushed nothing wants. The flat C names stay raw cdecl bodies for the
   direct calls src makes between them; only the twelve RECORD words move. */
static void __fastcall cc_st_1e74(void *s) { func_ov014_02111e74(s); }
static void __fastcall cc_st_1e14(void *s) { func_ov014_02111e14(s); }
static void __fastcall cc_st_1dc4(void *s) { func_ov014_02111dc4(s); }
static void __fastcall cc_st_1ca8(void *s) { func_ov014_02111ca8(s); }
static void __fastcall cc_st_1b70(void *s) { func_ov014_02111b70(s); }
static void __fastcall cc_st_1af0(void *s) { func_ov014_02111af0(s); }
static void __fastcall cc_st_1a6c(void *s) { func_ov014_02111a6c(s); }
static void __fastcall cc_st_15ec(void *s) { func_ov014_021115ec(s); }
static void __fastcall cc_st_15c0(void *s) { func_ov014_021115c0(s); }
static void __fastcall cc_st_150c(void *s) { func_ov014_0211150c(s); }
static void __fastcall cc_st_14d8(void *s) { func_ov014_021114d8(s); }
static void __fastcall cc_st_1484(void *s) { func_ov014_02111484(s); }

static const struct { PortChompPmf *slot; unsigned rom; PortChompFn host; }
g_chomp_states[] = {
    {data_ov014_021146e0, 0x02111e74, cc_st_1e74},
    {data_ov014_021146b8, 0x02111e14, cc_st_1e14},
    {data_ov014_021146f0, 0x02111dc4, cc_st_1dc4},
    {data_ov014_021146c0, 0x02111ca8, cc_st_1ca8},
    {data_ov014_021146a8, 0x02111b70, cc_st_1b70},
    {data_ov014_021146a0, 0x02111af0, cc_st_1af0},
    {data_ov014_021146f8, 0x02111a6c, cc_st_1a6c},
    {data_ov014_021146b0, 0x021115ec, cc_st_15ec},
    {data_ov014_021146d8, 0x021115c0, cc_st_15c0},
    {data_ov014_021146c8, 0x0211150c, cc_st_150c},
    {data_ov014_021146e8, 0x021114d8, cc_st_14d8},
    {data_ov014_021146d0, 0x02111484, cc_st_1484},
};

static void port_chomp_seat_one(PortChompPmf *p, unsigned rom, PortChompFn host,
                                const char *where, unsigned i)
{
    if (p->fn == (unsigned)(size_t)host)
        return;
    if (p->fn != rom || p->delta != 0) {
        std::fprintf(stderr, "FATAL: ChainChomp state half %u (%s): the mount "
                     "holds %08x/%d, the ROM's own table says %08x/0 -- WRONG "
                     "BYTES\n", i, where, p->fn, p->delta, rom);
        std::abort();
    }
    p->fn = (unsigned)(size_t)host;
}

extern "C" void port_chain_chomp_states_seat(void)
{
    static int done;
    if (done)
        return;
    done = 1;
    for (unsigned i = 0; i < 12; ++i) {
        PortChompPmf *dst = (i & 1) ? &data_ov014_0211476c[i / 2].main
                                    : &data_ov014_0211476c[i / 2].init;
        port_chomp_seat_one(g_chomp_states[i].slot, g_chomp_states[i].rom,
                            g_chomp_states[i].host, "static", i);
        port_chomp_seat_one(dst, g_chomp_states[i].rom, g_chomp_states[i].host,
                            "table", i);
    }
}

/* func_ov014_02111ebc and func_ov014_02111f08 RETIRED (run link100, lane
   PMFB1), and the static port_chomp_call with them -- nothing else called it.
   src/game/actors/d_a_wanwan.cpp and src/game/actors/d_a_wanwan.cpp carry both on
   port/slice_pmfc.txt. The header above says MSVC "widens the PMF to its four-
   word general representation"; that is dead, because /vmg /vmm target-wide
   (block R8) makes the pointer-to-member the ROM's own eight-byte {function,
   delta} pair. What was still wrong was the twenty-byte RECORD: MSVC gives a
   struct containing a pointer-to-member eight-byte alignment and rounds twenty
   up to twenty-four, so the matched TUs strode 24 where the ROM strides 0x14.
   A per-TU /Zp4 (block R9d in port/CMakeLists.txt) makes it twenty.
   port_chain_chomp_states_seat above STAYS: it is the delta proof itself, it
   still aborts on a wrong function word or a nonzero adjustment over all
   twelve source pairs, and hal/actor_classes_bob_enemy.cpp still calls it. */
