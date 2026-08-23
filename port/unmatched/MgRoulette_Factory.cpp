// PORT_HOST_ABI. HOST COPY of dScMgRoulette_c's factory,
// MgMushroomRoulette_Spawn at 0x0210a400 (SpawnInfo data_ov006_0213e2f0, actor
// id 0x17f, scene 383). Run mg11, lane RLT.
//
// ---- WHY A MATCHED, BYTE-EXACT TU IS DISPLACED -----------------------------
//
// src/MgMushroomRoulette_Spawn.cpp is MATCHED and READ-ONLY, and its second
// statement is
//
//     func_ov004_020b2adc();
//
// with NO ARGUMENT. port/unmatched/MgCup_Factory.cpp, which is the same repair
// for dScMgCup_c, says of that defect: "Exactly TWO drop it -- this one and
// src/MgMushroomRoulette_Spawn.cpp (id 0x17f, not seated)". THIS IS THE OTHER
// ONE, and with it the family's dropped-receiver factory set is closed. Every
// other minigame factory in the ROM spells `func_ov004_020b2adc(p)`.
//
// THE ROM, disassembled out of extracted/overlays/overlay_0006.bin at base
// 0x020bfec0 (size 0xac from config/arm9/overlays/ov006/symbols.txt; 35
// instructions plus an 8-word literal pool = 43 words = 0xac):
//
//     0210a400  push {r4, r5, lr} / sub sp, sp, #4
//     0210a408  mov  r0, #0x5400            the object size
//     0210a40c  bl   #0x2043444             ActorBase::operator new
//     0210a410  movs r4, r0                 r4 = p, AND r0 STILL HOLDS p
//     0210a414  beq  #0x210a47c             the null check
//     0210a418  bl   #0x20b2adc             <-- the base constructor, r0 = p
//     0210a41c  ldr r0,[pc,#0x68]           pool 0210a48c = 0x0000471c
//     0210a420  ldr r1,[pc,#0x68]           pool 0210a490 = 0x0213e448
//     0210a424  add r0,r4,r0 / str r1,[r4] / bl #0x2023204   SysTracker ctor
//     0210a430  ldr r0,[pc,#0x5c]           pool 0210a494 = 0x00004f38
//     0210a434  ldr r1,[pc,#0x5c]           pool 0210a498 = 0x0213e39c
//     0210a438  add r0,r4,r0 / str r1,[r4] / bl #0x20c1d80
//     0210a444  ldr r0,[pc,#0x50]           pool 0210a49c = 0x000051a8
//     0210a448  ldr r1,[pc,#0x50]           pool 0210a4a0 = 0x021079c8  -> [sp]
//     0210a44c  add r0,r4,r0 / str r1,[sp]
//     0210a454  mov r1,#5 / mov r2,#0x34
//     0210a45c  ldr r3,[pc,#0x40]           pool 0210a4a4 = 0x0210a4ac  -> r3
//     0210a460  bl  #0x20733a8              the array construction
//     0210a464  ldr r0,[pc,#0x3c]           pool 0210a4a8 = 0x0000530c
//     0210a468  add r5,r4,r0
//     0210a46c  add r0,r5,#0x10 / bl #0x2016d58     Model::Model
//     0210a474  add r0,r5,#0x60 / bl #0x2016d58     Model::Model
//     0210a47c  mov r0,r4 / add sp,sp,#4 / pop {r4,r5,lr} / bx lr
//
// On ARM `movs r4, r0` copies without clearing r0, so the base constructor is
// reached with r0 already holding the object. The instruction that would have
// set it up is absent because it is not needed. The src is a faithful
// transcription of that, and C has no way to say it.
//
// ON THE HOST IT IS A WILD WRITE, NOT A WILD READ. src/func_ov004_020b2adc.c
// dereferences on its FIRST statement -- `_ZN9ActorBaseC1Ev(self)` -- and
// follows it immediately with three vtable stores through the same pointer, so
// a __cdecl call with nothing pushed makes the callee take whatever is at
// [esp+4] and write three words through it. No fault is guaranteed; whatever it
// lands on is corrupted silently, on a machine where "the scene booted clean"
// would still be printed. MgCup_Factory.cpp's header derives that in full and
// this file does not repeat the derivation.
//
// ---- THE DISPLACEMENT RULING -----------------------------------------------
//
// Displacing a matched TU with a host copy costs ONE LINKED FUNCTION:
// port/tools/linkage.py counts src/MgMushroomRoulette_Spawn.cpp as unlinked
// from here on, because this object is what the binary carries. That price is
// paid deliberately and it is the same price, for the same reason, that
// port/mg_fanout_costs.txt section 12 granted lane CUP:
//
//     "The src TU is matched and read-only, so the port-side answer is a host
//      copy of the factory, which costs one linked function and therefore needs
//      a written ruling."
//
// The ruling there was scoped to "whichever lane seats 361" because 361 was the
// only one of the two ids on the board at the time. This lane seats 383, which
// is the other id the same paragraph names, the defect is identical instruction
// for instruction, and the alternatives are the ones MgCup_Factory.cpp already
// weighed and rejected:
//
//   * A FACE that lands the argument. An /alternatename cannot change an
//     argument list, and a __cdecl thunk in front of func_ov004_020b2adc would
//     have to invent the value the caller never pushed -- the value this file
//     holds for free, in `p`.
//   * REPAIRING THE src. src/ is the byte-gated tree; adding an argument
//     changes the compiled bytes and breaks the match. NOTHING IS OWED ON THE
//     DECOMP SIDE: the ROM's own code is correct. What is wrong is that C
//     cannot spell "r0 already holds it".
//   * DOING NOTHING. That is the wild write above.
//
// WHAT CHANGED, exactly, and nothing else moved: `func_ov004_020b2adc();`
// became `func_ov004_020b2adc(p);`. The declaration is spelled the way
// src/func_ov004_020b2adc.c DEFINES it, `void *(char *)`, rather than the way
// the displaced TU declared it, so port/tools/aritycheck.py sees a declaration
// that agrees with the definition instead of one more zero-argument row.
//
// IT DOES NOT RETIRE THE BASELINE ROW, AND AN EARLIER DRAFT OF THIS PARAGRAPH
// SAID IT DID. port/tools/aritycheck_plainfunc_baseline.txt carries
// `func_ov004_020b2adc|src/MgMushroomRoulette_Spawn.cpp`, and the ratchet
// measured on this tree after the seat reads "117 baselined, 117 live, 0 NEW,
// 0 RETIRED". The reason is worth carrying: aritycheck walks src/ TEXT and has
// no opinion about what the linker kept, so displacing a TU cannot retire its
// row -- only correcting the src spelling can, and the src spelling is right
// about the ROM. The row stays as decomp-side debt this file routes around.
//
// THE TWO ARRAY CALLBACKS ARE BOTH `bx lr` AND THAT IS MEASURED, not assumed
// from their size column. func_ov006_0210a4ac (r3, the constructor) and
// func_ov006_021079c8 ([sp], the destructor) are each one word,
// 0xe12fff1e = `bx lr`, in the shipped image. So the five 0x34-byte records at
// +0x51a8 get NO vptr from their element constructor -- word 0 of a record is
// whatever ActorBase::operator new left, and vtable slot 18's
// func_ov006_021092a0 later writes 0xc000 there as a plain fixed-point number.
// That is the mg9 element-vtable check answered for this class by measurement
// rather than by absence: this factory builds no object that carries a mounted
// ROM vtable of its own, and hal/scene_mg_roulette.cpp therefore fills three
// tables and not four.
//
// The object name does not collide with any src stem, which is what
// port/tools/objsrc_check.py asks of a host copy.

