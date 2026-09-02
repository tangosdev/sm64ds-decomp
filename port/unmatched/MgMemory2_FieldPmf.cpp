// PORT_HOST_ABI. The mwcc POINTER-TO-MEMBER WALL in its WORSE form: three
// dispatchers that read the member pointer out of an OBJECT FIELD rather than
// out of a table.  Run mg6, lane MEM (dScMgMemory2_c, actor id 0x16b,
// scene 363).
//
// Read unmatched/MgBase_StateDispatch.cpp's header first for the ROM's dispatch
// sequence and the framework's entry points; read
// unmatched/MgMemory2_StateDispatch.cpp for this class's TABLE half.  This file
// is only the field half.
//
// ---- 1. WHY A FIELD IS WORSE THAN A TABLE ---------------------------------
//
// port/mg_fanout_costs.txt section 4 states it and this lane's three cases are
// three more instances: a table dispatched at the wrong stride is a wrong
// stride inside a correctly laid-out object, while a member pointer stored IN
// the object is four bytes on MSVC where the ROM has eight, so every field
// after it moves and the body reads the wrong field BEFORE any dispatch
// happens.  One of the three below has exactly that defect and it is spelled
// out in section 3.
//
// A LINK NEVER NAMES ANY OF THE THREE.  There is no global to be unresolved --
// the pointer is a field -- so an LNK1120 is silent on all of them however the
// tables are declared.  Only the `::*` sweep finds them, which is why
// port/mg_fanout_costs.txt makes the sweep a rule rather than a courtesy.  This
// lane swept its whole 869-file closure (runs/mg6/out/MEM/pmf_sweep_16b.txt)
// and found fourteen `::*` TUs: the six ov006 ones the handover named, seven
// ov004 ones MG1/MG2/BASESET already host-copied, and ONE THAT NO LANE HAD
// SEEN -- src/func_ov004_020b52fc.cpp, section 4 below.
//
// ---- 2. func_ov006_020c19d0, THE MODEL SUB-OBJECT'S TICK -------------------
//
// dScMgMemory2_c::Behavior calls it on `this + 0x4f38` every frame (the last
// statement of func_ov006_020f7458, disassembled in the state-dispatch file).
// It is SHARED ov006 glue, not this class's: eight src TUs name it, among them
// bodies belonging to four other minigames.  This is the first lane to reach
// it, so this file is where it lands.
//
// The ROM at 0x020c19d0, out of extracted/overlays/overlay_0006.bin at base
// 0x020bfec0:
//
//     stmdb sp!,{r4,lr}
//     mov   r4,r0
//     ldr   r2,[r4]           the CODE word
//     cmp   r2,#0
//     beq   +6                the ROM'S OWN null-pmf guard
//     ldr   r1,[r4,#4]        the adjustment
//     add   r0,r4,r1,asr #1
//     ands  r1,r1,#1
//     ldrne r1,[r0] / ldrne r1,[r1,r2]
//     ldreq r1,[r4]
//     blx   r1
//
// which is src/func_ov006_020c19d0.cpp's `if (*(int*)c != 0) (o->*(o->pmf))();`
// instruction for instruction.  The pair is at offset 0 of the sub-object, so
// nothing after it moves and this one is a wrong-decode only.
//
// ---- 3. func_ov006_020c07e8, AND IT IS THE LAYOUT CASE ---------------------
//
// Reached from 020c19d0's last statement, on `c + 0xdc`.  Its src struct is
//
//     char pad0[0xb4];  PMF pmf;  char pad1[0xc8 - 0xb4 - 8];  Vector3 v0c8;
//
// -- the pad after the member pointer is written 0xc8 - 0xb4 - 8, which is only
// correct at EIGHT BYTES.  Compiled as spelled, MSVC's four-byte member pointer
// puts v0c8 at 0xc4 and every field after it four bytes early, so the body
// would read the wrong words before the dispatch is even reached.  Spelling the
// pair as MgPmf (two ints, eight bytes on both machines) restores every offset
// the src file itself asserts, and nothing else in the struct moves.
//
// ITS IDLE TEST READS THE STORED PAIR BY VALUE, which is the reason this seat
// routes at the DISPATCH SITE and does not rewrite the mounted pair.  The body
// ends with
//
//     int* p = (int*)((int)c + 0xb4);  int* g = data_ov006_0213ac78;
//     if (p[0] == g[0]) { if (p[1] == g[1]) return; ... }
//
// and data_ov006_0213ac78 is a real mwcc pair, {0x020c0364, 0}.  A seat of the
// ov085/ov100 kind -- write HOST addresses over the .data source pairs before
// the constructor copies them -- would leave that comparison permanently false
// while the dispatch itself looked fine.  port/mg_fanout_costs.txt section 4
// derives the same rule from src/func_ov006_020c3b80.c on the flower's
// sub-object.  The stored pair keeps the ROM's own words here.
//
// ---- 4. func_ov004_020b52fc IS A FIND, NOT AN INHERITANCE ------------------
//
// The handover for this class named six ov006 TUs to host-copy and said the
// framework's own (func_ov004_020b87e0) was already trapped.  The sweep found a
// SEVENTH ov004 dispatcher that no lane has ever handled, and it is on this
// class's live path:
//
//     func_ov006_020f7458  (vtable slot 6, every frame)
//       -> func_ov004_020b65e4
//            -> for i in 0..0x13: func_ov004_020b52fc(&data_ov004_020bfa34[i*0x24])
//
// src/func_ov004_020b52fc.cpp is six lines, `struct C { PMF pmf; };
// (c->*(c->pmf))();`, and it is NOT in unmatched/MgBase_StateSetter.cpp's
// eighty-address universe: that file bounds ov004's TABLE member pointers, and
// this one is a field of a .bss record array.  Neither func_ov004_020b65e4 nor
// func_ov004_020b52fc appears in the baseline walk_window.map, so nothing in
// the port reached it before this lane and no neighbour changes because of it.
//
// THE ROM HAS NO NULL GUARD HERE, and that is worth stating because this file
// adds one.  0x020b52fc is fourteen instructions with no `cmp r2,#0` anywhere:
// it decodes and `blx r1` unconditionally, so on the DS a record whose pair is
// still {0,0} would branch to zero.  The port routes through port_mg_call0,
// whose first statement is the framework's `if (code == 0) return;`.  That is a
// divergence in the safe direction and it is visible rather than hidden: the
// dispatch call count in hal/scene_mg_memory2.cpp's census counts every one of
// the twenty per frame, so a run in which the records are never populated reads
// as a large call count and a small routed count.
//
// THE TWENTY RECORDS' PMF UNIVERSE IS BOUNDED FROM THE SOURCE, the way lane
// BASESET bounded ov004's.  data_ov004_020bfa34 is twenty records of 0x24
// bytes, constructed by __sinit_ov004_020b9ad0 through func_020733a8 with
// func_ov004_020b67e4 as the element constructor -- and that constructor is
// EMPTY, so the pairs start as the .bss zeros the null guard above is about.
// The writers are func_ov004_020b506c (`*(struct W2*)c = data_ov004_020bc7d8`)
// and func_ov004_020b5108 (data_ov004_020bc800 and _020bc820), and the pair
// symbols they draw from are a contiguous run in the ov004 image:
//
//     data_ov004_020bc7d8  {0x020b4ff0, 0}      data_ov004_020bc800  {0x020b51f0, 0}
//     data_ov004_020bc7e0  {0x020b4d50, 0}      data_ov004_020bc808  {0x020b4aa4, 0}
//     data_ov004_020bc7e8  {0x020b5288, 0}      data_ov004_020bc810  {0x020b4dfc, 0}
//     data_ov004_020bc7f0  {0x020b5288, 0}      data_ov004_020bc818  {0x020b4c30, 0}
//     data_ov004_020bc7f8  {0x020b4f44, 0}      data_ov004_020bc820  {0x020b5288, 0}
//
// Ten pairs, EIGHT distinct code words, every adjustment zero, read out of
// extracted/overlays/overlay_0004.bin at base 0x020ad660.  All eight are routed
// below and all eight have a matched src TU.
//
// ---- 5. THE MODEL SUB-OBJECT'S PMF UNIVERSE, BOUNDED THE SAME WAY ---------
//
// The pairs 020c19d0 and 020c07e8 dispatch come from one contiguous run of
// ov006 .data, 0x0213ac48..0x0213acb8, every one {code, 0}:
//
//     0213ac48 {020c11c0,0}   0213ac60 {020c14bc,0}   0213ac78 {020c0364,0}
//     0213ac50 {020c1760,0}   0213ac68 {020c0364,0}   0213ac80 {020c0b74,0}
//     0213ac58 {020c1760,0}   0213ac70 {020c0264,0}   0213ac88 {020c0364,0}
//     0213ac90 {020c1760,0}   0213ac98 {020c0ce8,0}   0213aca0 {020c0df0,0}
//     0213aca8 {020c0f9c,0}   0213acb0 {020c11c0,0}   0213acb8 {020c0f9c,0}
//
// Fifteen pairs, NINE distinct code words, all with a matched src TU.  The
// writers are the ordinary ones -- func_ov006_020c0304 and _020c0264 write
// +0xb4, func_ov006_020c0e8c and _020c1164 write offset 0 -- and 0213ac78 is
// the by-value sentinel section 3 names.  0x0213acc0 is where the run stops
// being pairs: it reads {4, 0x0213ad58}, which is a length and a pointer, and
// is the reason this list is taken from the CONSUMERS rather than from an
// address range.
//
// ---- 6. WHAT A RUN IS STILL THE ONLY DETECTOR FOR --------------------------
//
// Seventeen addresses are routed below because the source names them.  Nothing
// here claims that is all of them: a field pmf can be written by any code that
// can reach the object, and port/mg_fanout_costs.txt section 4's third shape
// (a TU that open-codes the Itanium sequence in plain ints) is invisible to
// both the link and the sweep.  Anything this file does not own falls through
// to the framework, which prints the address on an UNHANDLED line, and
// hal/scene_mg_memory2.cpp's census reports the count.  A run with zero
// UNHANDLED is the evidence; this header is not.

