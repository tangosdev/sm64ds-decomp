//cpp
/* dScMgTeresa_c -- the Boo (Teresa) hide-and-seek minigame scene, ov006.
 *
 * Reconstructed translation unit: ROM ordinals 0..47 of the contiguous linker
 * run 0x0211cbd0..0x021207a8, i.e. 0x0211cbd0..0x0211e72c, 48 functions.
 * Assembled from the 48 one-function legacy sources, then reconciled by hand.
 *
 * WHY THIS IS A SUB-RANGE: a sourceless hole splits the run.  ROM ordinal 48,
 * func_ov006_0211e72c (0x0211e72c, size 0xac), has no source anywhere in the
 * tree -- it is a banked near-miss (config/match_attempts.jsonl, floor 26
 * divergences, reconfirmed by four independent model runs) and carries no
 * entry in config/arm9/overlays/ov006/delinks.txt at all, so the cartridge's
 * own bytes cover that range.  It sits in the MIDDLE of the run, and nothing
 * in this tree can express a .text claim with a hole in it.  The run therefore
 * has to be licensed as one of its two contiguous sides; this file is the
 * larger one, ordinals 0..47 (48 functions against 32 on the far side).  The
 * 32 above the hole keep their own shards and their own delinks.txt entries.
 *
 * FUNCTION ORDER IS THE ROM'S OWN, LOWEST ADDRESS FIRST, and that is one
 * decision with `#pragma defer_codegen off` below.  With codegen deferred
 * (the default) mwccarm 2004/b56 emits one .text section per function in the
 * REVERSE of source order; generating at parse time emits them in source
 * order.  Flipping either alone fails linkcheck [4b/8]'s ROM-ascending
 * emission-order audit.  Do not reorder.
 *
 * THREE MEMBERS CARRY BRACKETED OPTIMISATION PRAGMAS, AND THEY ARE REAL.
 * func_ov006_0211d924 and func_ov006_0211db7c want `opt_common_subs off` and
 * func_ov006_0211e220 wants `opt_strength_reduction off`; each carries its own
 * `#pragma push` / `#pragma pop` bracket.  Measured, not assumed, and the
 * delete-outright control was run first: delete all three from this file and
 * nothing else, and it drops from 48/48 to 45/48 with the DIFFs falling on
 * exactly those three members.  Keep the brackets but delete only
 * `defer_codegen off` and it is 45/48 again AND the emission order inverts to
 * "47 ordinal pair(s) NOT in ROM order" -- the brackets bind only while codegen
 * is generated at parse time.  One pragma buys the bytes and the order.
 *
 * THE DESTRUCTOR IS OUT OF LINE, AND THAT IS MEASURED, NOT INHERITED.  The
 * cartridge orders this class D1 (0x0211cbd0) BELOW D0 (0x0211cbf4).  The
 * received rule is that only an inline-in-class destructor gives D1-then-D0
 * and an out-of-line one gives D2/D0/D1 -- that rule holds only while codegen
 * is deferred.  Measured on this compiler, both ways, with the identical
 * header: out-of-line + deferred emits D2, D0, D1; out-of-line +
 * `defer_codegen off` emits D1, D0, D2, which is the cartridge's order with
 * the homeless D2 trailing where it costs nothing (it is deadstripped).  An
 * inline destructor is NOT usable here for a different reason: with it, the
 * class's key function becomes InitResources, which lies above the hole, so
 * this TU would emit no vtable, nothing would odr-use the inline destructor,
 * and D1/D0 would not be emitted at all (verified with a probe TU that
 * includes an inline-destructor header and emits one section, no destructor).
 * So include/dScMgTeresa_c.h needs NO edit: its out-of-line declaration is
 * what this TU wants, and it is what the still-enrolled shards above the hole
 * want too.
 *
 * Because the destructor is out of line, this TU owns the class's key
 * function and mwcc emits the whole inheritance chain's vtable and typeinfo
 * as vague-linkage passengers -- see the manifest's compiler_only_output
 * block, twelve rows: eleven RTTI/vtable data symbols (2 x 5 levels + 1
 * vtable) plus the homeless _ZN13dScMgTeresa_cD2Ev.
 *
 * Promoted members in this TU (ROM address order):
 *   [0]  0x0211cbd0  _ZN13dScMgTeresa_cD1Ev
 *   [1]  0x0211cbf4  _ZN13dScMgTeresa_cD0Ev
 *   [2]  0x0211cc2c  func_ov006_0211cc2c
 *   [3]  0x0211cc90  func_ov006_0211cc90
 *   [4]  0x0211cca8  func_ov006_0211cca8
 *   [5]  0x0211cd24  func_ov006_0211cd24
 *   [6]  0x0211ce90  func_ov006_0211ce90
 *   [7]  0x0211ce94  func_ov006_0211ce94
 *   [8]  0x0211cef4  func_ov006_0211cef4
 *   [9]  0x0211d018  func_ov006_0211d018
 *   [10] 0x0211d0f8  func_ov006_0211d0f8
 *   [11] 0x0211d224  func_ov006_0211d224
 *   [12] 0x0211d368  func_ov006_0211d368
 *   [13] 0x0211d4e8  func_ov006_0211d4e8
 *   [14] 0x0211d5a8  func_ov006_0211d5a8
 *   [15] 0x0211d608  func_ov006_0211d608
 *   [16] 0x0211d688  func_ov006_0211d688
 *   [17] 0x0211d69c  func_ov006_0211d69c
 *   [18] 0x0211d75c  func_ov006_0211d75c
 *   [19] 0x0211d7b0  func_ov006_0211d7b0
 *   [20] 0x0211d7b4  func_ov006_0211d7b4
 *   [21] 0x0211d7d8  func_ov006_0211d7d8
 *   [22] 0x0211d7ec  func_ov006_0211d7ec
 *   [23] 0x0211d86c  func_ov006_0211d86c
 *   [24] 0x0211d924  func_ov006_0211d924
 *   [25] 0x0211dad0  func_ov006_0211dad0
 *   [26] 0x0211db7c  func_ov006_0211db7c
 *   [27] 0x0211dce0  func_ov006_0211dce0
 *   [28] 0x0211dd0c  func_ov006_0211dd0c
 *   [29] 0x0211dd6c  func_ov006_0211dd6c
 *   [30] 0x0211ddb8  func_ov006_0211ddb8
 *   [31] 0x0211ddcc  func_ov006_0211ddcc
 *   [32] 0x0211de54  func_ov006_0211de54
 *   [33] 0x0211de7c  func_ov006_0211de7c
 *   [34] 0x0211dec0  func_ov006_0211dec0
 *   [35] 0x0211e020  func_ov006_0211e020
 *   [36] 0x0211e0c8  func_ov006_0211e0c8
 *   [37] 0x0211e118  func_ov006_0211e118
 *   [38] 0x0211e184  func_ov006_0211e184
 *   [39] 0x0211e220  func_ov006_0211e220
 *   [40] 0x0211e29c  func_ov006_0211e29c
 *   [41] 0x0211e318  func_ov006_0211e318
 *   [42] 0x0211e3e0  func_ov006_0211e3e0
 *   [43] 0x0211e460  func_ov006_0211e460
 *   [44] 0x0211e4e0  func_ov006_0211e4e0
 *   [45] 0x0211e55c  func_ov006_0211e55c
 *   [46] 0x0211e5cc  func_ov006_0211e5cc
 *   [47] 0x0211e658  func_ov006_0211e658
 */

