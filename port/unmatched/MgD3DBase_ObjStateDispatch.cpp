// PORT_HOST_ABI. The SECOND and THIRD mwcc pointer-to-member walls in
// dScMgJump_c's closure, and NEITHER belongs to that class. Run mg11, lane BNP.
//
// NAMED FOR THE BASE, BUT BNP-ONLY IN PRACTICE, and the distinction is a merge
// fact rather than a style point. The two dispatchers below live in ov006's
// shared minigame-object region (0x020c4000..0x020c9000), so any
// dScMgD3DBase_c subclass whose closure reaches them can call this file --
// which is why it carries the base's name. But lane BNT's tree has NO
// MgD3DBase_ObjStateDispatch.cpp and defines none of these symbols: 0x175's
// closure does not reach either dispatcher. So unlike the other three
// MgD3DBase_* files this one has no BNP/BNT add/add conflict at the merge, and
// a merge note that lumps all four together as "converged" is wrong by one.
//
// ---- 1. HOW THEY WERE FOUND, AND WHY THE FIRST SWEEP MISSED THEM ----------
//
// port/slice_bnp.txt section 6 records a `::*` sweep over the 41 TUs of
// dScMgJump_c's own code block and dScMgD3DBase_c's, and it returns exactly one
// file. THAT SWEEP WAS SCOPED TOO NARROWLY and the lane's own first proof run
// is what said so: with the state machine started, scene 372 faulted with
//
//     FAULT code c0000005 at +0x01cc4d1c accessing 020c4d1c
//       walker node 307FA7F4 actor 307FA7CC id 0x174
//       regs eax=020c4d1c ebx=020ee5b8
//
// -- a jump to a RAW DS ADDRESS, with ebx still holding func_ov006_020ee5b8,
// the state this class had just entered. 0x020c4d1c is a real four-byte ov006
// function, so the fault is not a wild pointer: it is a member pointer nobody
// routed.
//
// LANE LKY'S ENCODING DETECTOR RUN OVER THE WHOLE CLOSURE IS WHAT NAMES THEM.
// Decoding every word of the TUs in port/slice_bnp.txt (84 when this was first
// written, 129 once the twenty-five state bodies below were added) and looking for
// `add Rd,Rn,Rm,asr #1` immediately followed by `ands Rm,Rm,#1` finds THREE
// sites, not one:
//
//     020ee290  func_ov006_020ee27c   vtable slot 6, this class's own
//     020c4ce8  func_ov006_020c4cd8   the object adapter's tick, +0x30
//     020c7898  func_ov006_020c7860   the second object's tick, +0x3c
//
// The lesson is the scope, not the tool: a `::*` sweep over the CLASS is not a
// sweep over the CLOSURE, and a seat links the closure.
//
// ---- 2. THE TWO FIELDS AND THE TWENTY-FIVE STATES -------------------------
//
// func_ov006_020c4cd8 dispatches the pair at self+0x30 and then runs
// Animation::Advance(self+0x88) and func_ov006_020c4c00(self).
// func_ov006_020c7860 dispatches the pair at self+0x3c between an
// ApproachLinear/AddVec3 prologue and a four-call tail.
//
// Both read their pair out of MOUNTED .data, which is why nothing in the build
// names the state bodies and why port/tools/closure.py reported this slice
// closed: the code words are DS addresses in a data table, not symbol
// references. Enumerated from the RELOCATION SET rather than by walking an
// address range -- every load relocation whose source lies in ov006's .data,
// whose next word is zero, and whose target is a function start inside
// 0x020c4000..0x020c9000 -- there are TWENTY-FIVE distinct state bodies, and
// every one of them has a src file:
//
//   020c4d1c (0x4, `bx lr`, the idle state and the address the fault jumped to)
//   020c4e8c 020c4fa4 020c53f8 020c5530 020c5658 020c5928 020c5bf8 020c5d28
//   020c6088 020c61c4 020c627c 020c6378 020c6400 020c66bc 020c6a9c 020c78ec
//   020c7a30 020c7c68 020c8048 020c814c 020c833c 020c85bc 020c864c 020c8680
//
// All twenty-five are `void f(receiver)` in the ROM except 020c4d1c, which
// takes none and returns immediately. They are added to port/slice_bnp.txt by
// this lane; before it, none of them was in any slice in the tree.
//
// ---- 3. THE STORED PAIRS KEEP THE ROM'S OWN WORDS -------------------------
//
// Same rule as unmatched/MgBounceAndPounce_StateDispatch.cpp section 2 and for
// the same reason: this file DECODES the address the ROM wrote, it does not
// rewrite the mounted pairs. The pair run these two read from is shared with
// classes this port has not seated, and rewriting it would decide their
// behaviour from inside this lane.