#include <cstdio>

/* The eight-byte mwcc member pointer, in the only spelling that is true on both
   machines.  The same struct MgMemory2_StateDispatch.cpp uses; each TU declares
   its own because neither is a header. */
struct MgPmf { unsigned code; int adj; };

/* src/func_ov006_020c07e8.cpp's own Vector3, kept so the two Vec3_HorzAngle
   arguments are the shapes that file passes. */
struct MemVector3 { int x, y, z; };

extern "C" {

/* this class's table-half chain, which ends at the framework's port_mg_call0 */
void port_mg_memory2_call0(void *self, unsigned code, int adj);

/* ---- the eight ov004 record bodies, section 4 -------------------------- */
void func_ov004_020b4aa4(char *c);
void func_ov004_020b4c30(char *r4);
void func_ov004_020b4d50(char *c);
void func_ov004_020b4dfc(char *c);
void func_ov004_020b4f44(char *c);
void func_ov004_020b4ff0(char *c);
void func_ov004_020b51f0(char *c);
void func_ov004_020b5288(char *c);

/* ---- the nine ov006 model bodies, section 5 ---------------------------- */
void func_ov006_020c0264(char *c);
void func_ov006_020c0364(char *c);
void func_ov006_020c0b74(char *p);
void func_ov006_020c0ce8(char *c);
void func_ov006_020c0df0(char *c);
void func_ov006_020c0f9c(void *cc);
void func_ov006_020c11c0(char *c);
void func_ov006_020c14bc(char *c);
void func_ov006_020c1760(void);      /* 4 bytes in the ROM, `bx lr` */

/* the ordinary callees the two ov006 host copies keep, spelled as their own
   src TUs spell them */
void _ZN14BlendModelAnim7AdvanceEv(void *x);
int  _Z15ApproachLinear2Rsss(short &v, short a, short b);
short Vec3_HorzAngle(const void *a, const void *b);
void _Z11UpdateAngleRssis(short *p, short d, int n, short lim);
extern short data_ov006_0212b89c[];
extern int   data_ov006_0213ac78[2];
/* src/func_ov006_020c07e8.cpp declares this one OUTSIDE its extern "C" block,
   so it mangles as a C++ global and the ov006 mount's plain C definition does
   not satisfy it -- the ordinary name-spelling defect this port carries
   /alternatename rows for. The host copy declares it at C linkage instead,
   which is the same fix without an alias. */
extern struct MemVector3 data_ov006_0212b890;

/* host-copied below and called from above its own definition */
void func_ov006_020c07e8(void *x);

}  /* extern "C" */