extern "C" {

/* the ROM's own callees, each spelled as its own src TU defines it */
void *_ZN9ActorBasenwEj(unsigned int size);
void *func_ov004_020b2adc(char *self);            /* THE REPAIRED ARITY */
void  _ZN8Particle10SysTrackerC1Ev(void *self);
void  func_ov006_020c1d80(char *t);
void  func_020733a8(void *base, int n, int stride,
                    void (*ctor)(void *), void (*dtor)(void *));
void *_ZN5ModelC1Ev(void *self);

/* the two array-element callbacks, both a bare `bx lr` in the ROM. src
   declares them as `extern int name[]` and passes the array; this file
   declares them as the functions they are and casts at the call site, the
   MgCup_Factory.cpp spelling. */
void func_ov006_0210a4ac(void);
void func_ov006_021079c8(void);

/* the two vtables, at the C linkage the ov006 mount defines them with. src
   declares them OUTSIDE its extern "C" block as `extern int name[]`, which is
   the ordinary name-spelling defect this port carries /alternatename rows for;
   host-copying the factory retires both spellings rather than aliasing them. */
extern unsigned char data_ov006_0213e448[];   /* dScMgSingle3DBase_c */
extern unsigned char data_ov006_0213e39c[];   /* dScMgRoulette_c     */

void *MgMushroomRoulette_Spawn(void);

}  /* extern "C" */

extern "C" void *MgMushroomRoulette_Spawn(void)
{
    char *p = (char *)_ZN9ActorBasenwEj(0x5400);
    if (p) {
        /* THE ONE CHANGED LINE. src spells this `func_ov004_020b2adc();`
           because on ARM r0 already holds p; see the header. */
        func_ov004_020b2adc(p);

        *(void **)p = (void *)data_ov006_0213e448;
        _ZN8Particle10SysTrackerC1Ev(p + 0x471c);
        *(void **)p = (void *)data_ov006_0213e39c;
        func_ov006_020c1d80(p + 0x4f38);

        /* r3 is the constructor and [sp] the destructor, read off the ROM's own
           register/stack split at 0x0210a448..0x0210a45c rather than inferred
           from the argument order. */
        func_020733a8(p + 0x51a8, 5, 0x34,
                      (void (*)(void *))func_ov006_0210a4ac,
                      (void (*)(void *))func_ov006_021079c8);

        {
            char *m = p + 0x530c;
            _ZN5ModelC1Ev(m + 0x10);
            _ZN5ModelC1Ev(m + 0x60);
        }
    }
    return p;
}