#include <cstdio>

struct MgPmf { unsigned code; int adj; };

extern "C" {

/* the family chain's end */
void port_mg_call0(void *self, unsigned code, int adj);

/* the two dispatchers' own non-state callees, kept exactly as src has them */
void _ZN9Animation7AdvanceEv(void *a);
void func_ov006_020c4c00(void *c);
void _Z14ApproachLinearRiii(int *r, int target, int step);
void AddVec3(void *a, void *b, void *c);
void func_ov006_020bfec0(void *a, char *b, short *d);
void func_ov006_020c76e0(char *c);
extern int   data_ov006_0213b010;
extern int   data_ov006_0213b018;
extern void *data_ov006_02141a40;

/* the twenty-five state bodies */
void func_ov006_020c4d1c(void);
void func_ov006_020c4e8c(char *c);
void func_ov006_020c4fa4(char *c);
void func_ov006_020c53f8(char *c);
void func_ov006_020c5530(char *c);
void func_ov006_020c5658(char *c);
void func_ov006_020c5928(char *c);
void func_ov006_020c5bf8(char *c);
void func_ov006_020c5d28(char *c);
void func_ov006_020c6088(char *c);
void func_ov006_020c61c4(int c);
void func_ov006_020c627c(char *c);
void func_ov006_020c6378(int c);
void func_ov006_020c6400(char *c);
void func_ov006_020c66bc(char *c);
void func_ov006_020c6a9c(char *c);
void func_ov006_020c78ec(char *c);
void func_ov006_020c7a30(char *c);
void func_ov006_020c7c68(char *c);
void func_ov006_020c8048(void *c);
void func_ov006_020c814c(char *c);
void func_ov006_020c833c(char *c);
void func_ov006_020c85bc(char *c);
void func_ov006_020c864c(int *c);
void func_ov006_020c8680(char *c);

/* the two host copies, defined below */
void func_ov006_020c4cd8(void *c);
void func_ov006_020c7860(char *c);

/* the witness this file exports */
void     port_mg_objstate_counts(unsigned *calls, unsigned *hits,
                                 unsigned *nullpmf);
unsigned port_mg_objstate_count(void);
unsigned port_mg_objstate_addr(unsigned i);
unsigned port_mg_objstate_hit(unsigned i);

}  /* extern "C" */

// ---- the address switch ----------------------------------------------------

static unsigned g_obj_calls, g_obj_hits, g_obj_nullpmf;

static const unsigned kObjStates[] = {
    0x020c4d1cu, 0x020c4e8cu, 0x020c4fa4u, 0x020c53f8u, 0x020c5530u,
    0x020c5658u, 0x020c5928u, 0x020c5bf8u, 0x020c5d28u, 0x020c6088u,
    0x020c61c4u, 0x020c627cu, 0x020c6378u, 0x020c6400u, 0x020c66bcu,
    0x020c6a9cu, 0x020c78ecu, 0x020c7a30u, 0x020c7c68u, 0x020c8048u,
    0x020c814cu, 0x020c833cu, 0x020c85bcu, 0x020c864cu, 0x020c8680u,
};
enum { kObjStateCount = sizeof kObjStates / sizeof kObjStates[0] };
static unsigned g_obj_state_hits[kObjStateCount];

static void obj_note(unsigned code)
{
    for (unsigned i = 0; i < (unsigned)kObjStateCount; ++i)
        if (kObjStates[i] == code) { ++g_obj_state_hits[i]; return; }
}

