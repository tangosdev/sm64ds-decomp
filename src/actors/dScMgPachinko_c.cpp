//cpp
/* dScMgPachinko_c -- the Bob-omb Squad / pachinko minigame scene, ov006.
 *
 * Reconstructed translation unit: ROM ordinals 0..44 of the linker run
 * 0x020fa75c..0x020ff420, i.e. .text 0x020fa75c..0x020fc8c0, 45 functions,
 * 0x2164 bytes.  Assembled from the 45 one-function legacy sources, then
 * reconciled by hand.
 *
 * THE RUN, AND HOW ITS TWO ENDS WERE FIXED.  config/arm9/overlays/ov006/
 * symbols.txt was sorted by ADDRESS rather than grepped for the class name,
 * because an anchored grep cannot see a second class interleaved into a
 * range.  Sorted, the 76 functions from 0x020fa75c to 0x020ff420 carry
 * exactly six named symbols and all six are this class's own: D1, D0,
 * OnYoshiTryEat, Render, Behavior, InitResources, plus the factory
 * dScMgPachinko_c_classInit.  Every other entry is an unlabelled
 * func_ov006_*.  No other class interleaves.  The function immediately BELOW
 * the run is _ZN19dMgMCarlo2CardObj_cC1Ev, which belongs to the dScMgMCarlo2_c
 * block, and the function immediately ABOVE it is _ZN16dScMgPachinko2_cD1Ev,
 * the first symbol of the sibling dScMgPachinko2_c.  Both ends are pinned by
 * a different class, and the 76 functions are address-adjacent with no gaps:
 * every function's end is the next one's start.
 *
 * WHY THIS IS A SUB-RANGE -- THREE UNENROLLED HOLES SPLIT THE RUN.  A
 * licensed .text claim cannot span a range the ROM build does not compile,
 * and no file entry in any delinks.txt in this tree expresses more than one
 * .text claim, so the run can only be licensed as one of its contiguous
 * pieces.  Three of the 76 are not enrolled:
 *
 *   ROM ordinal 45, func_ov006_020fc8c0 (0x020fc8c0, size 0xf0) -- an honest
 *     non-matching draft, 6 divergent words of 60, whose own banner records
 *     the residue class.  It has NO entry in config/arm9/overlays/ov006/
 *     delinks.txt at all; the blocks jump from 0x020fc844-0x020fc8c0 straight
 *     to 0x020fc9b0.
 *   ROM ordinal 48, func_ov006_020fcb4c (0x020fcb4c, size 0x240) -- HAS a
 *     delinks entry for 0x020fcb4c-0x020fcd8c, but the entry carries no
 *     `complete` marker, so dsd supplies that range from the cartridge's own
 *     bytes and the source is never compiled.
 *   ROM ordinal 58, func_ov006_020fdaf0 (0x020fdaf0, size 0x250) -- again no
 *     delinks entry at all; the blocks jump from 0x020fda7c-0x020fdaf0
 *     straight to 0x020fdd40.
 *
 * The three holes cut the run into four contiguous enrolled stretches:
 *   ordinals  0..44  0x020fa75c..0x020fc8c0  45 functions  8548 bytes  <- THIS FILE
 *   ordinals 46..47  0x020fc9b0..0x020fcb4c   2 functions   412 bytes
 *   ordinals 49..57  0x020fcd8c..0x020fdaf0   9 functions  3428 bytes
 *   ordinals 59..75  0x020fdd40..0x020ff420  17 functions  5856 bytes
 *
 * THIS FILE IS THE LOWEST AND LARGEST STRETCH, 45 functions against 17 on the
 * next largest, and larger by bytes as well.  It is also the side that holds
 * the class's key function: the destructor, which include/dScMgPachinko_c.h
 * declares out of line and declares FIRST.  The 31 members above the holes --
 * including InitResources, Behavior, Render, OnYoshiTryEat and
 * dScMgPachinko_c_classInit -- keep their own shards and their own
 * delinks.txt entries and are untouched by this work.  The queue's
 * shard count of 74 is an upper bound on the unit, not a promise about any
 * one claim; tools/tubuild.py list independently reports this unit as
 * `72/75 ... unmatched:3`, the same three holes.
 *
 * VTABLE AND TYPEINFO.  Because the out-of-line destructor is declared first,
 * this TU owns the class's key function, so mwcc emits the whole inheritance
 * chain's vtable and typeinfo as vague-linkage passengers.  The cartridge
 * keeps its own canonical copies elsewhere in ov006 and in ov004/arm9, all
 * outside this TU's only licensed range, so production isolation discards the
 * object's copies without costing the image a byte -- see the manifest's
 * compiler_only_output block for each symbol and its canonical address.
 * _ZTV15dScMgPachinko_c, _ZTI15dScMgPachinko_c and _ZTS15dScMgPachinko_c each
 * have exactly one row in this overlay's symbols.txt, so dScMgPachinko_c is a
 * real ROM RTTI name and nothing here is a coined English placeholder.
 *
 * THE DESTRUCTOR PAIR.  The cartridge orders D1 (0x020fa75c, size 0x24) below
 * D0 (0x020fa780, size 0x38), adjacent, with no room between them for a D2 --
 * 0x020fa780 - 0x020fa75c = 0x24, exactly D1's size -- and there is no
 * _ZN15dScMgPachinko_cD2Ev anywhere in the image.  One out-of-line definition
 * therefore has to produce D1 then D0 and nothing in between, which is why
 * the two legacy shards that each spelled the same `~dScMgPachinko_c()` body
 * collapse into the single definition below carrying both symbol markers.
 *
 * FUNCTION ORDER IS THE ROM'S OWN, LOWEST ADDRESS FIRST, and that is one
 * decision with `#pragma defer_codegen off` below.  With codegen deferred
 * (the default) mwccarm 2004/b56 emits one .text section per function in the
 * REVERSE of source order; generating at parse time emits them in source
 * order.  Do not reorder.
 *
 * STATUS: BANKED AS text-verified, NOT PROMOTED, and the reason is the gate
 * rather than this TU.  tools/tubuild.py linkcheck reports
 * SCRATCH-LINK-VERIFIED: the whole licensed .text range reproduces from one
 * object, all 106 modules are byte-exact, dsd check symbols --fail shows 0 NEW
 * errors against the baseline control's 9 pre-existing ones, and the full ROM
 * builds identical to stock.  `tubuild promote` nevertheless has no mutating
 * path at all -- only --dry-run is implemented -- and its readiness gate
 * refuses this state: it wants status in (link-verified, data-verified) AND
 * linkcheck.result == status, while classify_link_result() prefixes the result
 * with `scratch-` for every TU that emits its own _ZTV/_ZTI/_ZTS.  The status
 * is left at exactly what the tooling reported rather than hand-edited to
 * clear the gate, so THIS FILE IS NOT YET ENROLLED: the 45 legacy shards listed
 * below keep their own delinks.txt entries and the ROM build is unchanged by
 * the commit that adds this file.  See the manifest's notes for the detail.
 *
 * Assembled from these legacy one-function sources (ROM address order); this
 * banner names none of their paths outside this list:
 *   [0]  0x020fa75c  the legacy dScMgPachinko_c D1 shard
 *   [1]  0x020fa780  the legacy dScMgPachinko_c D0 shard
 *   [2]  0x020fa7b8  the legacy func_ov006_020fa7b8 shard
 *   [3]  0x020fa844  the legacy func_ov006_020fa844 shard
 *   [4]  0x020fa924  the legacy func_ov006_020fa924 shard
 *   [5]  0x020fa9a0  the legacy func_ov006_020fa9a0 shard
 *   [6]  0x020fa9c8  the legacy func_ov006_020fa9c8 shard
 *   [7]  0x020faac8  the legacy func_ov006_020faac8 shard
 *   [8]  0x020fab70  the legacy func_ov006_020fab70 shard
 *   [9]  0x020fac48  the legacy func_ov006_020fac48 shard
 *   [10] 0x020fad34  the legacy func_ov006_020fad34 shard
 *   [11] 0x020fad90  the legacy func_ov006_020fad90 shard
 *   [12] 0x020fadfc  the legacy func_ov006_020fadfc shard
 *   [13] 0x020fae20  the legacy func_ov006_020fae20 shard
 *   [14] 0x020fae90  the legacy func_ov006_020fae90 shard
 *   [15] 0x020faeec  the legacy func_ov006_020faeec shard
 *   [16] 0x020faf14  the legacy func_ov006_020faf14 shard
 *   [17] 0x020faf6c  the legacy func_ov006_020faf6c shard
 *   [18] 0x020fb0fc  the legacy func_ov006_020fb0fc shard
 *   [19] 0x020fb1c4  the legacy func_ov006_020fb1c4 shard
 *   [20] 0x020fb230  the legacy func_ov006_020fb230 shard
 *   [21] 0x020fb45c  the legacy func_ov006_020fb45c shard
 *   [22] 0x020fb4e0  the legacy func_ov006_020fb4e0 shard
 *   [23] 0x020fb60c  the legacy func_ov006_020fb60c shard
 *   [24] 0x020fb670  the legacy func_ov006_020fb670 shard
 *   [25] 0x020fb74c  the legacy func_ov006_020fb74c shard
 *   [26] 0x020fb7e0  the legacy func_ov006_020fb7e0 shard
 *   [27] 0x020fb8fc  the legacy func_ov006_020fb8fc shard
 *   [28] 0x020fb97c  the legacy func_ov006_020fb97c shard
 *   [29] 0x020fba28  the legacy func_ov006_020fba28 shard
 *   [30] 0x020fba48  the legacy func_ov006_020fba48 shard
 *   [31] 0x020fba64  the legacy func_ov006_020fba64 shard
 *   [32] 0x020fbad4  the legacy func_ov006_020fbad4 shard
 *   [33] 0x020fbb2c  the legacy func_ov006_020fbb2c shard
 *   [34] 0x020fbbe8  the legacy func_ov006_020fbbe8 shard
 *   [35] 0x020fbcb8  the legacy func_ov006_020fbcb8 shard
 *   [36] 0x020fbd38  the legacy func_ov006_020fbd38 shard
 *   [37] 0x020fc144  the legacy func_ov006_020fc144 shard
 *   [38] 0x020fc1b4  the legacy func_ov006_020fc1b4 shard
 *   [39] 0x020fc1f8  the legacy func_ov006_020fc1f8 shard
 *   [40] 0x020fc2ec  the legacy func_ov006_020fc2ec shard
 *   [41] 0x020fc500  the legacy func_ov006_020fc500 shard
 *   [42] 0x020fc718  the legacy func_ov006_020fc718 shard
 *   [43] 0x020fc7d0  the legacy func_ov006_020fc7d0 shard
 *   [44] 0x020fc844  the legacy func_ov006_020fc844 shard
 */

