/* HOST COPY of src/_ZN9PowerStar8BehaviorEv.cpp -- the mwcc pointer-to-member
 * dispatch read as a plain function pointer, and the class's fourteen-entry
 * {function, delta} state table seated with host addresses. This is the
 * POWER_STAR (178) state seat, the KnockDownPlank/Fish/Door treatment for the
 * same reason:
 *
 *   * The matched source forms `typedef void (C::*PMF)()` over a struct C it
 *     defines only as `struct C { char pad[0x800]; }` -- a COMPLETE single-
 *     inheritance class, so MSVC hands the pointer-to-member its SMALLEST
 *     representation, which for a delta-0 record is just the function pointer.
 *     But the general PMF call `(((C *)this)->*data_ov002_021109d8[idx])()` still
 *     lowers to MSVC's __thiscall-through-member-pointer sequence, and seating
 *     the bss table with a plain function address would only agree with that if
 *     MSVC's member-pointer layout matched mwcc's. It does not in general, so
 *     the dispatch is spelled here as a plain call through the pair's first word
 *     and MSVC's PMF representation never enters. This is not a src edit and not
 *     undecompiled code -- all fourteen targets are matched ov002 src.
 *
 *   * __sinit_ov002_02106e40 (compiled since gate 10) copies fourteen SOURCE
 *     statics data_ov002_0210a990..0210a9f8 into the runtime dispatch table
 *     data_ov002_021109d8[0..13], one {fn, delta} per entry. UNLIKE the overlay
 *     classes whose sinits run inside port_actor_overlays_sinits (after their
 *     seat), this ov002 sinit runs INLINE in walk_window/smoke_player BEFORE the
 *     level boot's registry install, so by the time this seat runs the copy has
 *     already happened. The seat therefore rewrites the RUNTIME table
 *     data_ov002_021109d8[0..13] directly, each entry checked against the ROM
 *     .fn the sinit copied into it before its host body is written over it. The
 *     dispatch-index -> ROM-address mapping is the sinit's own store order. All
 *     fourteen .delta halves are 0 (the complete-class form, confirmed against
 *     the overlay's relocs: no reloc at any +4 offset), so each pair is just the
 *     function pointer, and all fourteen targets are matched ov002 src
 *     (func_ov002_020e96a0..020ea9d0), so there is no hole and no trap.
 *
 * The Behavior body is the matched source line for line; only the dispatch is a
 * plain call through the seated pair's first word, and idx is read at the
 * observed offset 0x440 (the matched source's unk_440).
 */
#include <cstdio>
#include <cstdlib>

extern "C" {

struct PortPmf { unsigned fn; int delta; };

/* the runtime dispatch table PowerStar::Behavior reads every frame, which
   __sinit_ov002_02106e40 has already filled from the fourteen SOURCE statics by
   the time this seat runs */
extern PortPmf data_ov002_021109d8[];

/* the fourteen state bodies, all matched ov002 src */
void func_ov002_020e9af4(void *); void func_ov002_020ea06c(void *);
void func_ov002_020ea420(void *); void func_ov002_020ea9d0(void *);
void func_ov002_020e9804(void *); void func_ov002_020e9d18(void *);
void func_ov002_020e96a0(void *); void func_ov002_020e99e8(void *);
void func_ov002_020ea410(void *); void func_ov002_020e9840(void *);
void func_ov002_020ea100(void *); void func_ov002_020ea7ac(void *);
void func_ov002_020ea824(void *); void func_ov002_020ea90c(void *);

/* KING OF THE STAR dupe guards (port/unmatched/PowerStar_KingGuards.cpp). They
   seat in place of the src bodies for dispatch states 5 (collect confirm/revoke)
   and 13 (toss-landing +1); off king mode they run the ROM behaviour exactly. */
void port_king_power_star_state5(void *);
void port_king_power_star_state13(void *);

/* the matched-src calls PowerStar::Behavior makes around the dispatch */
int _ZN5Enemy14UpdateYoshiEatER12WithMeshClsn(char *c, char *clsn);
void func_ov002_020d718c(void *p);
void _ZN12CylinderClsn5ClearEv(char *c);
void _ZN5Actor9UpdatePosEP12CylinderClsn(char *c, void *clsn);
void _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(char *c, const void *v);
void _ZN12CylinderClsn6UpdateEv(char *c);
void func_ov002_020e700c(void *p);
void func_ov002_020e763c(void *p);
void func_ov002_020e84ec(void *p);
void func_ov002_020e7eb8(void *p);
extern int data_0209b454;
extern int data_ov002_0210aa0c[3];

}  /* extern "C" */

/* The runtime table data_ov002_021109d8[idx], in dispatch-index order 0..13.
   The ROM .fn column is __sinit_ov002_02106e40's own store order:
       [0]<-0210a9a8(020ea9d0) [1]<-0210a9f8(020ea90c) [2]<-0210a9f0(020ea824)
       [3]<-0210a9e8(020ea7ac) [4]<-0210a9a0(020ea420) [5]<-0210a9e0(020ea100)
       [6]<-0210a998(020ea06c) [7]<-0210a9b8(020e9d18) [8]<-0210a9c8(020e99e8)
       [9]<-0210a9d8(020e9840) [10]<-0210a9d0(020ea410) [11]<-0210a990(020e9af4)
       [12]<-0210a9b0(020e9804) [13]<-0210a9c0(020e96a0)
   The seat rewrites each entry to its host body after checking the pair the
   sinit copied is {ROM addr, 0} exactly. */
