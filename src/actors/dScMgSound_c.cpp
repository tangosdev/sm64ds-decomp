//cpp
/* Reconstructed translation unit.
 * ov006/dScMgSound_c  (82 functions)
 *
 * The contiguous MG_SOUND scene run and the immediately adjacent unique
 * MG_SOUND factory are one original object. The class identity is ROM-proven by
 * RTTI (_ZTI12dScMgSound_c at ov006 0x0213f6e4, a __si_class_type_info over
 * _ZTI19dScMgSingle3DBase_c); state and helper names attached to address-only
 * symbols are descriptive coinage, documented in the TU manifest.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function, in the REVERSE of source order, so
 * the highest-address ROM function is written FIRST here. Codegen is left
 * DEFERRED (the compiler's default), which is what makes that reversal happen;
 * the two are one decision and neither may be changed alone.
 *
 * NO PRAGMA IS CARRIED. src/func_ov006_0211c478.c held a file-global
 * `#pragma opt_strength_reduction off`. The delete-outright control settles it:
 * the function reproduces 0x140/0x140 with that line stripped, so the pragma
 * was inert and no `#pragma defer_codegen off` bracket was needed to contain
 * it. Do not add one back without re-running that control.
 *
 * THE DESTRUCTOR IS INLINE IN include/dScMgSound_c.h and declared first, which
 * is what emits D1 then D0 -- the order ov006 uses (0x02119904 below
 * 0x02119958). Both pre-migration shards carried an identical out-of-line copy;
 * merged, that would have been a duplicate definition and the wrong order.
 *
 * Assembled from these legacy one-function sources (ROM address order):
 *   [0] 0x02119904  src/_ZN12dScMgSound_cD1Ev.cpp
 *   [1] 0x02119958  src/_ZN12dScMgSound_cD0Ev.cpp
 *   [2] 0x021199c0  src/func_ov006_021199c0.c
 *   [3] 0x02119a18  src/func_ov006_02119a18.c
 *   [4] 0x02119a88  src/func_ov006_02119a88.c
 *   [5] 0x02119aa8  src/func_ov006_02119aa8.c
 *   [6] 0x02119b00  src/func_ov006_02119b00.c
 *   [7] 0x02119ba4  src/func_ov006_02119ba4.c
 *   [8] 0x02119bc4  src/func_ov006_02119bc4.c
 *   [9] 0x02119bdc  src/func_ov006_02119bdc.cpp
 *   [10] 0x02119c74  src/func_ov006_02119c74.c
 *   [11] 0x02119d50  src/func_ov006_02119d50.c
 *   [12] 0x02119dc4  src/func_ov006_02119dc4.c
 *   [13] 0x02119e5c  src/func_ov006_02119e5c.c
 *   [14] 0x02119eec  src/func_ov006_02119eec.cpp
 *   [15] 0x02119f3c  src/func_ov006_02119f3c.c
 *   [16] 0x02119fb0  src/func_ov006_02119fb0.c
 *   [17] 0x0211a048  src/func_ov006_0211a048.c
 *   [18] 0x0211a0d8  src/func_ov006_0211a0d8.cpp
 *   [19] 0x0211a128  src/func_ov006_0211a128.c
 *   [20] 0x0211a19c  src/func_ov006_0211a19c.c
 *   [21] 0x0211a234  src/func_ov006_0211a234.c
 *   [22] 0x0211a2c4  src/func_ov006_0211a2c4.cpp
 *   [23] 0x0211a314  src/func_ov006_0211a314.c
 *   [24] 0x0211a388  src/func_ov006_0211a388.c
 *   [25] 0x0211a420  src/func_ov006_0211a420.c
 *   [26] 0x0211a4b0  src/func_ov006_0211a4b0.cpp
 *   [27] 0x0211a500  src/func_ov006_0211a500.c
 *   [28] 0x0211a578  src/func_ov006_0211a578.c
 *   [29] 0x0211a5ec  src/func_ov006_0211a5ec.c
 *   [30] 0x0211a648  src/func_ov006_0211a648.cpp
 *   [31] 0x0211a698  src/func_ov006_0211a698.c
 *   [32] 0x0211a69c  src/func_ov006_0211a69c.c
 *   [33] 0x0211a714  src/func_ov006_0211a714.c
 *   [34] 0x0211a7ac  src/func_ov006_0211a7ac.cpp
 *   [35] 0x0211a7fc  src/func_ov006_0211a7fc.c
 *   [36] 0x0211a910  src/func_ov006_0211a910.c
 *   [37] 0x0211a9fc  src/func_ov006_0211a9fc.c
 *   [38] 0x0211aa44  src/func_ov006_0211aa44.cpp
 *   [39] 0x0211aa94  src/func_ov006_0211aa94.c
 *   [40] 0x0211ab0c  src/func_ov006_0211ab0c.c
 *   [41] 0x0211ab80  src/func_ov006_0211ab80.c
 *   [42] 0x0211abdc  src/func_ov006_0211abdc.cpp
 *   [43] 0x0211ac2c  src/func_ov006_0211ac2c.c
 *   [44] 0x0211ac30  src/func_ov006_0211ac30.c
 *   [45] 0x0211ad00  src/func_ov006_0211ad00.c
 *   [46] 0x0211ad44  src/func_ov006_0211ad44.cpp
 *   [47] 0x0211ad94  src/func_ov006_0211ad94.c
 *   [48] 0x0211ae40  src/func_ov006_0211ae40.c
 *   [49] 0x0211aed0  src/func_ov006_0211aed0.c
 *   [50] 0x0211af60  src/func_ov006_0211af60.cpp
 *   [51] 0x0211afb0  src/func_ov006_0211afb0.c
 *   [52] 0x0211b05c  src/func_ov006_0211b05c.c
 *   [53] 0x0211b0ec  src/func_ov006_0211b0ec.c
 *   [54] 0x0211b17c  src/func_ov006_0211b17c.cpp
 *   [55] 0x0211b1cc  src/func_ov006_0211b1cc.c
 *   [56] 0x0211b278  src/func_ov006_0211b278.c
 *   [57] 0x0211b308  src/func_ov006_0211b308.c
 *   [58] 0x0211b398  src/func_ov006_0211b398.cpp
 *   [59] 0x0211b3e8  src/func_ov006_0211b3e8.c
 *   [60] 0x0211b3ec  src/func_ov006_0211b3ec.c
 *   [61] 0x0211b4fc  src/func_ov006_0211b4fc.c
 *   [62] 0x0211b590  src/func_ov006_0211b590.cpp
 *   [63] 0x0211b5e0  src/func_ov006_0211b5e0.cpp
 *   [64] 0x0211b654  src/func_ov006_0211b654.c
 *   [65] 0x0211b790  src/func_ov006_0211b790.c
 *   [66] 0x0211b80c  src/func_ov006_0211b80c.c
 *   [67] 0x0211b954  src/func_ov006_0211b954.cpp
 *   [68] 0x0211b9c8  src/func_ov006_0211b9c8.cpp
 *   [69] 0x0211ba88  src/func_ov006_0211ba88.cpp
 *   [70] 0x0211bbe0  src/func_ov006_0211bbe0.c
 *   [71] 0x0211bc68  src/func_ov006_0211bc68.c
 *   [72] 0x0211bc8c  src/func_ov006_0211bc8c.cpp
 *   [73] 0x0211bf44  src/func_ov006_0211bf44.c
 *   [74] 0x0211c080  src/func_ov006_0211c080.c
 *   [75] 0x0211c478  src/func_ov006_0211c478.c
 *   [76] 0x0211c5b8  src/_ZN12dScMgSound_c9Virtual50Ev.cpp
 *   [77] 0x0211c5d0  src/_ZN12dScMgSound_c13OnYoshiTryEatEi.cpp
 *   [78] 0x0211c6c4  src/_ZN12dScMgSound_c6RenderEv.cpp
 *   [79] 0x0211c720  src/_ZN12dScMgSound_c8BehaviorEv.cpp
 *   [80] 0x0211c984  src/_ZN12dScMgSound_c13InitResourcesEv.cpp
 *   [81] 0x0211cb70  src/d_s_mg_sound.cpp
 */

/* PRAGMA DECIDED, NOT DEFERRED. The only pragma in this TU's legacy sources was
 * `#pragma opt_strength_reduction off` at the top of src/func_ov006_0211c478.c.
 * That pragma is file-global last-wins in mwccarm 2004/b56, so carrying it would
 * recompile all 81 other members -- but the control shows there is nothing to
 * carry: stripping the line from the legacy file on its own reproduces
 * func_ov006_0211c478 exactly (0x140, build_pin 2004/b56). It was inert. */

/* Includes: union of the legacy files', first-seen in ROM-ascending
 * processing order. NOT verified for header ordering constraints (e.g. a
 * common.h-before-X rule) -- watch for new compile errors after this. */
#include "dScMgSound_c.h"
#include "types.h"
#include "decl_common.h"

/* Local implementation views, reconciled. The legacy one-function files each
 * spelled the scene through its own private tag, so the same layout arrived
 * here under several names and three different tags arrived with three
 * different layouts. Where two views are byte-identical they are merged onto
 * one name; where they genuinely differ they keep separate names. No view is
 * widened or narrowed -- every one still describes exactly the bytes its own
 * member touches.
 *
 * TWO POINTER-TO-MEMBER FAMILIES, DELIBERATELY. Eleven dispatch members saw
 * `struct C` as an INCOMPLETE type and two saw it complete. mwccarm picks a
 * pointer-to-member representation from what it knows about the class, so
 * merging the two views onto one complete `C` would change the dispatch
 * members' codegen. The incomplete view keeps the name `C`; the two members
 * that need the layout use `CFull`, which is a distinct type. */

/* --- the state-machine dispatch family (incomplete view) ------------------ */
struct C;                                   /* opaque: dispatch only */
typedef void (C::*PMF)(int);
struct Entry { PMF pmf; };                  /* one-word jump-table slot */

/* --- the same dispatch, seen by the two members that index the array ------ */
struct Elem {                               /* 0x24 stride at +0x51d1 */
  unsigned char idx;
  char pad[0x23];
};
struct CFull {
  char pad[0x51d1];
  Elem arr[1];
};
typedef void (CFull::*PMFFull)(int);
struct EntryFull { PMFFull pmf; };

/* --- the 0x24-stride array at +0x51b4, two disjoint field views ----------- */
struct BallSlot {                           /* func_ov006_0211a500's view */
    int a;            /* +0x00 (this+0x51b4) */
    int pad4;         /* +0x04 */
    int b;            /* +0x08 (this+0x51bc) */
    int c;            /* +0x0c (this+0x51c0) */
    char pad10[8];    /* +0x10 */
    unsigned char f18;/* +0x18 (this+0x51cc) */
    unsigned char f19;/* +0x19 (this+0x51cd) */
    char pad1a[10];   /* pad to 0x24 */
};
struct BallObj {
    char pad[0x51b4];
    struct BallSlot arr[4];
};

struct BigSlot {                            /* func_ov006_0211aa94's view */
    int b4;
    int unk4;
    int bc;
    int c0;
    int unk10;
    int unk14;
    unsigned char cc;
    unsigned char cd;
    unsigned char pad[10];
};
struct BigObj {
    char pad[0x51b4];
    BigSlot arr[16];
};

/* --- the 0x24-stride array at +0x51b8 ------------------------------------- */
struct NoteSlot {                           /* func_ov006_0211a9fc's view */
    u32 value;     /* +0x00 (this+0x51b8) */
    u32 counter;   /* +0x04 */
    char _pad8[8];
    u16 halfword;  /* +0x10 (this+0x51c8) */
    char _pad12[3];
    u8 flagA;      /* +0x15 (this+0x51cd) */
    char _pad16[3];
    u8 flagB;      /* +0x19 (this+0x51d1) */
    u8 type;       /* +0x1a (this+0x51d2) */
    char _pad1b[9];
};
struct NoteObj {
    char _pad[0x51b8];
    struct NoteSlot entries[1];  /* 0x51b8 */
};

/* --- the 0x14-stride array at +0x50e8, two field views -------------------- */
struct E { char _[0x14]; };                 /* stride only; no field is read */

