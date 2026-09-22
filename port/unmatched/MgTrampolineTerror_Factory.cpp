/* PORT_HOST_ABI.  dScMgTrampoline2_c's FACTORY, dScMgTrampoline2_c_classInit
 * (0x0212471c, 0x18c).  Run mg11, lane TTE.  actor id 0x181 = scene 385.
 *
 * THIS IS NOT AN ABI WORKAROUND.  It is a host copy because the matched-side
 * TU src/minigames/d_s_mg_trampoline2.cpp IS WRONG ABOUT THE ROM, in a way that is
 * invisible to every gate the tree has, and the copy is that file with SIX
 * ARGUMENT PAIRS PUT BACK IN THE ROM'S ORDER and nothing else changed.
 *
 * ---- 1. WHAT IS WRONG, AND HOW IT WAS FOUND -------------------------------
 *
 * The first boot of scene 385 faulted before frame 0:
 *
 *     FAULT code c0000005 at +0xffc00000 accessing 00000000
 *       stack[00] _ZN9ModelBase7SetFileEP8BMD_Fileii  +0x63
 *       stack[06] func_ov006_020ca430                 +0xe6
 *       stack[29] func_ov006_020cae9c                 +0x2c
 *       stack[34] _ZN18dScMgTrampoline2_c13InitResourcesEv                 +0x11a   (vtable slot 0)
 *       stack[50] tte_init                            (the seat's face)
 *
 * -- a call through a null pointer, not a null read: the faulting IP is
 * outside the module.  InitResources walks the five 0xdc-byte records at
 * this+0x500c and hands each one to ModelBase::SetFile, and those records had
 * never been constructed, so their first word (their vtable) was still zero.
 *
 * ---- 2. THE ROM SAYS WHICH ARGUMENT IS WHICH -----------------------------
 *
 * __cxa_vec_ctor is MSL's array-construct helper, and src/__cxa_vec_ctor.cpp is a
 * hand-written asm primitive whose own body settles the question:
 *
 *     movs r4, r3          <- the FOURTH argument is the CONSTRUCTOR
 *     ...
 *     ldr  r0, [r11,#0x38] <- the FIFTH is the destructor, used ONLY by the
 *     ...                     partial-array landing pad
 *     L3f4: mov r0,r8 / blx r4 / add r8,r8,r5 / subs r6,r6,#1 / bne L3f4
 *
 * so `blx r4` -- the only call in the loop -- is r3, the fourth argument.
 *
 * READ OUT OF THE ROM, extracted/overlays/overlay_0006.bin at base 0x020bfec0,
 * with every pool word resolved (the pool runs 0x02124848..0x021248a4):
 *
 *   site      base    n     stride  r3 = CTOR    [sp] = DTOR
 *   --------  ------  ----  ------  -----------  -----------
 *   02124798  0x500c     5   0xdc   0x020ca78c   0x020ca604
 *   021247b8  0x5458     3  0x32c   0x020d100c   0x020d1008
 *   021247d8  0x5ddc   0xa  0x1d0   0x021248a8   0x021227c8
 *   021247f8  0x6ffc   0xa   0x24   0x020efc08   0x020eed64
 *   02124818  0x7164  0x14   0x78   0x02122c90   0x02122c68
 *   02124838  0x7ad0     5   0x24   0x021225a8   0x02120938
 *
 * src/minigames/d_s_mg_trampoline2.cpp passes those two the OTHER WAY ROUND at ALL
 * SIX SITES.  So on the host every element array is "constructed" by its own
 * destructor and the real constructors are never called.
 *
 * THE TWO BODIES ARE NOT AMBIGUOUS EITHER.  0x020ca78c is a constructor --
 * `bl 0x020cd6f4` (the base's), `str r1,[r4]` with r1 = 0x0213b2e0 (the
 * vtable), then `bl 0x02016958` on r4+0x78 (the ModelAnim's constructor) --
 * and 0x020ca604 is a destructor: fourteen _ZN13SharedFilePtr7ReleaseEv calls
 * on GLOBAL SharedFilePtrs, thirteen global pointers zeroed, `bl 0x0201691c`
 * on r4+0x78, and the vtable walked back down.  This class's own D0
 * (src/minigames/d_s_mg_trampoline2.cpp, vtable slot 17) confirms the polarity from the
 * other end: it calls __cxa_vec_cleanup with 0x020ca604, 0x020d1008, 0x021227c8,
 * 0x020eed64, 0x02122c68 and 0x02120938 -- exactly the DTOR column above, and
 * that body was disassembled and ruled REAL_DECOMP by this lane.
 *
 * ---- 3. THE BANNER ON THAT FILE IS FALSE AND THE CORRECTION IS ROUTED -----
 *
 * src/minigames/d_s_mg_trampoline2.cpp's header reads
 *
 *     // NONMATCHING: register allocation (div=5). Logic verified correct vs
 *     // ROM; not byte-matchable from C at mwccarm 1.2/sp2p3.
 *     // Counts as decompiled, not matched.
 *
 * and "logic verified correct vs ROM" is not true of the six pairs above.
 * This is the banner-integrity class run mg9 lane BOX found in
 * src/actors/dScMgSound_c.cpp, in a file with NO delinks block, so no byte gate
 * has ever had an opinion about it.  ROUTED to the decomp side: the six
 * argument pairs want swapping and the banner wants its claim narrowed.  The
 * port does not edit the byte-verified tree from a port lane.
 *
 * ---- 4. WHAT ELSE THE COPY CHANGES: NOTHING ------------------------------
 *
 * Every other statement is src's, in src's order, and each was checked against
 * the disassembly: the 0x7bac allocation (pool 0x0212484c), _ZN11dScMgBase_cC2Ev
 * WITH its argument (so this class needs no displacement ruling -- section 12
 * of port/mg_fanout_costs.txt grants 0x169 one because ITS factory calls that
 * body with none), the _ZTV14dScMgD3DBase_c write (pool 0x02124850 =
 * 0x0213c62c), the +0x4664 zero, the 0xbc-stride walk from +0x466c to +0x47e4
 * (pools 0x02124854 / 0x02124858), Particle::SysTracker's constructor on
 * +0x47e4, and the _ZTV18dScMgTrampoline2_c write (pool 0x0212485c) that makes the
 * object a dScMgTrampoline2_c.  The order of the six sites is the ROM's.
 *
 * src/minigames/d_s_mg_trampoline2.cpp is therefore OUT of port/slice_tte.txt:
 * listing it would be an LNK2005 against this definition.  This file takes the
 * ROM's own symbol name, which is the MgCup_Factory.cpp precedent
 * (port/unmatched/MgCup_Factory.cpp defines dScMgCup_c_classInit and
 * port/slice_cup.txt drops the src line).
 */

