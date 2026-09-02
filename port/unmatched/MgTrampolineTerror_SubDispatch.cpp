/* PORT_HOST_ABI.  The TRAMPOLINE-MARIO sub-object's member-pointer wall, and
 * it is port/mg_fanout_costs.txt section 4's THIRD SHAPE -- the one a link and
 * a `::*` sweep are both blind to.  Run mg11, lane TTE.
 *
 * ---- 1. WHY THIS IS NOT IN THE CLASS'S OWN WALL FILE ----------------------
 *
 * port/unmatched/MgTrampolineTerror_StateDispatch.cpp section 1 scans the ROM
 * for the five-instruction Itanium encoding over dScMgTrampoline2_c's OWN forty
 * bodies and finds exactly ONE site, vtable slot 6.  That measurement is right
 * and this file does not contradict it: THIS body is not one of the class's.
 * func_ov006_020c8f20 belongs to the element class the factory constructs five
 * of at this+0x500c, and the ROM names that class itself -- the type_info at
 * 0x0213b238 points at 0x0213b25c, which reads "17dScMgTrmpln2Mario_c", and its
 * sibling at 0x0213b270 reads "18dScMgTrmpln3DMario_c".  The Marios on the
 * trampoline are their own class with their own state machine.
 *
 * AND IT IS INVISIBLE TO BOTH PRESCRIBED DETECTORS.  src/func_ov006_020c8f20.cpp
 * spells the decode in PLAIN INTS --
 *
 *     typedef void (*PMF)(void*);
 *     struct Closure { int off; int adj; };
 *     Closure* cl = (Closure*)(o + 0x70);
 *     void* tobj = o + (cl->adj >> 1);
 *     if (cl->adj & 1) fn = *(void(**)())(*(char**)tobj + cl->off);
 *     else             fn = (void(*)())cl->off;
 *     ((void(*)(void*))fn)(tobj);
 *
 * -- so there is no `::*` to sweep for and no unresolved symbol for a link to
 * name.  It COMPILES and it LINKS and then it jumps to a DS address.  This lane
 * convicted it the only way section 4 says it can be convicted: by a run.  The
 * third boot of scene 385 faulted with
 *
 *     FAULT code c0000005 at +0x01cca310 accessing 020ca310
 *       func_ov006_020c8f20
 *       _ZN6Player12St_Null_InitEv
 *       func_ov006_02123cb4      <- this class's PLAY state
 *       func_ov006_02123340      <- vtable slot 6
 *       tte_beh
 *
 * and 0x020ca310 is a real ov006 body: the faulting address IS the pair's code
 * word, called as if it were a host pointer.
 *
 * ---- 2. THE PAIR UNIVERSE, BOUNDED FROM THE ROM'S OWN RUN -----------------
 *
 * The field is o+0x70 and the writers name their sources, so the universe is
 * read off the .data rather than guessed.  Every writer this lane found copies
 * a named mwcc pair or an already-loaded pair:
 *
 *     src/func_ov006_020ca374.c   *(S2*)(o+0x70)   = data_ov006_0213b124
 *     src/func_ov006_020ca2ec.c   *(S8*)(o+0x70)   = data_ov006_0213b134
 *     src/func_ov006_020c9e7c.c   *(Vec2i*)(o+0x70)= data_ov006_0213b16c
 *     src/func_ov006_020c9098.c   from data_ov006_0213b114
 *     src/func_ov006_020c9c8c.c   from data_ov006_0213b194
 *     src/func_ov006_020c94e0.cpp from data_ov006_0213b1ec
 *
 * and every one of those symbols sits inside ONE CONTIGUOUS RUN of {code, 0}
 * pairs in ov006's .data, 0x0213b0f4 .. 0x0213b228, read out of
 * extracted/overlays/overlay_0006.bin at base 0x020bfec0.  The run's ends are
 * where it stops being pairs: 0x0213b19c is a type_info record ({0x0209a774,
 * 0x0213b288}) sitting inside it, and 0x0213b22c is a three-word pointer array
 * (0x02140590 / 0x02140560 / 0x02140580).  Every adjustment word in the run is
 * ZERO, so no site here is a virtual member pointer.
 *
 * TWENTY DISTINCT CODE WORDS, and all twenty have a matched src TU with a
 * delinks block.  They are switched below and their TUs are slice lines.  The
 * switch is bounded by the run rather than by the six writers because a field
 * pmf can be written by anything that can reach the object -- the same reason
 * port/unmatched/MgMemory2_FieldPmf.cpp gives -- and over-covering costs a
 * switch arm while under-covering costs a jump to a DS address.
 *
 * WHAT IS NOT COVERED FALLS THROUGH TO THE FRAMEWORK, which prints the address
 * on an UNHANDLED line and counts it (unmatched/MgBase_StateDispatch.cpp).  A
 * run with zero UNHANDLED is the evidence that the twenty are all of them; this
 * header is not.
 *
 * ---- 3. THE STORED PAIR IS NOT REWRITTEN ---------------------------------
 *
 * The seat routes at the DISPATCH SITE.  src/func_ov006_020c94e0.cpp compares
 * the stored words by VALUE (`if (s[0] != q[0] || (s[1] != q[1] && ...))`), so
 * writing host addresses over the .data pairs -- the ov085/ov100 treatment --
 * would leave that comparison permanently false while the dispatch itself
 * looked fine.  Same ruling as MgMemory2_FieldPmf.cpp section 3 and
 * port/mg_fanout_costs.txt section 4's flower case.
 *
 * ---- 4. THE HOST COPY IS ITS src TU WITH TWO LINES CHANGED ---------------
 *
 * The two redundant out-of-line member redeclarations MSVC rejects with C2761
 * (`System* System::New(...);` and `void Animation::Advance();` repeated after
 * the structs that already declare them) are dropped, and the five-line decode
 * is replaced by the routed call.  Every other statement, offset and constant
 * is src's, in src's order.  src/func_ov006_020c8f20.cpp is OUT of
 * port/slice_tte.txt: listing it would be an LNK2005 against this definition.
 * A hostgen MEMBER_REDECL entry
 * for that file would have been the alternative and was tried first; the host
 * copy supersedes it, so port/tools/hostgen.py carries only this lane's
 * HEADER_SHADOW row.
 */

