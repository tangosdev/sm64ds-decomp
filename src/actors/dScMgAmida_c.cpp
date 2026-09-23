//cpp
/* Ladder-lottery minigame scene (ov006/dScMgAmida_c), upper run only.
 *
 * PARTIAL TU: 13 of the run's 28 functions (.text 0x020d3624..0x020d5a54).
 * An unmatched draft at ordinal 14 splits the run; the lower stretch and
 * the key-function TU (destructor pair) live elsewhere, so no vtable is
 * emitted here and the class header is untouched by this promotion.
 * Source order is ROM-ascending under `#pragma defer_codegen off`.
 *
 * Leftover: 10 func_ov006_* + 21 data_* keep linker names (unnamed in
 *   symbols.txt); each needs a coined, behaviour-justified name.
 * Leftover: slot 36 dispatches through a local shim struct, not
 *   `this->Unk36()` -- see the per-function notes and the class header.
 * Leftover: ~300 *(T *)(p + 0x..) stay raw offsets; the class layout
 *   does not name them yet.
 * Leftover: hand-rolled C1/C2/D0/D1/D2 call shapes stand in for real
 *   constructors only a real ctor emits.
 */

#include "types.h"
#include "decl_common.h"
#include "dScMgAmida_c.h"

/* ------------------------------------------------------------------ */
/* Declarations, reconciled across the thirteen merged shards.         */
/*                                                                     */
/* Where two shards spelled one interface differently the more         */
/* complete observation wins and the narrower call site takes the      */
/* (codegen-free) pointer conversion, per the merge rule:              */
/*   - func_02054efc / _ZN11dScMgBase_cC2Ev / data_ov006_0213b918      */
/*     come from decl_common.h; three shards had re-declared them      */
/*     locally with a different return type, array-ness or arity.      */
/*   - MultiStore16 takes `void *`, so the `char *` call site converts */
/*     implicitly rather than the `void *` one needing a cast.         */
/*   - func_ov006_020d36a4 takes `dScMgAmida_c *` (its own definition  */
/*     dereferences named fields through it); the two `char *` call    */
/*     sites pass `this` / a reinterpret cast.                         */
/*   - func_ov006_020d3668 takes an ignored `void *`: its definition   */
/*     took none and its one call site passes the scene pointer, and   */
/*     only one prototype can exist under C linkage.                   */
/* ------------------------------------------------------------------ */

extern "C" {
/* --- shared ov004 / main helpers --- */
extern void func_ov004_020afdd0(void *a0, int a1, int a2, int a3, int a4);
extern void func_ov004_020b1e34(void *a0, int a1, int a2, int a3);
extern int  func_ov004_020ae5c4(void *a, int b, int c, int d, int e, int f, int g);
extern void MultiCopyHalf(void *dst, void *src, int nbytes);
extern void MultiStore16(u16 val, void *dst, int nbytes);
extern int  RandomIntInternal(int *seed);
extern int  Vec2_Len(int *v);
extern void FreeGfxSlotsById(int a);
extern u32  LoadCompressedFileAt(u16 fileID, void *target);
extern int  LoadFile(int handle);
extern void _ZN4CP1527FlushAndInvalidateDataCacheEjj(u32 a, u32 b);
extern void _ZN2GX10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN3GXS10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN2GX11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN3GXS11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
extern void *_ZN2G212GetBG3ScrPtrEv(void);
extern void *_ZN3G2S12GetBG3ScrPtrEv(void);
extern void *_ZN3G2S12GetBG2ScrPtrEv(void);
extern void *_ZN3G2S13GetBG0CharPtrEv(void);
extern void _ZN3G2x13SetBlendAlphaEPVttttj(void *p, u16 a, u16 b, u16 c, u16 d);
extern void SetSubBg2Offset(int a, int b);
extern void *_ZN7fBase_cnwEj(unsigned int sz);
extern void __cxa_vec_ctor(void *obj, int a, int b, void *cb1, void *cb2);
extern void func_0203d738(void *p);

/* --- the class's own functions that live OUTSIDE this licensed range --- */
extern void func_ov006_020d122c(void *sb, int v);
extern void func_ov006_020d1958(char *c);
extern void func_ov006_020d1a3c(char *c);
extern void func_ov006_020d1ba0(char *c);
extern void func_ov006_020d27dc(char *c);

/* --- the class's own functions defined below --- */
extern void func_ov006_020d3624(void);
extern void func_ov006_020d3668(void *sb);
extern void func_ov006_020d36a4(dScMgAmida_c *self);
extern void func_ov006_020d452c(void *thiz);
extern void func_ov006_020d47f4(char *c);
extern void func_ov006_020d5a50(void);
extern void *dScMgAmida_c_classInit(void);

/* --- data --- */
extern s16 data_02082214[];
extern u8  data_0209d45c;
extern u8  data_0209d454;
extern u8  data_020a0e40[];
extern u8  data_020a0de8[];
extern u8  data_020a0de9[];
extern s32 data_0208ee44;
extern char data_ov006_0212e1a8[];
extern char data_ov006_0212e1ac[];
extern char data_ov006_0212e1b0[];
extern char data_ov006_0212e1b4[];
extern char data_ov006_0212e1b8[];
extern void *data_ov006_0213a32c;
extern void *data_ov006_0213a35c;
extern int  data_ov006_0213a338[];
extern void *data_ov006_0213a390[];
extern void *data_ov006_0213a458[];
extern void *data_ov006_0213a4b0[];
extern int  data_ov006_0213a4c0[];
extern int  data_ov006_0213a568[];
extern int  data_ov006_0213b84c[13];
extern s32  data_ov006_02141640[];
extern s32  data_ov006_02141650[];
}

namespace G2S { char *GetBG0CharPtr(void); }
namespace Memory { void Deallocate(void *); }

typedef struct { int a; int b; } Pair;
typedef struct { s32 lo, hi; } P8;
extern Pair data_ov006_0213b8b8[11];

typedef struct { int v[14]; } Buf14;
extern Buf14 data_ov006_0213b880;

/* ------------------------------------------------------------------ */
/* The slot-36 vtable shims.                                           */
/*                                                                     */
/* Unk36 is a real declared virtual on dScMgAmida_c (slot 36), but four */
/* of the members below reach it through a local 37-slot shim struct    */
/* instead of `this->Unk36()`.  That is MEASURED, not style: an earlier */
/* attempt at the plain virtual call compiled Render 0xc bytes larger   */
/* than the ROM (0x2ac vs 0x2a0) and cascaded into ~1400 unrelated-     */
/* looking mismatches.  The class header's banner records the full      */
/* story and why the shims stay until someone re-measures them.         */
/*                                                                     */
/* The four shards each named their shim differently and two of them    */
/* both called it `Obj`; only that collision is renamed here, so every  */
/* call expression is otherwise textually what it was.  None of these   */
/* structs defines a virtual, so none is a key function and none emits  */
/* a vtable.                                                            */
/* ------------------------------------------------------------------ */
struct Base {
    virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3();
    virtual void v4(); virtual void v5(); virtual void v6(); virtual void v7();
    virtual void v8(); virtual void v9(); virtual void v10(); virtual void v11();
    virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
    virtual void v16(); virtual void v17(); virtual void v18(); virtual void v19();
    virtual void v20(); virtual void v21(); virtual void v22(); virtual void v23();
    virtual void v24(); virtual void v25(); virtual void v26(); virtual void v27();
    virtual void v28(); virtual void v29(); virtual void v30(); virtual void v31();
    virtual void v32(); virtual void v33(); virtual void v34(); virtual void v35();
    virtual int m_90();   /* slot 36 -> 0x90 */
};