extern "C" {

void *_ZN7fBase_cnwEj(unsigned int sz);
void  _ZN11dScMgBase_cC2Ev(void *p);
void  _ZN8Particle10SysTrackerC1Ev(void *p);
void  __cxa_vec_ctor(void *base, int n, int stride, void *ctor, void *dtor);

/* the six CONSTRUCTORS, in the ROM's r3 column */
void func_ov006_020ca78c(void);   /* 0x500c, symbolised _ZN6Player29Try... */
int *_ZN6Player29TryExitCharacterDoorWithIntroEv(int *);  /* matches the src definition; address-only use below */
void func_ov006_020d100c(void);   /* 0x5458 */
void func_ov006_021248a8(void);   /* 0x5ddc */
void func_ov006_020efc08(void);   /* 0x6ffc */
void func_ov006_02122c90(void);   /* 0x7164 */
void func_ov006_021225a8(void);   /* 0x7ad0 */

/* the six DESTRUCTORS, in the ROM's [sp] column */
void func_ov006_020ca604(void);
void func_ov006_020d1008(void);
void func_ov006_021227c8(void);
void func_ov006_020eed64(void);
void func_ov006_02122c68(void);
void func_ov006_02120938(void);

extern void *_ZTV14dScMgD3DBase_c;
extern void *_ZTV18dScMgTrampoline2_c;

void *dScMgTrampoline2_c_classInit(void);

}  /* extern "C" */

/* src's own trivial element type for the +0x466c..+0x47e4 walk, unchanged. */
struct TteTrivial { char pad[0xbc]; TteTrivial() {} };

/* 0x020ca78c has no func_ov006_ name in config/arm9/overlays/ov006/symbols.txt:
   line 221 calls it _ZN6Player29TryExitCharacterDoorWithIntroEv, which is a
   naming-pass label on a body that is plainly this element type's constructor
   (base ctor, vtable 0x0213b2e0, ModelAnim ctor at +0x78).  The name is used as
   the ROM spells it rather than invented; the mislabel is part of the routed
   decomp-side correction. */

extern "C" void *dScMgTrampoline2_c_classInit(void)
{
    char *c = (char *)_ZN7fBase_cnwEj(0x7bac);
    if (c) {
        _ZN11dScMgBase_cC2Ev(c);
        *(void **)c = &_ZTV14dScMgD3DBase_c;
        *(short *)(c + 0x4664) = 0;
        {
            TteTrivial *p   = (TteTrivial *)(c + 0x466c);
            TteTrivial *end = (TteTrivial *)(c + 0x47e4);
            do { p++; } while (p != end);
        }
        _ZN8Particle10SysTrackerC1Ev(c + 0x47e4);
        *(void **)c = &_ZTV18dScMgTrampoline2_c;

        /* THE DELTA IS HERE AND ONLY HERE: ctor fourth, dtor fifth. */
        __cxa_vec_ctor(c + 0x500c, 5,    0xdc,
                      (void *)_ZN6Player29TryExitCharacterDoorWithIntroEv,
                      (void *)func_ov006_020ca604);
        __cxa_vec_ctor(c + 0x5458, 3,    0x32c,
                      (void *)func_ov006_020d100c, (void *)func_ov006_020d1008);
        __cxa_vec_ctor(c + 0x5ddc, 0xa,  0x1d0,
                      (void *)func_ov006_021248a8, (void *)func_ov006_021227c8);
        __cxa_vec_ctor(c + 0x6ffc, 0xa,  0x24,
                      (void *)func_ov006_020efc08, (void *)func_ov006_020eed64);
        __cxa_vec_ctor(c + 0x7164, 0x14, 0x78,
                      (void *)func_ov006_02122c90, (void *)func_ov006_02122c68);
        __cxa_vec_ctor(c + 0x7ad0, 5,    0x24,
                      (void *)func_ov006_021225a8, (void *)func_ov006_02120938);
    }
    return c;
}