// ---- the field switch -------------------------------------------------------

static unsigned g_mem2_field_calls;
static unsigned g_mem2_field_hits;

static int mem2_field_try(void *self, unsigned code)
{
    char *c = (char *)self;
    switch (code) {
    /* ov004, data_ov004_020bfa34's ten pairs, eight distinct */
    case 0x020b4aa4u: func_ov004_020b4aa4(c); return 1;
    case 0x020b4c30u: func_ov004_020b4c30(c); return 1;
    case 0x020b4d50u: func_ov004_020b4d50(c); return 1;
    case 0x020b4dfcu: func_ov004_020b4dfc(c); return 1;
    case 0x020b4f44u: func_ov004_020b4f44(c); return 1;
    case 0x020b4ff0u: func_ov004_020b4ff0(c); return 1;
    case 0x020b51f0u: func_ov004_020b51f0(c); return 1;
    case 0x020b5288u: func_ov004_020b5288(c); return 1;
    /* ov006, the model sub-object's fourteen pairs, nine distinct */
    case 0x020c0264u: func_ov006_020c0264(c); return 1;
    case 0x020c0364u: func_ov006_020c0364(c); return 1;
    case 0x020c0b74u: func_ov006_020c0b74(c); return 1;
    case 0x020c0ce8u: func_ov006_020c0ce8(c); return 1;
    case 0x020c0df0u: func_ov006_020c0df0(c); return 1;
    case 0x020c0f9cu: func_ov006_020c0f9c(c); return 1;
    case 0x020c11c0u: func_ov006_020c11c0(c); return 1;
    case 0x020c14bcu: func_ov006_020c14bc(c); return 1;
    case 0x020c1760u: func_ov006_020c1760();  return 1;  /* bx lr body */
    default:                                  return 0;
    }
}