/* STILL MACHINE-SHAPED -- byte-exact, but what blocks each part:
 *  43 func_ov006_* + 24 data_*  unnamed in config symbols.txt; each needs a
 *                              coined, behaviour-justified name.
 *  2 pointer-to-member shims    two members dispatch through the ov006
 *                              pointer-to-member tables via a local shadow
 *                              class rather than a named interface.
 *  ~600 *(T *)(p + 0x..)        class layout does not name these offsets; the
 *                              header names mBall/mShot and little else.
 */

#pragma defer_codegen off

#include "types.h"
#include "dScMgPachinko_c.h"

/* ---------------------------------------------------------------------------
 * Shadow types.  Two members dispatch through the ov006 pointer-to-member
 * tables, and each legacy shard spelled the receiver as its own local
 * `struct C`.  The two spellings place their fields at DIFFERENT offsets
 * (0x5c0e/0x5c0f against 0x5bc6/0x5bc8), so they are two different shadow
 * classes that happened to share a name; the second is renamed here and that
 * rename is the only one in the merge, so every call expression is otherwise
 * textually what it was.
 *
 * In both, the pointer-to-member typedef is formed while the class is still
 * INCOMPLETE, and that is load-bearing: mwccarm 2004/b56 picks its
 * pointer-to-member representation from the completeness of the class at the
 * point the type is formed, so completing either one earlier would change the
 * codegen of the member that uses it.  Kept exactly as the shards had it.
 * ------------------------------------------------------------------------- */
struct C;
typedef void (C::*PMF)(int);
struct Entry { PMF pmf; };
struct C { char pad[0x5c0e]; unsigned char guard; unsigned char idx; };

struct C2;
typedef void (C2::*PMF2)(int);
struct C2 { char pad[0x5bc6]; unsigned char g; char gap; unsigned char idx; };

/* ROM ordinal 21's typed view of the 0x5bb4 array.  Only that member uses the
   typed spelling; every other member reaches the same bytes through raw
   offsets, and that difference is what the ROM has.  `volatile` on the
   position word is load-bearing -- the ROM reloads it after the store. */
typedef struct Ent {
    volatile int a; /* 0x00 position (20.12) */
    int unk4;       /* 0x04 */
    int b;          /* 0x08 velocity (20.12) */
    char _padC[0x10];
} Ent;

typedef struct Obj {
    char _pad0[0x5bb4]; /* 0x0000 */
    Ent entries[8];     /* 0x5bb4 */
} Obj;

/* ROM ordinal 43's 8-byte {code pointer, adjustment} records.  A second shard
   had also called its record type `Ent`; this is the renamed one. */
struct Ent7d0 { int a; int b; };

/* ROM ordinal 22's stride-only view of the same 0x1c-byte rows. */
struct E { char pad[0x1c]; };

/* Launder macros, each kept verbatim from the shard that needed it: they force
   mwccarm to materialise a base address independently instead of folding it
   into a neighbouring add.  See each member's own note. */
#define AT(p, off) ((void*)(int)((char*)(p) + (off)))
#define M1(a) (a)
#define M2(a) (a)
#define AC (*(int *)(((long long)((int)a + 0x5c14))))

/* ROM ordinal 40's typed row accessors. */
#define ST(c,b)       (*(u8*)((c) + 0x4eb4 + (b)))
#define CTA(c,b)      (*(u16*)((c) + 0x4eb0 + (b)))
#define FB(c,b)       (*(u8*)((c) + 0x4eb6 + (b)))
#define OUTFLAG(c,b)  (*(u8*)((c) + 0x4eb5 + (b)))
#define OUTFLAG2(c,b) (*(u8*)((c) + 0x4eb3 + (b)))
#define ACC(c,b)      (*(s32*)((c) + 0x4ea0 + (b)))
#define VEL(c,b)      (*(s32*)((c) + 0x4ea8 + (b)))

/* ---------------------------------------------------------------------------
 * ROM symbols this TU calls or reads, plus the forward declarations the
 * members need of each other.  ONE file-scope `extern "C"` region: a class
 * member function cannot sit inside an `extern "C" { }` block at all under
 * mwccarm 2004/b56, and a mangled ROM spelling outside one would be mangled a
 * second time -- a LINK failure that every byte gate passes.
 *
 * Where two shards spelled one interface differently the more complete
 * observation wins and the narrower site takes the codegen-free conversion:
 *   - func_ov004_020afdd0 takes `void *` first (the tree's own plurality, 17
 *     sites against 7) and returns void; three shards had spelled it
 *     `int`-first and one of those had also declared an `int` return whose
 *     result it discarded, so those three call sites now cast.
 *   - _ZN3G2S13GetBG2CharPtrEv returns `unsigned` (the tree's plurality, 15
 *     sites); the one shard that had declared it `char *` now casts.
 *   - MultiStore16 takes `char *` (the tree's plurality); the one `void *`
 *     call site now spells its cast `char *`.
 *   - func_02012468's eighth parameter is `short` (the tree's plurality, 11
 *     sites against 5); both call sites here pass a literal 0.
 *   - func_ov006_020fa844, func_ov006_020faf6c and func_ov006_020fb230 keep
 *     their own definitions' `char *` parameter; the shadow-class call sites
 *     that had re-declared them with a local struct pointer now cast.
 *   - func_ov006_020fbcb8 takes `void *` for its FIRST parameter, which its
 *     definition never reads, so the eight call sites keep passing a `char *`
 *     and convert implicitly rather than each taking an `(int)` cast.
 * ------------------------------------------------------------------------- */