#pragma defer_codegen off

#include "types.h"
#include "OamAttr.h"
#include "dScMgTeresa_c.h"
#include "decl_common.h"

/* ---------------------------------------------------------------------------
 * Shadow types.  These are the legacy files' own local views of objects whose
 * real types are not recovered.  Three different legacy files each called
 * their view `struct C` and the three disagree offset-for-offset, so they are
 * renamed apart here rather than merged; nothing else about them changed.
 * ------------------------------------------------------------------------- */

/* func_ov006_0211ce94's view of the per-Boo element array at +0x4bd4. */
struct BooElem {
    int a;
    int b;
    char pad0[5];
    unsigned char c;
    unsigned char d;
    char pad1[0x1c - 0xf];
};
struct BooArray {
    char pad[0x4bd4];
    struct BooElem member[1];
};

/* func_ov006_0211d5a8's `struct C`: the flag at +0x4be0 and the state index at
   +0x4be1, called through the PMF table data_ov006_02142f18. */
struct TeresaPmfA;
typedef void (TeresaPmfA::*PmfA)(int);
struct TeresaPmfA { char pad[0x4be0]; unsigned char g; unsigned char idx; };

/* func_ov006_0211dd0c's `struct C`: a DIFFERENT pair -- index at +0x4bba and
   flag at +0x4bbc -- driving a second PMF table, data_ov006_02142e88. */
struct TeresaPmfB;
typedef void (TeresaPmfB::*PmfB)(int);
struct TeresaPmfB { char pad[0x4bba]; unsigned char idx; char gap; unsigned char g; };

/* func_ov006_0211e0c8's `struct C`: a 0x24-stride row walk, no fields named. */
struct Row { char pad[0x24]; };
struct RowArray { struct Row rows[1]; };

/* func_ov006_0211dad0's view of the 0x20-stride element pair at +0x4bac. */
typedef struct SlotElem {
    int word0;
    short pad04;
    unsigned short cnt;
    unsigned short h08;
    short pad0a;
    unsigned char b0c;
    unsigned char pad0d;
    unsigned char b0e;
    unsigned char pad0f;
    unsigned char pad10;
    unsigned char b11;
    unsigned char pad12[0xe];
} SlotElem;
typedef struct S {
    char pad[0x4bac];
    SlotElem arr[2];
} S;

/* func_ov006_0211e118's view of the 0x10-stride HUD sprite array at +0x4960.
   Named apart from SlotElem, which the legacy files also spelled `Elem`. */
struct HudElem {
    int x;
    int y;
    int unk8;
    unsigned char idx;
    unsigned char unkd;
    unsigned char flag;
    unsigned char unkf;
};
struct HudArray {
    char pad[0x4960];
    struct HudElem arr[16];
};

/* func_ov006_0211e220's 0x10-stride step over the same array. */
struct E29 { unsigned char b[0x10]; };

/* func_ov006_0211e5cc's two windows onto the same object. */
typedef struct {
    char _pad0[0x4677];
    u8 active;   /* +0x4677 */
    u8 stage;    /* +0x4678 */
} View;
typedef struct {
    char _pad0[0x4c20];
    u8 latch;    /* +0x4c20 */
} Work;

/* ---------------------------------------------------------------------------
 * ROM symbols this TU calls or reads.  Anything decl_common.h already carries
 * is NOT repeated here -- a real header always wins, and repeating a
 * disagreeing spelling is `illegal function overloading` reported against the
 * definition rather than against the header.  decl_common.h supplies
 * SetBg3Offset, Deallocate, _ZN5Sound12PlayBank2_2DEj, data_0209d4b8,
 * func_ov004_020b0cac, func_ov004_020af948, and nine of this TU's own members
 * (see the definitions, which are spelled to match it).
 * ------------------------------------------------------------------------- */
extern "C" {

extern void  RenderOamMainScreen(void*, int, int, int, int);
extern void *LoadFile(int handle);
extern void  DecompressLZ16(const void *src, void *dst);
extern int   RandomIntInternal(int *seed);
extern void  FreeGfxSlotsById(int arg);
extern int   GetGameLanguage(void);
extern void  DrawOamSprite(int, int, int, int);
extern void  Hud_RenderSprite(void *fn, int a, int b, int c, int d);
extern void  func_ov004_020b2220(int, int, int, int, int, int, int);
extern void  _ZN3G2x13SetBlendAlphaEPVttttj(volatile void *p, unsigned short a,
                                            unsigned short b, unsigned short c,
                                            unsigned short d);
extern void  _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(int sub, struct OamAttr *data,
                                                         s32 x, s32 y, s32 palette,
                                                         s32 priority, s32 scaleX,
                                                         s32 scaleY, s32 rotation,
                                                         s32 mode);

extern unsigned char  data_0209d454;
extern unsigned char  data_0209d45c;
extern int            data_ov006_021350fc[];
extern unsigned char  data_ov006_0212efc4[];
extern unsigned short data_ov006_0212efc8[];
extern unsigned char  data_ov006_0212efcc[];
extern unsigned char  data_ov006_0212efd4[];
extern unsigned short data_ov006_0212efdc[];
extern void          *data_ov006_02133a5c;
extern void          *data_ov006_02134210[];
extern int            data_ov006_02135fc8[];
extern int            data_ov006_02137cd8[];
extern void          *data_ov006_0213a5f4;
extern struct OamAttr *data_ov006_0213a628[];
extern int           *data_ov006_0213f9d0[];

/* The two pointer-to-member tables, each typed to its own view of the object. */
extern PmfA data_ov006_02142f18[];
extern PmfB data_ov006_02142e88[];

/* A member of this class that lives ABOVE the sourceless hole and therefore
   keeps its own shard; this TU calls it. */
extern void func_ov006_0211f51c(char *c);

/* ---------------------------------------------------------------------------
 * Forward declarations of this TU's own members.  Source order is ROM order,
 * lowest address first, so a call to a higher-addressed member is a forward
 * reference.  The nine members decl_common.h already declares are omitted --
 * they are declared there and defined below with that header's exact
 * parameter types.
 * ------------------------------------------------------------------------- */
extern void func_ov006_0211cc2c(unsigned char *self);
extern void func_ov006_0211cc90(unsigned char *base);
extern void func_ov006_0211cd24(void *self, int idx);
extern void func_ov006_0211ce90(void);
extern void func_ov006_0211ce94(struct BooArray *base, int index);
extern void func_ov006_0211cef4(char *c, int i);
extern void func_ov006_0211d018(char *base, int idx);
extern void func_ov006_0211d0f8(char *base, int i);
extern void func_ov006_0211d224(char *c, int i);
extern void func_ov006_0211d368(char *obj, int i);
extern void func_ov006_0211d4e8(char *c, int i);
extern void func_ov006_0211d5a8(TeresaPmfA *c);
extern void func_ov006_0211d608(char *c);
extern void func_ov006_0211d688(char *p);
extern void func_ov006_0211d69c(char *obj);
extern void func_ov006_0211d7b0(void);
extern void func_ov006_0211d7d8(char *p);
extern void func_ov006_0211d86c(char *thiz, int idx);
extern void func_ov006_0211d924(char *p, int i);
extern void func_ov006_0211dad0(S *s, int i);
extern void func_ov006_0211db7c(char *c, int i);
extern void func_ov006_0211dce0(char *base, int i);
extern void func_ov006_0211dd0c(TeresaPmfB *c);
extern void func_ov006_0211ddb8(char *p);
extern void func_ov006_0211de54(char *p);
extern int  func_ov006_0211de7c(char *c);
extern void func_ov006_0211dec0(void *arg);
extern void func_ov006_0211e020(char *c, int i);
extern void func_ov006_0211e0c8(RowArray *c);
extern void func_ov006_0211e184(char *base);
extern void func_ov006_0211e220(unsigned char *c, int param);
extern void func_ov006_0211e318(char *c);
extern void func_ov006_0211e3e0(char *c);
extern void func_ov006_0211e4e0(char *base);
extern void func_ov006_0211e55c(char *c, int idx);
extern void func_ov006_0211e5cc(char *c);
extern void func_ov006_0211e658(unsigned char *base);

}  /* extern "C" */


