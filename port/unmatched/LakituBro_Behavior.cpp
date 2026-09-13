/* HOST COPY of src/_ZN9LakituBro8BehaviorEv.cpp -- the state dispatch read as
 * a plain function pointer, and the eleven states' tables seated with host
 * addresses.
 *
 * The gate-16 case for the third time (OneUpMushroom_Behavior.cpp,
 * SignPost_StateDispatch.cpp). LakituBro keeps a State* at +0x1ec and calls
 * the mwcc POINTER-TO-MEMBER-FUNCTION at its +8 every frame. MSVC forms a PMF
 * over an INCOMPLETE class as the four-word general representation, so the
 * source TU's `struct C;` + `typedef void (C::*PMF)();` makes `State` 0x18
 * bytes wide on the host instead of the ROM's 0x10 -- the dispatch reads its
 * neighbour and the two identity comparisons at the tail stop matching.
 *
 * WHAT IS SEATED, AND WHY IT IS THE SOURCE SIDE.
 *
 * __sinit_ov085_0212fa40 builds the eleven State objects at
 * ov085 0x02130790..0x02130830 by copying twenty-two {function, delta} pairs
 * out of the .data statics at 0x02130254..0x021302fc. Those statics are the
 * ROM's own words, which means DS CODE ADDRESSES -- the ovdata contract
 * ("callers patch code pointers at runtime").
 *
 * The seat rewrites the twenty-two STATICS, before the sinit runs, rather
 * than the eleven states after it. Same guarantee and one less mapping to get
 * wrong: each static is checked against the ROM address its host body was
 * compiled from and refuses to proceed on a disagreement, and the sinit then
 * copies host addresses wherever the ROM copies DS ones. Every one of the
 * twenty-two is the plain nonvirtual { function, 0 } form -- verified against
 * the overlay image, all deltas zero -- which is what makes the first word
 * readable as a function taking `this`.
 *
 * Nothing else changes. The body below is the matched source's control flow
 * line for line.
 */
#include <cstdio>
#include <cstdlib>

extern "C" {

unsigned short DecIfAbove0_Short(unsigned short *p);
void _ZN5Actor9UpdatePosEP12CylinderClsn(void *self, void *clsn);
void _ZN9Animation7AdvanceEv(void *anim);
void func_ov085_0212e778(void *self);
void func_ov085_0212e858(void *self);

/* the eleven states the sinit builds, and the two the Behavior compares
   against by identity */
extern unsigned char data_ov085_021307d0[];
extern unsigned char data_ov085_021307e0[];

/* the twenty-two {function, delta} statics, in ROM address order; storage is
   the per-symbol ov085 mount */
struct PortPmf { unsigned fn; int delta; };
extern PortPmf data_ov085_02130254[], data_ov085_0213025c[],
    data_ov085_02130264[], data_ov085_0213026c[], data_ov085_02130274[],
    data_ov085_0213027c[], data_ov085_02130284[], data_ov085_0213028c[],
    data_ov085_02130294[], data_ov085_0213029c[], data_ov085_021302a4[],
    data_ov085_021302ac[], data_ov085_021302b4[], data_ov085_021302bc[],
    data_ov085_021302c4[], data_ov085_021302cc[], data_ov085_021302d4[],
    data_ov085_021302dc[], data_ov085_021302e4[], data_ov085_021302ec[],
    data_ov085_021302f4[], data_ov085_021302fc[];

/* the twenty-two state bodies */
void func_ov085_0212d5dc(void *); void func_ov085_0212d724(void *);
void func_ov085_0212d73c(void *); void func_ov085_0212d8ec(void *);
void func_ov085_0212d9b8(void *); void func_ov085_0212db04(void *);
void func_ov085_0212dbdc(void *); void func_ov085_0212dcfc(void *);
void func_ov085_0212dd10(void *); void func_ov085_0212ddc4(void *);
void func_ov085_0212de5c(void *); void func_ov085_0212df84(void *);
void func_ov085_0212e078(void *); void func_ov085_0212e180(void *);
void func_ov085_0212e19c(void *); void func_ov085_0212e2ec(void *);
void func_ov085_0212e310(void *); void func_ov085_0212e480(void *);
void func_ov085_0212e4a4(void *); void func_ov085_0212e59c(void *);
void func_ov085_0212e5ac(void *); void func_ov085_0212e720(void *);

}  /* extern "C" */