#include <cstdio>

struct MgPmf { unsigned code; int adj; };

extern "C" {

/* the framework's one entry point for a zero-argument state call */
void port_mg_call0(void *self, unsigned code, int adj);

/* the twenty distinct code words in 0x0213b0f4..0x0213b228 */
void func_ov006_020c905c(void *o);
void func_ov006_020c91ac(void *o);
void func_ov006_020c97bc(void *o);
void func_ov006_020c9d7c(void *o);
void func_ov006_020c9efc(void *o);
void func_ov006_020ca070(void *o);
void func_ov006_020ca310(void *o);
void func_ov006_020ca39c(void *o);
void func_ov006_020cb16c(void *o);
void func_ov006_020cb2b4(void *o);
void func_ov006_020cb5c4(void *o);
void func_ov006_020cb72c(void *o);
void func_ov006_020cbd7c(void *o);
void func_ov006_020cc2ac(void *o);
void func_ov006_020cc408(void *o);
void func_ov006_020cc63c(void *o);
void func_ov006_020cc724(void *o);
void func_ov006_020cc9b8(void *o);
void func_ov006_020ccae0(void *o);
void func_ov006_020ccd64(void *o);

/* src/func_ov006_020c8f20.cpp's own callees, spelled as that file spells them */
void func_ov006_020c9024(char *o);
void func_ov006_020c8ecc(char *o);
extern int data_ov006_0214059c;

void func_ov006_020c8f20(char *o);

}  /* extern "C" */

/* The two names src/func_ov006_020c8f20.cpp reaches through local structs.  The
   Faces file carries the /alternatename rows that make both reachable; here
   they are declared with the same local shapes that file uses so the emitted
   call surface is identical. */
typedef int Fix12;
struct Vector3_16f;
struct Callback;
struct Animation { void Advance(); };
struct System {
    static System *New(unsigned, unsigned, Fix12, Fix12, Fix12,
                       const Vector3_16f *, Callback *);
};