/* The entry point the three host copies below use.  It tries this file's
   addresses and hands everything else to the class's table chain, which ends at
   the framework -- so the null-code guard, the nonzero-adjustment refusal and
   the UNHANDLED report still live in exactly one place. */
extern "C" void port_mg_memory2_field_call(void *self, unsigned code, int adj)
{
    ++g_mem2_field_calls;
    if (code != 0 && adj == 0 && mem2_field_try(self, code)) {
        ++g_mem2_field_hits;
        return;
    }
    port_mg_memory2_call0(self, code, adj);
}

extern "C" void port_mg_memory2_field_counts(unsigned *calls, unsigned *hits)
{
    if (calls) *calls = g_mem2_field_calls;
    if (hits)  *hits  = g_mem2_field_hits;
}

// ---- the three host copies -------------------------------------------------
//
// Each is its src TU verbatim except for the member-pointer declaration (MgPmf
// rather than a PMF field) and the dispatch site.  Where anything else moved it
// is stated on the line.

/* src/func_ov004_020b52fc.cpp, section 4.  Its whole body is the dispatch, so
   the host copy is the dispatch and nothing else.  The record pointer IS the
   object the pair belongs to, so `this` is the argument unchanged -- the ROM's
   `mov r0,r2` with a zero adjustment is exactly r0. */
// PORT_HOST_ABI: mwcc pointer-to-member dispatch through an object-field 8-byte member pointer MSVC's 4-byte pmf cannot reproduce
extern "C" void func_ov004_020b52fc(void *c)
{
    const MgPmf *p = (const MgPmf *)c;
    port_mg_memory2_field_call(c, p->code, p->adj);
}