/* ---- RUN link100 LANE PMFB7 GATE 2: THE PER-FRAME RECORDS ARE FACES -------
 * LakituBro's matched TU dispatches its state cell's +8 half as a real
 * pointer to member -- mov eax,[cell+8] / test eax,eax / je /
 * mov ecx,[cell+12] / add ecx,this / call eax, the ROM's own offsets, receiver
 * in ecx, NOTHING pushed, ARITY ZERO, /Zp4 diff 0 lines
 * (runs/link100/out/PMFB7/emit_all21_out.txt). The seat used to install plain
 * cdecl bodies that take their self off the stack, so each PER-FRAME record now
 * holds a zero-argument __fastcall face that forwards the receiver as the one
 * cdecl argument the ROM's own state body takes -- the same call the cell held
 * before, made through ecx instead of the stack.
 *
 * THE ENTER RECORDS DO NOT CHANGE: they are reached by the class's state-change
 * helper, which MSVC compiles as a one-call forwarder ending in `jmp`, so the
 * caller's own frame is reused and a plain cdecl body is right there.
 *
 * WHICH RECORD IS WHICH is read out of the class's own __sinit
 * (runs/link100/out/PMFB7/slots_gate2.txt), never assumed.
 */
static void __fastcall pmfb7_ov085_0212e4a4(void *self, void *dead_edx)
{
    (void)dead_edx;   /* record 0213026c, the per-frame half */
    ((void (*)(void *))(void *)func_ov085_0212e4a4)(self);
}
static void __fastcall pmfb7_ov085_0212d5dc(void *self, void *dead_edx)
{
    (void)dead_edx;   /* record 02130274, the per-frame half */
    ((void (*)(void *))(void *)func_ov085_0212d5dc)(self);
}
static void __fastcall pmfb7_ov085_0212d9b8(void *self, void *dead_edx)
{
    (void)dead_edx;   /* record 0213027c, the per-frame half */
    ((void (*)(void *))(void *)func_ov085_0212d9b8)(self);
}
static void __fastcall pmfb7_ov085_0212d73c(void *self, void *dead_edx)
{
    (void)dead_edx;   /* record 02130284, the per-frame half */
    ((void (*)(void *))(void *)func_ov085_0212d73c)(self);
}
static void __fastcall pmfb7_ov085_0212dd10(void *self, void *dead_edx)
{
    (void)dead_edx;   /* record 0213028c, the per-frame half */
    ((void (*)(void *))(void *)func_ov085_0212dd10)(self);
}
static void __fastcall pmfb7_ov085_0212e19c(void *self, void *dead_edx)
{
    (void)dead_edx;   /* record 02130294, the per-frame half */
    ((void (*)(void *))(void *)func_ov085_0212e19c)(self);
}
static void __fastcall pmfb7_ov085_0212dbdc(void *self, void *dead_edx)
{
    (void)dead_edx;   /* record 021302b4, the per-frame half */
    ((void (*)(void *))(void *)func_ov085_0212dbdc)(self);
}
static void __fastcall pmfb7_ov085_0212e5ac(void *self, void *dead_edx)
{
    (void)dead_edx;   /* record 021302d4, the per-frame half */
    ((void (*)(void *))(void *)func_ov085_0212e5ac)(self);
}
static void __fastcall pmfb7_ov085_0212e310(void *self, void *dead_edx)
{
    (void)dead_edx;   /* record 021302dc, the per-frame half */
    ((void (*)(void *))(void *)func_ov085_0212e310)(self);
}
static void __fastcall pmfb7_ov085_0212e078(void *self, void *dead_edx)
{
    (void)dead_edx;   /* record 021302f4, the per-frame half */
    ((void (*)(void *))(void *)func_ov085_0212e078)(self);
}
static void __fastcall pmfb7_ov085_0212de5c(void *self, void *dead_edx)
{
    (void)dead_edx;   /* record 021302fc, the per-frame half */
    ((void (*)(void *))(void *)func_ov085_0212de5c)(self);
}