/* ===========================================================================
 * [0] 0x0211cbd0  _ZN13dScMgTeresa_cD1Ev
 * [1] 0x0211cbf4  _ZN13dScMgTeresa_cD0Ev
 *
 * One out-of-line definition, emitting D1, D0 and a homeless D2 in that order
 * under `defer_codegen off`.  No member needs explicit destruction; the whole
 * body is the compiler-generated own-vtable write plus the base D2 call.
 * ========================================================================= */
// @symbol _ZN13dScMgTeresa_cD1Ev
// @symbol _ZN13dScMgTeresa_cD0Ev
dScMgTeresa_c::~dScMgTeresa_c()
{
}


/* [2] 0x0211cc2c */
// @symbol func_ov006_0211cc2c
extern "C" void func_ov006_0211cc2c(unsigned char *self) {
    if (self[0x4c1f]) {
        self[0x4be3] = 3;
    } else {
        self[0x4be3] = 4;
    }
    *(int *)(self + 0x4bcc) = 0x80000;
    *(int *)(self + 0x4bd0) = 0x100000;
    *(int *)(self + 0x4bd8) = -0x1800;
    *(short *)(self + 0x4bdc) = 0;
    self[0x4be4] = 0;
    self[0x4be1] = 8;
    self[0x4be2] = 1;
    self[0x4c1b] = 0;
}


/* [3] 0x0211cc90 */
// @symbol func_ov006_0211cc90
extern "C" void func_ov006_0211cc90(unsigned char *base) {
    base += 0x4000;
    if (base[0xbe0] != 0) {
        base[0xbe5] = 1;
    }
}


/* [4] 0x0211cca8 -- decl_common.h types this one `void*`. */
// @symbol func_ov006_0211cca8
extern "C" void func_ov006_0211cca8(void *c_){
  unsigned char *c = (unsigned char *)c_;
  int idx,v,p,q;
  if(*(unsigned char*)(c+0x4be2)==0) return;
  idx=*(unsigned char*)(c+0x4be3);
  p=*(int*)(c+0x4bcc)>>12;
  q=*(int*)(c+0x4bd0)>>12;
  if(*(unsigned char*)(c+0x4be4)!=0) idx+=4;
  if(*(unsigned char*)(c+0x4be1)==8) v=data_ov006_02135fc8[idx];
  else v=data_ov006_021350fc[idx];
  RenderOamMainScreen((void*)v,p,q,-1,-1);
}


/* [5] 0x0211cd24 */
#define E_W0  (*(int*)((char*)self + 0x4bd0 + off))
#define E_W8  (*(int*)((char*)self + 0x4bd8 + off))
#define E_HC  (*(unsigned short*)((char*)self + 0x4bdc + off))
#define E_B12 (*(u8*)((char*)self + 0x4be2 + off))
#define E_B13 (*(u8*)((char*)self + 0x4be3 + off))
#define FLAG  (*(u8*)((char*)self + 0x4c1f))
// @symbol func_ov006_0211cd24
extern "C" void func_ov006_0211cd24(void* self, int idx) {
    int off = idx * 0x1c;

    if ((E_W0 >> 12) == 0xc0) {
        E_HC++;
        if (FLAG != 0) {
            if (E_B13 >= 3) return;
            if (E_HC < 4) return;
            E_HC = 0;
            E_B13++;
        } else {
            if (E_B13 >= 6) return;
            if (E_HC < 8) return;
            E_HC = 0;
            E_B13++;
        }
    } else {
        E_B12 = 1;
        E_W0 += E_W8;
        E_W8 -= 0x200;
        if ((E_W0 >> 12) > 0xc0) return;
        E_W0 = 0xc0000;
        E_W8 = 0;
        E_HC = 0;
        if (FLAG != 0) E_B13 = 0;
    }
}
#undef E_W0
#undef E_W8
#undef E_HC
#undef E_B12
#undef E_B13
#undef FLAG


/* [6] 0x0211ce90 */
// @symbol func_ov006_0211ce90
extern "C" void func_ov006_0211ce90(void)
{
}


/* [7] 0x0211ce94 */
// @symbol func_ov006_0211ce94
extern "C" void func_ov006_0211ce94(struct BooArray *base, int index) {
    void *data;
    base->member[index].a = 0;
    base->member[index].b = 0;
    base->member[index].d = 0;
    data = LoadFile(0x103);
    DecompressLZ16(data, (void *)0x6400000);
    base->member[index].c = 7;
    Deallocate(data);
}


/* [8] 0x0211cef4 */
// @symbol func_ov006_0211cef4
extern "C" void func_ov006_0211cef4(char *c, int i)
{
    unsigned short *p16 = (unsigned short *)(c + 0x4bdc + i * 0x1c);
    int *sum;
    int *add;
    int v;
    *p16 = *p16 + 1;
    if (*p16 >= 8) {
        unsigned char *p8;
        int z = 0;
        *p16 = z;
        p8 = (unsigned char *)(c + 0x4be3 + i * 0x1c);
        *p8 = *p8 + 1;
        if (*p8 >= 4) *p8 = z;
    }
    add = (int *)(c + 0x4bd4 + i * 0x1c);
    sum = (int *)(c + 0x4bcc + i * 0x1c);
    *sum = *sum + *add;
    c = c + i * 0x1c;
    v = *sum >> 12;
    if (*(unsigned char *)(c + 0x4be4) != 0) {
        if (v >= 0x120) {
            *(unsigned char *)(c + 0x4be1) = 6;
            return;
        }
        if (*add <= 0x3000) *add = *add + 0x200;
    } else {
        if (v <= -0x20) {
            *(unsigned char *)(c + 0x4be1) = 6;
            return;
        }
        if (*add >= -0x3000) *add = *add - 0x200;
    }
}