struct Slot {                               /* func_ov006_0211b654's view */
    int f00; int f04; int f08; int f0c; int f10; int f14;
    u16 f18; u16 f1a;
    u8 f1c; u8 f1d; u8 f1e; u8 f1f; u8 f20; u8 f21; u8 f22; u8 f23;
};
struct Mgr { u8 pad[0x51b0]; struct Slot slot[30]; };

struct SndEntry {
    s32 x;
    u8 unk04[0x0c - 0x04];
    u8 b0c;
    u8 b0d;
    u8 b0e;
    u8 b0f;
    u8 b10;
    u8 b11;
    u8 b12;
    u8 b13;
};
struct SndCtx {
    u8 unk00[0x50e8];
    SndEntry ent[67];
    u8 unk5624[3];
    u8 mode;
};

/* --- func_ov006_0211b9c8 reaches the scene through its own vtable --------- */
struct SceneVt {
    virtual void p00(); virtual void p01(); virtual void p02(); virtual void p03();
    virtual void p04(); virtual void p05(); virtual void p06(); virtual void p07();
    virtual void p08(); virtual void p09(); virtual void p10(); virtual void p11();
    virtual void p12(); virtual void p13(); virtual void p14(); virtual void p15();
    virtual void p16(); virtual void p17(); virtual void p18(); virtual void p19();
    virtual void p20(); virtual void p21(); virtual void p22(); virtual void p23();
    virtual void p24(); virtual void p25(); virtual void p26(); virtual void p27();
    virtual void p28(); virtual void p29(); virtual void p30(); virtual void p31();
    virtual void p32(); virtual void p33(); virtual void p34();
    virtual int check();
};

/* --- the two RAW members' shared jump-table row --------------------------- */
struct Ent { int a; int b; };

/* shadow namespace 'Sound' */
namespace Sound { void PlayBank2_2D(unsigned int id); }

#define A(p) ((int)(p))
#define F1E(b,i) (*(unsigned char*)((char*)(b) + 0x51d1 + (i)*0x24))
#define M(p) ((void*)(int)(((long long)(int)(p)) & ~0LL))
#define RND (((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff)
#define LAUNDER(p) ((int)(p))