static const struct {
    PortPmf *slot;
    unsigned rom;
    void (*host)(void *);
    const char *name;
} g_states[] = {
    {data_ov085_02130254, 0x0212e480, func_ov085_0212e480, "02130254"},
    {data_ov085_0213025c, 0x0212e720, func_ov085_0212e720, "0213025c"},
    {data_ov085_02130264, 0x0212e180, func_ov085_0212e180, "02130264"},
    {data_ov085_0213026c, 0x0212e4a4, (void (*)(void *))(void *)pmfb7_ov085_0212e4a4, "0213026c"},
    {data_ov085_02130274, 0x0212d5dc, (void (*)(void *))(void *)pmfb7_ov085_0212d5dc, "02130274"},
    {data_ov085_0213027c, 0x0212d9b8, (void (*)(void *))(void *)pmfb7_ov085_0212d9b8, "0213027c"},
    {data_ov085_02130284, 0x0212d73c, (void (*)(void *))(void *)pmfb7_ov085_0212d73c, "02130284"},
    {data_ov085_0213028c, 0x0212dd10, (void (*)(void *))(void *)pmfb7_ov085_0212dd10, "0213028c"},
    {data_ov085_02130294, 0x0212e19c, (void (*)(void *))(void *)pmfb7_ov085_0212e19c, "02130294"},
    {data_ov085_0213029c, 0x0212d724, func_ov085_0212d724, "0213029c"},
    {data_ov085_021302a4, 0x0212e2ec, func_ov085_0212e2ec, "021302a4"},
    {data_ov085_021302ac, 0x0212d8ec, func_ov085_0212d8ec, "021302ac"},
    {data_ov085_021302b4, 0x0212dbdc, (void (*)(void *))(void *)pmfb7_ov085_0212dbdc, "021302b4"},
    {data_ov085_021302bc, 0x0212e59c, func_ov085_0212e59c, "021302bc"},
    {data_ov085_021302c4, 0x0212db04, func_ov085_0212db04, "021302c4"},
    {data_ov085_021302cc, 0x0212df84, func_ov085_0212df84, "021302cc"},
    {data_ov085_021302d4, 0x0212e5ac, (void (*)(void *))(void *)pmfb7_ov085_0212e5ac, "021302d4"},
    {data_ov085_021302dc, 0x0212e310, (void (*)(void *))(void *)pmfb7_ov085_0212e310, "021302dc"},
    {data_ov085_021302e4, 0x0212dcfc, func_ov085_0212dcfc, "021302e4"},
    {data_ov085_021302ec, 0x0212ddc4, func_ov085_0212ddc4, "021302ec"},
    {data_ov085_021302f4, 0x0212e078, (void (*)(void *))(void *)pmfb7_ov085_0212e078, "021302f4"},
    {data_ov085_021302fc, 0x0212de5c, (void (*)(void *))(void *)pmfb7_ov085_0212de5c, "021302fc"},
};

extern "C" void port_lakitu_bro_states_seat(void)
{
    static int done;
    if (done)
        return;
    done = 1;
    for (unsigned i = 0; i < sizeof g_states / sizeof g_states[0]; ++i) {
        PortPmf *p = g_states[i].slot;
        if (p->fn != g_states[i].rom) {
            std::fprintf(stderr, "FATAL: LakituBro state %s: the mount holds "
                         "%08x, the ROM's own table says %08x -- WRONG "
                         "BYTES\n", g_states[i].name, p->fn, g_states[i].rom);
            std::abort();
        }
        if (p->delta != 0) {
            std::fprintf(stderr, "FATAL: LakituBro state %s is not a plain "
                         "pointer-to-member (delta %d)\n", g_states[i].name,
                         p->delta);
            std::abort();
        }
        p->fn = (unsigned)(size_t)g_states[i].host;
    }
}

/* HOST COPY RETIRED, run link100 lane PMFB7 gate 2. src/_ZN9LakituBro8BehaviorEv.cpp
   dispatches its own field now: with /vmg /vmm (block R8) MSVC's pointer to
   member IS the ROM's eight-byte {code, adjust} pair, so the widening this
   banner was written for does not happen. The per-frame half of every state
   cell holds a zero-argument __fastcall face; the enter half does not change,
   because the helper that dispatches it tail-jumps. Measurements in
   port/slice_pmfb7.txt and runs/link100/out/PMFB7/. */