/* [9] 0x0211d018 -- the legacy file declared `r3`/`r4` mid-block, which C++
   allows and which is kept verbatim; `new_var`/`r` were unused and dropped. */
// @symbol func_ov006_0211d018
extern "C" void func_ov006_0211d018(char *base, int idx)
{
  int off = idx * 0x1c;
  int new_var2;
  if ((*((u8 *) (((base + off) + 0x4000) + 0xbe5))) != 0)
  {
    *((u8 *) (((base + off) + 0x4000) + 0xbe1)) = 6;
    if (1)
    {
      return;
    }
  }
  char *r3 = (base + 0x4bde) + off;
  if ((*((u16 *) r3)) != 0)
  {
    *((u16 *) r3) = (*((u16 *) r3)) - 1;
    return;
  }
  *((u8 *) (((base + off) + 0x4000) + 0xbe2)) = 1;
  new_var2 = 1;
  ;
  char *r4 = (base + 0x4bcc) + off;
  if (((*((s32 *) r4)) >> 0xc) < 0)
  {
    *((u8 *) (((base + off) + 0x4000) + 0xbe4)) = new_var2;
    *((s32 *) r4) = -0x80000;
    *((s32 *) (((base + off) + 0x4000) + 0xbd4)) = 0x4800;
    *((u8 *) (((base + off) + 0x4000) + 0xbe1)) = 3;
    return;
  }
  *((u8 *) (((base + off) + 0x4000) + 0xbe4)) = 0;
  *((s32 *) r4) = 0x180000;
  *((s32 *) (((base + off) + 0x4000) + 0xbd4)) = -0x4800;
  *((u8 *) (((base + off) + 0x4000) + 0xbe1)) = 2;
}


/* [10] 0x0211d0f8 */
#define CNT16(b,i) (*(u16*)((b) + 0x4bdc + (i)*0x1c))
#define CNT8(b,i)  (*(u8 *)((b) + 0x4be3 + (i)*0x1c))
#define VEL(b,i)   (*(int*)((b) + 0x4bd4 + (i)*0x1c))
#define ACC(b,i)   (*(int*)((b) + 0x4bcc + (i)*0x1c))
#define F15(b,i)   (*(u8 *)((b) + 0x4be1 + (i)*0x1c))
#define F12(b,i)   (*(u16*)((b) + 0x4bde + (i)*0x1c))
#define F16(b,i)   (*(u8 *)((b) + 0x4be2 + (i)*0x1c))
#define F19(b,i)   (*(u8 *)((b) + 0x4be5 + (i)*0x1c))
// @symbol func_ov006_0211d0f8
extern "C" void func_ov006_0211d0f8(char* base, int i)
{
    int v;
    CNT16(base, i)++;
    if (CNT16(base, i) >= 4) {
        CNT16(base, i) = 0;
        CNT8(base, i)++;
        if (CNT8(base, i) >= 4)
            CNT8(base, i) = 0;
    }
    ACC(base, i) += VEL(base, i);
    v = ACC(base, i) >> 0xc;
    if (v >= 0xc0) {
        if (VEL(base, i) >= 0x800)
            VEL(base, i) -= 0x80;
    } else {
        if (VEL(base, i) <= 0x7000)
            VEL(base, i) += 0x400;
    }
    if (v >= 0x120) {
        VEL(base, i) = 0;
        ACC(base, i) = 0x120000;
        F15(base, i) = 4;
        F12(base, i) = 8;
        F16(base, i) = 0;
        if (F19(base, i) != 0)
            F15(base, i) = 6;
    }
}
#undef CNT16
#undef CNT8
#undef VEL
#undef ACC
#undef F15
#undef F12
#undef F16
#undef F19


/* [11] 0x0211d224 */
// @symbol func_ov006_0211d224
extern "C" void func_ov006_0211d224(char* c, int i) {
    int b = i * 0x1c;
    int t;

    (*(u16*)(c + 0x4bdc + b))++;
    if (*(u16*)(c + 0x4bdc + b) >= 4) {
        *(u16*)(c + 0x4bdc + b) = 0;
        (*(u8*)(c + 0x4be3 + b))++;
        if (*(u8*)(c + 0x4be3 + b) >= 4) {
            *(u8*)(c + 0x4be3 + b) = 0;
        }
    }

    *(s32*)(c + 0x4bcc + b) = *(s32*)(c + 0x4bcc + b) + *(s32*)(c + 0x4bd4 + b);
    t = *(s32*)(c + 0x4bcc + b) >> 0xc;
    if (t <= 0x40) {
        if (*(s32*)(c + 0x4bd4 + b) <= -0x800) {
            *(s32*)(c + 0x4bd4 + b) = *(s32*)(c + 0x4bd4 + b) + 0x80;
        }
    } else {
        if (*(s32*)(c + 0x4bd4 + b) >= -0x7000) {
            *(s32*)(c + 0x4bd4 + b) = *(s32*)(c + 0x4bd4 + b) - 0x400;
        }
    }
    if (t > -0x20) {
        return;
    }
    *(s32*)(c + 0x4bcc + b) = -0x20000;
    *(s32*)(c + 0x4bd4 + b) = 0;
    *(u8*)(c + 0x4be1 + b) = 4;
    *(u16*)(c + 0x4bde + b) = 8;
    *(u8*)(c + 0x4be2 + b) = 0;
    if (*(u8*)(c + 0x4be5 + b) != 0) {
        *(u8*)(c + 0x4be1 + b) = 6;
    }
}


/* [12] 0x0211d368 */
// @symbol func_ov006_0211d368
extern "C" void func_ov006_0211d368(char *obj, int i)
{
    unsigned char state = *(unsigned char *)(obj + 0x4be3 + i * 0x1c);
    if ((unsigned char)(state - 8) < 2)
    {
        (*(unsigned short *)(obj + 0x4bdc + i * 0x1c))++;
        if (*(unsigned short *)(obj + 0x4bdc + i * 0x1c) >= data_ov006_0212efc8[(unsigned char)(state - 8)])
        {
            *(unsigned short *)(obj + 0x4bdc + i * 0x1c) = 0;
            (*(unsigned char *)(obj + 0x4be3 + i * 0x1c))++;
        }
    }
    if (*(unsigned short *)(obj + 0x4bde + i * 0x1c) != 0)
    {
        (*(unsigned short *)(obj + 0x4bde + i * 0x1c))--;
        return;
    }
    data_0209d45c &= ~4;
    SetBg3Offset(0, 0x100);
    {
    int rnd = RandomIntInternal(&data_0209d4b8);
    *(unsigned char *)(obj + 0x4be1 + i * 0x1c) =
        data_ov006_0212efc4[((unsigned int)(((unsigned int)rnd >> 16) & 0x7fff) * 2) >> 15];
    }
    *(unsigned short *)(obj + 0x4bdc + i * 0x1c) = 0;
    *(unsigned char *)(obj + 0x4be3 + i * 0x1c) = 0;
    if (*(unsigned char *)(obj + 0x4be1 + i * 0x1c) == 2)
    {
        *(unsigned char *)(obj + 0x4be4 + i * 0x1c) = 0;
        *(int *)(obj + 0x4bd4 + i * 0x1c) = -0x4800;
        *(int *)(obj + 0x4bcc + i * 0x1c) = 0x180000;
    }
    else
    {
        *(unsigned char *)(obj + 0x4be4 + i * 0x1c) = 1;
        *(int *)(obj + 0x4bd4 + i * 0x1c) = 0x4800;
        *(int *)(obj + 0x4bcc + i * 0x1c) = -0x80000;
    }
    if (*(unsigned char *)(obj + 0x4be5 + i * 0x1c) != 0)
    {
        *(unsigned char *)(obj + 0x4be1 + i * 0x1c) = 5;
    }
}