// ---- the field switch -------------------------------------------------------

static unsigned g_tte_sub_calls, g_tte_sub_routed;

static int tte_sub_try(void *self, unsigned code)
{
    switch (code) {
    case 0x020c905cu: func_ov006_020c905c(self); return 1;
    case 0x020c91acu: func_ov006_020c91ac(self); return 1;
    case 0x020c97bcu: func_ov006_020c97bc(self); return 1;
    case 0x020c9d7cu: func_ov006_020c9d7c(self); return 1;
    case 0x020c9efcu: func_ov006_020c9efc(self); return 1;
    case 0x020ca070u: func_ov006_020ca070(self); return 1;
    case 0x020ca310u: func_ov006_020ca310(self); return 1;
    case 0x020ca39cu: func_ov006_020ca39c(self); return 1;
    case 0x020cb16cu: func_ov006_020cb16c(self); return 1;
    case 0x020cb2b4u: func_ov006_020cb2b4(self); return 1;
    case 0x020cb5c4u: func_ov006_020cb5c4(self); return 1;
    case 0x020cb72cu: func_ov006_020cb72c(self); return 1;
    case 0x020cbd7cu: func_ov006_020cbd7c(self); return 1;
    case 0x020cc2acu: func_ov006_020cc2ac(self); return 1;
    case 0x020cc408u: func_ov006_020cc408(self); return 1;
    case 0x020cc63cu: func_ov006_020cc63c(self); return 1;
    case 0x020cc724u: func_ov006_020cc724(self); return 1;
    case 0x020cc9b8u: func_ov006_020cc9b8(self); return 1;
    case 0x020ccae0u: func_ov006_020ccae0(self); return 1;
    case 0x020ccd64u: func_ov006_020ccd64(self); return 1;
    default:                                     return 0;
    }
}

extern "C" void port_mg_tte_sub_call(void *self, unsigned code, int adj)
{
    ++g_tte_sub_calls;
    if (code != 0 && adj == 0 && tte_sub_try(self, code)) {
        ++g_tte_sub_routed;
        return;
    }
    port_mg_call0(self, code, adj);
}

extern "C" void port_mg_tte_sub_counts(unsigned *calls, unsigned *routed)
{
    if (calls)  *calls  = g_tte_sub_calls;
    if (routed) *routed = g_tte_sub_routed;
}

// ---- the host copy ----------------------------------------------------------

// PORT_HOST_ABI: dScMgTrmpln2Mario_c sub-object field-pmf dispatcher; the ROM open-codes the eight-byte {off, adj} decode at o+0x70 in plain ints, so it compiles and links but jumps to a DS address, and the host reads the pair and routes it.
extern "C" void func_ov006_020c8f20(char *o)
{
    *(int *)(o + 0x30) = *(int *)(o + 0x24);
    *(int *)(o + 0x34) = *(int *)(o + 0x28);
    *(int *)(o + 0x38) = *(int *)(o + 0x2c);

    /* THE DELTA.  src decodes {off, adj} by hand and calls through it; the
       adjustment is applied to the receiver exactly as src does, and the
       routing decision is left to one place. */
    if (*(int *)(o + 0x70) != 0) {
        const MgPmf *cl = (const MgPmf *)(o + 0x70);
        void *tobj = o + (cl->adj >> 1);
        port_mg_tte_sub_call(tobj, cl->code, cl->adj);
    }

    func_ov006_020c9024(o);
    {
        Fix12 v =
            (Fix12)(((long long)*(int *)(o + 0x4c) * 0xb4b + 0x800) >> 12);
        if (*(int *)(o + 0x40) > v) {
            if (*(int *)(o + 0xd8) == data_ov006_0214059c) {
                *(int *)(o + 0x5c) = (int)System::New(
                    *(int *)(o + 0x5c), 0xf5,
                    *(int *)(o + 0x24) << 3, *(int *)(o + 0x28) << 3,
                    *(int *)(o + 0x2c) << 3, 0, 0);
            }
        }
    }
    func_ov006_020c8ecc(o);
    ((Animation *)(o + 0xc8))->Advance();
}
