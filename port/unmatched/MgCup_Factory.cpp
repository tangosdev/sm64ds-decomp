// PORT_HOST_ABI. HOST COPY of dScMgCup_c's factory, dScMgCup_c_classInit
// (SpawnInfo g_profile_MG_CUP, actor id 0x169, scene 361). Run mg9, lane
// CUP.
//
// ---- WHY A MATCHED, BYTE-EXACT TU IS DISPLACED -----------------------------
//
// src/actors/dScMgCup_c.cpp is MATCHED and it is READ-ONLY, and its second
// statement is
//
//     _ZN11dScMgBase_cC2Ev();
//
// with NO ARGUMENT. Every other minigame factory in the ROM passes the object:
// dScMgMemory2_c_classInit.c, dScMgPachinko_c_classInit.c, dScMgPachinko2_c_classInit.c,
// dScMgCoin_c_classInit.cpp and the rest all spell `_ZN11dScMgBase_cC2Ev(p)`.
// Exactly TWO drop it -- this one and src/minigames/d_s_mg_roulette.cpp (id
// 0x17f, not seated) -- and the src is right about the ROM both times.
//
// THE ROM, disassembled out of extracted/overlays/overlay_0006.bin at base
// 0x020bfec0 (size 0xc0 from config/arm9/overlays/ov006/symbols.txt):
//
//     020e0574  push {r4, lr} / sub sp, sp, #8
//     020e057c  ldr  r0, [pc, #0x84]     0x5470, the object size
//     020e0580  bl   #0x2043444          ActorBase::operator new
//     020e0584  movs r4, r0              r4 = p, AND r0 STILL HOLDS p
//     020e0588  beq  #0x20e05f8          the null check
//     020e058c  bl   #0x20b2adc          <-- the base constructor, r0 = p
//     020e0590  ldr r0,[pc,#0x74] / ldr r1,[pc,#0x74]      0x471c / 0x0213e448
//     020e0598  add r0,r4,r0 / str r1,[r4] / bl #0x2023204 SysTracker ctor
//     020e05a4  ldr r0,[pc,#0x68] / ldr r1,[pc,#0x68]      0x4f38 / 0x0213c154
//     020e05ac  add r0,r4,r0 / str r1,[r4] / bl #0x20c33dc
//     020e05b8..020e05f4  the two __cxa_vec_ctor array constructions
//     020e05f8  mov r0, r4 / add sp, sp, #8 / pop {r4, lr} / bx lr
//
// On ARM `movs r4, r0` copies without clearing r0, so the base constructor is
// reached with r0 already holding the object. The instruction that would have
// set it up is absent because it is not needed. The src is a faithful
// transcription of that.
//
// ON THE HOST IT IS A WILD WRITE, NOT A WILD READ, which is lane MAR1's
// verification and is why this cannot be left alone. src/_ZN11dScMgBase_cC2Ev.cpp
// is
//
//     void *_ZN11dScMgBase_cC2Ev(char *self)
//     {
//         _ZN7fBase_cC2Ev(self);              <-- dereferences immediately
//         *(void **)self = &data_0208e4b8;
//         *(void **)self = &_ZTV8dScene_c;
//         ...
//
// so a __cdecl call with nothing pushed makes the callee read whatever is at
// [esp+4] and then STORE THREE VTABLE WORDS THROUGH IT. A wild write to an
// uninitialised value faults only by luck; the likely symptom is silent
// corruption of whatever the stack slot happened to point at, on a machine
// where "the scene booted clean" would still be printed.
//
// ---- THE DISPLACEMENT RULING -----------------------------------------------
//
// Displacing a matched TU with a host copy costs ONE LINKED FUNCTION: the
// linkage headline counts src/actors/dScMgCup_c.cpp as unlinked from here
// on, because this object is what the binary carries. That is a real, measured
// price and it is why this port does not take it casually.
//
// THE RULING IS GRANTED AND WRITTEN DOWN. port/mg_fanout_costs.txt section 12
// records it: "The src TU is matched and read-only, so the port-side answer is
// a host copy of the factory, which costs one linked function and therefore
// needs a written ruling. Per the coordinator, the displacement ruling will be
// granted to whichever lane seats 361." port/slice_mga361.txt section 5 says
// the same. This lane seats 361 and this file is the ruling being spent.
//
// THE ALTERNATIVES WERE CONSIDERED AND ARE WORSE:
//
//   * A FACE that lands the argument. An /alternatename cannot change an
//     argument list, and a __cdecl thunk in front of _ZN11dScMgBase_cC2Ev would
//     have to invent the value the caller never pushed -- the same value this
//     file has for free, because it holds `p`.
//   * REPAIRING THE src. src/ is the byte-gated tree: adding an argument
//     changes the compiled bytes and breaks the very match that makes the TU
//     worth having. The DECOMP-side observation is worth recording and is NOT
//     taken here: the ROM's own code is correct, so there is nothing to fix in
//     src -- what is wrong is that C cannot spell "r0 already holds it".
//   * DOING NOTHING. That is the wild write above.
//
// WHAT CHANGED, exactly, and nothing else moved: `_ZN11dScMgBase_cC2Ev();`
// became `_ZN11dScMgBase_cC2Ev(p);`. The declaration is spelled the way
// src/_ZN11dScMgBase_cC2Ev.cpp DEFINES it, `void *(char *)`, rather than the way
// the displaced TU declared it, so port/tools/aritycheck.py sees a declaration
// that agrees with the definition instead of one more zero-argument row.
//
// The object name does not collide with any src stem, which is what
// port/tools/objsrc_check.py asks of a host copy: the headline drops by the one
// function this displaces and by nothing else.