/* src/func_ov006_020c19d0.cpp, section 2.  The pair is at offset 0, the null
   guard is src's own (and the ROM's), and every other statement -- the
   BlendModelAnim advance at +0x1c, the ApproachLinear2 at +0x1e0, the +0x1e2
   counter, the data_ov006_0212b89c lookup keyed on +0x1e2, the +0x1e4 spin and
   the tail call into func_ov006_020c07e8 on c+0xdc -- is verbatim. */
// PORT_HOST_ABI: mwcc field pointer-to-member dispatch on the sub-object 8-byte pair MSVC's 4-byte pmf cannot reproduce
extern "C" void func_ov006_020c19d0(void *thiz)
{
    char *c = (char *)thiz;
    if (*(int *)c != 0) {
        const MgPmf *p = (const MgPmf *)c;
        port_mg_memory2_field_call(c, p->code, p->adj);
    }
    _ZN14BlendModelAnim7AdvanceEv(c + 0x1c);
    if (_Z15ApproachLinear2Rsss(*(short *)(c + 0x1e0), 0, 1) != 0) {
        short *q = (short *)(c + 0x1e2);
        *q = (short)(*q + 1);
        if (*(short *)(c + 0x100 + 0xe2) > 7)
            *(short *)(c + 0x100 + 0xe2) = 1;
        *(short *)(c + 0x1e0) = data_ov006_0212b89c[*(short *)(c + 0x100 + 0xe2)];
    }
    {
        short *r = (short *)(c + 0x1e4);
        *r = (short)(*r + 0x400);
    }
    func_ov006_020c07e8(c + 0xdc);
}

/* src/func_ov006_020c07e8.cpp, section 3.  The struct below is that file's own,
   with `PMF pmf` replaced by `MgPmf pmf` -- which is what makes the pads it
   spells (`0xc8 - 0xb4 - 8`) true again, and therefore what puts v0c8, a0ea,
   a0f0 and a0f4 back at the offsets the file asserts.  Nothing else in the
   layout or the body moves, including the by-value idle test at the end. */
struct MemModelC {
    char  pad0[0xb4];
    MgPmf pmf;                        /* 0xb4, two words -- DELTA: was PMF */
    char  pad1[0xc8 - 0xb4 - 8];
    MemVector3 v0c8;                  /* 0xc8 */
    char  pad2[0xea - 0xc8 - 12];
    short a0ea;                       /* 0xea */
    char  pad3[0xf0 - 0xea - 2];
    short a0f0;                       /* 0xf0 */
    char  pad4[0xf4 - 0xf0 - 2];
    int   a0f4;                       /* 0xf4 */
};

extern "C" void func_ov006_020c07e8(void *x)
{
    MemModelC *c = (MemModelC *)x;

    port_mg_memory2_field_call(c, c->pmf.code, c->pmf.adj);

    _ZN14BlendModelAnim7AdvanceEv((char *)c + 0x18);

    if (c->a0f4 != 0) {
        MemVector3 t;
        t.x = data_ov006_0212b890.x;
        t.y = data_ov006_0212b890.y;
        t.z = data_ov006_0212b890.z;
        short d;
        int ang;
        ang = Vec3_HorzAngle(&c->v0c8, &t);
        d = (short)(ang - c->a0ea);
        if (d < -0x3000) d = -0x3000;
        else if (d > 0x3000) d = 0x3000;
        _Z11UpdateAngleRssis(&c->a0f0, d, 8, 0x200);

        {
            int *p = (int *)(((char *)c) + 0xb4);
            int *g = data_ov006_0213ac78;
            if (p[0] == g[0]) {
                if (p[1] == g[1])
                    return;
                if (*(int *)((char *)c + 0xb4) == 0)
                    return;
            }
        }
        d = (short)(ang - d);
        _Z11UpdateAngleRssis(&c->a0ea, d, 8, 0x200);
    } else {
        _Z11UpdateAngleRssis(&c->a0f0, 0, 8, 0x200);
    }
}