/* [13] 0x0211d4e8 */
// @symbol func_ov006_0211d4e8
extern "C" void func_ov006_0211d4e8(char* c, int i) {
    int off = i * 0x1c;
    char* e = c + off;
    int* a = (int*)(c + 0x4bd0);
    int* b = (int*)(c + 0x4bd8);
    *(unsigned char*)(e + 0x4000 + 0xbe2) = 1;
    *(int*)((char*)a + off) = *(int*)((char*)a + off) + *(int*)((char*)b + off);
    *(int*)((char*)b + off) = *(int*)((char*)b + off) - 0x100;
    if (*(int*)((char*)a + off) >> 0xc > 0xc0)
        return;
    *(int*)((char*)a + off) = 0xc0000;
    *(int*)((char*)b + off) = 0;
    *(unsigned char*)(e + 0x4000 + 0xbe1) = 1;
    *(short*)(e + 0x4b00 + 0xde) = 0x30;
    func_ov004_020b0cac(0xc, 0x80, 0x40, 1, -1, 0xd);
}


/* [14] 0x0211d5a8 */
// @symbol func_ov006_0211d5a8
extern "C" void func_ov006_0211d5a8(TeresaPmfA *c){
  if (c->g == 0) return;
  (c->*data_ov006_02142f18[c->idx])(0);
}


/* [15] 0x0211d608 */
// @symbol func_ov006_0211d608
extern "C" void func_ov006_0211d608(char *c)
{
    *(unsigned char *)(c + 0x4be0) = 1;
    *(int *)(c + 0x4bcc) = 0x80000;
    *(int *)(c + 0x4bd0) = 0x100000;
    *(int *)(c + 0x4bd4) = 0;
    *(int *)(c + 0x4bd8) = -0x1800;
    *(unsigned short *)(c + 0x4bdc) = 0;
    *(unsigned short *)(c + 0x4bde) = 0;
    *(unsigned char *)(c + 0x4be1) = 0;
    *(unsigned char *)(c + 0x4be3) = 8;
    *(unsigned char *)(c + 0x4be4) = 0;
    *(unsigned char *)(c + 0x4be5) = 0;
    _ZN3G2x13SetBlendAlphaEPVttttj((volatile void *)0x4000050, 0, 0xc, 0xc, 0x10);
}


/* [16] 0x0211d688 */
// @symbol func_ov006_0211d688
extern "C" void func_ov006_0211d688(char *p)
{
    *(char *)(p + 0x4be0) = 0;
    *(char *)(p + 0x4be2) = 0;
}


/* [17] 0x0211d69c */
// @symbol func_ov006_0211d69c
extern "C" void func_ov006_0211d69c(char *obj)
{
    if (*(unsigned char *)(obj + 0x4c24) >= 8)
    {
        return;
    }
    (*(unsigned short *)(obj + 0x4c18))++;
    if (*(unsigned short *)(obj + 0x4c18) >= data_ov006_0212efdc[*(unsigned char *)(obj + 0x4c24)])
    {
        (*(unsigned char *)(obj + 0x4c24))++;
        *(unsigned short *)(obj + 0x4c18) = 0;
        if (*(unsigned char *)(obj + 0x4c24) & 1)
        {
            SetBg3Offset(0, 0);
        }
        else
        {
            SetBg3Offset(0, 0x100);
        }
    }
    if (*(unsigned char *)(obj + 0x4c24) >= 8)
    {
        SetBg3Offset(0, 0);
    }
}


/* [18] 0x0211d75c -- decl_common.h types this one `void*`. */
// @symbol func_ov006_0211d75c
extern "C" void func_ov006_0211d75c(void* c_){
  char* c = (char*)c_;
  if(*(unsigned char*)(c+0x4000+0xbc9)==0) return;
  RenderOamMainScreen(data_ov006_0213a5f4,
                      *(int*)(c+0x4000+0xbc0)>>0xc,
                      *(int*)(c+0x4000+0xbc4)>>0xc,
                      -1, -1);
}


/* [19] 0x0211d7b0 */
// @symbol func_ov006_0211d7b0
extern "C" void func_ov006_0211d7b0(void)
{
}


/* [20] 0x0211d7b4 */
// @symbol func_ov006_0211d7b4
extern "C" void func_ov006_0211d7b4(char *p)
{
    *(int *)(p + 0x4bc0) = 327680;
    *(int *)(p + 0x4bc4) = 262144;
    *(char *)(p + 0x4bc8) = 1;
    *(char *)(p + 0x4bc9) = 1;
}


/* [21] 0x0211d7d8 */
// @symbol func_ov006_0211d7d8
extern "C" void func_ov006_0211d7d8(char *p)
{
    *(char *)(p + 0x4bc8) = 0;
    *(char *)(p + 0x4bc9) = 0;
}


/* [22] 0x0211d7ec -- decl_common.h types this one `void*`. */
// @symbol func_ov006_0211d7ec
extern "C" void func_ov006_0211d7ec(void *c_)
{
    char *c = (char *)c_;
    if (*(unsigned char *)(c + 0x4bb9) == 0) return;
    _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(
        0,
        data_ov006_0213a628[*(unsigned char *)(c + 0x4bb8)],
        *(int *)(c + 0x4ba0) >> 12,
        *(int *)(c + 0x4ba4) >> 12,
        -1, 0,
        0x1000, 0x1000,
        *(unsigned short *)(c + 0x4bb0), -1);
}


/* [23] 0x0211d86c */
// @symbol func_ov006_0211d86c
extern "C" void func_ov006_0211d86c(char *thiz, int idx)
{
    char *base = thiz + (idx << 5);
    if (*(unsigned short*)(base + 0x4b00 + 0xb4) != 0) {
        *(unsigned short*)(thiz + 0x4bb4 + (idx << 5)) =
            *(unsigned short*)(thiz + 0x4bb4 + (idx << 5)) - 1;
        return;
    }
    *(unsigned char*)(base + 0x4000 + 0xbb9) = 0;
    *(unsigned char*)(base + 0x4000 + 0xbbc) = 0;
    func_ov006_0211d7d8(thiz);
    data_0209d45c |= 4;
    data_0209d454 |= 1;
    _ZN5Sound12PlayBank2_2DEj(0x1f6);
    *(int*)(thiz + 0x4000 + 0xbe8) = 2;
    func_ov006_0211f51c(thiz);
    func_ov006_0211d608(thiz);
    _ZN5Sound12PlayBank2_2DEj(0x1f5);
}