extern "C" {

/* --- shared ov004 / main / library helpers --- */
extern void func_ov004_020afdd0(void *a0, int a1, int a2, int a3, int a4);
extern void func_ov004_020b2444(int x, int y, int h, int a, int b, int c, int d);
extern int  func_ov004_020adbc0(void);
extern int  func_ov004_020b1a5c(int a, int b);
extern s8   func_ov004_020adc1c(s8 levelID);
extern void func_ov004_020b19f0(s8 courseID);
extern void func_ov004_020adb1c(int self);
extern int  func_020126e8(int a);
extern int  func_02012468(int a, int b, int c, int d, int e, int f, int g, short h);
extern void SetBg2Offset(int a, int b);
extern void MultiStore16(unsigned short val, char *dst, int nbytes);
extern int  RandomIntInternal(int *seed);
extern unsigned _ZN3G2S13GetBG2CharPtrEv(void);
extern int  _ZN4cstd4sqrtEy(unsigned long long val);
extern void _ZN5Sound12PlayBank2_2DEj(unsigned int id);
extern int  _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(int, void*, int, int, int, int, int, void*, int, int);

/* --- ROM data this TU reads --- */
extern int   data_0209d4b8;
extern unsigned char data_0209d45c;
extern void *data_ov006_0213ac24;
extern unsigned char data_ov006_0212eb0c[];
extern unsigned char data_ov006_0212eb10[];
extern unsigned char data_ov006_0212eb14[];
extern unsigned char data_ov006_0212eb1c[];
extern int   data_ov006_0212eb24[];
extern int   data_ov006_0212eb2c[];
extern u8    data_ov006_0212eb34[];
extern u8    data_ov006_0212eb3c[];
extern s32   data_ov006_0212eb44[];
extern int   data_ov006_0212eb50[];
extern s32   data_ov006_0212eb60[];
extern int   data_ov006_0212eb70[];
extern int   data_ov006_02133e10[];
extern int   data_ov006_02133e7c[];
extern int   data_ov006_02136b80[];
extern int   data_ov006_02136e0c[];
extern int   data_ov006_021375f4[];

/* The two pointer-to-member tables this stretch dispatches through, and the
   0x8-byte record table ordinal 43 walks.  __sinit_ov006_* fills them from the
   {code pointer, adjustment} records in ov006 .data. */
extern Entry  data_ov006_02142604[];
extern Ent7d0 data_ov006_02142624[];
extern PMF2   data_ov006_0214266c[];

/* Members of this class ABOVE the holes -- still their own shards, still
   their own delinks.txt entries, called from here. */
extern void func_ov006_020fe1a8(char *p);

/* This TU's own members, forward-declared for the callers below them. */
extern void func_ov006_020fa7b8(char *thiz);
extern void func_ov006_020fa844(char *self);
extern void func_ov006_020fa924(char *p);
extern void func_ov006_020fa9a0(char *p);
extern void func_ov006_020fa9c8(char *p, int i);
extern void func_ov006_020faac8(char *c, int i);
extern void func_ov006_020fab70(char *c, int i);
extern void func_ov006_020fac48(char *c, int idx);
extern void func_ov006_020fad34(C *c);
extern void func_ov006_020fad90(char *c);
extern void func_ov006_020fadfc(char *p);
extern void func_ov006_020fae20(char *base);
extern void func_ov006_020fae90(u8 *c);
extern void func_ov006_020faeec(char *p);
extern void func_ov006_020faf14(char *c);
extern void func_ov006_020faf6c(char *c, int idx);
extern void func_ov006_020fb0fc(char *c, int i);
extern void func_ov006_020fb1c4(char *c, int i);
extern void func_ov006_020fb230(char *p, int i);
extern void func_ov006_020fb45c(Obj *self, int i);
extern void func_ov006_020fb4e0(char *c, int idx);
extern void func_ov006_020fb60c(C2 *c);
extern void func_ov006_020fb670(char *obj);
extern void func_ov006_020fb74c(void *base);
extern void func_ov006_020fb7e0(char *thiz);
extern void func_ov006_020fb8fc(char *c, int a2, int a3, int a4, int a5, int a6);
extern void func_ov006_020fb97c(char *c);
extern void func_ov006_020fba28(void);
extern void func_ov006_020fba48(s8 levelID);
extern void func_ov006_020fba64(char *base);
extern void func_ov006_020fbad4(char *c);
extern void func_ov006_020fbb2c(char *c, int idx, unsigned short val);
extern void func_ov006_020fbbe8(char *c);
extern void func_ov006_020fbcb8(void *a0, int a1, int a2, int a3);
extern void func_ov006_020fbd38(void *arg0);
extern void func_ov006_020fc144(char *base);
extern void func_ov006_020fc1b4(char *base, int val);
extern int  func_ov006_020fc1f8(char *self, int idx);
extern void func_ov006_020fc2ec(char *c, int i);
extern void func_ov006_020fc500(char *c, int i);
extern void func_ov006_020fc718(char *thiz, int n);
extern void func_ov006_020fc7d0(char *c);
extern void func_ov006_020fc844(unsigned char *c);

}  /* extern "C" */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 0 -- _ZN15dScMgPachinko_cD1Ev, 0x020fa75c, size 0x24           */
/* ROM ordinal 1 -- _ZN15dScMgPachinko_cD0Ev, 0x020fa780, size 0x38           */
/*                                                                            */
/* The deleting variant has no source of its own: mwccarm emits D1 and D0     */
/* from this one definition, in that order, which is the cartridge's order.   */
/* Both markers are parked here because both ROM symbols come from this body. */
/* No member needs explicit destruction, so the empty body relies entirely on  */
/* the compiler-generated own-vtable-write plus base-D2 call, and the deleting */
/* variant reaches dScMgBase_c's own operator delete.                          */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15dScMgPachinko_cD1Ev
// @symbol _ZN15dScMgPachinko_cD0Ev
dScMgPachinko_c::~dScMgPachinko_c()
{
}

