/* HOST COPY of src/_ZN10BulletBill8BehaviorEv.cpp -- the mwcc pointer-to-member
 * dispatch read as a plain function pointer, and the two-entry state table
 * seated with host addresses. Whomp_Behavior's twin one class over, same
 * reasons:
 *
 *   * MSVC forms a pointer-to-member of an INCOMPLETE class as the four-word
 *     general representation. The matched source writes `struct Klass;` and
 *     `typedef void (Klass::*PMF)()`, then indexes `data_ov079_021282e0[which]`
 *     (an mwcc {function, delta} pair) and calls `(this->*(m->pmf))()`. Read
 *     here as the pair's first word, called as a plain function pointer.
 *   * __sinit_ov079_021279d4 copies two {function, delta} statics from ov079
 *     0x02127ea4 (entry 0) and 0x02127e9c (entry 1) into the bss table
 *     data_ov079_021282e0. Those statics are the overlay image's own words --
 *     DS CODE ADDRESSES -- so the seat rewrites the two STATICS before the
 *     sinit runs, each checked against the ROM address its host body was
 *     compiled from. Both deltas are 0.
 *
 * BOTH STATES ARE MATCHED SRC (func_ov079_0212682c and func_ov079_02126794).
 *
 * The body is the matched source line for line; only the dispatch is spelled
 * as a plain call through the pair's first word.
 */
#include <cstdio>
#include <cstdlib>

typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

struct PortVec3 { int x, y, z; };

extern "C" {
void func_0200f760(void *self, void *cc);
void _ZN5Actor9UpdatePosEP12CylinderClsn(void *self, void *cc);
void *_ZN5Actor10FindWithIDEj(u32 id);
void _ZN6Player16IncMegaKillCountEv();
void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(u32, int, int, int);
void _ZN9ActorBase18MarkForDestructionEv(void *self);
void func_02012694(int a, void *b);
void _ZN5Sound9PlayBank0EjRK7Vector3(u32 id, const PortVec3 *pos);
void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void *self, const PortVec3 *pos,
    u32 a, int b, u32 c, u32 d, u32 e);
void func_ov079_02126704(char *c);
void _ZN12CylinderClsn5ClearEv(void *self);
void _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(
    void *self, const PortVec3 *pos);
void _ZN12CylinderClsn6UpdateEv(void *self);

struct PortPmf { unsigned fn; int delta; };
extern PortPmf data_ov079_021282e0[];       /* the bss table, dispatched each frame */
extern PortPmf data_ov079_02127e9c[];       /* entry 1 source static */
extern PortPmf data_ov079_02127ea4[];       /* entry 0 source static */

void func_ov079_0212682c(void *);           /* state 0 */
void func_ov079_02126794(void *);           /* state 1 */
}

/* ---- data_ov079_021282e0 IS DISPATCHED BY THE MATCHED TU NOW -------------
 *
 * Run link100 lane PMFB5. src/_ZN10BulletBill8BehaviorEv.cpp compiles from src
 * and this file no longer defines the Behavior, so the two cells are reached by
 * `mov ecx, tab[i*8+4] / mov eax, tab[i*8] / add ecx, esi / call eax` with
 * nothing pushed and nothing cleaned, and the two plain cdecl bodies become
 * zero-argument __fastcall faces.
 *
 * THE STRIDE, ROM SIDE (runs/link100/out/PMFB5/rom_gate3.txt):
 * `add r3, r1, r0, lsl #3` at 02126bfc on the pool word 02126c40 = 021282e0.
 * EMITTED SIDE: [ebx*8] and [ebx*8+4]. ROM 8 == emitted 8; /Zp4 a measured
 * no-op.
 *
 * THE TWO SOURCE PAIRS both read {code, 0} in the overlay image at the
 * addresses src/__sinit_ov079_021279d4.c copies them from. The spelling is one
 * lane PMFB4's reader did not know: `data_ov079_021282e0.a = SRC;` where
 * `struct S4 { struct S2 a, b; }` and `struct S2 { int w[2]; }`, so the member
 * IS a whole eight-byte pair and its ordinal is the slot. Reading `.a =` as a
 * field-form fill -- the name looks exactly like one -- would have refused this
 * row for a reason that is not true. THE TABLE IS FULLY COVERED: the next
 * symbol is 16 bytes on, which is the two cells.
 *
 * NO /alternatename: the matched TU declares the table inside extern "C" and it
 * comes in as the plain _data_ov079_021282e0, which the object's UNDEF sweep
 * confirms (no decorated external at all). */

#define BB_FACE(slot, sym)                                                \
    static void __fastcall bb_s##slot(void *self, void *dead_edx)         \
    {                                                                     \
        (void)dead_edx;                                                   \
        sym(self);                                                        \
    }

BB_FACE(0, func_ov079_0212682c)
BB_FACE(1, func_ov079_02126794)

static const struct { PortPmf *slot; unsigned rom; void *host; }
g_bullet_bill_states[] = {
    {data_ov079_02127ea4, 0x0212682c, (void *)bb_s0},
    {data_ov079_02127e9c, 0x02126794, (void *)bb_s1},
};

extern "C" void port_bullet_bill_states_seat(void)
{
    static int done;
    if (done)
        return;
    done = 1;
    for (unsigned i = 0;
         i < sizeof g_bullet_bill_states / sizeof g_bullet_bill_states[0]; ++i) {
        PortPmf *p = g_bullet_bill_states[i].slot;
        if (p->fn != g_bullet_bill_states[i].rom || p->delta != 0) {
            std::fprintf(stderr, "FATAL: BulletBill state %u: the mount holds "
                         "%08x/%d, the ROM's own table says %08x/0 -- WRONG "
                         "BYTES\n", i, p->fn, p->delta,
                         g_bullet_bill_states[i].rom);
            std::abort();
        }
        p->fn = (unsigned)(size_t)g_bullet_bill_states[i].host;
    }
}

/* THE HOST COPY IS GONE. src/_ZN10BulletBill8BehaviorEv.cpp is on
   port/slice_pmfb5.txt and dispatches the seated table itself. */