/* [24] 0x0211d924 */
#pragma push
#pragma opt_common_subs off
#define AT(p,off) ((void*)(int)((char*)(p)+(off)))
// @symbol func_ov006_0211d924
extern "C" void func_ov006_0211d924(char* p, int i)
{
    u8 flag;

    flag = *(u8*)(p + 0x4000 + (i << 5) + 0xbbe);

    if (flag == 0) {
        *(u16*)(p + 0x4bb4 + (i << 5)) += 1;
        *(int*)(p + 0x4ba4 + (i << 5)) += *(int*)(p + 0x4000 + (i << 5) + 0xbac);
        *(int*)AT(p, 0x4bc4) += *(int*)(p + 0x4000 + (i << 5) + 0xbac);
        if (*(int*)(p + 0x4000 + (i << 5) + 0xbac) >= 0x800)
            *(int*)(p + 0x4bac + (i << 5)) -= 0x20;
        if (*(u16*)(p + 0x4b00 + (i << 5) + 0xb4) >= 0x10) {
            *(u16*)(p + 0x4b00 + (i << 5) + 0xb4) = 0;
            *(u8*)(p + 0x4bbe + (i << 5)) += 1;
            *(int*)(p + 0x4000 + (i << 5) + 0xbac) = 0;
        }
    } else if (flag == 1) {
        *(u16*)(p + 0x4bb4 + (i << 5)) += 1;
        if (*(u16*)(p + 0x4b00 + (i << 5) + 0xb4) == 0x10)
            *(int*)(p + 0x4000 + (i << 5) + 0xbac) = -0x1000;
        *(int*)(p + 0x4ba4 + (i << 5)) += *(int*)(p + 0x4000 + (i << 5) + 0xbac);
        *(int*)AT(p, 0x4bc4) += *(int*)(p + 0x4000 + (i << 5) + 0xbac);
        if (*(int*)(p + 0x4000 + (i << 5) + 0xbac) <= -0x800)
            *(int*)(p + 0x4bac + (i << 5)) += 0x20;
        if (*(u16*)(p + 0x4b00 + (i << 5) + 0xb4) >= 0x20) {
            *(u16*)(p + 0x4b00 + (i << 5) + 0xb4) = 0x20;
            *(u8*)(p + 0x4000 + (i << 5) + 0xbbe) = 0;
            *(u8*)(p + 0x4000 + (i << 5) + 0xbba) = 4;
        }
    }
}
#undef AT
#pragma pop


/* [25] 0x0211dad0 */
// @symbol func_ov006_0211dad0
extern "C" void func_ov006_0211dad0(S *s, int i) {
    s->arr[i].cnt++;
    if (s->arr[i].cnt < 8) return;
    s->arr[i].cnt = 0;
    s->arr[i].b11++;
    if (s->arr[i].b11 >= 8) {
        s->arr[i].b11 = 0;
        s->arr[i].b0c = 0;
        s->arr[i].b0e = 3;
        s->arr[i].h08 = 0;
        s->arr[i].word0 = 0x1000;
        _ZN5Sound12PlayBank2_2DEj(0x1f9);
    } else {
        s->arr[i].b0c = data_ov006_0212efd4[s->arr[i].b11];
    }
}


/* [26] 0x0211db7c */
#pragma push
#pragma opt_common_subs off
#define A16(off) (*(unsigned short *)(c + (off) + i * 32))
#define A8(off)  (*(unsigned char  *)(c + (off) + i * 32))
#define A32(off) (*(int            *)(c + (off) + i * 32))
#define B16(off) (*(unsigned short *)(c + i * 32 + (off)))
#define B8(off)  (*(unsigned char  *)(c + i * 32 + (off)))
#define B32(off) (*(int            *)(c + i * 32 + (off)))
// @symbol func_ov006_0211db7c
extern "C" void func_ov006_0211db7c(char *c, int i)
{
    A16(0x4bb2) += 1;
    if (B16(0x4bb2) >= 8) {
        B16(0x4bb2) = 0;
        A8(0x4bbd) += 1;
        if (B8(0x4bbd) >= 8)
            B8(0x4bbd) = 0;
        B8(0x4bb8) = data_ov006_0212efcc[B8(0x4bbd)];
    }
    if (B32(0x4ba0) >> 12 <= 0x80) {
        B32(0x4ba0) = 0x80000;
    } else {
        A32(0x4ba0) += B32(0x4ba8);
        if (B32(0x4ba8) <= -0x600)
            A32(0x4ba8) += 0x20;
    }
    if (B16(0x4bb4) != 0) {
        A16(0x4bb4) -= 1;
        return;
    }
    _ZN5Sound12PlayBank2_2DEj(0x1f3);
    B8(0x4bba) = 2;
    B8(0x4bbd) = 0;
    B8(0x4bb8) = 0;
    B16(0x4bb2) = 0;
    B32(0x4ba8) = 0;
}
#undef A16
#undef A8
#undef A32
#undef B16
#undef B8
#undef B32
#pragma pop


/* [27] 0x0211dce0 */
// @symbol func_ov006_0211dce0
extern "C" void func_ov006_0211dce0(char *base, int i)
{
  char *p = base + (i << 5);
  *((base + (i << 5)) + 0x4bba) = 1;
  *((short *) ((base + (i << 5)) + 0x4bb4)) = 0x40;
  *((int *) (p + 0x4ba8)) = -0xb00;
}


/* [28] 0x0211dd0c */
// @symbol func_ov006_0211dd0c
extern "C" void func_ov006_0211dd0c(TeresaPmfB *c){
  if (c->g == 0) return;
  (c->*data_ov006_02142e88[c->idx])(0);
}


/* [29] 0x0211dd6c */
// @symbol func_ov006_0211dd6c
extern "C" void func_ov006_0211dd6c(char *p)
{
    *(int *)(p + 0x4ba0) = 655360;
    *(int *)(p + 0x4ba4) = 393216;
    *(char *)(p + 0x4bbc) = 1;
    *(char *)(p + 0x4bb9) = 1;
    *(char *)(p + 0x4bba) = 0;
    *(char *)(p + 0x4bb8) = 0;
    *(short *)(p + 0x4bb0) = 0;
    *(short *)(p + 0x4bb2) = 0;
    *(short *)(p + 0x4bb4) = 0;
    *(char *)(p + 0x4bbb) = 0;
    *(char *)(p + 0x4bbd) = 0;
    *(char *)(p + 0x4bbe) = 0;
}


/* [30] 0x0211ddb8 */
// @symbol func_ov006_0211ddb8
extern "C" void func_ov006_0211ddb8(char *p)
{
    *(char *)(p + 0x4bb9) = 0;
    *(char *)(p + 0x4bbc) = 0;
}


/* [31] 0x0211ddcc -- decl_common.h types this one `void*`, and types
   func_ov004_020af948 `(void*, int, int, void*)`. */