extern "C" {

/* the ROM's own callees, each spelled as its own src TU defines it */
void *_ZN7fBase_cnwEj(unsigned int size);
void *_ZN11dScMgBase_cC2Ev(char *self);
void *_ZN8Particle10SysTrackerC1Ev(void *self);
int   func_ov006_020c33dc(char *t);
void  __cxa_vec_ctor(void *base, int n, int stride,
                    void (*ctor)(void *), void (*dtor)(void *));

/* the two array-element callbacks the factory hands __cxa_vec_ctor. Both are
   (void) in src because both ROM bodies ignore r0: func_ov006_020e0634 is
   0x020e0634 = e12fff1e, a bare `bx lr`, and NullDestructor_0203d47c is the
   arm9 null destructor. Cast at the call sites the way src casts them. */
void func_ov006_020e0634(void);
void func_ov006_020deac4(void);
void func_0203d738(void);
void NullDestructor_0203d47c(void);

/* the two vtables, at the C linkage the mount defines them with. src declared
   them OUTSIDE its extern "C" block, which is where port/slice_mga361.txt's
   "2 NAME-SPELLING FACES" frontier item came from; host-copying the factory
   retires that item rather than aliasing around it. */
extern unsigned char _ZTV19dScMgSingle3DBase_c[];   /* dScMgSingle3DBase_c */
extern unsigned char _ZTV10dScMgCup_c[];   /* dScMgCup_c          */

void *dScMgCup_c_classInit(void);

}  /* extern "C" */

// PORT_HOST_ABI: src spells the base constructor call _ZN11dScMgBase_cC2Ev() with no argument because on ARM r0 already holds the object, a register ride-through C cannot spell; the host passes p explicitly so the callee does not store three vtable words through an uninitialised stack slot.
extern "C" void *dScMgCup_c_classInit(void)
{
    char *p = (char *)_ZN7fBase_cnwEj(0x5470);
    if (p) {
        /* THE ONE CHANGED LINE. src spells this `_ZN11dScMgBase_cC2Ev();`
           because on ARM r0 already holds p; see the header. */
        _ZN11dScMgBase_cC2Ev(p);

        *(void **)p = (void *)_ZTV19dScMgSingle3DBase_c;
        _ZN8Particle10SysTrackerC1Ev(p + 0x471c);
        *(void **)p = (void *)_ZTV10dScMgCup_c;
        func_ov006_020c33dc(p + 0x4f38);

        /* pool 0x020e0624 = 020e0634 (ctor) and 0x020e0620 = 020deac4 (dtor);
           pool 0x020e0630 = 0203d738 (ctor) and 0x020e0628 = 0203d47c (dtor).
           Both orders read off the ROM's r3-versus-[sp] split, not inferred. */
        __cxa_vec_ctor(p + 0x50e8, 0x20, 0x18,
                      (void (*)(void *))func_ov006_020e0634,
                      (void (*)(void *))func_ov006_020deac4);
        __cxa_vec_ctor(p + 0x53e8, 3, 8,
                      (void (*)(void *))func_0203d738,
                      (void (*)(void *))NullDestructor_0203d47c);
    }
    return p;
}