struct VtObj {
    virtual void d00(); virtual void d01(); virtual void d02(); virtual void d03();
    virtual void d04(); virtual void d05(); virtual void d06(); virtual void d07();
    virtual void d08(); virtual void d09(); virtual void d10(); virtual void d11();
    virtual void d12(); virtual void d13(); virtual void d14(); virtual void d15();
    virtual void d16(); virtual void d17(); virtual void d18(); virtual void d19();
    virtual void d20(); virtual void d21(); virtual void d22(); virtual void d23();
    virtual void d24(); virtual void d25(); virtual void d26(); virtual void d27();
    virtual void d28(); virtual void d29(); virtual void d30(); virtual void d31();
    virtual void d32(); virtual void d33(); virtual void d34(); virtual void d35();
    virtual int m36();
};

struct Obj {
    virtual void v00();
    virtual void v04();
    virtual void v08();
    virtual void v0c();
    virtual void v10();
    virtual void v14();
    virtual void v18();
    virtual void v1c();
    virtual void v20();
    virtual void v24();
    virtual void v28();
    virtual void v2c();
    virtual void v30();
    virtual void v34();
    virtual void v38();
    virtual void v3c();
    virtual void v40();
    virtual void v44();
    virtual void v48();
    virtual void v4c();
    virtual void v50();
    virtual void v54();
    virtual void v58();
    virtual void v5c();
    virtual void v60();
    virtual void v64();
    virtual void v68();
    virtual void v6c();
    virtual void v70();
    virtual void v74();
    virtual void v78();
    virtual void v7c();
    virtual void v80();
    virtual void v84();
    virtual void v88();
    virtual void v8c();
    virtual int v90();
};

/* Was also called `Obj` in its own shard; renamed for the merge only. */
struct Obj3ba0 {
    virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3();
    virtual void v4(); virtual void v5(); virtual void v6(); virtual void v7();
    virtual void v8(); virtual void v9(); virtual void v10(); virtual void v11();
    virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
    virtual void v16(); virtual void v17(); virtual void v18(); virtual void v19();
    virtual void v20(); virtual void v21(); virtual void v22(); virtual void v23();
    virtual void v24(); virtual void v25(); virtual void v26(); virtual void v27();
    virtual void v28(); virtual void v29(); virtual void v30(); virtual void v31();
    virtual void v32(); virtual void v33(); virtual void v34(); virtual void v35();
    virtual int m_90();
};

#pragma defer_codegen off


/* ---------------------------------------------------------------- */
/* ROM ordinal 15 -- func_ov006_020d3624, 0x020d3624, size 0x44      */
/* ---------------------------------------------------------------- */
// @symbol func_ov006_020d3624
extern "C" void func_ov006_020d3624(void) {
  char *a = (char *)func_02054efc();
  MultiCopyHalf((char *)func_02054efc() + 0xc000, a, 0x6000);
  char *b = G2S::GetBG0CharPtr();
  MultiCopyHalf(G2S::GetBG0CharPtr() + 0x6000, b, 0x6000);
}


/* ---------------------------------------------------------------- */
/* ROM ordinal 16 -- func_ov006_020d3668, 0x020d3668, size 0x3c      */
/* ---------------------------------------------------------------- */
/* The parameter is declared and never read.  Its own shard took none;
   its one call site (func_ov006_020d3ba0, below) passes the scene
   pointer, and under C linkage only one prototype can exist. */
// @symbol func_ov006_020d3668
extern "C" void func_ov006_020d3668(void *) {
  char *a = (char *)func_02054efc() + 0xc000;
  MultiCopyHalf((char *)func_02054efc(), a, 0x6000);
  char *b = G2S::GetBG0CharPtr() + 0x6000;
  MultiCopyHalf(G2S::GetBG0CharPtr(), b, 0x6000);
}