extern "C" {  /* .c-derived members: C linkage for everything below */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- func_ov006_020fa7b8, 0x020fa7b8, size 0x8c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020fa7b8
void func_ov006_020fa7b8(char* thiz)
{
    for (int i = 0; i < 3; i++) {
        if (*(unsigned char*)(thiz + 0x4000 + 0xe6d) != 0) {
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(false, data_ov006_0213ac24,
                *(int*)(thiz + 0x4000 + 0xe58) >> 12,
                *(int*)(thiz + 0x4000 + 0xe5c) >> 12,
                -1, -1, 0x1000, (void*)0x1000,
                *(unsigned short*)(thiz + 0x4e00 + 0x68), -1);
        }
        thiz += 0x18;
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- func_ov006_020fa844, 0x020fa844, size 0xe0 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020fa844
void func_ov006_020fa844(char *self)
{
    int i;
    char *w;

    w = self;
    for (i = 0; i < 3; i++) {
        *(s32 *)(w + 0x4e58) = (data_ov006_0212eb44[i] << 12) - *(s32 *)(self + 0x5bfc);
        *(s32 *)(w + 0x4e5c) = -*(s32 *)(self + 0x5c00);
        if (*(u8 *)(w + 0x4e6e) != 0) {
            *(u16 *)AT(w, 0x4e68) -= 0x40;
            if (*(s16 *)(w + 0x4e68) <= -0x1000) {
                *(u16 *)(w + 0x4e68) = 0xf000;
                *(u8 *)(w + 0x4e6e) = 0;
            }
        } else {
            *(u16 *)AT(w, 0x4e68) += 0x40;
            if (*(s16 *)(w + 0x4e68) >= 0x1000) {
                *(s16 *)(w + 0x4e68) = 0x1000;
                *(u8 *)AT(w, 0x4e6e) += 1;
            }
        }
        w += 0x18;
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- func_ov006_020fa924, 0x020fa924, size 0x7c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020fa924
void func_ov006_020fa924(char *p)
{
    int i;
    char *s = p;
    for (i = 0; i < 3; i++, p += 0x18)
    {
        if (*(unsigned char *)(p + 0x4e6c) == 0)
        {
            *(unsigned char *)(p + 0x4e6c) = 1;
            *(unsigned char *)(p + 0x4e6d) = 1;
            *(int *)(p + 0x4e58) = (data_ov006_0212eb44[i] << 12) - *(int *)(s + 0x5bfc);
            *(int *)(p + 0x4e5c) = -*(int *)(s + 0x5c00);
            *(short *)(p + 0x4e6a) = 0;
            *(short *)(p + 0x4e68) = 0;
            *(unsigned char *)(p + 0x4e6e) = 0;
        }
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- func_ov006_020fa9a0, 0x020fa9a0, size 0x28 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020fa9a0
void func_ov006_020fa9a0(char *p)
{
    int i;
    for (i = 0; i < 3; i++) {
        *(unsigned char *)(p + 0x4e6c) = 0;
        *(unsigned char *)(p + 0x4e6d) = 0;
        p += 0x18;
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- func_ov006_020fa9c8, 0x020fa9c8, size 0x100 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020fa9c8
void func_ov006_020fa9c8(char *p, int i)
{
    int n = i * 0x14;
    int dv;
    *(int *)(p + 0x5bfc + n) += *(int *)(p + 0x5c04 + n);
    *(int *)(p + 0x5c00 + n) += *(int *)(p + 0x5c08 + n);
    if (*(int *)(p + 0x5c04 + n) <= -0x1000)
        *(int *)(p + 0x5c04 + n) += 0x40;
    if (*(int *)(p + 0x5c08 + n) >= 0x3000)
        *(int *)(p + 0x5c08 + n) += 0x100;
    dv = *(int *)(p + 0x5c00 + n) >> 12;
    SetBg2Offset(*(int *)(p + 0x5bfc + n) >> 12, dv);
    if (dv < 0x40) return;
    *(short *)(p + 0x5c0c + n) = 0x258;
    *(unsigned char *)(p + 0x5c0f + n) = 0;
    data_0209d45c &= ~4;
    func_ov006_020fa9a0(p);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- func_ov006_020faac8, 0x020faac8, size 0xa8 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020faac8
void func_ov006_020faac8(char* c, int i)
{
    int* b = (int*)(c + 0x5c04 + i * 0x14);
    int* a = (int*)(c + 0x5bfc + i * 0x14);
    int v;

    *a = *a + *b;

    if (*b <= -0x2000) {
        *b = *b - 0x100;
    }

    v = *a >> 12;
    SetBg2Offset(v, *(int*)(c + i * 0x14 + 0x5c00) >> 12);

    if (v > -0x90) return;

    *(int*)(c + i * 0x14 + 0x5c08) = 0x800;
    *(unsigned char*)(c + i * 0x14 + 0x5c0f) = 3;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- func_ov006_020fab70, 0x020fab70, size 0xd8 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020fab70
void func_ov006_020fab70(char* c, int i)
{
    int off = i * 0x14;
    int* p04;
    int* p08;
    int* pfc;
    int* p00;
    int a;
    int b;

    *(int*)(c + 0x5bfc + off) += *(int*)(c + 0x5c04 + off);
    *(int*)(c + 0x5c00 + off) += *(int*)(c + 0x5c08 + off);
    p04 = (int*)(c + 0x5c04 + off);
    pfc = (int*)(c + 0x5bfc + off);
    p00 = (int*)(c + 0x5c00 + off);
    p08 = (int*)(c + 0x5c08 + off);
    if (*p04 <= -0x2000) *p04 -= 0x100;
    if (*p08 <= -0xc00) *p08 += 0x40;
    a = *p00 >> 12;
    b = *pfc >> 12;
    if (a <= 0) {
        a = 0;
        *p00 = 0;
        *p08 = 0;
        *(unsigned char*)(c + off + 0x5c0f) = 2;
    }
    SetBg2Offset(b, a);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 9 -- func_ov006_020fac48, 0x020fac48, size 0xec */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020fac48
void func_ov006_020fac48(char *c, int idx)
{
    int n;
    unsigned short t;
    char *p_row;
    int neg;
    int b;
    int a;
    int lo;
    int hi;
    unsigned char flag;

    if (*(int *)((c + 0x5000) + 0xc10) == 2)
        return;

    n = idx * 0x14;
    t = *(unsigned short *)((c + 0x5c0c) + n);
    if (t != 0) {
        *(unsigned short *)((c + 0x5c0c) + n) = t - 1;
        return;
    }

    flag = data_0209d45c;
    p_row = c + n;
    neg = 0x1000;
    data_0209d45c = flag | 4;
    *(int *)((c + 0x5bfc) + n) = 0x100000;
    *(int *)((c + 0x5c00) + n) = 0x40000;
    neg = -neg;
    /* Mix (c+n) and p_row so mwccarm colors r4/r5 like the ROM. */
    *(int *)(((c + n) + 0x5000) + 0xc04) = neg;
    *(int *)((p_row + 0x5000) + 0xc08) = neg;
    a = *(int *)((c + 0x5c00) + n);
    b = *(int *)((c + 0x5bfc) + n);
    lo = b >> 12;
    hi = a >> 12;
    *(unsigned char *)(((c + n) + 0x5000) + 0xc0f) = 1;
    *(volatile int *)0x4000018 = (0x1ff & lo) | (0x1ff0000 & (hi << 16));
    func_ov006_020fa924(c);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 10 -- func_ov006_020fad34, 0x020fad34, size 0x5c */
/* Dispatches through the ov006 pointer-to-member table at 0x02142604.        */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020fad34
void func_ov006_020fad34(C* c) {
    if (c->guard) {
        (c->*(data_ov006_02142604[c->idx].pmf))(0);
    }
    func_ov006_020fa844((char *)c);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 11 -- func_ov006_020fad90, 0x020fad90, size 0x6c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020fad90
void func_ov006_020fad90(char *c){
    *(unsigned char*)(c+0x5000+0xc0e) = 1;
    *(int*)(c+0x5000+0xbfc) = 0x100000;
    *(int*)(c+0x5000+0xc00) = 0x40000;
    *(int*)(c+0x5000+0xc04) = 0;
    *(int*)(c+0x5000+0xc08) = 0;
    *(unsigned char*)(c+0x5000+0xc0f) = 0;
    *(short*)(c+0x5c00+0xc) = 0;
    *(volatile int*)0x4000018 =
        (0x1ff & (*(int*)(c+0x5000+0xbfc) >> 12)) |
        (0x1ff0000 & ((*(int*)(c+0x5000+0xc00) >> 12) << 16));
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 12 -- func_ov006_020fadfc, 0x020fadfc, size 0x24 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020fadfc
void func_ov006_020fadfc(char *p)
{
    data_0209d45c &= ~4;
    *(unsigned char *)(p + 0x5c0e) = 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 13 -- func_ov006_020fae20, 0x020fae20, size 0x70 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020fae20
void func_ov006_020fae20(char *base)
{
    int i;
    char *o = base;
    for (i = 0; i < 4; i++) {
        if (*(unsigned char *)(o + 0x5000 + 0xbd5) != 0) {
            func_ov004_020afdd0(
                (void *)data_ov006_02136e0c[*(unsigned char *)(o + 0x5000 + 0xbd6)],
                *(int *)(o + 0x5000 + 0xbcc) >> 0xc,
                *(int *)(o + 0x5000 + 0xbd0) >> 0xc,
                -1, 1);
        }
        o += 0xc;
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 14 -- func_ov006_020fae90, 0x020fae90, size 0x5c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020fae90
void func_ov006_020fae90(u8 *c)
{
    int i;
    for (i = 0; i < 4; i++, c += 0xc) {
        *(u8 *)(c + 0x5bd4) = 1;
        *(u8 *)(c + 0x5bd5) = 1;
        *(int *)(c + 0x5bcc) = data_ov006_0212eb70[i] << 12;
        *(int *)(c + 0x5bd0) = 0xa0000;
        *(u8 *)(c + 0x5bd6) = 0;
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 15 -- func_ov006_020faeec, 0x020faeec, size 0x28 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020faeec
void func_ov006_020faeec(char *p)
{
    int i;
    for (i = 0; i < 4; i++) {
        *(unsigned char *)(p + 0x5bd4) = 0;
        *(unsigned char *)(p + 0x5bd5) = 0;
        p += 0xc;
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 16 -- func_ov006_020faf14, 0x020faf14, size 0x58 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020faf14
void func_ov006_020faf14(char* c){
  if(*(unsigned char*)(c+0x5000+0xbc7)==0) return;
  int x=*(int*)(c+0x5000+0xbb0);
  int y=*(int*)(c+0x5000+0xbb4);
  func_ov004_020afdd0((void *)data_ov006_021375f4[*(unsigned char*)(c+0x5000+0xbc9)],
    x>>0xc, y>>0xc, -1, -1);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 17 -- func_ov006_020faf6c, 0x020faf6c, size 0x190 */
/* -------------------------------------------------------------------------- */
#pragma push
#pragma opt_common_subs off
// @symbol func_ov006_020faf6c
void func_ov006_020faf6c(char *c, int idx)
{
    int i;
    int j;
    char *p;
    char *q;
    int dx;
    int dy;
    u8 st;
    int n;

    if (*(u8 *)(c + idx * 0x1c + 0x5bc8) == 0)
        return;

    p = c;
    for (i = 0; i < 0x30; i++, p += 0x38) {
        if (*(u8 *)(p + 0x4f0c) == 0)
            continue;
        if (*(u8 *)(p + 0x4f0d) < 2)
            continue;
        dx = (*(int *)(c + idx * 0x1c + 0x5bb0) - *(int *)(p + 0x4ed8)) >> 12;
        dy = (*(int *)(c + idx * 0x1c + 0x5bb4) - *(int *)(p + 0x4edc)) >> 12;
        if (_ZN4cstd4sqrtEy(dx * dx + dy * dy) > 0x18)
            continue;

        n = idx * 0x1c;
        *(short *)(c + 0x5c2a) = 0x20;
        *(u8 *)(c + i * 0x38 + 0x4f0c) = 0;
        *(u8 *)(c + i * 0x38 + 0x4f0e) = 0;
        func_ov006_020fb8fc(c,
                            *(int *)(c + n + 0x5bb0),
                            *(int *)(c + n + 0x5bb4),
                            2, 0, 0);
        q = c;
        for (j = 0; j < 0x1e; j++, q += 0x38) {
            if (*(u8 *)(q + 0x468c) == 0)
                continue;
            st = *(u8 *)(q + 0x468f);
            if (st == 0)
                continue;
            if (st < 5) {
                *(u8 *)(q + 0x468f) = 6;
                *(short *)(q + 0x4688) = 0x10;
            }
        }
        *(u8 *)(c + n + 0x5bc7) = 0;
        *(u8 *)(c + n + 0x5bc6) = 0;
        *(u8 *)(c + 0x5c34) = 0;
        _ZN5Sound12PlayBank2_2DEj(0x18a);
        return;
    }
}
#pragma pop

/* -------------------------------------------------------------------------- */
/* ROM ordinal 18 -- func_ov006_020fb0fc, 0x020fb0fc, size 0xc8 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020fb0fc
void func_ov006_020fb0fc(char *c, int i){
  int n = i*0x1c;
  int *a = (int*)(c + 0x5bb0);
  int *b = (int*)(c + 0x5bc0);
  *(int*)((char*)b + n) = func_02012468(*(int*)((char*)b + n), 2, 0x189, 4, 0, 0, func_020126e8(*(int*)((char*)a + n)), 0);
  *(int*)((char*)a + n) = *(int*)((char*)a + n) + *(int*)(c + 0x5bb8 + n);
  {
    int v = *(int*)((char*)a + n) >> 12;
    if (v >= 0x110) goto clear;
    if (v > -0x10) return;
  }
clear:
  *(unsigned char*)(c + 0x5bc6 + n) = 0;
  *(unsigned char*)(c + 0x5bc7 + n) = 0;
  *(unsigned char*)(c + 0x5c34) = 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 19 -- func_ov006_020fb1c4, 0x020fb1c4, size 0x6c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020fb1c4
void func_ov006_020fb1c4(char *c, int i){
    short *e = (short*)(c + 0x5bc4 + i*0x1c);
    if(*(unsigned short*)e != 0){
        *e = *(unsigned short*)e - 1;
        if(*e < 0) *e = 0;
        return;
    }
    *(unsigned char*)(c+i*0x1c+0x5000+0xbc8)=2;
    *(unsigned char*)(c+0x5bca+i*0x1c) = *(unsigned char*)(c+0x5bca+i*0x1c) - 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 20 -- func_ov006_020fb230, 0x020fb230, size 0x22c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020fb230
void func_ov006_020fb230(char *p, int i)
{
  int n = i * 0x1c;
  int v;
  u8 flag;
  *((int *) ((p + 0x5bc0) + n)) = func_02012468(*((int *) ((p + 0x5bc0) + n)), 2, 0x189, 4, 0, 0, func_020126e8(*((int *) ((p + 0x5bb0) + n))), 0);
  *((int *) ((p + 0x5bb0) + n)) += *((int *) ((p + 0x5bb8) + n));
  flag = *((u8 *) ((p + 0x5bc9) + n));
  v = (*((int *) ((p + 0x5bb0) + n))) >> 12;
  if (flag != 0)
  {
    if (v >= 0xc0)
      *((int *) ((p + 0x5bb8) + n)) -= 0x80;
    else if ((*((int *) ((p + 0x5bb8) + n))) <= 0x3000)
      *((int *) ((p + 0x5bb8) + n)) += 0x200;
    if (v >= 0xf0)
    {
      u32 a;
      if ((*((u8 *) ((p + 0x5bca) + n))) == 0) { *((u8 *) ((p + 0x5bc8) + n)) = 4; return; }
      *((int *) ((p + 0x5bb0) + n)) = 0xf0000;
      *((int *) ((p + 0x5bb8) + n)) = 0;
      *((u8 *) ((p + 0x5bc9) + n)) ^= 1;
      a = (((u32) RandomIntInternal(&data_0209d4b8)) >> 16) & 0x7fff;
      *((u16 *) ((p + 0x5bc4) + n)) = (u16) ((((a << 3) >> 15) * 2) + 0x10);
      *((u8 *) ((p + 0x5bc8) + n)) = 3;
      return;
    }
  }
  else
  {
    if (v <= 0x40)
      *((int *) ((p + 0x5bb8) + n)) += 0x80;
    else if ((*((int *) ((p + 0x5bb8) + n))) >= (-0x3000))
      *((int *) ((p + 0x5bb8) + n)) -= 0x200;
    if (v <= 0x10)
    {
      u32 a;
      if ((*((u8 *) ((p + 0x5bca) + n))) == 0) { *((u8 *) ((p + 0x5bc8) + n)) = 4; return; }
      *((int *) ((p + 0x5bb0) + n)) = 0x10000;
      *((int *) ((p + 0x5bb8) + n)) = 0;
      *((u8 *) ((p + 0x5bc9) + n)) ^= 1;
      a = (((u32) RandomIntInternal(&data_0209d4b8)) >> 16) & 0x7fff;
      *((u16 *) ((p + 0x5bc4) + n)) = (u16) ((((a << 3) >> 15) * 2) + 0x10);
      *((u8 *) ((p + 0x5bc8) + n)) = 3;
      return;
    }
  }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 21 -- func_ov006_020fb45c, 0x020fb45c, size 0x84              */
/* Integrates entry i (stride 0x1c, array at +0x5bb4): pos += vel; below the  */
/* -0xc0 floor keep accelerating vel by -0x60, otherwise snap pos to -0xc0000 */
/* and zero vel.  Tail work delegated to ROM ordinal 20.                      */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020fb45c
void func_ov006_020fb45c(Obj* self, int i) {
    self->entries[i].a += self->entries[i].b;
    if (self->entries[i].a >> 12 >= -0xc0) {
        self->entries[i].a = -0xc0000;
        self->entries[i].b = 0;
    } else {
        self->entries[i].b -= 0x60;
    }
    func_ov006_020fb230((char *)self, i);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 22 -- func_ov006_020fb4e0, 0x020fb4e0, size 0x12c */
/* -------------------------------------------------------------------------- */
#pragma push
#pragma opt_common_subs off
// @symbol func_ov006_020fb4e0
void func_ov006_020fb4e0(char* c, int idx)
{
    int off;
    short* p;
    unsigned int v;
    off = idx * 0x1c;
    p = (short*)((char*)&((struct E*)(c + 0x5bc4))[idx]);
    if (*(unsigned short*)p != 0) {
        *p = *(unsigned short*)p - 1;
        if (*p < 0) *p = 0;
        return;
    }
    _ZN5Sound12PlayBank2_2DEj(0x188);
    *(char*)(c + off + 0x5000 + 0xbc8) = 1;
    v = (((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 1;
    v = v >> 0xf;
    *(char*)(c + off + 0x5000 + 0xbc9) = v;
    if (v != 0) {
        *(char*)(c + off + 0x5000 + 0xbc7) = 1;
        *(int*)(c + off + 0x5000 + 0xbb0) = -0x10000;
        *(int*)(c + off + 0x5000 + 0xbb8) = 0x400;
    } else {
        *(char*)(c + off + 0x5000 + 0xbc7) = 1;
        *(int*)(c + off + 0x5000 + 0xbb0) = 0x110000;
        *(int*)(c + off + 0x5000 + 0xbb8) = -0x400;
    }
    *(int*)(c + off + 0x5000 + 0xbb4) = -0xf8000;
    *(char*)(c + off + 0x5000 + 0xbca) = ((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 3 >> 0xf) + 2;
    *(int*)(c + off + 0x5000 + 0xbbc) = 0x2000;
}
#pragma pop

/* -------------------------------------------------------------------------- */
/* ROM ordinal 23 -- func_ov006_020fb60c, 0x020fb60c, size 0x64 */
/* Dispatches through the ov006 pointer-to-member table at 0x0214266c.        */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020fb60c
void func_ov006_020fb60c(C2 *c){
  if (c->g == 0) return;
  (c->*data_ov006_0214266c[c->idx])(0);
  func_ov006_020faf6c((char *)c, 0);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 24 -- func_ov006_020fb670, 0x020fb670, size 0xdc */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020fb670
void func_ov006_020fb670(char *obj)
{
  int cnt;
  int i;
  unsigned int r;
  char *p;
  if ((*((unsigned char *) ((obj + 0x5000) + 0xc34))) != 0)
  {
    return;
  }
  *((unsigned char *) ((obj + 0x5000) + 0xbc6)) = 1;
  *((unsigned char *) ((obj + 0x5000) + 0xbc8)) = 0;
  *((int *) ((obj + 0x5000) + 0xbc0)) = 0;
  r = (((unsigned int) RandomIntInternal(&data_0209d4b8)) >> 16) & 0x7fff;
  *((short *) ((obj + 0x5b00) + 0xc4)) = (short) ((((r << 5) >> 15) << 3) + 0x200);
  cnt = 0;
  for (i = 0, p = obj; i < 4; i++)
  {
    if ((*((unsigned char *) ((p + 0x5000) + 0xbd4))) != 0)
    {
      cnt++;
    }
    p += 0xc;
  }

  if (cnt == 1)
  {
    r = (((unsigned int) RandomIntInternal(&data_0209d4b8)) >> 16) & 0x7fff;
    *((short *) ((obj + 0x5b00) + 0xc4)) = (short) ((((r << 5) >> 15) << 2) + 0x200);
  }
  unsigned char *bp = (unsigned char *) (((int) obj + 0x5c34));
  *bp = *bp + 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 25 -- func_ov006_020fb74c, 0x020fb74c, size 0x94 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020fb74c
void func_ov006_020fb74c(void* base) {
    int i;
    char* p = (char*)base;
    for (i = 0; i < 30; i++, p += 0x14) {
        if (*(unsigned char*)(p + 0x5966)) {
            int type = *(unsigned char*)(p + 0x5967);
            int x = *(int*)(p + 0x5958) >> 12;
            int y = *(int*)(p + 0x595c) >> 12;
            int idx = *(unsigned char*)(p + 0x5965);
            if (type != 2) {
                func_ov004_020afdd0((void*)data_ov006_02133e10[idx], x, y, -1, -1);
            } else {
                func_ov004_020afdd0((void*)data_ov006_02133e7c[idx], x, y, -1, -1);
            }
        }
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 26 -- func_ov006_020fb7e0, 0x020fb7e0, size 0x11c              */
/* The distinct launder macros force independent RMW base materialization     */
/* (add plus pool).  A plain *(b+0x5960) for the compare/zero reloads folds to */
/* add #0x5900 + #0x60 and loses the ROM's shape.                              */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020fb7e0
void func_ov006_020fb7e0(char *thiz)
{
    int i;
    char *b = thiz;
    for (i = 0; i < 0x1e; i++) {
        if (*(u8 *)(b + 0x5964) != 0) {
            u16 *cnt = (u16 *)(int)M1((u32)b + 0x5960);
            int lim;
            u8 sub;
            (*cnt)++;
            if (*(u16 *)(b + 0x5960) >= 6) {
                *(u16 *)(b + 0x5960) = 0;
                (*(u8 *)(int)M2((u32)b + 0x5965))++;
                lim = 3;
                if (*(u8 *)(b + 0x5967) == 2) lim = 5;
                sub = *(u8 *)(b + 0x5968);
                if (sub != 0) {
                    if (lim - 2 == *(u8 *)(b + 0x5965)) {
                        char *q = thiz + (sub - 1) * 0xc + 0x5000;
                        *(u8 *)(q + 0xbd4) = 0;
                        *(u8 *)(q + 0xbd6) = 1;
                    }
                }
                if (*(u8 *)(b + 0x5965) >= lim) {
                    *(u8 *)(b + 0x5966) = 0;
                    *(u8 *)(b + 0x5964) = 0;
                    if (*(u16 *)(b + 0x5962) != 0) {
                        func_ov006_020fbb2c(thiz, i, *(u16 *)(b + 0x5962));
                    }
                }
            }
        }
        b = b + 0x14;
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 27 -- func_ov006_020fb8fc, 0x020fb8fc, size 0x80 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020fb8fc
void func_ov006_020fb8fc(char *c, int a2, int a3, int a4, int a5, int a6)
{
    int i = 0;
    char *p = c;
    do {
        if (*(unsigned char *)(p + 0x5964) == 0) {
            char *e = c + i * 0x14;
            *(int *)(e + 0x5958) = a2;
            *(int *)(e + 0x595c) = a3;
            *(unsigned char *)(e + 0x5964) = 1;
            *(unsigned char *)(e + 0x5966) = 1;
            *(unsigned short *)(e + 0x5960) = 0;
            *(unsigned char *)(e + 0x5965) = 0;
            *(unsigned char *)(e + 0x5967) = a4;
            *(unsigned short *)(e + 0x5962) = a5;
            *(unsigned char *)(e + 0x5968) = a6;
            return;
        }
        i++;
        p += 0x14;
    } while (i < 0x1e);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 28 -- func_ov006_020fb97c, 0x020fb97c, size 0xac */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020fb97c
void func_ov006_020fb97c(char* c){
  volatile unsigned short sp;
  int count = 0;
  int i = 0;
  char* p = c;
  do {
    if (*(unsigned char*)(p + 0x5bd4) != 0) {
      count++;
      break;
    }
    i++;
    p += 0xc;
  } while (i < 4);
  if (count != 0)
    return;
  *(int*)(c + 0x5c10) = 2;
  *(short*)(c + 0x5c18) = 0x40;
  {
    unsigned r = _ZN3G2S13GetBG2CharPtrEv();
    sp = 0;
    MultiStore16(sp, (char*)r, 0x6000);
  }
  *(unsigned char*)(c + 0x5c30) = 0;
  func_ov006_020fe1a8(c);
  func_ov006_020fc1b4(c, 1);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 29 -- func_ov006_020fba28, 0x020fba28, size 0x20 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020fba28
void func_ov006_020fba28(void){
  func_ov004_020b1a5c(func_ov004_020adbc0(), 6);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 30 -- func_ov006_020fba48, 0x020fba48, size 0x1c              */
/* Converts a sublevel ID to a course ID, then opens the cannon for it.       */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020fba48
void func_ov006_020fba48(s8 levelID)
{
    s8 courseID = func_ov004_020adc1c(levelID);
    func_ov004_020b19f0(courseID);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 31 -- func_ov006_020fba64, 0x020fba64, size 0x70 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020fba64
void func_ov006_020fba64(char *base)
{
    int i;
    char *o = base;
    for (i = 0; i < 0x1e; i++) {
        if (*(unsigned short *)(o + 0x4c00 + 0xfa) != 0) {
            func_ov004_020b2444(
                *(int *)(o + 0x4000 + 0xcf0) >> 0xc,
                *(int *)(o + 0x4000 + 0xcf4) >> 0xc,
                *(unsigned short *)(o + 0x4c00 + 0xf8),
                -1, -1, 0, 0);
        }
        o += 0xc;
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 32 -- func_ov006_020fbad4, 0x020fbad4, size 0x58 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020fbad4
void func_ov006_020fbad4(char *c)
{
  int i;
  for (i = 0; i < 0x1e; i++)
  {
    char *r2 = c + 0x4c00;
    unsigned short v = *((unsigned short *) (r2 + 0xfa));
    if (v != 0)
    {
      unsigned short *p = (unsigned short *) (((long long) ((int) (c + 0x4cfa))));
      *p = (*p) - 1;
      if ((*((short *) ((c + 0x4c00) + 0xfa))) < 0)
      {
        *((short *) ((c + 0x4c00) + 0xfa)) = 0;
      }
    }
    c += 0xc;
  }

}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 33 -- func_ov006_020fbb2c, 0x020fbb2c, size 0xbc */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020fbb2c
void func_ov006_020fbb2c(char *c, int idx, unsigned short val)
{
    int i;
    char *p = c + 0x15c;
    for (i = 0x1d; i >= 0; p -= 0xc, i--) {
        if (*(unsigned short *)(p + 0x4cfa) != 0) continue;
        *(int *)(c + i * 0xc + 0x4cf0) = *(int *)(c + idx * 0x14 + 0x5958);
        *(int *)(c + i * 0xc + 0x4cf4) = *(int *)(c + idx * 0x14 + 0x595c);
        *(unsigned short *)(c + i * 0xc + 0x4cfa) = 0x30;
        *(unsigned short *)(c + 0x4cf8 + i * 0xc) = val;
        func_ov004_020adb1c(*(unsigned short *)(c + 0x4cf8 + i * 0xc) + func_ov004_020adbc0());
        if ((unsigned int)func_ov004_020adbc0() < 0xbb8) return;
        if (*(unsigned char *)(c + 0xc3) != 0)
            *(unsigned char *)(c + 0xc3) = 0;
        return;
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 34 -- func_ov006_020fbbe8, 0x020fbbe8, size 0xd0 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020fbbe8
void func_ov006_020fbbe8(char* c)
{
    volatile unsigned short tmp;
    int n = *(unsigned char*)(c + 0x5c2f);
    int a, b;
    char* base;

    if (n == 0) return;

    base = c + (n - 1) * 0x38;
    a = 0x80 - (*(int*)(base + 0x4ed8) >> 12);
    b = 0x20 - (*(int*)(base + 0x4edc) >> 12);

    if (a < -6) return;
    if (a > 6) return;
    if (b < -6) return;
    if (b > 6) return;
    if (*(unsigned char*)(base + 0x4f0d) != 2) return;

    *(unsigned char*)(c + 0x5c2f) = 0;
    {
        char* dst = (char*)_ZN3G2S13GetBG2CharPtrEv();
        tmp = 0;
        MultiStore16(tmp, dst, 0x6000);
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 35 -- func_ov006_020fbcb8, 0x020fbcb8, size 0x80              */
/* The first parameter is never read; it is spelled `void *` so that ordinal  */
/* 36's eight call sites keep passing their `char *` unchanged.               */
/* -------------------------------------------------------------------------- */
#pragma push
#pragma opt_loop_invariants off
// @symbol func_ov006_020fbcb8
void func_ov006_020fbcb8(void *a0, int a1, int a2, int a3)
{
    int i, j, x, lo, hi, col, row, yb, s;
    u32 *bp;
    s = a1;
    for (i = 0; i < 2; i++)
    {
        x = a2 - 1 + i;
        lo = x & 7;
        hi = x >> 3;
        col = lo * 4;
        for (j = 0; j < 2; j++)
        {
            row = hi * 32;
            yb = s - 1;
            a1 = yb + j;
            bp = (u32 *)_ZN3G2S13GetBG2CharPtrEv();
            *(u32 *)((char *)(bp + (row + (a1 >> 3)) * 8) + col) |= a3 << ((a1 & 7) * 4);
        }
    }
}
#pragma pop

/* -------------------------------------------------------------------------- */
/* ROM ordinal 36 -- func_ov006_020fbd38, 0x020fbd38, size 0x40c */
/* -------------------------------------------------------------------------- */
#pragma push
#pragma opt_common_subs off
#pragma opt_loop_invariants off
// @symbol func_ov006_020fbd38
void func_ov006_020fbd38(void *arg0)
{
    char *a = (char *)arg0;
    int i;
    int tx;
    int ty;
    int *sx;
    int *sy;
    int *pf00;
    int *pf18;
    int idx;
    int sX0;
    int sX1;
    int sY;
    volatile u16 fill;
    int off;
    int py;
    int cx;
    int cy;
    int dx;
    int adx;
    int dy;
    int ady;
    char *buf;
    int n;

    n = *(u8 *)(a + 0x5c2f);
    if (n == 0)
        return;
    idx = n - 1;
    off = idx * 0x38;
    pf18 = (int *)(a + 0x4ef0 + off);
    pf00 = (int *)(a + 0x4ed8 + off);
    if (*pf00 != *pf18 || *(int *)(a + 0x4edc + off) != *(int *)(a + 0x4ef4 + off)) {
        buf = (char *)_ZN3G2S13GetBG2CharPtrEv();
        fill = 0;
        MultiStore16(fill, buf, 0x6000);
        sx = (int *)(a + off + 0x4ed8);
        sy = (int *)(a + off + 0x4edc);
        sX0 = 0x6c;
        sX1 = 0x94;
        sY = 0x22;
        for (i = 0; i < 2; i++) {
            if (i == 0) {
                cx = sX0;
                tx = *sx >> 12;
                cy = sY;
                py = *sy >> 12;
                if (py >= 0x22) ty = py + 4; else ty = py - 4;
                tx -= 4;
            } else {
                cx = sX1;
                tx = *sx >> 12;
                cy = sY;
                py = *sy >> 12;
                if (py >= 0x22) ty = py + 4; else ty = py - 4;
                tx += 4;
            }
            dx = tx - cx;
            adx = dx;
            if (dx < 0) adx = -dx;
            dy = ty - cy;
            ady = dy;
            if (dy < 0) ady = -dy;
            if (adx >= ady) {
                *(int *)(a + 0x5c14) = adx / 2;
                for (;;) {
                    if (dx == 0) {
                        func_ov006_020fbcb8(a, cx, cy, 2);
                        break;
                    } else if (dx > 0) {
                        cx++;
                        AC += ady;
                        if (*(int *)(a + 0x5c14) > adx) {
                            if (dy >= 0) cy++; else cy--;
                            AC -= adx;
                        }
                        func_ov006_020fbcb8(a, cx, cy, 2);
                        if (cx == tx)
                            break;
                    } else {
                        cx--;
                        AC += ady;
                        if (*(int *)(a + 0x5c14) > adx) {
                            if (dy >= 0) cy++; else cy--;
                            AC -= adx;
                        }
                        func_ov006_020fbcb8(a, cx, cy, 2);
                        if (cx == tx)
                            break;
                    }
                }
            } else {
                *(int *)(a + 0x5c14) = ady / 2;
                for (;;) {
                    if (dy == 0) {
                        func_ov006_020fbcb8(a, cx, cy, 2);
                        break;
                    } else if (dy > 0) {
                        cy++;
                        AC += adx;
                        if (*(int *)(a + 0x5c14) > ady) {
                            if (dx >= 0) cx++; else cx--;
                            AC -= ady;
                        }
                        func_ov006_020fbcb8(a, cx, cy, 2);
                        if (cy == ty)
                            break;
                    } else {
                        cy--;
                        AC += adx;
                        if (*(int *)(a + 0x5c14) > ady) {
                            if (dx >= 0) cx++; else cx--;
                            AC -= ady;
                        }
                        func_ov006_020fbcb8(a, cx, cy, 2);
                        if (cy == ty)
                            break;
                    }
                }
            }
        }
    }
    *pf18 = *pf00;
    *(int *)(a + 0x4ef4 + idx * 0x38) = *(int *)(a + 0x4edc + idx * 0x38);
}
#pragma pop

/* -------------------------------------------------------------------------- */
/* ROM ordinal 37 -- func_ov006_020fc144, 0x020fc144, size 0x70 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020fc144
void func_ov006_020fc144(char *base)
{
    int i;
    char *o = base;
    for (i = 0; i < 2; i++) {
        if (*(unsigned char *)(o + 0x4000 + 0xeb8) != 0) {
            func_ov004_020afdd0(
                (void *)data_ov006_02136b80[*(unsigned char *)(o + 0x4000 + 0xeb5)],
                *(int *)(o + 0x4000 + 0xea0) >> 0xc,
                *(int *)(o + 0x4000 + 0xea4) >> 0xc,
                -1, 2);
        }
        o += 0x1c;
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 38 -- func_ov006_020fc1b4, 0x020fc1b4, size 0x44 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020fc1b4
void func_ov006_020fc1b4(char* base, int val) {
    int i;
    for (i = 0; i < 2; i++) {
        if (*(unsigned char*)(base + 0x4eb7)) {
            *(unsigned char*)(base + 0x4eb3) = 3;
            *(unsigned char*)(base + 0x4eba) = 0;
            *(unsigned char*)(base + 0x4eb4) = (unsigned char)val;
        }
        base += 0x1c;
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 39 -- func_ov006_020fc1f8, 0x020fc1f8, size 0xf4 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020fc1f8
int func_ov006_020fc1f8(char* self, int idx)
{
  u16* counter = (u16*)(self + 0x4eb0 + idx*0x1c);
  u8* p6 = (u8*)(self + 0x4eb6 + idx*0x1c);
  u16 c = *counter;
  *counter = c + 1;
  u8 f6 = *p6;
  if (*counter >= data_ov006_0212eb3c[f6]) {
    *counter = 0;
    *p6 = *p6 + 1;
    if (*p6 >= 8) {
      *p6 = 0;
      *(u8*)(self + 0x4eba + idx*0x1c) = *(u8*)(self + 0x4eba + idx*0x1c) + 1;
    }
    *(u8*)(self + idx*0x1c + 0x4000 + 0xeb5) = data_ov006_0212eb34[*p6];
  }
  {
    u8* base = (u8*)(self + idx*0x1c + 0x4000);
    if (base[0xeb4] != 0) {
      base[0xeba] = 0;
      return 0;
    }
  }
  {
    u8* p3 = (u8*)(self + 0x4eba);
    u8 c3 = p3[idx*0x1c];
    if (c3 >= 2) {
      c3 = 0;
      *(u8*)(self + idx*0x1c + 0x4000 + 0xeb3) = c3;
      p3[idx*0x1c] = c3;
    }
    return c3;
  }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 40 -- func_ov006_020fc2ec, 0x020fc2ec, size 0x214 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020fc2ec
void func_ov006_020fc2ec(char* c, int i)
{
    int b = i * 0x1c;
    int v;

    if (ST(c, b) != 0) {
        CTA(c, b)++;
        if (CTA(c, b) < 4)
            return;
        CTA(c, b) = 0;
        FB(c, b)++;
        if (FB(c, b) >= 3) {
            FB(c, b) = 0;
            OUTFLAG2(c, b) = 1;
            ST(c, b) = 0;
            return;
        }
        OUTFLAG(c, b) = data_ov006_0212eb10[FB(c, b)];
        return;
    }

    CTA(c, b)++;
    if (CTA(c, b) >= 4) {
        CTA(c, b) = 0;
        FB(c, b)++;
        if (FB(c, b) >= 6) {
            FB(c, b) = 0;
        }
        OUTFLAG(c, b) = data_ov006_0212eb1c[FB(c, b)];
    }

    ACC(c, b) += VEL(c, b);
    v = ACC(c, b) >> 0xc;
    if (v >= data_ov006_0212eb60[i + 2]) {
        if (VEL(c, b) >= 0x800)
            VEL(c, b) -= 0x80;
    } else {
        VEL(c, b) += 0x80;
    }
    if (v < data_ov006_0212eb60[i])
        return;

    ACC(c, b) = data_ov006_0212eb60[i] << 0xc;
    VEL(c, b) = -0x1000;
    ST(c, b)++;
    CTA(c, b) = 0;
    FB(c, b) = 0;
    OUTFLAG(c, b) = data_ov006_0212eb10[0];
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 41 -- func_ov006_020fc500, 0x020fc500, size 0x218 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020fc500
void func_ov006_020fc500(char* c, int i) {
    int b = i * 0x1c;
    int t;

    if (*(u8*)(c + 0x4eb4 + b) != 0) {
        (*(u16*)(c + 0x4eb0 + b))++;
        if (*(u16*)(c + 0x4eb0 + b) < 4) {
            return;
        }
        *(u16*)(c + 0x4eb0 + b) = 0;
        (*(u8*)(c + 0x4eb6 + b))++;
        if (*(u8*)(c + 0x4eb6 + b) >= 3) {
            *(u8*)(c + 0x4eb6 + b) = 0;
            *(u8*)(c + 0x4eb3 + b) = 2;
            *(u8*)(c + 0x4eb4 + b) = 0;
            return;
        }
        *(u8*)(c + 0x4eb5 + b) = data_ov006_0212eb0c[*(u8*)(c + 0x4eb6 + b)];
        return;
    }

    (*(u16*)(c + 0x4eb0 + b))++;
    if (*(u16*)(c + 0x4eb0 + b) >= 4) {
        *(u16*)(c + 0x4eb0 + b) = 0;
        (*(u8*)(c + 0x4eb6 + b))++;
        if (*(u8*)(c + 0x4eb6 + b) >= 6) {
            *(u8*)(c + 0x4eb6 + b) = 0;
        }
        *(u8*)(c + 0x4eb5 + b) = data_ov006_0212eb14[*(u8*)(c + 0x4eb6 + b)];
    }

    *(s32*)(c + 0x4ea0 + b) = *(s32*)(c + 0x4ea0 + b) + *(s32*)(c + 0x4ea8 + b);
    t = *(s32*)(c + 0x4ea0 + b) >> 0xc;
    if (t <= data_ov006_0212eb50[i + 2]) {
        if (*(s32*)(c + 0x4ea8 + b) <= -0x800) {
            *(s32*)(c + 0x4ea8 + b) = *(s32*)(c + 0x4ea8 + b) + 0x80;
        }
    } else {
        *(s32*)(c + 0x4ea8 + b) = *(s32*)(c + 0x4ea8 + b) - 0x80;
    }

    if (t > data_ov006_0212eb50[i]) {
        return;
    }
    *(s32*)(c + 0x4ea0 + b) = data_ov006_0212eb50[i] << 0xc;
    *(s32*)(c + 0x4ea8 + b) = 0x1000;
    (*(u8*)(c + 0x4eb4 + b))++;
    *(u16*)(c + 0x4eb0 + b) = 0;
    *(u8*)(c + 0x4eb6 + b) = 0;
    *(u8*)(c + 0x4eb5 + b) = data_ov006_0212eb0c[0];
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 42 -- func_ov006_020fc718, 0x020fc718, size 0xb8              */
/* Resets one row of the ov006 table (rows are 0x1c bytes apart) after        */
/* rolling a random kind from data_ov006_0212eb2c.  The row base `thiz + off` */
/* is spelled twice on purpose: the ROM computes it once for the +0x4000      */
/* stores and again, right after the kind reload and before the predicated    */
/* if/else, for the tail stores.  With common-subexpression elimination on,   */
/* mwccarm folds the second into the first and sinks the add below the        */
/* if/else, which recolours the whole tail.                                    */
/* -------------------------------------------------------------------------- */
#pragma push
#pragma opt_common_subs off
// @symbol func_ov006_020fc718
void func_ov006_020fc718(char *thiz, int n)
{
    int rnd = RandomIntInternal(&data_0209d4b8);
    int off = n * 0x1c;
    unsigned int x = ((unsigned)rnd >> 16) & 0x7fff;
    unsigned int idx = (x << 1) >> 0xf;
    int v = data_ov006_0212eb2c[idx];
    unsigned char *p = (unsigned char*)(thiz + 0x4eb3);
    char *q;
    p[off] = (unsigned char)v;
    q = thiz + off + 0x4000;
    *(unsigned char*)(q + 0xeb4) = 0;
    q = thiz + off;
    if (p[off] == 2) {
        *(unsigned char*)(q + 0x4000 + 0xeb9) = 1;
        *(int*)(q + 0x4000 + 0xea8) = 0x1000;
    } else {
        *(unsigned char*)(q + 0x4000 + 0xeb9) = 0;
        *(int*)(q + 0x4000 + 0xea8) = -0x1000;
    }
    *(short*)(thiz + off + 0x4e00 + 0xb0) = 0;
    *(unsigned char*)(thiz + off + 0x4000 + 0xeb6) = 0;
    *(unsigned char*)(thiz + off + 0x4000 + 0xeb8) = 1;
}
#pragma pop

/* -------------------------------------------------------------------------- */
/* ROM ordinal 43 -- func_ov006_020fc7d0, 0x020fc7d0, size 0x74              */
/* Walks the 8-byte {code pointer, adjustment} records at 0x02142624; an odd  */
/* adjustment selects the virtual spelling through the receiver's own vtable. */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020fc7d0
void func_ov006_020fc7d0(char* c){
  int i=0;
  char* r5=c;
  do{
    if(*(unsigned char*)(r5+0x4000+0xeb7)!=0){
      int idx=*(unsigned char*)(r5+0x4000+0xeb3);
      Ent7d0* e=&data_ov006_02142624[idx];
      int adj=e->b;
      char* obj=c+(adj>>1);
      int fn;
      if(adj&1){
        fn=*(int*)(*(int*)obj + e->a);
      } else {
        fn=e->a;
      }
      ((void(*)(void*,int))fn)(obj,i);
    }
    i++;
    r5+=0x1c;
  }while(i<2);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 44 -- func_ov006_020fc844, 0x020fc844, size 0x7c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020fc844
void func_ov006_020fc844(unsigned char* c){
  int i;
  for(i=0;i<2;i++,c+=0x1c){
    *(unsigned char*)(c+0x4eb7)=1;
    *(unsigned char*)(c+0x4eb8)=0;
    *(unsigned char*)(c+0x4eb3)=0;
    *(unsigned char*)(c+0x4eb4)=0;
    *(unsigned char*)(c+0x4eb5)=0;
    *(short*)(c+0x4eb0)=0;
    *(int*)(c+0x4ea0)=data_ov006_0212eb24[i]<<12;
    *(int*)(c+0x4ea4)=0x68000;
    *(int*)(c+0x4ea8)=0;
    *(int*)(c+0x4eac)=0;
    *(unsigned char*)(c+0x4eb9)=0;
    *(unsigned char*)(c+0x4eba)=0;
  }
}

}  /* extern "C" */