// @symbol func_ov006_0211ddcc
extern "C" void func_ov006_0211ddcc(void *c_){
    char *c = (char *)c_;
    int i;
    for (i = 0; i < 0x10; i++) {
        if (*(unsigned char*)(c + 0x4a71)) {
            int b = (*(int*)(c + 0x4a64)) >> 12;
            int a = (*(int*)(c + 0x4a60)) >> 12;
            if (b <= 8) b = 8;
            if (b >= 0xb8) b = 0xb8;
            func_ov004_020af948((void*)data_ov006_02137cd8[*(unsigned char*)(c + 0x4a72) + 1], a, b, (void*)0);
        }
        c += 0x14;
    }
}


/* [32] 0x0211de54 */
// @symbol func_ov006_0211de54
extern "C" void func_ov006_0211de54(char *p)
{
    int i;
    for (i = 0; i < 0x10; i++) {
        *(unsigned char *)(p + 0x4a70) = 0;
        *(unsigned char *)(p + 0x4a71) = 0;
        p += 0x14;
    }
}


/* [33] 0x0211de7c */
// @symbol func_ov006_0211de7c
extern "C" int func_ov006_0211de7c(char *c) {
    int cnt = 0;
    int i;
    for (i = 0; i < 0x10; i++) {
        if (*(unsigned char*)(c + 0x4a70) != 0 && *(unsigned char*)(c + 0x4a73) != 2) {
            cnt++;
        }
        c += 0x14;
    }
    return cnt != 0;
}


/* [34] 0x0211dec0 */
// @symbol func_ov006_0211dec0
extern "C" void func_ov006_0211dec0(void *arg) {
    int i;
    char *p = (char *)arg;
    for (i = 0; i < 16; i++, p += 0x14) {
        unsigned char v;
        if (*(unsigned char *)(p + 0x4a70) == 0) {
            continue;
        }
        v = *(unsigned char *)(p + 0x4a73);
        if (v == 0) {
            if (*(int *)(p + 0x4a6c) != 0) {
                *(int *)(p + 0x4a6c) -= 1;
                if ((short)*(int *)(p + 0x4a6c) < 0) {
                    *(int *)(p + 0x4a6c) = 0;
                }
            } else {
                _ZN5Sound12PlayBank2_2DEj(0x1bc);
                *(int *)(p + 0x4a68) = -0x400;
                *(int *)(p + 0x4a6c) = 0x10;
                *(unsigned char *)(p + 0x4a73) += 1;
                *(unsigned char *)(p + 0x4a71) = 1;
            }
        } else if (v == 1) {
            *(int *)(p + 0x4a64) += *(int *)(p + 0x4a68);
            *(int *)(p + 0x4a68) -= 0x80;
            if (*(int *)(p + 0x4a6c) != 0) {
                *(int *)(p + 0x4a6c) -= 1;
                if ((short)*(int *)(p + 0x4a6c) < 0) {
                    *(int *)(p + 0x4a6c) = 0;
                }
            } else {
                *(int *)(p + 0x4a6c) = 0x18;
                *(unsigned char *)(p + 0x4a73) += 1;
            }
        }
    }
}


/* [35] 0x0211e020 */
// @symbol func_ov006_0211e020
extern "C" void func_ov006_0211e020(char *c, int i)
{
  int n;
  char *p = c;
  for (n = 0; n < 0x10; n++)
  {
    if (((*((unsigned char *) (p + 0x4a70))) == 0) != 0)
    {
      char *d = (c + (n * 0x14)) + 0x4000;
      char *s = c + (i * 0x24);
      unsigned char *tbl = (unsigned char *) (c + 0x4680);
      *((unsigned char *) (((c + (n * 0x14)) + 0x4000) + 0xa70)) = 1;
      *((unsigned char *) (d + 0xa71)) = 0;
      *((int *) (((c + (n * 0x14)) + 0x4000) + 0xa60)) = *((int *) ((c + (i * 0x24)) + 0x4660));
      *((int *) (((c + (n * 0x14)) + 0x4000) + 0xa64)) = (*((int *) (s + 0x4664))) - 0x10000;
      *((int *) (((c + (n * 0x14)) + 0x4000) + 0xa6c)) = (tbl[i * 0x24] * 0x3c) + 0x10;
      *((unsigned char *) (((c + (n * 0x14)) + 0x4000) + 0xa73)) = 0;
      *((unsigned char *) (((c + (n * 0x14)) + 0x4000) + 0xa72)) = ((unsigned char *) (c + 0x4680))[i * 0x24];
      return;
    }
    p += 0x14;
  }

}


/* [36] 0x0211e0c8 -- the legacy file's `C*` is RowArray here, and the call to
   func_ov006_0211e020 is cast because that member's own definition (and the
   ROM's own argument setup) is `char*`. */
// @symbol func_ov006_0211e0c8
extern "C" void func_ov006_0211e0c8(RowArray* c) {
    int i;
    struct Row* r = c->rows;
    for (i = 0; i < 0x10; i++) {
        if (((unsigned char*)r + 0x4000)[0x677] != 0 && ((unsigned char*)r + 0x4000)[0x678] == 5)
            func_ov006_0211e020((char*)c, i);
        r++;
    }
}


/* [37] 0x0211e118 -- decl_common.h types this one `void*`. */
// @symbol func_ov006_0211e118
extern "C" void func_ov006_0211e118(void* a0_) {
    struct HudArray* a0 = (struct HudArray*)a0_;
    int i;
    for (i = 0; i < 16; i++) {
        if (a0->arr[i].flag) {
            Hud_RenderSprite(data_ov006_02134210[a0->arr[i].idx], a0->arr[i].x >> 12, a0->arr[i].y >> 12, -1, -1);
        }
    }
}


/* [38] 0x0211e184 */
#define LI(i) ((int)((long long)(i)) * 0x10)
#define A(p) ((int)(p))
// @symbol func_ov006_0211e184
extern "C" void func_ov006_0211e184(char *base)
{
    int i;

    for (i = 0; i < 0x10; i++) {
        unsigned char *fp = (unsigned char *)A(base + LI(i) + 0x496d);
        unsigned short *c16;
        unsigned char *c8;
        if (*fp == 0) continue;
        c16 = (unsigned short *)A(base + LI(i) + 0x4968);
        *c16 += 1;
        if (*c16 < 4) continue;
        *c16 = 0;
        c8 = (unsigned char *)A(base + LI(i) + 0x496c);
        *c8 += 1;
        if (*c8 >= 5) {
            *c8 = 0;
            *(unsigned char *)(base + LI(i) + 0x496e) = 0;
            *fp = 0;
        }
    }
}
#undef LI
#undef A


/* [39] 0x0211e220 */
#pragma push
#pragma opt_strength_reduction off
// @symbol func_ov006_0211e220
extern "C" void func_ov006_0211e220(unsigned char* c,int param){
  struct E29* a=(struct E29*)c;
  int i;
  for(i=0;i<16;i++){
    unsigned char* s=(unsigned char*)&a[i];
    unsigned char* base = s + 0x4000;
    if(base[0x96d]==0){
      unsigned char* src = c + param*0x24;
      src += 0x4000;
      int v1 = *(int*)(src + 0x660);
      unsigned char* base2 = s + 0x4900;
      *(int*)(base + 0x960) = v1;
      int v2 = *(int*)(src + 0x664);
      int zero = 0;
      *(int*)(base + 0x964) = v2;
      *(short*)(base2 + 0x68) = zero;
      base[0x96c] = zero;
      base[0x96d] = 1;
      base[0x96e] = 1;
      return;
    }
  }
}
#pragma pop