/* ---- THE FOURTEEN FACES, AND THE ALIAS (run link100, lane PMFB2) --------
   src/_ZN9PowerStar8BehaviorEv.cpp is a real pointer-to-member dispatch and
   MSVC emits it as `mov ecx, <table>[eax*8+4] / mov eax, <table>[eax*8] /
   add ecx, <this> / call eax`.
   -- `call <reg>` with the receiver in ECX and nothing pushed. The state
   bodies are plain cdecl and read the receiver off the stack, so the code word
   holds a __fastcall FACE and the face calls the body. TWO OF THE FOURTEEN ARE NOT src BODIES: states 5
   and 13 are the King-of-the-Star dupe guards, which are host functions of
   exactly the same cdecl shape, so they take exactly the same face.

   THE ALIAS. The matched TU declares its table outside extern "C" and comes in
   as ?data_ov002_021109d8@@3PAP8C@@AEXXZA; the mount defines the plain C name.
   alternatename_guard's rule holds: the LHS is never DEFINED, only referenced. */
#pragma comment(linker, "/alternatename:?data_ov002_021109d8@@3PAP8C@@AEXXZA=_data_ov002_021109d8")

#define STAR_FACE(sym)                                                        \
    static void __fastcall pmf_face_##sym(void *self, void *dead_edx)         \
    { sym(self); }
STAR_FACE(func_ov002_020ea9d0)
STAR_FACE(func_ov002_020ea90c)
STAR_FACE(func_ov002_020ea824)
STAR_FACE(func_ov002_020ea7ac)
STAR_FACE(func_ov002_020ea420)
STAR_FACE(port_king_power_star_state5)
STAR_FACE(func_ov002_020ea06c)
STAR_FACE(func_ov002_020e9d18)
STAR_FACE(func_ov002_020e99e8)
STAR_FACE(func_ov002_020e9840)
STAR_FACE(func_ov002_020ea410)
STAR_FACE(func_ov002_020e9af4)
STAR_FACE(func_ov002_020e9804)
STAR_FACE(port_king_power_star_state13)
#undef STAR_FACE

#define SF(sym) (void (*)(void *))pmf_face_##sym
static const struct { unsigned rom; void (*host)(void *); }
g_power_star_states[14] = {
    {0x020ea9d0, SF(func_ov002_020ea9d0)},   /* [0]  */
    {0x020ea90c, SF(func_ov002_020ea90c)},   /* [1]  */
    {0x020ea824, SF(func_ov002_020ea824)},   /* [2]  */
    {0x020ea7ac, SF(func_ov002_020ea7ac)},   /* [3]  */
    {0x020ea420, SF(func_ov002_020ea420)},   /* [4]  */
    {0x020ea100, SF(port_king_power_star_state5)},   /* [5]  king guard wraps src */
    {0x020ea06c, SF(func_ov002_020ea06c)},   /* [6]  */
    {0x020e9d18, SF(func_ov002_020e9d18)},   /* [7]  */
    {0x020e99e8, SF(func_ov002_020e99e8)},   /* [8]  */
    {0x020e9840, SF(func_ov002_020e9840)},   /* [9]  */
    {0x020ea410, SF(func_ov002_020ea410)},   /* [10] */
    {0x020e9af4, SF(func_ov002_020e9af4)},   /* [11] */
    {0x020e9804, SF(func_ov002_020e9804)},   /* [12] */
    {0x020e96a0, SF(port_king_power_star_state13)},   /* [13] king guard (host copy) */
};
#undef SF

extern "C" void port_power_star_states_seat(void)
{
    static int done;
    if (done)
        return;
    done = 1;
    for (unsigned i = 0; i < 14; ++i) {
        PortPmf *p = &data_ov002_021109d8[i];
        if (p->fn != g_power_star_states[i].rom || p->delta != 0) {
            std::fprintf(stderr, "FATAL: PowerStar state %u: the runtime table "
                         "holds %08x/%d, the ROM's own copy says %08x/0 -- WRONG "
                         "BYTES\n", i, p->fn, p->delta,
                         g_power_star_states[i].rom);
            std::abort();
        }
        p->fn = (unsigned)(size_t)g_power_star_states[i].host;
    }
}

/* _ZN9PowerStar8BehaviorEv RETIRED (run link100, lane PMFB2). It is on
   port/slice_pmfb2.txt and compiles from src/_ZN9PowerStar8BehaviorEv.cpp,
   which recovered as a real C++ method (?Behavior@PowerStar@@QAEHXZ); the
   Itanium C name its fill site calls is one cdecl line in
   hal/except_faces.cpp and no fill site changes.

   MEASURED FOR THAT LANE. The ROM strides the table by eight
   (`add r3, r1, r0, lsl #3` at 0x020eb164) and the matched TU emits [eax*8].
   All fourteen source pairs (0x0210a990..0x0210a9f8) read {code, 0} in
   extracted/overlays/overlay_0002.bin, so `this` is never adjusted and the ROM
   takes its ldreq arm. What was left was the calling convention, and the
   fourteen faces above are that repair -- INCLUDING the two king-guard states,
   which keep their guard and only gain a face. port_power_star_states_seat is
   unchanged in what it checks. */