static int obj_try(void *self, unsigned code)
{
    char *c = (char *)self;
    obj_note(code);
    switch (code) {
    case 0x020c4d1cu: func_ov006_020c4d1c();  return 1;
    case 0x020c4e8cu: func_ov006_020c4e8c(c); return 1;
    case 0x020c4fa4u: func_ov006_020c4fa4(c); return 1;
    case 0x020c53f8u: func_ov006_020c53f8(c); return 1;
    case 0x020c5530u: func_ov006_020c5530(c); return 1;
    case 0x020c5658u: func_ov006_020c5658(c); return 1;
    case 0x020c5928u: func_ov006_020c5928(c); return 1;
    case 0x020c5bf8u: func_ov006_020c5bf8(c); return 1;
    case 0x020c5d28u: func_ov006_020c5d28(c); return 1;
    case 0x020c6088u: func_ov006_020c6088(c); return 1;
    case 0x020c61c4u: func_ov006_020c61c4((int)(size_t)c); return 1;
    case 0x020c627cu: func_ov006_020c627c(c); return 1;
    case 0x020c6378u: func_ov006_020c6378((int)(size_t)c); return 1;
    case 0x020c6400u: func_ov006_020c6400(c); return 1;
    case 0x020c66bcu: func_ov006_020c66bc(c); return 1;
    case 0x020c6a9cu: func_ov006_020c6a9c(c); return 1;
    case 0x020c78ecu: func_ov006_020c78ec(c); return 1;
    case 0x020c7a30u: func_ov006_020c7a30(c); return 1;
    case 0x020c7c68u: func_ov006_020c7c68(c); return 1;
    case 0x020c8048u: func_ov006_020c8048(c); return 1;
    case 0x020c814cu: func_ov006_020c814c(c); return 1;
    case 0x020c833cu: func_ov006_020c833c(c); return 1;
    case 0x020c85bcu: func_ov006_020c85bc(c); return 1;
    case 0x020c864cu: func_ov006_020c864c((int *)c); return 1;
    case 0x020c8680u: func_ov006_020c8680(c); return 1;
    default:                                  return 0;
    }
}

extern "C" void port_mg_objstate_call0(void *self, unsigned code, int adj)
{
    ++g_obj_calls;
    if (code == 0)
        ++g_obj_nullpmf;
    if (code != 0 && adj == 0 && obj_try(self, code)) {
        ++g_obj_hits;
        return;
    }
    port_mg_call0(self, code, adj);
}

extern "C" void port_mg_objstate_counts(unsigned *calls, unsigned *hits,
                                        unsigned *nullpmf)
{
    if (calls)   *calls   = g_obj_calls;
    if (hits)    *hits    = g_obj_hits;
    if (nullpmf) *nullpmf = g_obj_nullpmf;
}

extern "C" unsigned port_mg_objstate_count(void)
{ return (unsigned)kObjStateCount; }
extern "C" unsigned port_mg_objstate_addr(unsigned i)
{ return i < (unsigned)kObjStateCount ? kObjStates[i] : 0u; }
extern "C" unsigned port_mg_objstate_hit(unsigned i)
{ return i < (unsigned)kObjStateCount ? g_obj_state_hits[i] : 0u; }

// ---- the two host copies ---------------------------------------------------

/* src/func_ov006_020c4cd8.cpp. Its `struct Foo` carried nothing but the padding
   and the member pointer, so replacing the dispatch removes the struct and no
   layout moves: the two remaining calls keep src's offsets exactly. */
extern "C" void func_ov006_020c4cd8(void *c)
{
    {
        const MgPmf *p = (const MgPmf *)((char *)c + 0x30);
        port_mg_objstate_call0(c, p->code, p->adj);
    }
    _ZN9Animation7AdvanceEv((char *)c + 0x88);
    func_ov006_020c4c00(c);
}

/* src/func_ov006_020c7860.cpp. Same shape: `struct C { char pad[0x3c]; PMF m; }`
   is only there to hold the member pointer, and the dispatch sits between an
   unchanged prologue and an unchanged tail. Every other access in the TU is a
   raw char* offset and is copied verbatim. */
extern "C" void func_ov006_020c7860(char *c)
{
    _Z14ApproachLinearRiii((int *)(c + 0x24), data_ov006_0213b010,
                           data_ov006_0213b018);
    AddVec3(c + 0x14, c + 0x20, c + 0x14);
    {
        const MgPmf *p = (const MgPmf *)(c + 0x3c);
        port_mg_objstate_call0(c, p->code, p->adj);
    }
    func_ov006_020bfec0(*(void **)&data_ov006_02141a40, c + 0x14,
                        (short *)(c + 0x36));
    func_ov006_020c76e0(c);
    _ZN9Animation7AdvanceEv(c + 0x9c);
}