/* ---------------------------------------------------------------- */
/* ROM ordinal 17 -- func_ov006_020d36a4, 0x020d36a4, size 0x4fc     */
/* ---------------------------------------------------------------- */
// @symbol func_ov006_020d36a4
extern "C" void func_ov006_020d36a4(dScMgAmida_c *self)
{
    int i;
    int j;
    int k;
    int r;
    int t;
    u32 u;
    int again;
    u8 mark0[4];
    u8 mark1[4];

    self->unk_53dd = 0;
    if (self->Unk36() != 0) {
        again = 1;
        for (i = 0; i < 4; i++) {
            self->unk_4694[i] = i;
        }
        do {
            for (k = 0; k < 4; k++) {
                u = (u32)RandomIntInternal(&data_0209d4b8) >> 16;
                r = ((u & 0x7fff) * 4) >> 15;
                t = self->unk_4694[k];
                self->unk_4694[k] = self->unk_4694[r];
                self->unk_4694[r] = t;
            }
            if (self->mRoundCount == 0) {
                again = 0;
                switch (self->mPatternIndex) {
                case 0:
                    self->unk_46a4[0] = 0;
                    self->unk_46a4[1] = 0;
                    self->unk_46a4[2] = 3;
                    self->unk_46a4[3] = 3;
                    break;
                case 1:
                case 3:
                case 5:
                    self->unk_46a4[0] = 0;
                    self->unk_46a4[1] = 0;
                    self->unk_46a4[2] = 1;
                    self->unk_46a4[3] = 3;
                    break;
                default:
                    for (i = 0; i < 4; i++) {
                        self->unk_46a4[i] = i;
                    }
                    break;
                }
                for (j = 0; j < 4; j++) {
                    data_ov006_02141640[self->unk_4694[j]] = self->unk_46a4[j];
                }
            } else {
                for (i = 0; i < 4; i++) {
                    data_ov006_02141650[i] = data_ov006_02141640[i];
                }
                for (j = 0; j < 4; j++) {
                    data_ov006_02141640[self->unk_4694[j]] = self->unk_46a4[j];
                }
                for (j = 0; j < 4; j++) {
                    if (data_ov006_02141650[j] != data_ov006_02141640[j]) {
                        again = 0;
                        break;
                    }
                }
            }
        } while (again == 1);
    } else if (self->unk_46c8 == 1) {
        if (self->mRoundCount == 0) {
            r = ((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 4) >> 15;
            t = self->unk_4694[0];
            self->unk_4694[0] = self->unk_4694[r];
            self->unk_4694[r] = t;
        } else {
            r = (((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 3) >> 15) + 1;
            t = self->unk_4694[0];
            self->unk_4694[0] = self->unk_4694[r];
            self->unk_4694[r] = t;
        }
    } else {
        again = 1;
        for (int n = 0; n < 4; n++) {
            mark1[n] = 0;
        }
        for (j = 0; j < self->unk_46c8; j++) {
            mark1[self->unk_4694[j]] = 1;
        }
        do {
            for (int n = 0; n < 4; n++) {
                mark0[n] = 0;
            }
            for (i = 0; i < 4; i++) {
                self->unk_4694[i] = i;
            }
            for (k = 0; k < 4; k++) {
                u = (u32)RandomIntInternal(&data_0209d4b8) >> 16;
                r = ((u & 0x7fff) * 4) >> 15;
                t = self->unk_4694[k];
                self->unk_4694[k] = self->unk_4694[r];
                self->unk_4694[r] = t;
            }
            for (j = 0; j < self->unk_46c8; j++) {
                mark0[self->unk_4694[j]] = 1;
            }
            if (self->mRoundCount == 0) {
                again = 0;
            } else {
                for (int n = 0; n < 4; n++) {
                    if (mark0[n] != mark1[n]) {
                        again = 0;
                        break;
                    }
                }
            }
        } while (again == 1);
    }

    for (i = 0; i < 4; i++) {
        if (self->Unk36() != 0) {
            self->unk_46b8[i] = 0;
        } else {
            self->unk_46b8[i] = i * *(s32 *)(data_ov006_0212e1a8 + self->mPatternIndex * 0x1c + 0x14) * 0x3c;
        }
        self->unk_4660[i][0] = (self->unk_4694[i] << 6) + 0x20;
        if (self->Unk36() != 0) {
            self->unk_4660[i][1] = -0xcc;
        } else {
            self->unk_4660[i][1] = -0xd4;
        }
        self->unk_4684[i] = -1;
        self->unk_4680[i] = 0;
        self->unk_46b4[i] = 0;
    }
    self->unk_46cc = 0;
    self->mRoundTimer = 0;
}


/* ---------------------------------------------------------------- */
/* ROM ordinal 18 -- func_ov006_020d3ba0, 0x020d3ba0, size 0x98c     */
/* ---------------------------------------------------------------- */
#define VCALL90(p) (((Obj3ba0 *)(p))->m_90())

// @symbol func_ov006_020d3ba0
#pragma push
#pragma opt_strength_reduction off
extern "C" void func_ov006_020d3ba0(char *sb)
{
    int r5;
    int r4;
    Pair sp3C[11];
    int sp10[11];

    *(u8 *)(sb + 0x46d4) = 0;
    *(u8 *)(sb + 0x46d5) = 0;

    if (*(s32 *)(sb + 0x53e8) > 0x270f) {
        *(s32 *)(sb + 0x53e8) = 0x270f;
    }
    r4 = 0;

    r5 = *(s32 *)(sb + 0xbc);
    if (VCALL90(sb) != 0) {
        if ((u32) *(s32 *)(sb + 0xbc) >= 7) {
            r5 = 7;
            r4 = 1;
        }
    } else if (r5 >= 0xa) {
        r4 = 1;
        r5 = (((u32)(((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 5) >> 15) + 4;
        if (r5 == *(s32 *)(sb + 0x53d4)) {
            r5 = 9;
        }
    }
    *(s32 *)(sb + 0x53d4) = r5;

    if (VCALL90(sb) != 0) {
        *(s32 *)(sb + 0x46c8) = 4;
    } else {
        *(s32 *)(sb + 0x46c8) = *(int *)(data_ov006_0212e1a8 + *(s32 *)(sb + 0x53d4) * 0x1c);
    }

    *(s32 *)(sb + 0x5368) = ((*(int *)(data_ov006_0212e1b0 + *(s32 *)(sb + 0x53d4) * 0x1c) - 1) * 5) + 0xb;

    {
        u32 t = *(u32 *)(sb + 0xbc);
        if (t >= 0x13) {
            int *p5368 = (int *)((int)sb + 0x5368);
            *p5368 = (int)((t / 10 - 1) * 5) + *p5368;
            if (*(s32 *)(sb + 0x5368) > 0x64) {
                *(s32 *)(sb + 0x5368) = 0x64;
            }
        }
    }

    {
        char *d;
        volatile u16 v1;
        volatile u16 v2;
        d = (char *)func_02054efc();
        v1 = 0;
        MultiStore16(v1, d, 0x6000);
        d = G2S::GetBG0CharPtr();
        v2 = 0;
        MultiStore16(v2, d, 0x6000);
    }

    *(s32 *)(sb + 0x46d8) = -1;
    *(s32 *)(sb + 0x46dc) = -1;
    *(s32 *)(sb + 0x46e0) = -1;
    *(s32 *)(sb + 0x46e4) = -1;
    *(s32 *)(sb + 0x46f0) = -1;
    *(s32 *)(sb + 0x46f4) = -1;
    *(s32 *)(sb + 0x46e8) = -1;
    *(s32 *)(sb + 0x46ec) = -1;

    {
        int z = 0;
        *(u8 *)(sb + 0x4704) = z;
        *(u8 *)(sb + 0x4705) = z;
        *(u8 *)(sb + 0x4708) = z;
        *(u8 *)(sb + 0x4709) = z;
        *(u8 *)(sb + 0x53c8) = z;
        *(s32 *)(sb + 0x53cc) = z;
        do {
            ((s32 *)(sb + 0x4694))[z] = z;
            z += 1;
        } while (z < 4);
    }

    *(s32 *)(sb + 0x5374) = 0;
    func_ov006_020d36a4((dScMgAmida_c *)sb);
    func_ov006_020d122c(sb, *(int *)(data_ov006_0212e1ac + *(s32 *)(sb + 0x53d4) * 0x1c));
    func_ov004_020b04d0(0x20);

    *(u8 *)(sb + 0x4707) = 1;
    *(u8 *)(sb + 0x470a) = 1;
    {
        int i, j, off;
        for (i = 0, off = 0; i < 0x100; i++, off += 0x158) {
            for (j = 0; j < 0x158; j++) {
                *(*(u8 **)(sb + 0x470c) + off + j) = 0;
                *(*(u8 **)(sb + 0x4710) + off + j) = 0;
            }
        }
    }

    if (VCALL90(sb) != 0) {
        func_ov004_020ae5c4(sb, 0x20, -0xb4, 0x20, *(s32 *)(sb + 0x4700), *(s32 *)(sb + 0x53e4), 1);
        func_ov004_020ae5c4(sb, 0x60, -0xb4, 0x60, *(s32 *)(sb + 0x4700), *(s32 *)(sb + 0x53e4), 1);
        func_ov004_020ae5c4(sb, 0xa0, -0xb4, 0xa0, *(s32 *)(sb + 0x4700), *(s32 *)(sb + 0x53e4), 1);
        func_ov004_020ae5c4(sb, 0xe0, -0xb4, 0xe0, *(s32 *)(sb + 0x4700), *(s32 *)(sb + 0x53e4), 1);
    } else {
        func_ov004_020ae5c4(sb, 0x20, -0xd4, 0x20, *(s32 *)(sb + 0x4700), *(s32 *)(sb + 0x53e4), 1);
        func_ov004_020ae5c4(sb, 0x60, -0xd4, 0x60, *(s32 *)(sb + 0x4700), *(s32 *)(sb + 0x53e4), 1);
        func_ov004_020ae5c4(sb, 0xa0, -0xd4, 0xa0, *(s32 *)(sb + 0x4700), *(s32 *)(sb + 0x53e4), 1);
        func_ov004_020ae5c4(sb, 0xe0, -0xd4, 0xe0, *(s32 *)(sb + 0x4700), *(s32 *)(sb + 0x53e4), 1);
    }

    (*(u8 *)((int)sb + 0x470a))++;
    {
        typedef struct { Pair e[11]; } Blk88;
        *(Blk88 *)sp3C = *(Blk88 *)data_ov006_0213b8b8;
    }

    {
        int i;
        for (i = 0; i < 0xb; i++) {
            sp10[i] = 0;
        }
    }

    if (r4 == 1) {
        if (VCALL90(sb) != 0) {
            int msk = 0x7fff;
            int cnt = 3;
            do {
                int idx = ((u32)(((u32)RandomIntInternal(&data_0209d4b8) >> 16) & msk) * 0xb) >> 15;
                if (sp10[idx] == 0) {
                    cnt--;
                    sp10[idx] = 1;
                }
            } while (cnt > 0);
        } else {
            int sel = *(int *)(data_ov006_0212e1b4 + *(s32 *)(sb + 0x53d4) * 0x1c);
            switch (sel) {
            case 0:
                break;
            case 1:
                sp10[((u32)(((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 5) >> 15] = 1;
                break;
            case 2: {
                int cnt = (((u32)(((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 3) >> 15) + 1;
                int j = 0;
                if (cnt > 0) {
                    int *seedp2 = &data_0209d4b8;
                    do {
                        sp10[((u32)(((u32)RandomIntInternal(seedp2) >> 16) & 0x7fff) * 0xb) >> 15] = 1;
                        j++;
                    } while (j < cnt);
                }
                break;
            }
            default:
                break;
            }
        }
    }

    {
        int sel2;
        if (VCALL90(sb) != 0) {
            switch (*(s32 *)(sb + 0x53d4)) {
            case 0:
            case 1:
            case 2:
                sel2 = 0;
                break;
            case 3:
            case 4:
                sel2 = 1;
                break;
            default:
                sel2 = 2;
                break;
            }
        } else {
            sel2 = *(int *)(data_ov006_0212e1b4 + *(s32 *)(sb + 0x53d4) * 0x1c);
        }

        switch (sel2) {
        case 0:
            break;
        case 1: {
            int i;
            for (i = 0; i < 5; i++) {
                if (sp10[i] == 0) {
                    int x = sp3C[i].a;
                    int y = sp3C[i].b - 0x20;
                    func_ov004_020ae5c4(sb, x, y, x + 0x40, y, *(s32 *)(sb + 0x53e4), 1);
                }
            }
            break;
        }
        case 2:
        case 3: {
            int i;
            for (i = 0; i < 0xb; i++) {
                if (sp10[i] == 0) {
                    int x = sp3C[i].a;
                    int y = sp3C[i].b - 0x20;
                    func_ov004_020ae5c4(sb, x, y, x + 0x40, y, *(s32 *)(sb + 0x53e4), 1);
                }
            }
            break;
        }
        default:
            break;
        }
    }

    (*(u8 *)((unsigned int)sb + 0x470au))++;
    if (VCALL90(sb) == 0) {
        int v = *(int *)(data_ov006_0212e1b8 + *(s32 *)(sb + 0x53d4) * 0x1c);
        if (v != 0 && v == 1) {
            func_ov004_020ae5c4(sb, 0x60, 0x2d, 0xa0, 0x2d, *(s32 *)(sb + 0x53e4), 1);
            func_ov004_020ae5c4(sb, 0x20, 0x5a, 0x60, 0x5a, *(s32 *)(sb + 0x53e4), 1);
            func_ov004_020ae5c4(sb, 0xa0, 0x5a, 0xe0, 0x5a, *(s32 *)(sb + 0x53e4), 1);
        }
    }

    *(u8 *)(sb + 0x4707) = 0;
    (*(u8 *)(sb + 0x470a))++;
    {
        int i, j, off;
        for (i = 0, off = 0; i < 0x100; i++, off += 0x158) {
            for (j = 0; j < 0x158; j++) {
                *(*(u8 **)(sb + 0x470c) + off + j) = 0;
            }
        }
    }

    func_ov006_020d3668(sb);
    *(s32 *)(sb + 0x5370) = 1;
    *(volatile u16 *)0x04000050 = 0;
    _ZN3G2x13SetBlendAlphaEPVttttj((void *)0x04001050, 4, 8, 6, 0x10);
    func_ov004_020b0cac(0xd, 0x80, 0x60, 1, -1, 0xd);

    *(s32 *)(sb + 0x53c4) = 0x3c;
    *(s32 *)(sb + 0x46d0) = 1;
    {
        int i = 0;
        for (; i < 4; i++) {
            ((s32 *)(sb + 0x5378))[i] = 0;
            ((s32 *)(sb + 0x5388))[i] = 0;
            ((u8 *)(sb + 0x5398))[i] = 0;
            ((s32 *)(sb + 0x539c))[i] = 0;
            ((s32 *)(sb + 0x53ac))[i] = 0;
        }
        *(s16 *)(sb + 0x53bc) = 0;
        *(s32 *)(sb + 0x53d0) = 0;
        *(s32 *)(sb + 0x53d8) = 0;
        *(u8 *)(sb + 0x53dc) = 0;
        *(u8 *)(sb + 0x53dd) = 0;
        *(u8 *)(sb + 0x53de) = 0;
        {
            int z = 0;
            int xw[2];
            xw[0] = 0x20;
            xw[1] = 0;
            for (; z < 4; z++) {
                ((P8 *)(sb + 0x4724))[z].lo = xw[0] << 12;
                ((P8 *)(sb + 0x4724))[z].hi = 0xb0000;
                ((P8 *)(sb + 0x4744))[z].lo = xw[1];
                ((P8 *)(sb + 0x4744))[z].hi = xw[1];
                xw[0] += 0x40;
            }
            *(s32 *)(sb + 0x4764) = xw[1];
            {
                int v = 0;
                for (; v < 0x80; v++) {
                    *(u8 *)(sb + 0x477c) = 0;
                    *(s32 *)(sb + 0x4778) = 0;
                    sb += 0x18;
                }
            }
        }
    }
}
#pragma pop


/* ---------------------------------------------------------------- */
/* ROM ordinal 19 -- func_ov006_020d452c, 0x020d452c, size 0x2c8     */
/* ---------------------------------------------------------------- */
// @symbol func_ov006_020d452c
#pragma push
#pragma opt_strength_reduction off
#pragma opt_common_subs off
extern "C" void func_ov006_020d452c(void *thiz)
{
    typedef struct { int a[13]; } T13;
    unsigned char *c = (unsigned char *)thiz;
    int fix = -1;
    int i;
    int y;

    i = 0;
    y = 0x20;
    for (; i < 4; y += 0x40, i++) {
        int *p = (int *)(c + i * 4);
        if (*(int *)((char *)p + 0x4714) != 0) {
            int *t = (int *)(((int)p + 0x539c));
            (*t)++;
            if (*t >= 6) {
                int *u = (int *)(((int)p + 0x53ac));
                *t = 0;
                (*u)++;
                if (*u >= 14)
                    *u = 0;
            }
            func_ov004_020afdd0(data_ov006_0213a458[*(int *)(c + i * 4 + 0x53ac)],
                                *(int *)(c + i * 8 + 0x4724) >> 12,
                                (*(int *)(c + i * 8 + 0x4728) >> 12) - 4,
                                fix, 0);
        } else {
            int *cnt;
            int *cnt2;
            int tbl[13];
            if (*(c + i + 0x5398) == 0) {
                cnt = (int *)(((int)p + 0x5378));
                cnt2 = (int *)(((int)p + 0x5388));
                (*cnt)++;
                if (*cnt2 >= 12) {
                    if (*cnt >= 6) {
                        *cnt = 0;
                        *cnt2 = 0;
                    }
                } else if (*cnt >= 4) {
                    *cnt = 0;
                    (*cnt2)++;
                }
            } else {
                cnt = (int *)(((int)p + 0x5378));
                cnt2 = (int *)(((int)p + 0x5388));
                (*cnt)++;
                if (*cnt2 >= 12) {
                    if (*cnt >= 5) {
                        *cnt = 0;
                        *cnt2 = 0;
                    }
                } else if (*cnt >= 2) {
                    *cnt = 0;
                    (*cnt2)++;
                }
            }
            *(T13 *)tbl = *(T13 *)data_ov006_0213b84c;
            func_ov004_020afdd0(data_ov006_0213a390[tbl[*cnt2]],
                                y, 0xb8, fix, 0);
        }
    }
    {
        int j;
        for (j = 0; j < *(int *)(c + 0x46c8); j++) {
            func_ov004_020afdd0(data_ov006_0213a32c,
                                *(int *)(c + j * 8 + 0x4660),
                                *(int *)(c + j * 8 + 0x4664),
                                -1, 0);
        }
    }
    func_ov004_020afdd0(data_ov006_0213a35c, 8, 0x60, -1, 0);
    func_ov004_020afdd0(data_ov006_0213a35c, 0xf8, 0x60, -1, 0);
}
#pragma pop


/* ---------------------------------------------------------------- */
/* ROM ordinal 20 -- func_ov006_020d47f4, 0x020d47f4, size 0xe8      */
/* ---------------------------------------------------------------- */
// @symbol func_ov006_020d47f4
#pragma push
#pragma opt_strength_reduction off
#pragma opt_common_subs off
extern "C" void func_ov006_020d47f4(char *c){
  int fix = -1;
  int i;
  int j;
  int x;
  if(*(unsigned char*)(c+0x53dd)==0){
    func_ov004_020afdd0((void*)data_ov006_0213a338[0],0x80,0xb0,fix,0);
  }
  for(i=0;i<4;i++){
    func_ov004_020afdd0((void*)data_ov006_0213a568[((int*)(c+0x46a4))[i]],
                        *(int*)(c+i*8+0x4660),
                        *(int*)(c+i*8+0x4664),
                        fix,0);
  }
  j = 0;
  x = 0x20;
  for(; j < 4; x += 0x40, j++){
    func_ov004_020afdd0((void*)data_ov006_0213a4c0[((int*)(c+0x4714))[j]],
                        x,0x78,fix,0);
  }
}
#pragma pop


/* ---------------------------------------------------------------- */
/* ROM ordinal 21 -- dScMgAmida_c::Render, 0x020d48dc, size 0x2a0    */
/* ---------------------------------------------------------------- */
/* Kept on the pre-migration source's vtable-shim dispatch (`Base::m_90()`)
   for slot 36 even though Unk36 is a real declared virtual method: an
   earlier attempt at a normal `this->Unk36()` virtual call compiled 0xc
   bytes larger than the ROM (0x2ac vs 0x2a0), and that single size delta
   cascaded through the rest of the module (dsd does not hard-fail a
   declared-vs-compiled size mismatch, it just shifts every following
   object) -- caught by rombuild dropping from 106/106 to 102/106 with
   ~1400 unrelated-looking mismatches, traced via final_link.o.xMAP to this
   function's own placed size. Reverting the dispatch alone did NOT fix it
   -- the real cause turned out to be the two #pragma lines below, dropped
   by accident during that rewrite; they are restored here verbatim from
   the pre-migration source. Kept the shim dispatch anyway rather than
   re-testing the real virtual call a second time, since the shim is proven
   byte-correct and changing dispatch style was never actually required. */
// @symbol _ZN12dScMgAmida_c6RenderEv
#pragma push
#pragma opt_strength_reduction off
#pragma opt_common_subs off
s32 dScMgAmida_c::Render()
{
    char *self = (char *)this;

    mBgScrollPhase += 0xc0;
    {
        u8 *p2 = (u8*)(self + 0x5300);
        u16 idx = *(u16*)(p2 + 0xbc);
        int v = data_02082214[(idx >> 4) << 1];
        int off = (v + (int)((unsigned)(v >> 7) >> 24)) >> 8;
        SetSubBg2Offset(off, 0);
    }
    func_ov004_020b1e34(self, 0xe0, 0x14, 1);

    if (mState == 3 && mEndDelayTimer == 0) {
        data_0209d45c &= ~1;
        data_0209d454 &= ~1;
        if (((Base*)self)->m_90() == 0 && mFinished == 1) {
            int i;
            for (i = 0; i < 4; i++) {
                if (unk_4714[i] != 0) {
                    int *counterA = (int*)(self + i * 4 + 0x539c);
                    (*counterA)++;
                    Buf14 local = data_ov006_0213b880;
                    if (*counterA >= local.v[i]) {
                        int *counterB;
                        *counterA = 0;
                        counterB = (int*)(self + i * 4 + 0x53ac);
                        (*counterB)++;
                        if (*counterB >= 0xe)
                            *counterB = 0;
                    }
                    func_ov004_020afdd0(
                        data_ov006_0213a458[mLaneAnimFrame[i]],
                        mLanePos[i][0] >> 12,
                        (mLanePos[i][1] >> 12) - 4,
                        -1,
                        0);
                }
            }

            int r8_val = 0;
            int r7_val = 0;
            int r4_val = -1;
            for (; r8_val < 0x80; r8_val++, self += 0x18) {
                if (*(u8*)(self + 0x477c) != 0) {
                    int ang = *(int*)(self + 0x4778);
                    int sb_val = *(int*)(self + 0x4768);
                    int idx = ang / 4;
                    int r2_val = *(int*)(self + 0x476c);
                    func_ov004_020afdd0(data_ov006_0213a4b0[idx], sb_val >> 12, r2_val >> 12, r4_val, r7_val);
                }
            }

        }
        return 1;
    } else {
        data_0209d45c |= 1;
        data_0209d454 |= 1;
        if (((Base*)self)->m_90() != 0) {
            func_ov006_020d47f4(self);
        } else {
            func_ov006_020d452c(self);
        }
        return 1;
    }
}
#pragma pop


/* ---------------------------------------------------------------- */
/* ROM ordinal 22 -- dScMgAmida_c::Behavior, 0x020d4b7c, size 0x774  */
/* ---------------------------------------------------------------- */
/* Also keeps the pre-migration source's vtable-shim dispatch for slot 36
   (`VtObj::m36()`, the VIRT() macro) -- see Render's note above for the
   measurement. The macro-based raw offset access (I/B/H/IA on `char *c`)
   is kept exactly as recovered: this class's own fields (0x46d0 on) sit at
   the identical byte offsets in the real struct, so the macros still reach
   the right bytes. */
#define AT(p,off) ((void*)(int)((char*)(p)+(off)))
#define I(o)  (*(int*)(c+(o)))
#define B(o)  (*(u8*)(c+(o)))
#define H(o)  (*(short*)(c+(o)))
#define IA(o) (*(int*)AT(c,(o)))
#define IP(p,o) (*(int*)((char*)(p)+(o)))
#define BP(p,o) (*(u8*)((char*)(p)+(o)))
#define IAP(p,o) (*(int*)AT(p,(o)))
#define MULFX(a,b) ((int)(((s64)(a)*(b)+0x800)>>12))
#define VIRT() (((VtObj*)c)->m36())

// @symbol _ZN12dScMgAmida_c8BehaviorEv
#pragma push
#pragma opt_strength_reduction off
s32 dScMgAmida_c::Behavior()
{
    char *c = (char *)this;
    int i2;
    char *p;
    int *vp;
    int t;
    int q;
    int k;
    int spd;
    int idx, j4, flag;
    int i1;
    int j;
    int i3;
    char *sc;
    int rnd1, rnd2;
    u32 bv, av;
    s16 cA;
    int idxA;
    char *e;
    int cosB, sinA;
    char *z;
    int *w;

    switch (I(0x46d0)) {
    case 0:
        if (B(0x46d4) == 1) {
            func_ov006_020d3ba0(c);
        } else if (B(0x46d5) == 1) {
            func_ov006_020d3ba0(c);
        } else {
            func_ov006_020d36a4(this);
        }
        I(0x46d0) = 1;
        // fall through
    case 1:
        func_ov006_020d1a3c(c);
        func_ov006_020d1958(c);
        func_ov006_020d1ba0(c);
        if (VIRT() != 0) {
            if (B(0x53dd) == 1) {
                IA(0x536c) += I(0x5374) * 5 + 0x20;
            }
        } else if (B(0x53dc) == 1) {
            IA(0x536c) += 0x64;
        } else {
            IA(0x536c) += I(0x5368);
        }
        spd = I(0x536c);
        IA(0x536c) &= 0xf;
        q = spd / 16;
        if (VIRT() == 0 || B(0x53dd) != 0) {
            if (I(0x53e0) > 0) {
                IA(0x53e0) -= 1;
                if (I(0x53e0) == 0) {
                    if (B(0x46d5) == 1) {
                        I(0x53c0) = 0x3c;
                        I(0x46d0) = 2;
                    } else if (I(0x5374) < 5) {
                        I(0x46d0) = 0;
                    } else {
                        I(0x53c0) = 0x3c;
                        I(0x46d0) = 2;
                        B(0x46d4) = 1;
                    }
                }
            }
            k = 0;
            if (q > 0) {
                do {
                    if (I(0x46d0) != 1)
                        break;
                    func_ov006_020d27dc(c);
                    k++;
                } while (k < q);
            }
        }
        if (I(0x53c4) > 0) {
            IA(0x53c4) -= 1;
            if (I(0x53c4) <= 0) {
                FreeGfxSlotsById(0xd);
                if (B(0xc4) == 0) {
                    B(0xc3) = 1;
                    B(0xc4) = 1;
                    H(0xc0) = 0;
                }
            }
        }
        func_ov004_020adb1c(I(0x53e8));
        I(0xb4) = I(0x53e8);
        break;
    case 2:
        B(0x53dc) = 0;
        if (VIRT() != 0) {
            if (I(0x53c0) > 0)
                IA(0x53c0) -= 1;
        } else {
            if (I(0x53c0) > 0)
                IA(0x53c0) -= 1;
        }
        if (I(0x53c0) != 0)
            break;
        B(0xc3) = 0;
        if (B(0x46d4) == 1) {
            I(0x46d0) = 3;
            func_ov004_020b0a54(0);
        } else {
            I(0x46d0) = 3;
            func_ov004_020b0a54(0x12);
            I(0x53d0) = 0xb4;
            I(0x4764) = 0;
        }
        break;
    case 3:
        if (I(0x53d0) > 0)
            IA(0x53d0) -= 1;
        flag = 0;
        idx = data_020a0e40[0];
        j4 = idx * 4;
        if (data_020a0de8[idx * 4] != 0) {
            if (data_020a0de9[j4] != 0)
                flag = 1;
        }
        if (flag != 0)
            I(0x53d0) = 0;
        if (VIRT() != 0)
            break;
        if (B(0x46d4) != 1)
            break;
        IA(0x4764) += 1;
        i1 = 0;
        do {
            char *w = c + i1 * 8;
            IAP(w, 0x4748) -= 0x100;
            IAP(w, 0x4724) += IP(w, 0x4744);
            IAP(w, 0x4728) += IAP(w, 0x4748);
            i1++;
        } while (i1 < 4);

        p = c;
        vp = (int *)AT(c, 0x4770);
        for (i2 = 0; i2 < 0x80; i2++) {
            if (BP(p, 0x477c) != 0) {
                IAP(p, 0x4768) += IP(p, 0x4770);
                IAP(p, 0x476c) += IP(p, 0x4774);
                t = Vec2_Len(vp) * 7 / 8;
                if (func_0203d434(vp) != 0)
                    func_0203d630(vp, t);
                *(int*)AT(p, 0x4778) = *(int*)AT(p, 0x4778) + 1;
                {
                    int* tb = (int*)(p + 0x4000);
                    if (tb[0x778/4] >= 0x10)
                        *(u8 *)(p + 0x477c) = 0;
                }
            }
            p += 0x18;
            vp += 6;
        }

        for (j = 0; j < 4; j++) {
            char *y = c + j * 4;
            if (IP(y, 0x4714) != 0) {
                i3 = 0;
                sc = c;
                do {
                    if (BP(sc, 0x477c) != 1) {
                        rnd1 = RandomIntInternal(&data_0209d4b8);
                        rnd2 = RandomIntInternal(&data_0209d4b8);
                        bv = ((u32)rnd2 >> 16) & 0x7fff;
                        av = ((u32)rnd1 >> 16) & 0x7fff;
                        cosB = data_02082214[((int)((bv << 15) >> 16) >> 4) * 2 + 1];
                        idxA = ((int)((av << 17) >> 16) >> 4) * 2;
                        cA = data_02082214[idxA + 1];
#define WP (c + i3 * 0x18)
                        {
                            IAP(WP, 0x4768) = MULFX(cosB, (int)((0x8000LL * cA + 0x800) >> 12));
                            sinA = data_02082214[idxA];
                            IAP(WP, 0x476c) = MULFX(cosB, (int)((0x8000LL * sinA + 0x800) >> 12));
                            z = c + j * 8;
                            IAP(WP, 0x4768) += IP(z, 0x4724);
                            IAP(WP, 0x476c) += IP(z, 0x4728);
                            IP(WP, 0x4770) = MULFX(cosB, (int)((0x1000LL * (int)(cA) + 0x800) >> 12));
                            IAP(WP, 0x4774) = MULFX(cosB, (int)((0x1000LL * sinA + 0x800) >> 12));
                        }
                        IAP(WP, 0x4774) -= 0x400;
                        BP(WP, 0x477c) = 1;
                        IP(WP, 0x4778) = 0;
                        break;
                    }
                    i3++;
                    sc += 0x18;
                } while (i3 < 0x80);
            }
        }
        break;
    }
    return 1;
}
#pragma pop


/* ---------------------------------------------------------------- */
/* ROM ordinal 23 -- dScMgAmida_c::OnYoshiTryEat, 0x020d52f0, 0x94   */
/* ---------------------------------------------------------------- */
/* Recovered from vtable slot identity. Own new slot 18, left as a raw
   helper (see the class header's banner -- same precedent as every other
   dScMgBase_c leaf's slot 18). 0xb4/0xbc are dScMgBase_c's own inherited
   fields (0xbc falls inside that class's pad_0bc, not a named field there
   either), reached via a raw char* offset instead of the class header, the
   same precedent dScMgPachinko_c's own slot 18 helper already uses for the
   same offset. */
// @symbol _ZN12dScMgAmida_c13OnYoshiTryEatEi
void dScMgAmida_c::OnYoshiTryEat(int arg)
{
    char *c = (char *)this;

    if (arg == 0) {
        int *p;
        if (*(unsigned int *)(c + 0xbc) >= 0x7cf)
            goto final;
        p = (int *)(c + 0xbc);
        *p = *p + 1;
        if (*(unsigned int *)(c + 0xbc) > 0x270e)
            *(int *)(c + 0xbc) = 0x270e;
    } else {
        *(int *)((c + 0x5000) + 0x3e8) = 0;
        *(int *)(c + 0xbc) = 0;
        if (*(unsigned int *)(c + 0xbc) > 0x270e)
            *(int *)(c + 0xbc) = 0x270e;
        func_ov004_020adb1c(*(int *)((c + 0x5000) + 0x3e8));
        *(int *)(c + 0xb4) = *(int *)((c + 0x5000) + 0x3e8);
    }
final:
    func_ov006_020d3ba0(c);
}


/* ---------------------------------------------------------------- */
/* ROM ordinal 24 -- dScMgAmida_c::InitResources, 0x020d5384, 0x5a0  */
/* ---------------------------------------------------------------- */
/* Also keeps the pre-migration source's vtable-shim dispatch for slot 36
   (`Obj::v90()`) -- see Render's note above for the measurement. Render is
   the only one of the three callers actually proven to grow; this one and
   Behavior keep the shim rather than leaving three call sites on two
   different dispatch conventions for the same slot. unk_0bc is
   dScMgBase_c's own inherited field, but it falls inside that class's
   pad_0bc (not a named field there either), so it stays a raw char*
   offset. */
// @symbol _ZN12dScMgAmida_c13InitResourcesEv
s32 dScMgAmida_c::InitResources()
{
    Obj *c = (Obj *)this;
    volatile u16 sp4;
    volatile u16 sp6;
    void *f;

    unk_470c = (u8 *)_ZN6Memory8AllocateEj(0x15800);
    unk_4710 = (u8 *)_ZN6Memory8AllocateEj(0x15800);

    if (c->v90() != 0) {
        mLineEndY = 0x78;
        unk_53e4 = 2;
    } else {
        mLineEndY = 0x98;
        unk_53e4 = 2;
    }

    data_0208ee44 = 1;
    *(volatile u16 *)0x4000008 = (*(volatile u16 *)0x4000008 & 0x43) | 0x3218;
    *(volatile u16 *)0x4000008 &= ~0x40;
    *(volatile s32 *)0x4000010 = 0;
    *(volatile u16 *)0x4000008 = (*(volatile u16 *)0x4000008 & ~3) | 1;

    f = func_02054efc();
    sp4 = 0;
    MultiStore16((u16)sp4, f, 0x6000);

    *(volatile u16 *)0x4001008 = (*(volatile u16 *)0x4001008 & 0x43) | 0x2214;
    *(volatile u16 *)0x4001008 &= ~0x40;
    *(volatile s32 *)0x4001010 = 0;
    *(volatile u16 *)0x4001008 = (*(volatile u16 *)0x4001008 & ~3) | 1;

    f = _ZN3G2S13GetBG0CharPtrEv();
    sp6 = 0;
    MultiStore16((u16)sp6, f, 0x6000);

    func_ov004_020af2f8((char *)this, 1, 0, 2);

    *(volatile u16 *)0x400000e = (*(volatile u16 *)0x400000e & 0x43) | 0x140c;
    *(volatile u16 *)0x400000e &= ~0x40;
    *(volatile s32 *)0x400001c = 0;
    *(volatile u16 *)0x400000e = (*(volatile u16 *)0x400000e & ~3) | 3;

    if (c->v90() != 0) {
        LoadCompressedFileAt(0x1f, (void *)func_02054d88());
        f = (void *)LoadFile(0x20);
        _ZN4CP1527FlushAndInvalidateDataCacheEjj((u32)f, 0x1a0);
        _ZN2GX10LoadBGPlttEPKvjj(f, 0x60, 0x1a0);
        Deallocate(f);
        LoadCompressedFileAt(0x21, _ZN2G212GetBG3ScrPtrEv());
    } else {
        LoadCompressedFileAt(0x15, (void *)func_02054d88());
        f = (void *)LoadFile(0x16);
        _ZN4CP1527FlushAndInvalidateDataCacheEjj((u32)f, 0x1a0);
        _ZN2GX10LoadBGPlttEPKvjj(f, 0x60, 0x1a0);
        Deallocate(f);
        LoadCompressedFileAt(0x17, _ZN2G212GetBG3ScrPtrEv());
    }

    *(volatile u16 *)0x400100c = (*(volatile u16 *)0x400100c & 0x43) | 0x40c;
    *(volatile u16 *)0x400100c &= ~0x40;
    *(volatile s32 *)0x4001018 = 0;
    *(volatile u16 *)0x400100c &= ~3;
    *(volatile u16 *)0x400100e = (*(volatile u16 *)0x400100e & 0x43) | 0x60c;
    *(volatile u16 *)0x400100e &= ~0x40;
    *(volatile s32 *)0x400101c = 0;
    *(volatile u16 *)0x400100e = (*(volatile u16 *)0x400100e & ~3) | 3;

    if (c->v90() != 0) {
        LoadCompressedFileAt(0x1c, _ZN3G2S13GetBG3CharPtrEv());
        f = (void *)LoadFile(0x1d);
        _ZN3GXS10LoadBGPlttEPKvjj(f, 0x60, 0x1a0);
        Deallocate(f);
        LoadCompressedFileAt(0x1e, _ZN3G2S12GetBG3ScrPtrEv());
        data_0209d45c |= 9;
        data_0209d454 |= 9;
    } else {
        LoadCompressedFileAt(0x1a, _ZN3G2S13GetBG3CharPtrEv());
        f = (void *)LoadFile(0x1b);
        _ZN3GXS10LoadBGPlttEPKvjj(f, 0x60, 0x1a0);
        Deallocate(f);
        LoadCompressedFileAt(0x19, _ZN3G2S12GetBG2ScrPtrEv());
        LoadCompressedFileAt(0x18, _ZN3G2S12GetBG3ScrPtrEv());
        *(volatile s16 *)0x4000050 = 0;
        _ZN3G2x13SetBlendAlphaEPVttttj((void *)0x4001050, 4, 8, 6, 0x10);
        data_0209d45c |= 9;
        data_0209d454 |= 0xd;
    }

    if (c->v90() != 0) {
        if (GetOwnerLanguage() == 5) {
            LoadCompressedFileAt(0xf, (void *)0x6400000);
            LoadCompressedFileAt(0xf, (void *)0x6600000);
        } else if (GetOwnerLanguage() == 4) {
            LoadCompressedFileAt(0xc, (void *)0x6400000);
            LoadCompressedFileAt(0xc, (void *)0x6600000);
        } else if (GetOwnerLanguage() == 3) {
            LoadCompressedFileAt(0xb, (void *)0x6400000);
            LoadCompressedFileAt(0xb, (void *)0x6600000);
        } else if (GetOwnerLanguage() == 2) {
            LoadCompressedFileAt(0xa, (void *)0x6400000);
            LoadCompressedFileAt(0xa, (void *)0x6600000);
        } else {
            LoadCompressedFileAt(0xd, (void *)0x6400000);
            LoadCompressedFileAt(0xd, (void *)0x6600000);
        }
        f = (void *)LoadFile(0xe);
        _ZN2GX11LoadOBJPlttEPKvjj(f, 0, 0x100);
        _ZN3GXS11LoadOBJPlttEPKvjj(f, 0, 0x100);
        Deallocate(f);
    } else {
        LoadCompressedFileAt(0x10, (void *)0x6400000);
        LoadCompressedFileAt(0x10, (void *)0x6600000);
        f = (void *)LoadFile(0x11);
        _ZN2GX11LoadOBJPlttEPKvjj(f, 0, 0x100);
        _ZN3GXS11LoadOBJPlttEPKvjj(f, 0, 0x100);
        Deallocate(f);
    }

    mPatternIndex = 0;
    mScore = *(int *)((char *)this + 0xbc) * 5;
    func_ov006_020d3ba0((char *)this);
    return 1;
}


/* ---------------------------------------------------------------- */
/* ROM ordinal 25 -- dScMgAmida_c::AfterCleanupResources, 0x020d5924 */
/* ---------------------------------------------------------------- */
/* The recovered source locally declared the base override as returning
   void*; the real dScMgBase_c.h override returns void, so this calls
   dScMgBase_c::AfterCleanupResources as a plain statement instead of
   returning it. The early `if (vfSuccess != 2) return` is preserved
   exactly as recovered -- the base-class call is skipped too when
   vfSuccess != 2, not just the two Deallocate calls. */
// @symbol _ZN12dScMgAmida_c21AfterCleanupResourcesEj
void dScMgAmida_c::AfterCleanupResources(u32 vfSuccess)
{
    if (vfSuccess != 2)
        return;
    Memory::Deallocate(unk_470c);
    Memory::Deallocate(unk_4710);
    dScMgBase_c::AfterCleanupResources(vfSuccess);
}


/* ---------------------------------------------------------------- */
/* ROM ordinal 26 -- dScMgAmida_c_classInit, 0x020d5974, size 0xdc   */
/* ---------------------------------------------------------------- */
/* Reconstructed source-style name: SM64DS proves dScMgAmida_c through RTTI,
 * allocation size, vtable identity, and the MG_AMIDA registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: func_ov006_020d5974.
 *
 * The vtable pointer is installed from `&data_ov006_0213b918`, the flat
 * spelling of _ZTV12dScMgAmida_c, rather than being emitted by this TU:
 * the class's key function (its destructor) is outside this licensed
 * range, so nothing here owns the table. */
// @symbol dScMgAmida_c_classInit
extern "C" void *dScMgAmida_c_classInit(void) {
    char *o = (char *)_ZN7fBase_cnwEj(0x53fc);
    if (o != 0) {
        _ZN11dScMgBase_cC2Ev(o);
        *(int *)o = (int)&data_ov006_0213b918;
        __cxa_vec_ctor(o + 0x4660, 4, 8, (void *)func_0203d738, (void *)NullDestructor_0203d47c);
        __cxa_vec_ctor(o + 0x4724, 4, 8, (void *)func_0203d738, (void *)NullDestructor_0203d47c);
        __cxa_vec_ctor(o + 0x4744, 4, 8, (void *)func_0203d738, (void *)NullDestructor_0203d47c);
        __cxa_vec_ctor(o + 0x4768, 0x80, 0x18, (void *)func_ov006_020d5a50, (void *)func_ov006_020d116c);
    }
    return o;
}


/* ---------------------------------------------------------------- */
/* ROM ordinal 27 -- func_ov006_020d5a50, 0x020d5a50, size 0x4       */
/* ---------------------------------------------------------------- */
/* The per-element constructor classInit hands __cxa_vec_ctor for the 0x80
   falling pieces: four bytes, `bx lr`. */
// @symbol func_ov006_020d5a50
extern "C" void func_ov006_020d5a50(void)
{
}