extern "C" {
extern int Hud_RenderSprite(int a,int b,int c,int d,int e);
extern int data_ov006_02137ae8[];
extern unsigned short data_ov006_0212ee38[];
extern int data_ov006_02138d28[];
extern u16 data_ov006_0212ee28[];
extern void func_ov004_020b2574(int a, int b);
extern const short data_02082214[];
extern void* data_ov006_0213f6f0[];
extern int data_ov006_0212ef30[];
extern int data_ov006_0212ef0c[];
extern "C" EntryFull data_ov006_02142db0[];
extern int data_ov006_0212eeac[];
extern int data_ov006_0212ee88[];
extern "C" EntryFull data_ov006_02142d08[];
extern int data_ov006_0212ee58[];
extern int data_ov006_0212ee4c[];
extern PMF data_ov006_02142cc0[];
extern int data_ov006_0212ee64[];
extern int data_ov006_0212ef00[];
extern "C" PMF data_ov006_02142d98[];
extern int data_ov006_0212eedc[];
extern "C" Entry data_ov006_02142d68[];
extern int data_ov006_0212ee94[];
extern "C" Entry data_ov006_02142d20[];
extern u32 data_ov006_0212ee70[];
extern "C" Entry data_ov006_02142cf0[];
extern int data_ov006_0212ee7c[];
extern "C" Entry data_ov006_02142cd8[];
extern int data_ov006_0212ee40[];
extern "C" Entry data_ov006_02142de0[];
extern int data_ov006_0212ef18[];
extern int data_ov006_0212ef24[];
extern "C" Entry data_ov006_02142dc8[];
extern int data_ov006_0212eef4[];
extern int data_ov006_0212eee8[];
extern "C" Entry data_ov006_02142d80[];
extern int data_ov006_0212eed0[];
extern int data_ov006_0212eec4[];
extern "C" Entry data_ov006_02142d50[];
extern int data_ov006_0212eeb8[];
extern int data_ov006_0212eea0[];
extern unsigned char data_ov006_0212ee0c[];
extern "C" PMF data_ov006_02142d38[];
extern u8 data_ov006_0212ee30[];
extern void func_02012790(int);
extern void func_ov006_020c271c(void *c);
extern void func_ov006_020c2664(char *c);
extern int data_ov006_0212ef7c[];
extern int data_ov006_0212ef8c[];
extern int data_ov006_0212ef5c[];
extern int data_ov006_0212ef6c[];
void func_ov006_0211ba88(char *base, int idx);
extern unsigned short data_ov006_0212ef4c[];
void func_ov006_0211b654(struct Mgr *m, int n);
void func_ov006_020c2300(char *p);
void func_02012174(u32 bank, u32 id);
extern u16 data_ov006_0212ef3c[];
extern u16 data_ov006_0213f794[];
extern u16 data_ov006_0213f7e8[];
extern u8 data_020a0e40[];
extern u8 data_020a0de8[];
extern u8 data_020a0de9[];
extern u8 data_020a0dea[];
extern u8 data_020a0deb[];
extern void _ZN5Sound12PlayBank2_2DEj(unsigned int soundId);
extern int RandomIntInternal(int *seed);
extern int data_0209d4b8;
extern u8 data_ov006_0212ee10[];
extern u8 data_ov006_0212ee18[];
extern u8 data_ov006_0212ee20[];
extern int data_ov006_0212ef9c[];
extern int data_ov006_0212efb0[];
extern u16 *data_ov006_0213f6fc[];
extern void func_ov006_020c2924(char *c);
extern void func_ov006_020c2594(void *c);
extern void func_ov006_0211c478(char *base);
extern void func_ov006_0211c080(char *o);
extern "C" void func_ov004_020b1e34(void *c, int a, int b, int d);
void FreeGfxSlotsById(int id);
int LoadFile(int handle);
void _ZN2GX11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
unsigned _ZN3G2S13GetBG2CharPtrEv(void);
void _ZN3GXS10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
void _ZN3GXS11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
void DecompressLZ16(int src, void *dst);
void func_ov006_020c225c(void *p);
int func_ov006_020c3050(void *p);
extern u8 data_0209d45c;
extern u8 data_0209d454;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 81 -- dScMgSound_c_classInit, 0x0211cb70, size 0x60 */
/* -------------------------------------------------------------------------- */
// @symbol dScMgSound_c_classInit
/* The MG_SOUND registry factory. tools/tu_map.py cut it into a unit of its own
   because the cut is made on the symbol NAME and this one is not spelled
   `_ZN12dScMgSound_c...`, so the class label never reached it; the linker run
   is in fact contiguous -- InitResources ends at 0x0211cb70 and this starts
   there, with the next class's destructor at 0x0211cbd0. It allocates
   sizeof(dScMgSound_c) = 0x562c, installs this class's own cartridge vtable,
   and calls the 0x4f38 helper's constructor, so it can only have been compiled
   where this class's definition was visible. ov006/dScMgMemory2_c's manifest
   records the same one-function extension for the same reason.

   THE TWO VTABLE STORES ARE SPELLED DIFFERENTLY ON PURPOSE.
   _ZTV19dScMgSingle3DBase_c is external here, and config's symbols.txt names
   its ADDRESS POINT, so it is stored raw. _ZTV12dScMgSound_c is emitted by
   THIS translation unit, where mwccarm puts the symbol at the start of the
   table's storage -- eight bytes of offset-to-top and typeinfo below the
   address point -- so it needs the `+ 2`. Storing it raw loses 8 from the
   addend, which only tools/objisolate.py's addend check would catch. */
extern "C" void *_ZN7fBase_cnwEj(unsigned int);
extern "C" void _ZN11dScMgBase_cC2Ev(void *);
extern "C" void _ZN8Particle10SysTrackerC1Ev(void *);
extern "C" void func_ov006_020c33dc(void *);
extern void *_ZTV19dScMgSingle3DBase_c[];
extern void *_ZTV12dScMgSound_c[];

extern "C" void *dScMgSound_c_classInit(void)
{
    char *p = (char *)_ZN7fBase_cnwEj(0x562c);
    if (p) {
        _ZN11dScMgBase_cC2Ev(p);
        *(void **)p = _ZTV19dScMgSingle3DBase_c;
        _ZN8Particle10SysTrackerC1Ev(p + 0x471c);
        *(void **)p = _ZTV12dScMgSound_c + 2;
        func_ov006_020c33dc(p + 0x4f38);
    }
    return p;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 80 -- _ZN12dScMgSound_c13InitResourcesEv, 0x0211c984, size 0x1ec */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12dScMgSound_c13InitResourcesEv
/* dScMgSound_c::InitResources -- vtable slot 0, ov006 0x0211c984.
 *
 * Attributed by the vtable: dScMgSound_c's own table is ov006 0x0213f844 and
 * its slot 0 relocates here. The signature is include/fBase_c.h's own slot
 * 0, `virtual s32 InitResources()`.
 *
 * THE SPLIT LITERALS ARE ORIGINAL AND ARE LEFT ALONE. `r7 + 0x5000 + 0xe0` is
 * not the same instruction as `r7 + 0x50e0`: ARM's immediate encoding makes
 * mwccarm build the base once and address off it, which is what the cartridge
 * does. Folding them would be a readability change that has to be re-measured,
 * and they are also the form include/dScMgSound_c.h's banner means when it
 * says these offsets are matched access INSIDE mTable rather than fields of
 * this class -- 0x50e0, 0x5608, 0x5618 and 0x5626 all fall in the 0x6f4 the
 * ctor/dtor pair func_ov006_020c33dc/020c3288 owns.
 *
 * mHudScore is dScMgBase_c's, and reads as an inherited member; the
 * pre-migration file wrote it as `*(int *)(r7 + 0xb4)`.
 *
 * The GX/G2S/GXS entry points keep their mangled spellings INSIDE extern "C",
 * which is the tree's idiom for them (src/_ZN12dScMgLuigi_c13InitResourcesEv.cpp
 * does the same). Inside extern "C" the identifier is emitted verbatim; only a
 * bare namespace-scope `extern` of a mangled name would mangle a SECOND time,
 * which is the defect include/SharedFilePtr.h's banner records. */
s32 dScMgSound_c::InitResources()
{
    u8 *r7 = (u8 *)this;
    int r6, r5, r4;

    data_0209d45c |= 0x11;
    r6 = LoadFile(0xff);
    r5 = LoadFile(0x100);
    DecompressLZ16(r6, (void *)0x6400000);
    _ZN2GX11LoadOBJPlttEPKvjj((const void *)r5, 0, 0x100);

    data_0209d454 |= 4;
    r4 = LoadFile(0x98);
    *(volatile u16 *)0x400100c = (*(volatile u16 *)0x400100c & 0x43) | 0x214;
    DecompressLZ16(r4, (void *)_ZN3G2S13GetBG2CharPtrEv());
    Deallocate((void *)r4);

    r4 = LoadFile(0x99);
    _ZN3GXS10LoadBGPlttEPKvjj((const void *)r4, 0x60, 0x1a0);
    Deallocate((void *)r4);

    r4 = LoadFile(0x9a);
    func_02056374((const void *)r4, 0, 0x800);
    Deallocate((void *)r4);

    *(volatile u16 *)0x400100c &= ~0x40;
    *(volatile u32 *)0x4001018 = 0;
    *(volatile u16 *)0x400100c = (*(volatile u16 *)0x400100c & ~3) | 1;
    DecompressLZ16(r6, (void *)0x6600000);
    _ZN3GXS11LoadOBJPlttEPKvjj((const void *)r5, 0, 0x100);
    Deallocate((void *)r6);
    Deallocate((void *)r5);

    func_ov006_020c225c((void *)(r7 + 0x4660));
    if (func_ov006_020c3050((void *)mTable) == 0)
        return 0;

    *(int *)(r7 + 0x5000 + 0xe0) = 1;
    func_ov006_0211c478((char *)r7);
    func_ov006_0211c080((char *)r7);
    *(u8 *)(r7 + 0x5000 + 0x626) = 3;
    *(int *)(r7 + 0x5000 + 0x608) = 1;
    *(u16 *)(r7 + 0x5600 + 0x18) = 0x20;
    func_ov004_020b6808();
    func_ov004_020b0cac(0xd, 0x80, 0xa8, 1, -1, 0xd);
    mHudScore = 0;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 79 -- _ZN12dScMgSound_c8BehaviorEv, 0x0211c720, size 0x264 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12dScMgSound_c8BehaviorEv
/* dScMgSound_c::Behavior -- vtable slot 6, ov006 0x0211c720.
 *
 * Attributed by the vtable: dScMgSound_c's own table is ov006 0x0213f844 and
 * its slot 6 relocates here. The signature is include/dScMgBase_c.h's own slot
 * 6, `virtual s32 Behavior()`.
 *
 * The minigame's state machine, on the word at 0x5608: 0 = init, 1 = intro
 * countdown at 0x5618, 2 = result countdown at 0x5616 with the win/lose
 * handling and the 9999-capped win counter, 3 = retry countdown. Every one of
 * those offsets is INSIDE mTable, the single 0x6f4-byte member
 * include/dScMgSound_c.h describes, so they stay raw casts off the scene
 * rather than becoming fields of this class -- func_ov006_020c33dc and
 * func_ov006_020c3288 own that object's shape, and nothing in this tree has
 * recovered it.
 *
 * mPromptBlinkTimer, mPromptEnabled and mPromptBlinkCount are dScMgBase_c's, and read as inherited
 * members. The pre-migration file wrote all three as `*(u8 *)(c + 0xc3)` and
 * could not have said whose they were.
 *
 * THE THREE COUNTDOWN DECREMENTS KEEP THEIR `(int)` LAUNDER. Writing
 * `(*(u16 *)(c + 0x5618))--` lets mwccarm re-fold the address into the
 * post-decrement and the function comes out short; the cast through int is
 * what forces the address into a register first, which is what the cartridge
 * does. Same lever notes/mwccarm-codegen.md records for compound assignment. */
s32 dScMgSound_c::Behavior()
{
    char *c = (char *)this;

    switch (*(int *)(c + 0x5608)) {
    case 0:
        *(int *)(c + 0x5608) = 1;
        break;
    case 1:
        if (*(u16 *)(c + 0x5618) != 0) {
            (*(u16 *)(int)(c + 0x5618))--;
            if (*(u16 *)(c + 0x5618) == 0) {
                FreeGfxSlotsById(0x1d);
                if (mPromptBlinkCount == 0) {
                    mPromptEnabled = 1;
                    mPromptBlinkCount = 1;
                    mPromptBlinkTimer = 0;
                }
            }
        }
        func_ov006_0211b954(c);
        func_ov006_0211b80c(c);
        func_ov006_0211b5e0(c);
        func_ov006_0211b790(c);
        break;
    case 2:
        func_ov006_0211b954(c);
        func_ov006_0211b5e0(c);
        if (*(u16 *)(c + 0x5616) != 0) {
            (*(u16 *)(int)(c + 0x5616))--;
            if (*(u16 *)(c + 0x5616) == 0) {
                if (*(u8 *)(c + 0x5626) != 0) {
                    *(int *)(c + 0x50e0) = 0;
                    func_ov006_020c2594((char *)mTable);
                    if (*(u8 *)(c + 0x5626) == 3)
                        func_ov004_020b67f8();
                    func_ov004_020b0a54(0);
                    {
                        char *g = (char *)data_ov004_020beb68;
                        if (g != 0) {
                            if (*(int *)(g + 0xb4) < 9999)
                                (*(int *)(int)(g + 0xb4))++;
                            if (*(int *)(g + 0xb4) > *(int *)(g + 0xb8))
                                *(int *)(g + 0xb8) = *(int *)(g + 0xb4);
                        }
                    }
                    func_ov004_020adb1c(data_ov004_020beb68 != 0
                                            ? *(int *)((char *)data_ov004_020beb68 + 0xb4) : 0);
                    func_ov006_02119ba4(c);
                    func_ov006_02119a88(c);
                    mPromptEnabled = 0;
                } else {
                    *(int *)(c + 0x5608) = 3;
                    *(u16 *)(c + 0x5616) = 0x20;
                    func_ov006_0211b9c8(c);
                }
            }
        } else {
            func_ov006_02119b00(c);
            func_ov006_02119a18(c);
        }
        break;
    case 3:
        func_ov006_0211b954(c);
        func_ov006_0211b5e0(c);
        if (*(u16 *)(c + 0x5616) != 0) {
            (*(u16 *)(int)(c + 0x5616))--;
            if (*(s16 *)(c + 0x5616) <= 0) {
                *(int *)(c + 0x50e0) = 0;
                func_ov006_020c2440((char *)mTable);
                func_ov004_020b0a54(0x12);
                mPromptEnabled = 0;
                *(u16 *)(c + 0x5616) = 0;
            }
        }
        break;
    }

    func_ov006_020c2b8c((char *)mTable);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 78 -- _ZN12dScMgSound_c6RenderEv, 0x0211c6c4, size 0x5c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12dScMgSound_c6RenderEv
/* dScMgSound_c::Render -- vtable slot 9, ov006 0x0211c6c4.
 *
 * Attributed by the vtable: dScMgSound_c's own table is ov006 0x0213f844 and
 * its slot 9 relocates here, so this is this class's own override and not one
 * it inherits from dScMgSingle3DBase_c. The signature is
 * include/dScMgBase_c.h's own slot 9, `virtual s32 Render()`.
 *
 * Five of the six calls take the scene itself; the last takes mTable, the one
 * member this class owns, at 0x4f38. */
s32 dScMgSound_c::Render()
{
    char *c = (char *)this;

    func_ov004_020b1e34(c, 0xe0, 0x14, 1);
    func_ov006_02119c74(c);
    func_ov006_02119bdc(c);
    func_ov006_02119bc4(c);
    func_ov006_021199c0(c);
    func_ov006_02119aa8(c);
    func_ov006_020c29dc(mTable);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 77 -- _ZN12dScMgSound_c13OnYoshiTryEatEi, 0x0211c5d0, size 0xf4 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12dScMgSound_c13OnYoshiTryEatEi
// recovered name: dScMgSound_c_OnYoshiTryEat_0211c5d0
/* recovered: renamed to Class_Method, declarations from a shared header */
/* recovered: renamed to Class_Method, vtable slot 18 -- an override of
   dScMgBase_c::OnYoshiTryEat(int). The signature must repeat the base
   declaration exactly, or mwcc appends a slot instead of overriding.

   `self` IS A `char *`, NOT AN `int`, AND THAT IS THE WHOLE FUNCTION. The
   pre-migration file laundered `this` through an int, which forced every one
   of the four large offsets (0x5608, 0x50e0, 0x5626, 0x5618) into a literal
   pool and made the function 0x104 against the cartridge's 0xf4 -- the one
   member of this TU that did not reproduce, and the reason its delink entry
   carried `complete=False`. Kept as a pointer, mwccarm splits each offset the
   way the cartridge does, `add rN, r4, #0x5000` once and a 12-bit displacement
   per access, and the literal pool shrinks to the two constants the ROM has.
   Measured: 0xf4/0xf4, every differing word a relocation wildcard. The `& ~0`
   launder on the +0xbc read-modify-write went with it; it existed only to stop
   the int form re-folding the address. */
void dScMgSound_c::OnYoshiTryEat(int r1)
{
    char *self = (char *)this;

    int *p;
    int *q;
    int *base;
    int v;

    *(int*)(self + 0x5608) = 0;

    base = (int*)self;
    if (r1 == 0) {
        q = (int*)(self + 0xbc);
        *q = *q + 1;
        if ((unsigned int)base[0xbc/4] > 0x270e)
            base[0xbc/4] = 0x270e;
    } else {
        base[0xbc/4] = 0;
        if ((unsigned int)base[0xbc/4] > 0x270e)
            base[0xbc/4] = 0x270e;

        p = (int *)data_ov004_020beb68;
        if (p)
            *(int*)((int)p + 0xb4) = 0;

        p = (int *)data_ov004_020beb68;
        if (p)
            v = *(int*)((int)p + 0xb4);
        else
            v = 0;
        func_ov004_020adb1c(v);
    }

    *(int*)(self + 0x50e0) = 1;
    func_ov006_0211c478(self);

    *(signed char*)(self + 0x5626) = 3;
    func_ov006_0211c080(self);

    *(short*)(self + 0x5618) = 0x20;
    func_ov004_020b0cac(0xd, 0x80, 0xa8, 1, -1, 0xd);

    _ZN5Sound22LoadAndSetMusic_Layer1Ei(6);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 76 -- _ZN12dScMgSound_c9Virtual50Ev, 0x0211c5b8, size 0x18 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12dScMgSound_c9Virtual50Ev
// recovered name: dScMgSound_c_Virtual50
/* recovered: renamed to Class_Method */
/* dScMgSound_c::Virtual50 - recovered from vtable slot identity */
int dScMgSound_c::Virtual50()
{
    char *c = (char *)this;

    func_ov006_020c2594(c + 0x4f38);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 75 -- func_ov006_0211c478, 0x0211c478, size 0x140 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0211c478
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0211c478(char *base)
{
    int i;
    int cnt1;
    int cnt2;
    char *b = base;

    for (cnt1 = 0; cnt1 < 10; cnt1++) {
        *(int *)(b + 0x50e8) = 0;
        *(int *)(b + 0x50ec) = 0;
        *(short *)(b + 0x50f0) = 0;
        *(unsigned char *)(b + 0x50f4) = 0;
        *(unsigned char *)(b + 0x50f5) = 0;
        *(unsigned char *)(b + 0x50f6) = 0;
        *(unsigned char *)(b + 0x50f7) = 0;
        *(unsigned char *)(b + 0x50f8) = 0;
        b += 0x14;
    }

    for (cnt2 = 0; cnt2 < 5; cnt2++) {
        *(unsigned char *)(base + 0x561a + cnt2) = 0;
        *(unsigned char *)(base + 0x561f + cnt2) = 0;
    }

    b = base;
    for (i = 0; i < 0x1e; i++) {
        *(int *)(b + 0x51b0) = 0;
        *(int *)(b + 0x51b4) = 0;
        *(int *)(b + 0x51b8) = 0;
        *(int *)(b + 0x51bc) = 0;
        *(int *)(b + 0x51c4) = 0;
        *(short *)(b + 0x51c8) = 0;
        *(unsigned char *)(b + 0x51cc) = 0;
        *(unsigned char *)(b + 0x51cd) = 0;
        *(unsigned char *)(b + 0x51ce) = 0;
        *(unsigned char *)(b + 0x51cf) = 0;
        *(unsigned char *)(b + 0x51cc) = 0;
        *(unsigned char *)(b + 0x51d0) = 0;
        *(unsigned char *)(b + 0x51d1) = 0;
        *(unsigned char *)(b + 0x51d2) = 0;
        *(unsigned char *)(b + 0x51d3) = 0;
        b += 0x24;
    }

    *(unsigned char *)(base + 0x5604) = 0;
    *(unsigned char *)(base + 0x5605) = 1;
    *(int *)(base + 0x55f8) = 0xbd000;
    *(int *)(base + 0x55fc) = 0x97000;
    *(unsigned char *)(base + 0x5606) = 0;
    *(unsigned char *)(base + 0x55f4) = 0;
    *(unsigned char *)(base + 0x55f5) = 1;
    *(int *)(base + 0x55e8) = 0xa0000;
    *(int *)(base + 0x55ec) = 0x9d000;
    *(unsigned char *)(base + 0x55f6) = 0;

    *(short *)(base + 0x5612) = 0;
    *(short *)(base + 0x5610) = *(unsigned short *)(base + 0x5612);
    *(short *)(base + 0x5614) = 0;
    *(unsigned char *)(base + 0x5625) = 0;
    *(unsigned char *)(base + 0x5624) = 0;
    *(short *)(base + 0x5616) = 0;

    func_ov006_020c2924(base + 0x4f38);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 74 -- func_ov006_0211c080, 0x0211c080, size 0x3f8 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0211c080
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0211c080(char *o)
{
    int sel;
    int k;
    int count;
    int n;
    int half;
    int speed;
    int i;

    sel = *(int *)(o + 0xbc);
    *(u8 *)(o + 0x5628) = *(u8 *)(o + 0x5627);
    if (sel >= 5) {
        sel = (u32)(RND * 5) >> 15;
        if (sel == *(u8 *)(o + 0x5628)) {
            sel += ((u32)(RND << 2) >> 15) + 1;
            if (sel >= 5)
                sel -= 5;
        }
    }
    *(u8 *)(o + 0x5627) = sel;
    k = *(u8 *)(o + 0x5627);
    count = data_ov006_0212ee18[k];
    speed = data_ov006_0212ee10[k];
    if (k == 3) {
        int a = (u32)(RND << 2) >> 15;
        int b = (u32)(RND * 3) >> 15;
        *(u8 *)(o + 0x561f) = b;
        *(u8 *)(o + 0x5620) = *(u8 *)(o + 0x561f) + 1;
        if (*(u8 *)(o + 0x5620) >= 3)
            *(u8 *)LAUNDER(o + 0x5620) -= 3;
        *(u8 *)(o + 0x5621) = *(u8 *)(o + 0x5620) + 1;
        if (*(u8 *)(o + 0x5621) >= 3)
            *(u8 *)LAUNDER(o + 0x5621) -= 3;
        *(u8 *)LAUNDER(o + 0x561f) += a * 3;
        *(u8 *)LAUNDER(o + 0x5620) += a * 3;
        *(u8 *)LAUNDER(o + 0x5621) += a * 3;
    } else {
        half = count >> 1;
        for (i = 0; i < half; i++) {
            int v = (u32)(RND * speed) >> 15;
            *(u8 *)(o + i + 0x561f) = v;
            if (i != 0) {
                int dup;
                int j;
                for (;;) {
                    dup = 0;
                    j = 0;
                    for (; j < i; j++) {
                        if (*(u8 *)(o + i + 0x561f) == *(u8 *)(o + j + 0x561f)) {
                            dup = 1;
                            break;
                        }
                    }
                    if (dup == 0)
                        break;
                    *(u8 *)LAUNDER(o + i + 0x561f) = (u32)(RND * speed) >> 15;
                }
            }
        }
    }
    k = *(u8 *)(o + 0x5627);
    if (k == 1 || k == 4) {
        *(int *)(o + 0x560c) = data_ov006_0212efb0[((u32)*(int *)(o + 0xbc) >> 2) & 3];
    }
    half = count >> 1;
    for (i = 0; i < half; i++) {
        *(u8 *)LAUNDER(o + i + 0x561f) += data_ov006_0212ef9c[*(u8 *)(o + 0x5627)];
    }
    {
        int xi;
        char *p;
        int zi;
        char *q;
        u8 *slot;
        int t;

        n = 0;
        if (count <= 0)
            return;
        zi = 1;
        xi = 0;
        p = o;
        do {
            t = data_ov006_0212ee20[*(u8 *)(o + 0x5627)];
            *(int *)(p + 0x50e8) = data_ov006_0213f6fc[t][xi] << 12;
            *(int *)(p + 0x50ec) = (*(u16 * volatile *)&data_ov006_0213f6fc[t])[zi] << 12;
            *(u16 *)(p + 0x50f0) = 0;
            *(u8 *)(p + 0x50f4) = 0;
            *(u8 *)(p + 0x50f5) = 1;
            *(u8 *)(p + 0x50f6) = 1;
            *(u8 *)(p + 0x50f7) = 0;
            *(u8 *)(p + 0x50f9) = 0;
            do {
                q = o + ((u32)(RND * half) >> 15);
                slot = (u8 *)LAUNDER(q + 0x561a);
            } while (*slot >= 2);
            *(u8 *)(p + 0x50f8) = *(u8 *)(q + 0x561f);
            (*slot)++;
            xi += 2;
            p += 0x14;
            zi += 2;
            n++;
        } while (n < count);
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 73 -- func_ov006_0211bf44, 0x0211bf44, size 0x13c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0211bf44
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0211bf44(char* base, int slot)
{
    u8 player;
    int offset;
    int active;
    char* entry;
    int dx;
    int dy;

    if (*(int*)(base + 0x5608) != 1) {
        return;
    }
    if (*(u8*)(base + 0x5624) >= 2) {
        return;
    }

    player = data_020a0e40[0];
    offset = player * 4;
    active = 0;
    if (data_020a0de8[offset] != 0) {
        if (data_020a0de9[offset] != 0) {
            active = 1;
        }
    }
    if (active == 0) {
        return;
    }

    entry = base + slot * 0x14;
    entry += 0x5000;
    dx = data_020a0dea[player * 4] - (*(int*)(entry + 0xe8) >> 12);
    dy = data_020a0deb[player * 4] - (*(int*)(entry + 0xec) >> 12);

    if (dx > 0x18) {
        return;
    }
    if (dx < -0x18) {
        return;
    }
    if (dy > 0x18) {
        return;
    }
    if (dy < -0x18) {
        return;
    }

    *(u8*)(entry + 0xf4) = 1;
    *(u16*)(entry + 0xf0) = 0;
    *(u8*)(entry + 0xf7) = 0;
    {
        u8* counter = (u8*)(int)(base + 0x5624);
        *counter = *counter + 1;
    }
    _ZN5Sound12PlayBank2_2DEj(0x201);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 72 -- func_ov006_0211bc8c, 0x0211bc8c, size 0x2b8 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0211bc8c
/* recovered: dScMgSound_c per-note animation tick, ov006 0x0211bc8c (696
 * bytes). One note slot per call. The slot's frame counter at +0x50f0 is
 * advanced and compared against the per-pattern length in
 * data_ov006_0212ef3c; below it the call returns. At the wrap the counter is
 * cleared and the slot's pattern index at +0x50f7 steps on, and while that
 * index is still 0..6 the call returns as well. Past 6 the index is pinned at
 * 6, the slot's state byte at +0x50f4 goes to 2, the note id at +0x50f8 (+1)
 * is appended to the playback queue at +0x5610 with the slot number packed
 * into the high byte, the queue length at +0x5625 grows, the queue timer at
 * +0x5614 is armed, and the note is handed to func_ov006_0211b654 before
 * being played. Which player it goes to depends on dScMgBase_c::Virtual8C
 * (the scene's own spawn-parameter predicate) and on the instrument select at
 * +0x5627 -- the two-player answer folds instruments 0/2 together and reads
 * data_ov006_0213f794 to pick between func_02012790 and the +0x560c bank
 * handle, instrument 1 quantises the note to the start of its group of four
 * (the repeated-subtraction divide of id - 0x1a by 4), and everything else
 * goes to Sound::PlayBank2_2D; the one-player answer is the same set of
 * destinations wired differently. Finally, when the mode byte at +0x5624 is
 * 1, the whole helper object at +0x4f38 is retriggered.
 *
 * Every offset here is inside mTable, the single 0x6f4-byte member
 * include/dScMgSound_c.h describes, so they stay raw casts off the scene, the
 * same way src/func_ov006_0211ba88.cpp and src/func_ov006_0211bbe0.c reach them.
 *
 * TWO SPELLINGS ARE LOAD-BEARING, both measured. First,
 * func_ov006_0211b654's SECOND argument: the callee is (scene, slot) --
 * src/func_ov006_0211b654.c's own recovered signature -- and passing `idx`
 * costs zero bytes here because it is already in r1, but it keeps idx's web
 * alive across the queue-append block. Dropping it (a one-argument call)
 * leaves the whole function schedule-identical and rotates ten registers in
 * that block, because the `orr` then coalesces into idx's dead register
 * instead of the loaded halfword's (div 10, and no spelling, pragma or
 * declaration order moves it -- see notes/mwccarm-codegen.md 6ab). Second,
 * the queue slot at +0x5610 and the length at +0x5625 are written through the
 * plain member forms and mwccarm picks the two address shapes itself: the
 * plain store folds to `this + n*2` with an `add #0x5600`/`[#0x10]` pair
 * while the read-modify-write takes the pool-loaded base with the scaled
 * index. Forcing either shape breaks the other. */
extern "C" void func_ov006_0211bc8c(dScMgSound_c *self, int idx)
{
    char *c = (char *)self;
    int o = idx * 0x14;
    u8 id;
    u8 v;

    *(u16 *)(c + 0x50f0 + o) = *(u16 *)(c + 0x50f0 + o) + 1;
    if (*(u16 *)(c + 0x50f0 + o) < data_ov006_0212ef3c[((u8 *)(c + 0x50f7))[o]]) return;
    *(u16 *)(c + 0x50f0 + o) = 0;
    ((u8 *)(c + 0x50f7))[o] = ((u8 *)(c + 0x50f7))[o] + 1;
    if (((u8 *)(c + 0x50f7))[o] <= 6) return;
    ((u8 *)(c + 0x50f7))[o] = 6;
    ((u8 *)(c + o))[0x50f4] = 2;

    *(u16 *)(c + *(u8 *)(c + 0x5625) * 2 + 0x5610) = ((u8 *)(c + 0x50f8))[o] + 1;
    ((u16 *)(c + 0x5610))[*(u8 *)(c + 0x5625)] |= idx << 8;
    (*(u8 *)(c + 0x5625))++;
    *(u16 *)(c + 0x5614) = 0x20;

    func_ov006_0211b654((struct Mgr *)self, idx);
    id = ((u8 *)(c + 0x50f8))[o];

    if (self->Virtual8C()) {
        v = *(u8 *)(c + 0x5627);
        if (v == 0 || v == 2) {
            if (data_ov006_0213f794[id] == 2) {
                func_02012790(data_ov006_0213f7e8[id]);
            } else {
                func_02012174(*(u32 *)(c + 0x560c), data_ov006_0213f7e8[id]);
            }
        } else if (v == 1) {
            int n = id - 0x1a;
            int k = 0;
            while (n >= 4) { n -= 4; k++; }
            func_02012174(n, data_ov006_0213f7e8[0x1a + k * 4]);
        } else {
            Sound::PlayBank2_2D(data_ov006_0213f7e8[id]);
        }
    } else {
        v = *(u8 *)(c + 0x5627);
        if (v == 0 || v == 2) {
            func_02012790(data_ov006_0213f7e8[id]);
        } else if (v == 1) {
            func_02012174(*(u32 *)(c + 0x560c), data_ov006_0213f7e8[id]);
        } else if (v == 4) {
            if (data_ov006_0213f794[id] == 2) {
                Sound::PlayBank2_2D(data_ov006_0213f7e8[id]);
            } else {
                func_02012174(*(u32 *)(c + 0x560c), data_ov006_0213f7e8[id]);
            }
        } else {
            Sound::PlayBank2_2D(data_ov006_0213f7e8[id]);
        }
    }

    if (*(u8 *)(c + 0x5624) == 1) func_ov006_020c2300((char *)self->mTable);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 71 -- func_ov006_0211bc68, 0x0211bc68, size 0x24 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0211bc68
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0211bc68(char* c, int idx) {
    if (*(unsigned char*)(c+0x5624) == 0) {
        struct E* a = (struct E*)(c+0x5000);
        *(unsigned char*)((char*)&a[idx]+0xf4) = 3;
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 70 -- func_ov006_0211bbe0, 0x0211bbe0, size 0x88 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0211bbe0
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0211bbe0(char *c, int idx);
void func_ov006_0211bbe0(char *c, int idx){
    int o = idx * 0x14;
    char *cb = c + 0x50f0;
    char *p = c + o;
    unsigned char b;
    *(unsigned short*)(cb + o) = (unsigned short)(*(unsigned short*)(cb + o) + 1);
    b = ((unsigned char *)p)[0x50f7];
    if (*(unsigned short*)(cb + o) < data_ov006_0212ef4c[b]) return;
    *(unsigned short*)(cb + o) = 0;
    ((unsigned char *)(c + 0x50f7))[o] = (unsigned char)(((unsigned char *)(c + 0x50f7))[o] - 1);
    if (((unsigned char *)(c + 0x50f7))[o] == 0) ((unsigned char *)p)[0x50f4] = 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 69 -- func_ov006_0211ba88, 0x0211ba88, size 0x158 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0211ba88
/* recovered: dScMgSound_c helper, ov006 0x0211ba88 (344 bytes). One entry of
 * the 0x14-byte table at +0x50e8 scrolls left by 0x10 units per call; when
 * its +0x11 flag is clear the entry index is reduced modulo the per-mode
 * column count (data_ov006_0212ef5c / 0212ef6c, chosen by Virtual8C, indexed
 * by the mode byte at +0x5627) with a repeated-subtraction loop, a zero
 * remainder marks the entry, and otherwise the entries before it in the same
 * column are checked for a crowding stop (+0x0c := 4). Once the position
 * falls below -0x17 the entry's +0x0d/+0x0e bytes are cleared.
 *
 * The three shapes the cartridge insists on: the question is asked of the
 * scene ITSELF through the vtable (C++ `Virtual8C()`, which loads the table
 * through the r0 `this` copy; a C `p->vt->f(p)` spelling loads it through the
 * callee-saved copy), the crowding-stop store at +0x0c is a BYTE, and the
 * final two clears index the table as an array (`ent[idx]`) while every
 * earlier access is explicit `idx * 0x14` pointer math -- mwccarm keeps the
 * subscript scale and the explicit product in different common-subexpression
 * classes, so the epilogue recomputes idx*0x14 with a fresh mla and r6 is
 * free for the loop's hoisted #0x14. */
void func_ov006_0211ba88(char *base, int idx)
{
    SndCtx *ctx = (SndCtx *)base;
    int n;
    int i;
    int limit;
    s32 *cur;
    u8 *flag;

    cur = (s32 *)(base + 0x50e8 + idx * 0x14);
    *cur = *cur - 0x10000;

    if (((dScMgSound_c *)base)->Virtual8C() != 0) {
        limit = data_ov006_0212ef5c[*(u8 *)(base + 0x5000 + 0x627)];
    } else {
        limit = data_ov006_0212ef6c[*(u8 *)(base + 0x5000 + 0x627)];
    }

    flag = (u8 *)(base + 0x50f9 + idx * 0x14);
    if (*flag == 0) {
        n = idx;
        if (idx >= limit) {
            do {
                n = n - limit;
            } while (n >= limit);
        }
        if (n == 0) {
            *flag = 1;
            return;
        }
        for (i = 0; i < n; i++) {
            char *e = base + (idx - i - 1) * 0x14;
            if (*(u8 *)(e + 0x5000 + 0xf5) != 0) {
                if ((*(int *)(base + idx * 0x14 + 0x50e8) - *(int *)(e + 0x5000 + 0xe8)) >> 0xc <= 4) {
                    *(u8 *)(base + idx * 0x14 + 0x50f9) = 1;
                    *(u8 *)(e + 0x5000 + 0xf4) = 4;
                }
            }
        }
    }

    if (*cur >> 0xc > -0x18) return;

    ctx->ent[idx].b0d = 0;
    ctx->ent[idx].b0e = 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 68 -- func_ov006_0211b9c8, 0x0211b9c8, size 0xc0 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0211b9c8
extern "C" void func_ov006_0211b9c8(char *pc) {
    SceneVt *obj = (SceneVt *)pc;
    int lr;
    int ip;
    int found;
    int off;
    if (obj->check()) {
        lr = data_ov006_0212ef7c[*(unsigned char*)((char*)obj + 0x5627)];
    } else {
        lr = data_ov006_0212ef8c[*(unsigned char*)((char*)obj + 0x5627)];
    }
    for (ip = 0, off = 0; ip < 2; ip++) {
        int sb;
        found = -1;
        for (sb = 0; sb < lr; sb++) {
            int e = sb + off;
            if (*(unsigned char*)((char*)obj + e * 20 + 0x50f5) != 0)
                found = e;
        }
        if (found != -1) {
            *(unsigned char*)((char*)obj + found * 20 + 0x50f4) = 4;
        }
        off += lr;
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 67 -- func_ov006_0211b954, 0x0211b954, size 0x74 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0211b954
extern "C" {
extern Ent data_ov006_02142df8[];
void func_ov006_0211b954(char* c){
  int i=0;
  char* r5=c;
  do{
    if(*(unsigned char*)(r5+0x50f5)!=0){
      int idx=*(unsigned char*)(r5+0x50f4);
      Ent* e=&data_ov006_02142df8[idx];
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
    r5+=0x14;
  }while(i<10);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 66 -- func_ov006_0211b80c, 0x0211b80c, size 0x148 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0211b80c
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0211b80c(char *c){
  if((unsigned char)c[0x5625] < 2) return;
  if(*(unsigned short*)(c+0x5614) != 0){
    *(unsigned short*)M(c + 0x5614) -= 1;
    if(*(short*)(c+0x5614) <= 0) *(unsigned short*)(c+0x5614) = 0;
    return;
  }
  if((*(unsigned short*)(c+0x5610) & 0xff) == (*(unsigned short*)(c+0x5612) & 0xff)){
    *((char*)&((struct E*)c)[*(unsigned short*)(c+0x5610) >> 8] + 0x50f5) = 0;
    *((char*)&((struct E*)c)[*(unsigned short*)(c+0x5612) >> 8] + 0x50f5) = 0;
    func_02012790(0x26);
    { volatile void* p = (void*)(c+0x4f38); func_ov006_020c271c((void*)p); }
  } else {
    *((char*)&((struct E*)c)[*(unsigned short*)(c+0x5610) >> 8] + 0x50f4) = 3;
    *((char*)&((struct E*)c)[*(unsigned short*)(c+0x5612) >> 8] + 0x50f4) = 3;
    func_02012790(0xe);
    (*(unsigned char*)M(c + 0x5626))--;
    func_02012790(0x12f);
    func_ov006_020c2664(c+0x4f38);
  }
  *(unsigned short*)(c+0x5612) = 0;
  *(unsigned short*)(c+0x5610) = *(unsigned short*)(c+0x5612);
  c[0x5625] = 0;
  c[0x5624] = 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 65 -- func_ov006_0211b790, 0x0211b790, size 0x7c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0211b790
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0211b790(char* base)
{
    if (*(u8*)(base + 0x5626) == 0) {
        *(int*)(base + 0x5608) = 2;
        *(s16*)(base + 0x5616) = 0x50;
        return;
    }
    {
        int count = 0;
        int i = 0;
        char* p = base;
        do {
            if (*(u8*)(p + 0x50f5) != 0) {
                count++;
                break;
            }
            i++;
            p += 0x14;
        } while (i < 0xa);
        if (count != 0) return;
        *(s16*)(base + 0x5616) = 0x50;
        *(int*)(base + 0x5608) = 2;
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 64 -- func_ov006_0211b654, 0x0211b654, size 0x13c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0211b654
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0211b654(struct Mgr* m, int n)
{
    int k;
    int i;
    u8* g = data_ov006_0212ee30;
    int* s00 = (int*)((char*)m + n * 0x14 + 0x50e8);
    int* s04 = (int*)((char*)m + n * 0x14 + 0x50ec);

    for (k = 0; k < 3; g++, k++) {
        for (i = 0; i < 30; i++) {
            if (m->slot[i].f1c == 0) {
                m->slot[i].f1c = 1;
                m->slot[i].f00 = *s00;
                int* d04 = (int*)(((int)m + i * 0x24 + 0x51b4));
                *d04 = *s04 - 0x8000;
                u16* d18 = (u16*)(((int)m + i * 0x24 + 0x51c8));
                m->slot[i].f10 = *d04;
                m->slot[i].f1e = *g;
                m->slot[i].f22 = *g;
                *d18 = 0;
                if (*(u8*)((char*)m + n * 0x14 + 0x50f8) == 7) {
                    if (k >= 3) { *d18 = 0x10; } else { *d18 = 8; }
                }
                m->slot[i].f14 = 0x1000;
                m->slot[i].f08 = 0;
                m->slot[i].f0c = 0;
                m->slot[i].f20 = 0;
                m->slot[i].f21 = 0;
                break;
            }
        }
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 63 -- func_ov006_0211b5e0, 0x0211b5e0, size 0x74 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0211b5e0
extern "C" {
extern Ent data_ov006_02142e20[];
void func_ov006_0211b5e0(char* c){
  int i=0;
  char* r5=c;
  do{
    if(*(unsigned char*)(r5+0x51cc)!=0){
      int idx=*(unsigned char*)(r5+0x51d0);
      Ent* e=&data_ov006_02142e20[idx];
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
    r5+=0x24;
  }while(i<30);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 62 -- func_ov006_0211b590, 0x0211b590, size 0x50 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0211b590
extern "C" void func_ov006_0211b590(char *o, int i)
{
    unsigned char idx = *(unsigned char *)(o + i * 0x24 + 0x51d1);
    (((C *)o)->*data_ov006_02142d38[idx])(i);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 61 -- func_ov006_0211b4fc, 0x0211b4fc, size 0x94 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0211b4fc
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0211b4fc(char *c, int i)
{
    unsigned char typeIndex;

    *(unsigned char*)(c + i * 0x24 + 0x5000 + 0x1cd) = 1;

    typeIndex = *(unsigned char*)(c + 0x51d2 + i * 0x24);
    *(int*)(c + 0x51b0 + i * 0x24) += data_ov006_0212eeb8[typeIndex];
    *(int*)(c + i * 0x24 + 0x5000 + 0x1bc) = 0;

    typeIndex = *(unsigned char*)(c + 0x51d2 + i * 0x24);
    *(int*)(c + i * 0x24 + 0x5000 + 0x1b8) = data_ov006_0212eea0[typeIndex];

    typeIndex = *(unsigned char*)(c + 0x51d2 + i * 0x24);
    *(unsigned char*)(c + i * 0x24 + 0x5000 + 0x1d3) = data_ov006_0212ee0c[typeIndex];
    *(unsigned char*)(c + i * 0x24 + 0x5000 + 0x1d1) = 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 60 -- func_ov006_0211b3ec, 0x0211b3ec, size 0x110 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0211b3ec
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0211b3ec(char *base, int index)
{
    int dir;

    *(int *)(base + 0x51b0 + index * 0x24) += *(int *)(base + 0x51b8 + index * 0x24);

    dir = *(unsigned char *)(base + 0x51d3 + index * 0x24);
    if (dir == 0) {
        *(int *)(base + 0x51b8 + index * 0x24) -= 0x100;
        if (*(int *)(base + 0x51b8 + index * 0x24) < -0xc00) {
            *(int *)(base + 0x51b8 + index * 0x24) = -0xc00;
            *(unsigned char *)(base + 0x51d3 + index * 0x24) = 1;
        }
    } else if (dir != 0) {
        *(int *)(base + 0x51b8 + index * 0x24) += 0x100;
        if (*(int *)(base + 0x51b8 + index * 0x24) > 0xc00) {
            *(int *)(base + 0x51b8 + index * 0x24) = 0xc00;
            *(unsigned char *)(base + 0x51d3 + index * 0x24) = 0;
        }
    }

    *(int *)(base + 0x51b4 + index * 0x24) += *(int *)(base + 0x51bc + index * 0x24);
    *(int *)(base + 0x51bc + index * 0x24) -= 0xc0;

    if ((*(int *)(base + 0x51c0 + index * 0x24) - *(int *)(base + 0x51b4 + index * 0x24)) >> 12 >= 0x40) {
        *(unsigned char *)(base + 0x51cc + index * 0x24) = 0;
        *(unsigned char *)(base + 0x51cd + index * 0x24) = 0;
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 59 -- func_ov006_0211b3e8, 0x0211b3e8, size 0x4 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0211b3e8
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0211b3e8(void)
{
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 58 -- func_ov006_0211b398, 0x0211b398, size 0x50 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0211b398
extern "C" void func_ov006_0211b398(char *base, int idx)
{
    unsigned char state = F1E(base, idx);
    (((C*)base)->*data_ov006_02142d50[state].pmf)(idx);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 57 -- func_ov006_0211b308, 0x0211b308, size 0x90 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0211b308
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0211b308(char *c, int idx)
{
  int off = idx * 0x24;
  *((unsigned char *) (((c + off) + 0x5000) + 0x1cd)) = 1;
  *((int *) ((c + 0x51b0) + off)) = (*((int *) ((c + 0x51b0) + off))) + data_ov006_0212eed0[*((unsigned char *) ((c + 0x51d2) + off))];
  if (off && off)
  {
  }
  *((int *) (((c + off) + 0x5000) + 0x1bc)) = -0x2800;
  *((int *) (((c + off) + 0x5000) + 0x1b8)) = data_ov006_0212eec4[*((unsigned char *) ((c + 0x51d2) + off))];
  *((short *) (((c + off) + 0x5100) + 0xc8)) = 0;
  *((unsigned char *) (((c + off) + 0x5000) + 0x1d1)) = 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 56 -- func_ov006_0211b278, 0x0211b278, size 0x90 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0211b278
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0211b278(char *c, int i){
    int o = i * 0x24;
    *(int*)(c + 0x51b0 + o) += *(int*)(c + 0x51b8 + o);
    *(int*)(c + 0x51b4 + o) += *(int*)(c + 0x51bc + o);
    *(int*)(c + 0x51bc + o) += 0x200;
    if(*(int*)(c + 0x51bc + o) > 0){
        *(int*)(c + 0x51bc + o) = 0;
        *(unsigned char*)(c + 0x51d1 + o) = 2;
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 55 -- func_ov006_0211b1cc, 0x0211b1cc, size 0xac */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0211b1cc
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0211b1cc(char* c, int i){
  int n = i * 0x24;
  *(int*)(c + 0x51b0 + n) = *(int*)(c + 0x51b0 + n) + *(int*)(c + 0x5000 + n + 0x1b8);
  *(int*)(c + 0x51b4 + n) = *(int*)(c + 0x51b4 + n) + *(int*)(c + 0x51bc + n);
  *(int*)(c + 0x51bc + n) = *(int*)(c + 0x51bc + n) + 0x100;
  if (((*(int*)(c + 0x5000 + n + 0x1c0) - *(int*)(c + 0x51b4 + n)) >> 0xc) > 0x18)
    return;
  if (*(int*)(c + 0x51bc + n) > 0) {
    *(unsigned char*)(c + 0x5000 + n + 0x1cc) = 0;
    *(unsigned char*)(c + 0x5000 + n + 0x1cd) = 0;
  }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 54 -- func_ov006_0211b17c, 0x0211b17c, size 0x50 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0211b17c
extern "C" void func_ov006_0211b17c(char *base, int idx) {
    unsigned char state = *(unsigned char*)((char*)base + idx * 0x24 + 0x51d1);
    (((C*)base)->*data_ov006_02142d80[state].pmf)(idx);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 53 -- func_ov006_0211b0ec, 0x0211b0ec, size 0x90 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0211b0ec
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0211b0ec(char *c, int i)
{
    int o = i * 0x24;
    char *bIdx = c + 0x51d2;
    char *bVal = c + 0x51b0;
    char *b50 = (c + o) + 0x5000;
    unsigned char idx;
    *((unsigned char *)(((c + (o & 0xFFFFFFFFFFFFFFFFu)) + 0x5000) + 0x1cd)) = 1;
    idx = *((unsigned char *)(bIdx + o));
    *((int *)(bVal + o)) =
        (*((int *)(bVal + o))) + data_ov006_0212eef4[idx];
    *((int *)(((c + o) + 0x5000) + 0x1bc)) = -0x2000;
    *((int *)(b50 + 0x1b8)) =
        data_ov006_0212eee8[*((unsigned char *)(bIdx + o))];
    *((short *)((c + o) + 0x51c8)) = 0;
    *((unsigned char *)(((c + o) + 0x5000) + 0x1d1)) = 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 52 -- func_ov006_0211b05c, 0x0211b05c, size 0x90 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0211b05c
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0211b05c(char *c, int i){
    int o = i * 0x24;
    *(int*)(c + 0x51b0 + o) += *(int*)(c + 0x51b8 + o);
    *(int*)(c + 0x51b4 + o) += *(int*)(c + 0x51bc + o);
    *(int*)(c + 0x51bc + o) += 0x100;
    if(*(int*)(c + 0x51bc + o) > 0){
        *(int*)(c + 0x51bc + o) = 0;
        *(unsigned char*)(c + 0x51d1 + o) = 2;
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 51 -- func_ov006_0211afb0, 0x0211afb0, size 0xac */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0211afb0
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0211afb0(char* c, int i){
  int n = i * 0x24;
  *(int*)(c + 0x51b0 + n) = *(int*)(c + 0x51b0 + n) + *(int*)(c + 0x5000 + n + 0x1b8);
  *(int*)(c + 0x51b4 + n) = *(int*)(c + 0x51b4 + n) + *(int*)(c + 0x51bc + n);
  *(int*)(c + 0x51bc + n) = *(int*)(c + 0x51bc + n) + 0x100;
  if (((*(int*)(c + 0x5000 + n + 0x1c0) - *(int*)(c + 0x51b4 + n)) >> 0xc) > 0x10)
    return;
  if (*(int*)(c + 0x51bc + n) > 0) {
    *(unsigned char*)(c + 0x5000 + n + 0x1cc) = 0;
    *(unsigned char*)(c + 0x5000 + n + 0x1cd) = 0;
  }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 50 -- func_ov006_0211af60, 0x0211af60, size 0x50 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0211af60
extern "C" void func_ov006_0211af60(char *base, int idx) {
    unsigned char state = *(unsigned char*)((char*)base + idx * 0x24 + 0x51d1);
    (((C*)base)->*data_ov006_02142dc8[state].pmf)(idx);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 49 -- func_ov006_0211aed0, 0x0211aed0, size 0x90 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0211aed0
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0211aed0(char *c, int i)
{
  int new_var;
  int o = i * 0x24;
  char *bIdx = c + 0x51d2;
  char *bVal = c + 0x51b0;
  char *b50 = (c + o) + 0x5000;
  unsigned char idx;
  *((unsigned char *) (b50 + 0x1cd)) = 1;
  idx = *((unsigned char *) (bIdx + o));
  new_var = (*((int *) (bVal + o))) + data_ov006_0212ef18[idx];
  *((int *) (bVal + o)) = *((int *) (bVal + o));
  if (((!o) && (!o)) && (!o))
  {
  }
  *((int *) (bVal + o)) = new_var;
  *((int *) (((c + o) + 0x5000) + 0x1bc)) = -0x4800;
  *((int *) (((c + o) + 0x5000) + 0x1b8)) = data_ov006_0212ef24[*((unsigned char *) (bIdx + o))];
  *((short *) ((c + o) + 0x51c8)) = 0;
  *((unsigned char *) (((c + o) + 0x5000) + 0x1d1)) = 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 48 -- func_ov006_0211ae40, 0x0211ae40, size 0x90 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0211ae40
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0211ae40(char *c, int i){
    int o = i * 0x24;
    *(int*)(c + 0x51b0 + o) += *(int*)(c + 0x51b8 + o);
    *(int*)(c + 0x51b4 + o) += *(int*)(c + 0x51bc + o);
    *(int*)(c + 0x51bc + o) += 0x400;
    if(*(int*)(c + 0x51bc + o) > 0){
        *(int*)(c + 0x51bc + o) = 0;
        *(unsigned char*)(c + 0x51d1 + o) = 2;
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 47 -- func_ov006_0211ad94, 0x0211ad94, size 0xac */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0211ad94
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0211ad94(char* c, int i){
  int n = i * 0x24;
  *(int*)(c + 0x51b0 + n) = *(int*)(c + 0x51b0 + n) + *(int*)(c + 0x5000 + n + 0x1b8);
  *(int*)(c + 0x51b4 + n) = *(int*)(c + 0x51b4 + n) + *(int*)(c + 0x51bc + n);
  *(int*)(c + 0x51bc + n) = *(int*)(c + 0x51bc + n) + 0x200;
  if (((*(int*)(c + 0x5000 + n + 0x1c0) - *(int*)(c + 0x51b4 + n)) >> 0xc) > 0x20)
    return;
  if (*(int*)(c + 0x51bc + n) > 0) {
    *(unsigned char*)(c + 0x5000 + n + 0x1cc) = 0;
    *(unsigned char*)(c + 0x5000 + n + 0x1cd) = 0;
  }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 46 -- func_ov006_0211ad44, 0x0211ad44, size 0x50 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0211ad44
extern "C" void func_ov006_0211ad44(char *base, int idx) {
    unsigned char state = *(unsigned char*)((char*)base + idx * 0x24 + 0x51d1);
    (((C*)base)->*data_ov006_02142de0[state].pmf)(idx);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 45 -- func_ov006_0211ad00, 0x0211ad00, size 0x44 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0211ad00
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0211ad00(char *c, int idx) {
    c += idx * 0x24;
    *(int*)(c + 0x51c4) = 0x3000;
    *(unsigned char*)(c + 0x51cd) = 1;
    *(int*)(c + 0x51bc) = -0x1000;
    *(int*)(c + 0x51b8) = data_ov006_0212ee40[*(unsigned char*)(c + 0x51d2)];
    *(unsigned char*)(c + 0x51d1) = 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 44 -- func_ov006_0211ac30, 0x0211ac30, size 0xd0 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0211ac30
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0211ac30(char *self, int idx)
{
    int i = idx * 0x24;
    *(int*)(self + 0x51b0 + i) = *(int*)(self + 0x51b0 + i) + *(int*)(self + 0x51b8 + i);
    *(int*)(self + 0x51b4 + i) = *(int*)(self + 0x51b4 + i) + *(int*)(self + 0x51bc + i);
    if (*(int*)(self + 0x51c4 + i) > 0x800) {
        *(int*)(self + 0x51c4 + i) = *(int*)(self + 0x51c4 + i) - 0x100;
        if (*(int*)(self + 0x51c4 + i) < 0x800)
            *(int*)(self + 0x51c4 + i) = 0x800;
    }
    *(int*)(self + 0x51bc + i) = *(int*)(self + 0x51bc + i) - 0x20;
    {
        int d = (*(int*)(self + 0x51c0 + i) - *(int*)(self + 0x51b4 + i)) >> 12;
        if (d >= 0x38) {
            *(unsigned char*)(self + 0x51cc + i) = 0;
            *(unsigned char*)(self + 0x51cd + i) = 0;
        }
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 43 -- func_ov006_0211ac2c, 0x0211ac2c, size 0x4 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0211ac2c
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0211ac2c(void)
{
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 42 -- func_ov006_0211abdc, 0x0211abdc, size 0x50 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0211abdc
extern "C" void func_ov006_0211abdc(char *base, int idx) {
    unsigned char state = *(unsigned char*)((char*)base + idx * 0x24 + 0x51d1);
    (((C*)base)->*data_ov006_02142cd8[state].pmf)(idx);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 41 -- func_ov006_0211ab80, 0x0211ab80, size 0x5c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0211ab80
/* func_ov006_0211ab80 at 0x0211ab80
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov006).
 */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0211ab80(char *r0, int idx) {
    int n = idx * 0x24;
    char *base = r0 + 0x51b0;
    char *p = r0 + n;
    *(unsigned char *)(p + 0x51cd) = 1;
    unsigned char k = *(unsigned char *)(p + 0x51d2);
    *(int *)(base + n) = *(int *)(base + n) + data_ov006_0212ee7c[k];
    *(int *)(p + 0x51bc) = -0x4000;
    *(unsigned char *)(p + 0x51d1) = 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 40 -- func_ov006_0211ab0c, 0x0211ab0c, size 0x74 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0211ab0c
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0211ab0c(char* c, int i)
{
    int off = i * 0x24;
    *(int*)(c + 0x51b4 + off) = *(int*)(c + 0x51b4 + off) + *(int*)(c + 0x51bc + off);
    *(int*)(c + 0x51bc + off) = *(int*)(c + 0x51bc + off) + 0x400;
    if (*(int*)(c + 0x51bc + off) > 0) {
        *(int*)(c + 0x51bc + off) = 0;
        *(unsigned char*)(c + off + 0x5000 + 0x1d1) = 2;
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 39 -- func_ov006_0211aa94, 0x0211aa94, size 0x78 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0211aa94
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0211aa94(BigObj *self, int idx) {
    self->arr[idx].b4 += self->arr[idx].bc;
    self->arr[idx].bc += 0x100;
    if (((self->arr[idx].c0 - self->arr[idx].b4) >> 12) <= 0x18) {
        self->arr[idx].cc = 0;
        self->arr[idx].cd = 0;
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 38 -- func_ov006_0211aa44, 0x0211aa44, size 0x50 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0211aa44
extern "C" void func_ov006_0211aa44(char *base, int idx) {
    unsigned char state = *(unsigned char*)((char*)base + idx * 0x24 + 0x51d1);
    (((C*)base)->*data_ov006_02142cf0[state].pmf)(idx);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 37 -- func_ov006_0211a9fc, 0x0211a9fc, size 0x48 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0211a9fc
/* func_ov006_0211a9fc at 0x0211a9fc
 *
 * Activates entry i of a 0x24-stride array at this+0x51b8: sets flags,
 * clears counters, and loads its duration from data_ov006_0212ee70[type].
 */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0211a9fc(struct NoteObj *self, int i)
{
    self->entries[i].flagA = 1;
    self->entries[i].counter = 0;
    self->entries[i].value = data_ov006_0212ee70[self->entries[i].type];
    self->entries[i].flagB = 1;
    self->entries[i].halfword = 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 36 -- func_ov006_0211a910, 0x0211a910, size 0xec */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0211a910
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0211a910(char* c, int i)
{
    int n = i * 0x24;
    int v;
    *(int*)(c + 0x51b0 + n) = *(int*)(c + 0x51b0 + n) + *(int*)(c + 0x51b8 + n);
    *(int*)(c + 0x51b4 + n) = *(int*)(c + 0x51b4 + n) + *(int*)(c + 0x51bc + n);
    v = *(int*)(c + 0x51b8 + n);
    if (v > 0x20) {
        *(int*)(c + 0x51b8 + n) = v - 0x20;
    } else if (v < -0x20) {
        *(int*)(c + 0x51b8 + n) = v + 0x20;
    }
    *(int*)(c + 0x51bc + n) = *(int*)(c + 0x51bc + n) - 0x40;
    *(unsigned short*)(c + 0x51c8 + n) = *(unsigned short*)(c + 0x51c8 + n) + 1;
    if (*(unsigned short*)(c + 0x51c8 + n) >= 0x28) {
        *(unsigned short*)(c + 0x51c8 + n) = 0;
        *(unsigned char*)(c + n + 0x5000 + 0x1d1) = 2;
        *(unsigned char*)(c + n + 0x5000 + 0x1d3) = 0;
        *(int*)(c + 0x51b8 + n) = 0xc00;
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 35 -- func_ov006_0211a7fc, 0x0211a7fc, size 0x114 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0211a7fc
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0211a7fc(char *c, int idx)
{
    int n = idx * 0x24;
    int f;
    *(int *)(c + 0x51b0 + n) = *(int *)(c + 0x51b0 + n) + *(int *)(c + 0x51b8 + n);
    *(int *)(c + 0x51b4 + n) = *(int *)(c + 0x51b4 + n) + *(int *)(c + 0x51bc + n);
    *(int *)(c + 0x51bc + n) = *(int *)(c + 0x51bc + n) - 0x40;
    f = *(unsigned char *)(c + 0x51d3 + n);
    if (f == 0) {
        *(int *)(c + 0x51b8 + n) -= 0x100;
        if (*(int *)(c + 0x51b8 + n) < -0xc00) {
            *(int *)(c + 0x51b8 + n) = -0xc00;
            *(unsigned char *)(c + 0x51d3 + n) = 1;
        }
    } else if (f != 0) {
        *(int *)(c + 0x51b8 + n) += 0x100;
        if (*(int *)(c + 0x51b8 + n) > 0xc00) {
            *(int *)(c + 0x51b8 + n) = 0xc00;
            *(unsigned char *)(c + 0x51d3 + n) = 0;
        }
    }
    if ((*(int *)(c + 0x51c0 + n) - *(int *)(c + 0x51b4 + n)) >> 12 >= 0x30) {
        *(unsigned char *)(c + 0x51cc + n) = 0;
        *(unsigned char *)(c + 0x51cd + n) = 0;
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 34 -- func_ov006_0211a7ac, 0x0211a7ac, size 0x50 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0211a7ac
extern "C" void func_ov006_0211a7ac(char *base, int idx) {
    unsigned char state = *(unsigned char*)((char*)base + idx * 0x24 + 0x51d1);
    (((C*)base)->*data_ov006_02142d20[state].pmf)(idx);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 33 -- func_ov006_0211a714, 0x0211a714, size 0x98 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0211a714
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0211a714(char* c, int i) {
  int idx = i * 0x24;
  unsigned short v = *(unsigned short*)(c + 0x51c8 + idx);
  if (v != 0) {
    *(short*)(c + 0x51c8 + idx) = v - 1;
    if (*(short*)(c + 0x51c8 + idx) < 0)
      *(short*)(c + 0x51c8 + idx) = 0;
    return;
  }
  *(unsigned char*)(c + 0x51cd + idx) = 1;
  *(int*)(c + 0x51b0 + idx) += data_ov006_0212ee94[*(unsigned char*)(c + 0x51d2 + idx)];
  *(int*)(c + 0x51bc + idx) = -0x4000;
  *(unsigned char*)(c + 0x51d1 + idx) = 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 32 -- func_ov006_0211a69c, 0x0211a69c, size 0x78 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0211a69c
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0211a69c(char *c, int i)
{
    int off = i * 0x24;
    char *pA = c + 0x51bc;
    char *pB = c + 0x51b4;
    int a = *(int *)(pA + off);
    int b = *(int *)(pB + off);
    *(int *)(pB + off) = b + a;
    a = *(int *)(pA + off);
    *(int *)(pA + off) = a + 0x400;
    {
        int d = *(int *)(c + off + 0x51c0) - *(int *)(pB + off);
        d >>= 12;
        if (d >= 0x20) {
            *(unsigned char *)(c + off + 0x51cc) = 0;
            *(unsigned char *)(c + off + 0x51cd) = 0;
        }
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 31 -- func_ov006_0211a698, 0x0211a698, size 0x4 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0211a698
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0211a698(void)
{
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 30 -- func_ov006_0211a648, 0x0211a648, size 0x50 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0211a648
extern "C" void func_ov006_0211a648(char *base, int idx) {
    unsigned char state = *(unsigned char*)((char*)base + idx * 0x24 + 0x51d1);
    (((C*)base)->*data_ov006_02142d68[state].pmf)(idx);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 29 -- func_ov006_0211a5ec, 0x0211a5ec, size 0x5c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0211a5ec
/* func_ov006_0211a5ec at 0x0211ab80
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov006).
 */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0211a5ec(char *r0, int idx) {
    int n = idx * 0x24;
    char *base = r0 + 0x51b0;
    char *p = r0 + n;
    *(unsigned char *)(p + 0x51cd) = 1;
    unsigned char k = *(unsigned char *)(p + 0x51d2);
    *(int *)(base + n) = *(int *)(base + n) + data_ov006_0212eedc[k];
    *(int *)(p + 0x51bc) = -0x4000;
    *(unsigned char *)(p + 0x51d1) = 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 28 -- func_ov006_0211a578, 0x0211a578, size 0x74 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0211a578
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0211a578(char* c, int i)
{
    int off = i * 0x24;
    *(int*)(c + 0x51b4 + off) = *(int*)(c + 0x51b4 + off) + *(int*)(c + 0x51bc + off);
    *(int*)(c + 0x51bc + off) = *(int*)(c + 0x51bc + off) + 0x400;
    if (*(int*)(c + 0x51bc + off) > 0) {
        *(int*)(c + 0x51bc + off) = 0;
        *(unsigned char*)(c + off + 0x5000 + 0x1d1) = 2;
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 27 -- func_ov006_0211a500, 0x0211a500, size 0x78 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0211a500
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0211a500(BallObj *self, int idx) {
    self->arr[idx].a += self->arr[idx].b;
    self->arr[idx].b -= 0x60;
    if ((self->arr[idx].c - self->arr[idx].a) >> 0xc >= 0x3c) {
        self->arr[idx].f18 = 0;
        self->arr[idx].f19 = 0;
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 26 -- func_ov006_0211a4b0, 0x0211a4b0, size 0x50 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0211a4b0
extern "C" void func_ov006_0211a4b0(char *o, int i)
{
    unsigned char idx = *(unsigned char *)(o + i * 0x24 + 0x51d1);
    (((C *)o)->*data_ov006_02142d98[idx])(i);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 25 -- func_ov006_0211a420, 0x0211a420, size 0x90 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0211a420
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0211a420(char *c, int i)
{
  int o = i * 0x24;
  unsigned char idx;
  char *bIdx = c + 0x51d2;
  char *bVal;
  char *b50 = (c + o) + 0x5000;
  *((unsigned char *) (b50 + 0x1cd)) = 1;
  bVal = c + 0x51b0;
  idx = *((unsigned char *) (bIdx + o));
  *((int *) (bVal + o)) = (*((int *) (bVal + o))) + data_ov006_0212ee64[idx];
  if ((o && o) && o)
  {
  }
  if (1)
  {
    *((int *) (((c + o) + 0x5000) + 0x1bc)) = -0x3800;
    *((int *) (((c + o) + 0x5000) + 0x1b8)) = data_ov006_0212ef00[*((unsigned char *) (bIdx + o))];
    *((short *) (0x51c8 + (c + o))) = 0;
    *((unsigned char *) (((c + o) + 0x5000) + 0x1d1)) = 1;
  }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 24 -- func_ov006_0211a388, 0x0211a388, size 0x98 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0211a388
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0211a388(char* c, int i) {
  int idx = i * 0x24;
  *(int*)(c + 0x51b0 + idx) += *(int*)(c + 0x51b8 + idx);
  *(int*)(c + 0x51b4 + idx) += *(int*)(c + 0x51bc + idx);
  *(int*)(c + 0x51bc + idx) += 0x200;
  if (*(int*)(c + 0x51bc + idx) <= 0) return;
  *(int*)(c + 0x51bc + idx) = 0;
  *(unsigned char*)(c + 0x51d1 + idx) = 2;
  *(short*)(c + 0x51c8 + idx) = 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 23 -- func_ov006_0211a314, 0x0211a314, size 0x74 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0211a314
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0211a314(char* c, int i)
{
    int off = i * 0x24;
    int v = *(int*)(c + 0x51c4 + off);
    if (v > 0x800) {
        *(int*)(c + 0x51c4 + off) = v - 0x60;
        if (*(int*)(c + 0x51c4 + off) < 0x800)
            *(int*)(c + 0x51c4 + off) = 0x800;
    }
    *(unsigned short*)(c + 0x51c8 + off) = *(unsigned short*)(c + 0x51c8 + off) + 1;
    if (*(unsigned short*)(c + 0x51c8 + off) >= 0x20) {
        *(unsigned char*)(c + off + 0x5000 + 0x1cc) = 0;
        *(unsigned char*)(c + off + 0x5000 + 0x1cd) = 0;
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 22 -- func_ov006_0211a2c4, 0x0211a2c4, size 0x50 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0211a2c4
extern "C" void func_ov006_0211a2c4(C* c, int i){
  unsigned char sel = *((unsigned char*)((char*)c + i*0x24 + 0x51d1));
  (c->*data_ov006_02142cc0[sel])(i);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 21 -- func_ov006_0211a234, 0x0211a234, size 0x90 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0211a234
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0211a234(char *c, int i)
{
  int o = i * 0x24;
  char *bIdx = c + 0x51d2;
  char *bVal = c + 0x51b0;
  char *b50;
  unsigned char idx;
  *((unsigned char *) (((c + o) + 0x5000) + 0x1cd)) = 1;
  idx = *((unsigned char *) (bIdx + o));
  *((int *) (bVal + o)) = (*((int *) (bVal + o))) + data_ov006_0212ee58[idx];
  if ((i && i) != 0)
  {
  }
  *((int *) (((c + o) + 0x5000) + 0x1bc)) = -0x3200;
  *((int *) (((c + o) + 0x5000) + 0x1b8)) = data_ov006_0212ee4c[*((unsigned char *) (bIdx + o))];
  *((short *) ((c + o) + 0x51c8)) = 0;
  *((unsigned char *) (((c + o) + 0x5000) + 0x1d1)) = 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 20 -- func_ov006_0211a19c, 0x0211a19c, size 0x98 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0211a19c
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0211a19c(char* c, int i) {
  int idx = i * 0x24;
  *(int*)(c + 0x51b0 + idx) += *(int*)(c + 0x51b8 + idx);
  *(int*)(c + 0x51b4 + idx) += *(int*)(c + 0x51bc + idx);
  *(int*)(c + 0x51bc + idx) += 0x200;
  if (*(int*)(c + 0x51bc + idx) <= 0) return;
  *(int*)(c + 0x51bc + idx) = 0;
  *(unsigned char*)(c + 0x51d1 + idx) = 2;
  *(short*)(c + 0x51c8 + idx) = 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 19 -- func_ov006_0211a128, 0x0211a128, size 0x74 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0211a128
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0211a128(char* c, int i)
{
    int off = i * 0x24;
    int v = *(int*)(c + 0x51c4 + off);
    if (v > 0x800) {
        *(int*)(c + 0x51c4 + off) = v - 0x70;
        if (*(int*)(c + 0x51c4 + off) < 0x800)
            *(int*)(c + 0x51c4 + off) = 0x800;
    }
    *(unsigned short*)(c + 0x51c8 + off) = *(unsigned short*)(c + 0x51c8 + off) + 1;
    if (*(unsigned short*)(c + 0x51c8 + off) >= 0x20) {
        *(unsigned char*)(c + off + 0x5000 + 0x1cc) = 0;
        *(unsigned char*)(c + off + 0x5000 + 0x1cd) = 0;
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 18 -- func_ov006_0211a0d8, 0x0211a0d8, size 0x50 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0211a0d8
extern "C" void func_ov006_0211a0d8(CFull* c, int i){
  (c->*(data_ov006_02142d08[c->arr[i].idx].pmf))(i);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 17 -- func_ov006_0211a048, 0x0211a048, size 0x90 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0211a048
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0211a048(char *c, int idx)
{
  int off = idx * 0x24;
  *((unsigned char *) (((c + off) + 0x5000) + 0x1cd)) = 1;
  *((int *) ((c + 0x51b0) + off)) = (*((int *) ((c + 0x51b0) + off))) + data_ov006_0212eeac[*((unsigned char *) ((c + 0x51d2) + off))];
  if (off && off)
  {
  }
  *((int *) (((c + off) + 0x5000) + 0x1bc)) = -0x2800;
  *((int *) (((c + off) + 0x5000) + 0x1b8)) = data_ov006_0212ee88[*((unsigned char *) ((c + 0x51d2) + off))];
  *((short *) (((c + off) + 0x5100) + 0xc8)) = 0;
  *((unsigned char *) (((c + off) + 0x5000) + 0x1d1)) = 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 16 -- func_ov006_02119fb0, 0x02119fb0, size 0x98 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_02119fb0
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_02119fb0(char* c, int i) {
  int idx = i * 0x24;
  *(int*)(c + 0x51b0 + idx) += *(int*)(c + 0x51b8 + idx);
  *(int*)(c + 0x51b4 + idx) += *(int*)(c + 0x51bc + idx);
  *(int*)(c + 0x51bc + idx) += 0x200;
  if (*(int*)(c + 0x51bc + idx) <= 0) return;
  *(int*)(c + 0x51bc + idx) = 0;
  *(unsigned char*)(c + 0x51d1 + idx) = 2;
  *(short*)(c + 0x51c8 + idx) = 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 15 -- func_ov006_02119f3c, 0x02119f3c, size 0x74 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_02119f3c
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_02119f3c(char* c, int i)
{
    int off = i * 0x24;
    int v = *(int*)(c + 0x51c4 + off);
    if (v > 0xa80) {
        *(int*)(c + 0x51c4 + off) = v - 0x60;
        if (*(int*)(c + 0x51c4 + off) < 0xa80)
            *(int*)(c + 0x51c4 + off) = 0xa80;
    }
    *(unsigned short*)(c + 0x51c8 + off) = *(unsigned short*)(c + 0x51c8 + off) + 1;
    if (*(unsigned short*)(c + 0x51c8 + off) >= 0x18) {
        *(unsigned char*)(c + off + 0x5000 + 0x1cc) = 0;
        *(unsigned char*)(c + off + 0x5000 + 0x1cd) = 0;
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 14 -- func_ov006_02119eec, 0x02119eec, size 0x50 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_02119eec
extern "C" void func_ov006_02119eec(CFull* c, int i){
  (c->*(data_ov006_02142db0[c->arr[i].idx].pmf))(i);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 13 -- func_ov006_02119e5c, 0x02119e5c, size 0x90 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_02119e5c
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_02119e5c(char *c, int i)
{
  int o = i * 0x24;
  char *bIdx = c + 0x51d2;
  char *bVal = c + 0x51b0;
  char *b50 = (c + o) + 0x5000;
  unsigned char idx;
  *((unsigned char *) (((c + (o & 0xFFFFFFFFFFFFFFFFu)) + 0x5000) + 0x1cd)) = 1;
  idx = *((unsigned char *) (bIdx + o));
  *((int *) (bVal + o)) = (*((int *) (bVal + o))) + data_ov006_0212ef30[idx];
  *((int *) (((c + o) + 0x5000) + 0x1bc)) = -0x2400;
  *((int *) (b50 + 0x1b8)) = data_ov006_0212ef0c[*((unsigned char *) (bIdx + o))];
  *((short *) ((c + o) + 0x51c8)) = 0;
  *((unsigned char *) (((c + o) + 0x5000) + 0x1d1)) = 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 12 -- func_ov006_02119dc4, 0x02119dc4, size 0x98 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_02119dc4
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_02119dc4(char* c, int i) {
  int idx = i * 0x24;
  *(int*)(c + 0x51b0 + idx) += *(int*)(c + 0x51b8 + idx);
  *(int*)(c + 0x51b4 + idx) += *(int*)(c + 0x51bc + idx);
  *(int*)(c + 0x51bc + idx) += 0x200;
  if (*(int*)(c + 0x51bc + idx) <= 0) return;
  *(int*)(c + 0x51bc + idx) = 0;
  *(unsigned char*)(c + 0x51d1 + idx) = 2;
  *(short*)(c + 0x51c8 + idx) = 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 11 -- func_ov006_02119d50, 0x02119d50, size 0x74 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_02119d50
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_02119d50(char* c, int i)
{
    int off = i * 0x24;
    int v = *(int*)(c + 0x51c4 + off);
    if (v > 0xd80) {
        *(int*)(c + 0x51c4 + off) = v - 0x60;
        if (*(int*)(c + 0x51c4 + off) < 0xd80)
            *(int*)(c + 0x51c4 + off) = 0xd80;
    }
    *(unsigned short*)(c + 0x51c8 + off) = *(unsigned short*)(c + 0x51c8 + off) + 1;
    if (*(unsigned short*)(c + 0x51c8 + off) >= 0x18) {
        *(unsigned char*)(c + off + 0x5000 + 0x1cc) = 0;
        *(unsigned char*)(c + off + 0x5000 + 0x1cd) = 0;
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 10 -- func_ov006_02119c74, 0x02119c74, size 0xdc */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_02119c74
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov004_020b023c(void* a0, int a1, int a2, int a3, void* a4);

void func_ov006_02119c74(void* pv)
{
    char* p=(char*)pv;
    int i;
    int tb = data_02082214[0];
    long long ta = data_02082214[1];

    for (i = 0; i < 30; i++) {
        if (*(unsigned char*)(p + 0x51cd)) {
            int m[4];
            long long vv = *(int*)(p + 0x51c4);
            int x = *(int*)(p + 0x51b0);
            int y = *(int*)(p + 0x51b4);
            int a = (int)((ta * vv + 0x800) >> 12);
            int b = (int)(((long long)tb * (int)vv + 0x800) >> 12);
            m[0] = a;
            m[3] = a;
            m[1] = b;
            m[2] = -b;
            func_ov004_020b023c(data_ov006_0213f6f0[*(unsigned char*)(p + 0x51ce)],
                                (x >> 12) - 8, (y >> 12) - 0x10, -1, m);
        }
        tb = data_02082214[0];
        p += 0x24;
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 9 -- func_ov006_02119bdc, 0x02119bdc, size 0x98 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_02119bdc
extern "C" {
extern void* data_ov006_0213f730[];
void func_ov006_02119bdc(void* pv) {
  char* c=(char*)pv;
  int i;
  char* o = c;
  for (i = 0; i < 10; i++) {
    if (*(unsigned char*)(o + 0x50f6) != 0 && *(unsigned char*)(o + 0x50f5) != 0) {
      int a1 = *(int*)(o + 0x50e8) >> 12;
      int a2 = *(int*)(o + 0x50ec) >> 12;
      int a4 = 0;
      if (*(int*)(c + 0x5608) != 1) a4 = 1;
      unsigned char sel = *(unsigned char*)(o + 0x50f7);
      Hud_RenderSprite((int)data_ov006_0213f730[sel], a1, a2, -1, a4);
    }
    o += 0x14;
  }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- func_ov006_02119bc4, 0x02119bc4, size 0x18 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_02119bc4
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_02119bc4(void *c)
{
    func_ov004_020b2574(*(unsigned char *)((char *)c + 0x5000 + 0x626), 1);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- func_ov006_02119ba4, 0x02119ba4, size 0x20 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_02119ba4
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_02119ba4(char *p)
{
    *(char *)(p + 0x55f4) = 1;
    *(short *)(p + 0x55f0) = 0;
    *(char *)(p + 0x55f6) = 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- func_ov006_02119b00, 0x02119b00, size 0xa4 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_02119b00
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_02119b00(char *o)
{
    if (*(u8 *)(o + 0x55f4) == 0)
        return;
    if (*(u8 *)(o + 0x55f6) >= 2)
        return;
    {
        u16 *p = (u16 *)(((int)o + 0x55f0));
        *p = *p + 1;
    }
    if (*(u16 *)(o + 0x55f0) < data_ov006_0212ee28[*(u8 *)(o + 0x55f6)])
        return;
    *(u16 *)(o + 0x55f0) = 0;
    {
        u8 *q = (u8 *)(((int)o + 0x55f6));
        *q = *q + 1;
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- func_ov006_02119aa8, 0x02119aa8, size 0x58 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_02119aa8
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_02119aa8(void*pv){
  char*c=(char*)pv;
  if(*(unsigned char*)(c+0x5000+0x5f5)==0) return;
  int x=*(int*)(c+0x5000+0x5e8);
  int y=*(int*)(c+0x5000+0x5ec);
  Hud_RenderSprite(data_ov006_02138d28[*(unsigned char*)(c+0x5000+0x5f6)],
    x>>0xc, y>>0xc, -1, -1);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- func_ov006_02119a88, 0x02119a88, size 0x20 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_02119a88
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_02119a88(char *p)
{
    *(char *)(p + 0x5604) = 1;
    *(short *)(p + 0x5600) = 0;
    *(char *)(p + 0x5606) = 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- func_ov006_02119a18, 0x02119a18, size 0x70 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_02119a18
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_02119a18(char *b)
{
    if (*(unsigned char *)(b + 0x5604) == 0) return;
    if (*(unsigned char *)(b + 0x5606) >= 3) return;
    *(unsigned short *)A(b + 0x5600) += 1;
    if (*(unsigned short *)(b + 0x5600) < data_ov006_0212ee38[*(unsigned char *)(b + 0x5606)]) return;
    *(unsigned short *)(b + 0x5600) = 0;
    *(unsigned char *)A(b + 0x5606) += 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- func_ov006_021199c0, 0x021199c0, size 0x58 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_021199c0
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_021199c0(void*pv){
  char*c=(char*)pv;
  if(*(unsigned char*)(c+0x5000+0x605)==0) return;
  int x=*(int*)(c+0x5000+0x5f8);
  int y=*(int*)(c+0x5000+0x5fc);
  Hud_RenderSprite(data_ov006_02137ae8[*(unsigned char*)(c+0x5000+0x606)],
    x>>0xc, y>>0xc, -1, -1);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN12dScMgSound_cD0Ev, 0x02119958, size 0x68 */
/* ROM ordinal 0 -- _ZN12dScMgSound_cD1Ev, 0x02119904, size 0x54 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12dScMgSound_cD1Ev
// @symbol _ZN12dScMgSound_cD0Ev
/* BOTH DESTRUCTOR VARIANTS COME FROM THE ONE INLINE BODY in
   include/dScMgSound_c.h. The pre-migration tree carried two identical
   out-of-line definitions, one per shard, because a one-function file has no
   other way to emit its variant; merged they are a duplicate definition AND
   the wrong order. mwccarm 2004/b56 emits D1 then D0 for an in-class body,
   which is the order ov006 uses -- D1 at 0x02119904 below D0 at 0x02119958.
   The body itself is unchanged: `func_ov006_020c3288((char *)mTable)`. */