/* [40] 0x0211e29c -- decl_common.h types this one `void*`. */
// @symbol func_ov006_0211e29c
extern "C" void func_ov006_0211e29c(void* c_){
  unsigned char* c = (unsigned char*)c_;
  if(*(unsigned char*)(c+0x4c1b)==0) return;
  func_ov004_020b2220(0x80,0x60,*(unsigned short*)(c+0x4c14),1,0,0x800,0);
  DrawOamSprite(data_ov006_0213f9d0[GetGameLanguage()][3],0x80,0x48,0);
}


/* [41] 0x0211e318 */
// @symbol func_ov006_0211e318
extern "C" void func_ov006_0211e318(char *c){
  if (*(unsigned char*)(c + 0x4c1c) == 0) return;
  if (*(unsigned short*)(c + 0x4c14) == 0) return;
  *(unsigned char*)(((int)c + 0x4c1a)) = *(unsigned char*)(((int)c + 0x4c1a)) + 1;
  if (*(unsigned char*)(c + 0x4c1a) >= 0x3c){
    *(unsigned char*)(c + 0x4c1a) = 0;
    *(unsigned short*)(((int)c + 0x4c14)) = *(unsigned short*)(((int)c + 0x4c14)) - 1;
    if (*(short*)(c + 0x4c14) < 0) *(short*)(c + 0x4c14) = 0;
    if (*(unsigned short*)(c + 0x4c14) != 0)
      _ZN5Sound12PlayBank2_2DEj(0xa7);
    else
      _ZN5Sound12PlayBank2_2DEj(0xa6);
  }
  if (*(unsigned short*)(c + 0x4c14) == 0){
    *(unsigned short*)(c + 0x4c14) = 0;
    *(unsigned char*)(c + 0x4c1a) = 0;
  }
}


/* [42] 0x0211e3e0 */
// @symbol func_ov006_0211e3e0
extern "C" void func_ov006_0211e3e0(char *c)
{
    if (*(unsigned char *)(c + 0x4c20) == 0) return;
    if (*(unsigned char *)(c + 0x4c1c) != 0) return;
    if (*(unsigned int *)(c + 0xbc) >= 0xf)
        *(unsigned short *)(c + 0x4c14) = 2;
    else
        *(unsigned short *)(c + 0x4c14) = 3;
    *(unsigned char *)(c + 0x4c1a) = 0;
    *(unsigned char *)(c + 0x4c1b) = 1;
    *(unsigned char *)(c + 0x4c1c) = 1;
    _ZN5Sound12PlayBank2_2DEj(0xa7);
}


/* [43] 0x0211e460 -- decl_common.h types this one `void*`. */
// @symbol func_ov006_0211e460
extern "C" void func_ov006_0211e460(void *c_)
{
    char *c = (char *)c_;
    int i;
    for (i = 0; i < 0x10; i++) {
        if (*(unsigned char *)(c + 0x48aa) != 0) {
            int v0 = *(int *)(c + 0x48a0) >> 12;
            int v4 = *(int *)(c + 0x48a4) >> 12;
            if (v4 <= 8) v4 = 8;
            if (v4 >= 0xb8) v4 = 0xb8;
            Hud_RenderSprite(data_ov006_02133a5c, v0, v4, -1, -1);
        }
        c += 0xc;
    }
}


/* [44] 0x0211e4e0 */
#define A(p) ((unsigned char *)(int)(p))
// @symbol func_ov006_0211e4e0
extern "C" void func_ov006_0211e4e0(char *base)
{
    int i;

    for (i = 0; i < 0x10; i++) {
        if (*(unsigned char *)(base + 0x48a9) != 0) {
            if (*(unsigned char *)(base + 0x48a8) != 0) {
                unsigned char *p = A(base + 0x48a8);
                int v;
                *p -= 1;
                v = *(unsigned char *)(base + 0x48a8);
                if (v < 0)
                    *(unsigned char *)(base + 0x48a8) = 0;
            } else {
                *(unsigned char *)(base + 0x48a9) = 0;
                *(unsigned char *)(base + 0x48aa) = 0;
            }
        }
        base += 0xc;
    }
}
#undef A


/* [45] 0x0211e55c */
// @symbol func_ov006_0211e55c
extern "C" void func_ov006_0211e55c(char* c, int idx) {
    int i;
    char* slot = c;
    for (i = 0; i < 0x10; i++) {
        if (*(unsigned char*)(slot + 0x4000 + 0x8a9) == 0) {
            *(unsigned char*)(c + i * 0xc + 0x4000 + 0x8a9) = 1;
            *(unsigned char*)(c + i * 0xc + 0x4000 + 0x8aa) = 1;
            *(int*)(c + i * 0xc + 0x4000 + 0x8a0) = *(int*)(c + idx * 0x24 + 0x4000 + 0x660);
            *(int*)(c + i * 0xc + 0x4000 + 0x8a4) = *(int*)(c + idx * 0x24 + 0x4000 + 0x664) - 0x18000;
            *(unsigned char*)(c + i * 0xc + 0x4000 + 0x8a8) = 0x60;
            return;
        }
        slot += 0xc;
    }
}


/* [46] 0x0211e5cc */
// @symbol func_ov006_0211e5cc
extern "C" void func_ov006_0211e5cc(char* c)
{
    int found;
    int i;
    char* p;
    if (((Work*)c)->latch != 0)
        return;
    found = 0;
    for (i = 0, p = c; i < 0x10; i++, p += 0x24) {
        if (((View*)p)->active != 0) {
            if (((View*)p)->stage <= 2) {
                found++;
                break;
            }
        }
    }
    if (found != 0)
        return;
    FreeGfxSlotsById(0xc);
    (*(u8*)(c + 0x4c20))++;
}


/* [47] 0x0211e658 */
// @symbol func_ov006_0211e658
extern "C" void func_ov006_0211e658(unsigned char* base)
{
    if (*(unsigned short*)(base + 0x4c14) == 0 && *(unsigned char*)(base + 0x4c20) != 0) {
        *(unsigned char*)(base + 0x4c1f) = 0;
        *(int*)(base + 0x4be8) = 3;
        *(unsigned short*)(base + 0x4c0c) = 0x60;
        func_ov006_0211cc90(base);
        *(unsigned char*)(base + 0x4c27) = 1;
        return;
    }
    {
        int count = 0;
        int i = 0;
        unsigned char* p = base;
        do {
            if (*(unsigned char*)(p + 0x4677) != 0) {
                if (*(unsigned char*)(p + 0x467c) == 0) {
                    count++;
                    break;
                }
            }
            i++;
            p += 0x24;
        } while (i < 0x10);
        if (count != 0)
            return;
        *(int*)(base + 0x4be8) = 3;
        *(unsigned short*)(base + 0x4c0c) = 0x60;
        func_ov006_0211cc90(base);
        *(unsigned char*)(base + 0x4c1f) = 1;
    }
}
