//cpp
/* Translation unit ov006/dScMgMCarlo2_c  (23 function(s)).
 * Reconstructed with tools/tubuild.py create, then reconciled by hand.
 *
 * ENROLLED AND CANONICAL. config/arm9/overlays/ov006/delinks.txt names this
 * one file for the whole .text range 0x020f8ef4..0x020fa6ac, so every byte
 * of that range in the retail overlay is built from the source below -- the
 * 23 legacy one-function files it replaced are gone. Verified: all 23 members
 * byte-exact, the linked module byte-identical to the cartridge, and the full
 * ROM identical to the stock build. See the measurements in
 * config/tu_manifest.d/ov006/dScMgMCarlo2_c.json.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function, in the REVERSE of source order, so
 * the highest-address ROM function is written FIRST here. Do not reorder;
 * see notes/tu-reconstruction-pilot-report.md sec 3 for the one documented
 * exception (a destructor's D0/D1/D2 group has compiler-chosen order).
 *
 * Assembled from these legacy one-function sources (ROM address order):
 *   [0] 0x020f8ef4  src/_ZN14dScMgMCarlo2_cD1Ev.cpp
 *   [1] 0x020f8f68  src/_ZN14dScMgMCarlo2_cD0Ev.cpp
 *   [2] 0x020f8ff0  src/func_ov006_020f8ff0.c
 *   [3] 0x020f9000  src/func_ov006_020f9000.cpp
 *   [4] 0x020f94f4  src/func_ov006_020f94f4.c
 *   [5] 0x020f9560  src/func_ov006_020f9560.c
 *   [6] 0x020f95f0  src/func_ov006_020f95f0.c
 *   [7] 0x020f9668  src/func_ov006_020f9668.c
 *   [8] 0x020f96e0  src/func_ov006_020f96e0.c
 *   [9] 0x020f9760  src/func_ov006_020f9760.c
 *   [10] 0x020f98dc  src/func_ov006_020f98dc.c
 *   [11] 0x020f9994  src/func_ov006_020f9994.c
 *   [12] 0x020f9bec  src/func_ov006_020f9bec.c
 *   [13] 0x020f9cbc  src/func_ov006_020f9cbc.c
 *   [14] 0x020f9d68  src/func_ov006_020f9d68.c
 *   [15] 0x020f9db8  src/func_ov006_020f9db8.c
 *   [16] 0x020f9f40  src/func_ov006_020f9f40.c
 *   [17] 0x020f9fe0  src/_ZN14dScMgMCarlo2_c16CleanupResourcesEv.cpp
 *   [18] 0x020f9ffc  src/_ZN14dScMgMCarlo2_c6RenderEv.cpp
 *   [19] 0x020fa13c  src/_ZN14dScMgMCarlo2_c8BehaviorEv.cpp
 *   [20] 0x020fa3d0  src/func_ov006_020fa3d0.c
 *   [21] 0x020fa4d4  src/func_ov006_020fa4d4.cpp
 *   [22] 0x020fa56c  src/_ZN14dScMgMCarlo2_c13InitResourcesEv.cpp
 */

/* TUBUILD NOTE -- #pragma directive(s) were present in the legacy sources
 * of this TU. `#pragma long_calls` is POSITIONAL in mwccarm 2004/b56 and is
 * carried verbatim before its own member below, bracketed with `off` so it
 * cannot leak into later members (dropping it silently costs the pooled
 * cross-overlay tail-call -- a byte diff; see ShutterBob in ov014).
 * Any OTHER pragma is FILE-GLOBAL last-wins (opt_propagation,
 * optimize_for_size). Two legacy files carried one:
 *   func_ov006_020f95f0: #pragma opt_propagation off   [NOT carried]
 *   _ZN14dScMgMCarlo2_c13OnTurnIntoEggEi: #pragma opt_propagation off   [NOT carried]
 * Neither is carried, and the measurements say the cartridge's own
 * translation unit did not have it either. opt_propagation is file-global
 * last-wins, so a TU has exactly one setting for all 23 members, and there
 * is only one place it could go -- the top. Set there it costs four OTHER
 * members their match (ordinals 3, 5, 10 and 18), and prepending it to the
 * unmodified legacy sources of those four breaks them there too. So `off`
 * cannot be this TU's setting; it was a per-file crutch for two bodies
 * whose shape had been tuned against it one file at a time.
 *
 * Both members verify byte-exact with propagation ON once their source is
 * shaped the way the cartridge's was -- see the note on each below. That
 * is the load-bearing evidence: a single setting, no pragma, 23/23.
 */

/* Includes: union of the legacy files', first-seen in ROM-ascending
 * processing order. NOT verified for header ordering constraints (e.g. a
 * common.h-before-X rule) -- watch for new compile errors after this. */
#include "dScMgMCarlo2_c.h"
#include "types.h"
#include "decl_common.h"

/* THE BOARD IS 40 PIECES ON ONE INTRUSIVE DOUBLY-LINKED LIST, each 0x30
 * bytes -- the destructor's own `__destroy_arr(this + 0x51a8, 0x28, 0x30,
 * func_ov006_020f8ff0)` sizes and counts them, and MgPairAGoneAndOn_Spawn
 * constructs the same 0x28 x 0x30 array. The 23 one-function legacy files
 * each modelled the element partially and under four different names
 * (`Node`, `Obj`, `A`, and a vtable-less typedef); every offset they agree
 * on is unified here, and the disagreements are resolved by offset:
 * Render's own `struct Node { virtual void m0(); Node *next; ... }` put its
 * link at 0x04, which is `prev` in every other file, so Render walks the
 * list BACKWARDS -- see its own note.
 *
 * data_ov006_0214257c is the head of the live list and _02142568 its tail
 * (func_ov006_020f9760 builds it and leaves both set); _0214256c heads the
 * list of pieces already cleared, which func_ov006_020f9000 recycles back
 * onto the tail. */
struct Node {
    virtual void v0();              /* slot 0 -- draw this piece            */
    virtual void v1(s16 index);     /* slot 1 -- advance this piece         */
    /* 0x04 */ Node* prev;
    /* 0x08 */ Node* next;
    /* 0x0c */ int f_0c;            /* x, 20.12 fixed point                 */
    /* 0x10 */ int f_10;            /* y                                    */
    /* 0x14 */ int f_14;            /* target x                             */
    /* 0x18 */ int f_18;            /* target y                             */
    /* 0x1c */ int f_1c;            /* x step toward the target             */
    /* 0x20 */ int f_20;            /* y step; its sign picks the draw pass */
    /* 0x24 */ int f_24;            /* scale, approached toward 0x4000      */
    /* 0x28 */ s16 f_28;            /* countdown                            */
    /* 0x2a */ s16 f_2a;            /* board slot index                     */
    /* 0x2c */ u8  f_2c;            /* face, drawn by func_ov006_020f9560   */
    /* 0x2d */ u8  f_2d;            /* state                                */
    /* 0x2e */ u8  f_2e;            /* drawn this frame                     */
};

/* Shape-only shadow of this scene class's OWN vtable, used for the slot-18
 * self-dispatch at the end of InitResources. Not a distinct type -- delete
 * it once dScMgBase_c declares slots 18..35 for real. */
struct SceneVtable {
    virtual void v0();  virtual void v1();  virtual void v2();  virtual void v3();
    virtual void v4();  virtual void v5();  virtual void v6();  virtual void v7();
    virtual void v8();  virtual void v9();  virtual void v10(); virtual void v11();
    virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
    virtual void v16(); virtual void v17(); virtual void v18(int x);
};

/* The minigame's own globals. Everything already declared by
 * include/decl_common.h with a type this file agrees with is NOT repeated
 * here -- only the four list pointers, which decl_common cannot spell
 * because it does not know `Node`. */
extern "C" {
extern Node* data_ov006_02142568;   /* tail of the live list                */
extern Node* data_ov006_0214256c;   /* head of the cleared list             */
extern Node* data_ov006_02142570;   /* first piece of the pair being matched */
extern Node* data_ov006_02142574;   /* second piece                          */
extern Node* data_ov006_02142578;   /* where Render starts its backward walk */
extern Node* data_ov006_0214257c;   /* head of the live list                */
extern s16 data_ov006_02142558;     /* frames left before the pair resolves  */
extern s16 data_ov006_0214255c;     /* pieces still animating                */
extern s16 data_ov006_02142560;     /* difficulty row into data_ov006_0212e97c */
extern int data_ov006_02142580[];   /* per-face weights the picker draws from  */
extern int data_ov006_021425a8[];   /* sprite handles, indexed by 0213d770      */
extern int data_ov006_0213d728[];   /* the array element's vtable-ish payload   */
extern unsigned short data_ov006_0213d770[];
extern int data_ov006_0212e954[];   /* the starting weights                     */
extern int data_ov006_0212e97c[][10]; /* per-difficulty weight top-ups          */
extern int data_0209e650[];
extern int data_020a0db0;
extern unsigned char data_0209d454;
extern unsigned char data_0209d45c;
extern u8 data_020a0e40[];
extern u8 data_020a0de8[];
extern u8 data_020a0de9[];
extern u8 data_020a0dea[];
extern u8 data_020a0deb[];

int  RandomIntInternal(void* seed);
int  RenderOamMainScreen(int a0, int a1, int a2, int a3, int a4);
void Hud_RenderSprite(void* a0, int a1, int a2, int a3, int a4);
int  GetGameLanguage(void);
int  LoadFile(int handle);
void DecompressLZ16(int src, int dst);
void Vec2_Sub(int* out, int* a, int* b);
unsigned int func_02012790(unsigned int id);
int  func_0203d5dc(void* a, void* b);
void _ZN5Sound12PlayBank2_2DEj(unsigned int id);
void _ZN2GX11LoadOBJPlttEPKvjj(const void* p, unsigned int a, unsigned int b);
void _ZN3GXS11LoadOBJPlttEPKvjj(const void* p, unsigned int a, unsigned int b);
void func_ov004_020ad90c(void* t);
void func_ov004_020adb1c(int score);
void func_ov006_0210a534(void* c);
void func_ov006_020c0aa8(void* c);
void func_ov006_020c1604(char* t, int unused, short a2, void* a3);
int  func_ov006_020c1718(void* t);
void func_ov006_020c1804(void* t);
void func_ov006_020c19d0(void* t);
int  func_ov006_020c1a88(void* t);

/* This TU's own members, forward-declared for the calls that run up the file */
void func_ov006_020f8ff0(int* p);
void func_ov006_020f9000(void);
void func_ov006_020f94f4(void);
int  func_ov006_020f9560(void);
int  func_ov006_020f95f0(void);
int  func_ov006_020f9668(void);
int  func_ov006_020f96e0(void);
void func_ov006_020f9760(Node* base);
void func_ov006_020f98dc(char* piece);
void func_ov006_020f9994(char* piece, int slot);
int  func_ov006_020f9bec(Node* piece);
int  func_ov006_020f9cbc(Node* a, Node* b);
void func_ov006_020f9d68(Node* piece, int slot);
void func_ov006_020f9db8(Node* piece, int slot);
/* Spelled `(char*, int)` because THE DEFINITION is: the legacy file that
 * holds it byte-matches with those parameter types, and the two legacy
 * callers that spelled it `(Node*, s16)` byte-match too, so the two
 * spellings are provably the same code. The definition wins. */
void func_ov006_020f9f40(char* piece, int slot);
}

/* The ROM's own symbols demangle to these, so this is how they are spelled:
 * _Z14ApproachLinearRiii is ApproachLinear(int&, int, int) and
 * _Z15ApproachLinear2Rsss is ApproachLinear2(short&, short, short). Declared
 * with C++ linkage -- the reference parameter is what mangles them. */
int ApproachLinear(int& value, int target, int step);
int ApproachLinear2(s16& value, s16 target, s16 step);

/* -------------------------------------------------------------------------- */
/* ROM ordinal 22 -- _ZN14dScMgMCarlo2_c13InitResourcesEv, 0x020fa56c, size 0x140 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14dScMgMCarlo2_c13InitResourcesEv
/* dScMgMCarlo2_c::InitResources -- vtable slot 0.
 *
 * Attributed by tools/rtti_vtables.py --own dScMgMCarlo2_c, this class's own
 * slot 0 (fBase_c::InitResources). The old file's `recovered name:
 * dScMgMCarlo2_c_InitResources` agreed.
 *
 * The final `((SceneVtable *)c)->v18(-1)` is a self-dispatch through this
 * class's own vtable slot 18. It stays a shape-only shim because
 * include/dScMgBase_c.h stops declaring slots at 17; once it declares 18..35
 * this becomes a plain `this->...(-1)` and SceneVtable goes away. */
s32 dScMgMCarlo2_c::InitResources()
{
    char *c = (char *)this;
    int handle;
    int f1, f2;
    func_ov004_020b04d0(0x20);
    func_ov006_0210a534(c);
    handle = GetGameLanguage();
    f1 = LoadFile(data_ov006_0213d744[handle]);
    f2 = LoadFile(0xbb);
    DecompressLZ16(f1, 0x6400000);
    DecompressLZ16(f1, 0x6600000);
    _ZN2GX11LoadOBJPlttEPKvjj((void *)f2, 0, 0x100);
    _ZN3GXS11LoadOBJPlttEPKvjj((void *)f2, 0, 0x100);
    Deallocate((void *)f1);
    Deallocate((void *)f2);
    data_0209d45c = 0x11;
    data_0209d454 = 0x18;
    *(volatile unsigned short *)0x4000008 = (*(volatile unsigned short *)0x4000008 & ~3) | 1;
    *(volatile unsigned short *)0x400100a = *(volatile unsigned short *)0x400100a & ~3;
    func_ov006_020c0aa8((void *)(c + 0x4660));
    if (func_ov006_020c1a88((void *)(c + 0x4f38)) == 0) return 0;
    func_ov004_020b682c();
    ((SceneVtable *)c)->v18(-1);
    this->unk_592a = 0;
    this->unk_0a8 = 0xa;
    this->unk_0ac = this->unk_0a8;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 21 -- _ZN14dScMgMCarlo2_c13OnYoshiTryEatEi, 0x020fa4d4, size 0x98 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14dScMgMCarlo2_c13OnYoshiTryEatEi
// recovered name: dScMgMCarlo2_c_OnYoshiTryEat_020fa4d4
/* Resets the whole board: rebuild the 40 pieces, clear the match latch,
 * re-arm the shared table, then hand the score display a zero. */
extern "C" {
void dScMgMCarlo2_c::OnYoshiTryEat(int /* arg */)
{
    char *c = (char *)this;

    struct dScMgMCarlo2_c *self = (struct dScMgMCarlo2_c *)(void *)c;
  func_ov006_020f9760((Node *)(c + 0x51a8));
  data_ov006_0213d6fc = 0;
  self->unk_592e = 0;
  self->unk_511e = 1;
  func_ov006_020c1604(c + 0x4f38, 4, 4, c + 0x592e);
  self->unk_4f52 = 1;
  self->unk_592a = 0;
  func_ov004_020adb1c(0);
  self->unk_5928 = 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 20 -- _ZN14dScMgMCarlo2_c13OnTurnIntoEggEi, 0x020fa3d0, size 0x104 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14dScMgMCarlo2_c13OnTurnIntoEggEi
extern "C" {  /* .c-derived member: C linkage for the whole block */
int dScMgMCarlo2_c::OnTurnIntoEgg(int /* mode */)
{
    char *self = (char *)this;

    short st = *(short *)(self + 0x5928);
    switch (st) {
    case 4:
        if (func_ov006_020c1718((int *)(self + 0x4f38)) != 0) {
            short *p = (short *)(self + 0x5928);
            *p += 1;
        }
        break;
    case 5: {
        unsigned char idx = data_020a0e40[0];
        /* Written as a nested test rather than `found = A && B`, which is what
         * the ROM's code shape says the original was: the cartridge materialises
         * `found` (mov r3,#0 ... movne r3,#1) and then tests it (cmp r3,#0).
         * Fused into one && expression the compiler folds the variable away and
         * branches straight out -- three instructions short. */
        int found = 0;
        if (data_020a0de8[idx * 4] != 0) {
            found = data_020a0de9[idx * 4] != 0;
        }
        if (found != 0) {
            short *p;
            func_02012790(0x62);
            *(short *)(self + 0x592a) = 0x1e;
            func_ov006_020f94f4();
            p = (short *)(self + 0x5928);
            *p += 1;
        }
        break;
    }
    case 6:
    default:
        if (ApproachLinear2(*(s16 *)(self + 0x592a), 0, 1) != 0) {
            return 1;
        }
        break;
    }
    return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 19 -- _ZN14dScMgMCarlo2_c8BehaviorEv, 0x020fa13c, size 0x294 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14dScMgMCarlo2_c8BehaviorEv
/* dScMgMCarlo2_c::Behavior -- vtable slot 6, ov006 0x020fa13c.
 *
 * Attributed by the vtable: dScMgMCarlo2_c's own table is ov006 0x0213d7e8 and
 * its slot 6 relocates here. The signature is include/dScMgBase_c.h's own slot
 * 6, `virtual s32 Behavior()`.
 *
 * THE STATE COUNTER IS BUMPED THROUGH A LAUNDERED POINTER, three times, and
 * that is not a leftover: `unk_5928++` recomputes the address for the store
 * where `s16 *p = (s16 *)((int)this + 0x5928); (*p)++;` makes mwccarm CSE it
 * into a register, which is what the cartridge does. Same lever
 * notes/mwccarm-codegen.md records for compound assignment. Everything the
 * class owns outright and reads once -- unk_592e, unk_4f52, unk_511e -- reads
 * as a member.
 *
 * The 0x270-byte shared table at 0x4f38 stays opaque, as
 * include/dScMgMCarlo2_c.h's banner says: five siblings use the same one, and
 * the helpers here take its base address rather than anything inside it. The
 * two fields that ARE named within it, unk_4f52 and unk_511e, are named
 * because this function and its siblings write them by hand. */
s32 dScMgMCarlo2_c::Behavior()
{
    unsigned char *c = (unsigned char *)this;

    switch (unk_5928) {
    case 1:
        {
            s16 *p = (s16 *)((int)c + 0x5928);
            (*p)++;
        }
        if (c[0xc4] == 0) {
            c[0xc3] = 1;
            c[0xc4] = 1;
            *(s16 *)(c + 0xc0) = 0;
        }
        /* fall through */
    case 2:
        data_ov006_0213d700 = unk_592e * 5 << 12;
        if (unk_592e == 4) {
            if (func_ov006_020f9668() == 0) {
                unk_592e = 0;
                {
                    s16 *p = (s16 *)((int)c + 0x5928);
                    (*p)++;
                }
            }
        }
        break;
    case 3:
        if (func_ov006_020f95f0() == 0) {
            int r5;
            if (data_ov006_0213d6fc == 1)
                data_ov006_0213d6fc = 0;
            r5 = data_ov006_0213d700 >> 12;
            if (data_ov006_0213d6f4 != 0 && r5 > 10 && r5 <= 18
                && func_ov006_020c1718(c + 0x4f38) != 0) {
                unk_592e = 0;
                unk_511e = 0;
                func_ov006_020c1164(c + 0x4f38, 2, &unk_592e);
                if (data_ov006_0213d6f4 == 2)
                    unk_4f52 = 0;
            } else {
                int lim = unk_592e + 0x12;
                if (r5 >= lim) {
                    int flag = (data_ov006_02142570 != 0 && data_ov006_02142574 != 0);
                    if (flag == 0)
                        data_ov006_0213d700 = lim << 12;
                }
            }
        } else {
            if (data_ov006_0213d6fc == 0) {
                if (func_ov006_020f96e0() != 0) {
                    data_ov006_0213d6fc = 1;
                } else {
                    if (data_ov006_0213d6f8 != 0) {
                        if (unk_4f52 == 1) {
                            unk_4f52 = 0;
                        } else if (func_ov006_020c16b4(c + 0x4f38) != 0) {
                            func_ov006_020c0d68(c + 0x4f38);
                            func_ov004_020b0a54(0x12);
                            c[0xc3] = 0;
                            {
                                s16 *p = (s16 *)((int)c + 0x5928);
                                (*p)++;
                            }
                        }
                    }
                    data_ov006_0213d6fc = 0;
                }
            }
        }
        break;
    }

    func_ov006_020c19d0(c + 0x4f38);
    func_ov006_020f9000();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 18 -- _ZN14dScMgMCarlo2_c6RenderEv, 0x020f9ffc, size 0x140 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14dScMgMCarlo2_c6RenderEv
/* dScMgMCarlo2_c::Render -- vtable slot 9.
 *
 * THE TWO DRAW PASSES WALK `prev`, NOT `next`. The pre-migration file
 * modelled the element as `struct Node { virtual void m0(); Node *next;
 * char pad[0x18]; int f20; };` -- one link, at offset 0x04. Every other
 * file in this TU puts `prev` at 0x04 and `next` at 0x08, so the link this
 * function follows is `prev`: it starts from data_ov006_02142578 and walks
 * toward the head. Renaming it without re-reading the offset would have
 * changed the emitted load.
 *
 * Attributed by tools/rtti_vtables.py --own dScMgMCarlo2_c, this class's own
 * slot 9 (dScMgBase_c::Render). The old file's `recovered name:
 * dScMgMCarlo2_c_Render` agreed. */
s32 dScMgMCarlo2_c::Render()
{
    char *c = (char *)this;
    short v;
    Node *n6;
    int i5;
    int i6;
    Node *n5;

    func_ov006_020c0aa8(c + 0x4660);

    v = data_ov006_02142564;
    if (v > 0x270f)
        v = 0x270f;
    func_ov004_020b1ea4(0xe8, 0x28, v, 1, -1, 0, 0);

    RenderOamMainScreen(data_ov006_02133f18, 0xe8, 0x18, -1, -1);

    if (this->unk_5928 == 5)
        func_ov004_020b0d8c(c, 0xe0, 0xa0);

    n6 = data_ov006_02142578;
    for (i5 = 0; i5 < 0x14; i5++) {
        if (n6 == 0)
            break;
        if (n6->f_20 > 0)
            n6->v0();
        n6 = n6->prev;
    }

    n5 = data_ov006_02142578;
    for (i6 = 0; i6 < 0x14; i6++) {
        if (n5 == 0)
            break;
        if (n5->f_20 == 0)
            n5->v0();
        n5 = n5->prev;
    }

    func_ov006_020c1804(c + 0x4f38);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 17 -- _ZN14dScMgMCarlo2_c16CleanupResourcesEv, 0x020f9fe0, size 0x1c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14dScMgMCarlo2_c16CleanupResourcesEv
/* dScMgMCarlo2_c::CleanupResources -- vtable slot 3, ov006 0x020f9fe0.
 *
 * Attributed by the vtable: dScMgMCarlo2_c's own table is ov006 0x0213d7e8 and
 * its slot 3 relocates here, so this is this class's own override and not one
 * it inherits from dScMgSingle3DBase_c. The signature is include/fBase_c.h's
 * own slot 3, `virtual s32 CleanupResources()`.
 *
 * func_ov004_020ad90c IS ARITY-AMBIGUOUS ACROSS THE FAMILY, and this file does
 * not settle it: dScMgJump2_c's and dScMgTrampoline2_c's CleanupResources both
 * call it with no argument at all, this one passes the scene. `this` is
 * already in r0 at the call, so both spellings produce the same word and
 * neither is evidence. Kept as the pre-migration file had it. */
s32 dScMgMCarlo2_c::CleanupResources()
{
    func_ov004_020ad90c(this);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 16 -- func_ov006_020f9f40, 0x020f9f40, size 0xa0 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020f9f40
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020f9f40(char* c, int r1){
    *(short*)(c+0x2a) = (short)r1;
    if (r1 >= 0x14) {
        *(short*)(c+0x28) = 1;
    } else {
        *(short*)(c+0x28) = (short)(((r1 % 5) << 1) + 1);
    }
    *(int*)(c+0x24) = 0;
    if (r1 == 0x13) data_ov006_02142578 = (Node *)c;
    *(unsigned char*)(c+0x2e) = 1;
    *(int*)(c+0x1c) = 0;
    *(int*)(c+0x20) = 0;
    *(unsigned char*)(c+0x2d) = 0;
    *(unsigned char*)(c+0x2c) = (unsigned char)func_ov006_020f9560();
    *(int*)(c+8) = 0;
    *(int*)(c+4) = *(int*)(c+8);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 15 -- func_ov006_020f9db8, 0x020f9db8, size 0x188 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020f9db8
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020f9db8(Node *o, int a)
{
    int v[3];

    if (a >= 0x14)
        return;

    if (o->f_2a >= 0x14) {
        o->f_0c = (0x70 - ((data_ov006_0213d6f4 >> 2) << 1)) << 12;
        o->f_10 = -0x30000;
        data_ov006_02142578 = o;
        o->f_2d = 1;
    } else if (o->f_2d == 0) {
        o->f_0c = (0x70 - ((data_ov006_0213d6f4 >> 2) << 1)) << 12;
        o->f_10 = -0x30000;
        o->f_2d = 4;
        data_ov006_0214255c++;
    } else {
        o->f_2d = 4;
        data_ov006_0214255c++;
    }

    o->f_14 = ((a % 5) * 32 + 0x30) << 12;
    o->f_18 = ((a / 5) * 0x30) << 12;
    o->f_2a = (short)a;

    Vec2_Sub(v, &o->f_14, &o->f_0c);

    o->f_1c = v[0];
    o->f_20 = v[1];
    func_0203d630(&o->f_1c, 0x124);

    if (o->f_1c < 0)
        o->f_1c = -o->f_1c;
    if (o->f_20 < 0)
        o->f_20 = -o->f_20;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 14 -- func_ov006_020f9d68, 0x020f9d68, size 0x50 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020f9d68
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020f9d68(Node *o, int n) {
    *(short*)((char*)o + 0x28) = (short)((4 - n % 5) * 2);
    *(unsigned char*)((char*)o + 0x2d) = 5;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 13 -- func_ov006_020f9cbc, 0x020f9cbc, size 0xac */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020f9cbc
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov006_020f9cbc(Node *a, Node *b)
{
    int ai, bi, dm, dd;
    if (b->f_2c != a->f_2c)
        goto fail;
    bi = b->f_2a;
    ai = a->f_2a;
    dm = ai % 5 - bi % 5;
    dd = ai / 5 - bi / 5;
    if (dm < 0)
        dm = -dm;
    if (dm >= 2)
        goto fail;
    if (dd < 0)
        dd = -dd;
    if (dd >= 2)
        goto fail;
    return 1;
fail:
    return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 12 -- func_ov006_020f9bec, 0x020f9bec, size 0xd0 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020f9bec
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov006_020f9bec(Node *c)
{
    u8 idx;
    int off;
    int has;
    int a, b;

    if (data_ov006_0213d6fc == 0) return 0;
    if (func_ov006_020f9668() != 0) goto fail;

    idx = data_020a0e40[0];
    off = idx * 4;
    has = 0;
    if (data_020a0de8[off]) {
        if (data_020a0de9[off]) has = 1;
    }
    if (has == 0) goto fail;

    a = data_020a0dea[idx * 4] - (c->f_0c >> 12);
    b = data_020a0deb[idx * 4] - (c->f_10 >> 12);
    if (a > 7 && a < 0x28 && b > 0 && b < 0x31) return 1;
fail:
    return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 11 -- func_ov006_020f9994, 0x020f9994, size 0x258 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020f9994
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020f9994(char *c, int b)
{
    switch (*(unsigned char *)(c + 0x2d)) {
    case 0:
        {
            short *p = (short *)(c + 0x28);
            short v = *(short *)p;
            *(short *)p = (short)(v - 1);
        }
        if (*(short *)(c + 0x28) != 0)
            return;
        data_ov006_0213d6f4--;
        func_ov006_020f9db8((Node *)c, b);
        return;
    case 2:
        if (*(short *)(c + 0x2a) != b) {
            func_ov006_020f9db8((Node *)c, b);
            return;
        }
        if (func_ov006_020f9bec((Node *)c) == 0)
            return;
        if (data_ov006_02142570 == 0) {
            data_ov006_02142570 = (Node *)c;
            *(unsigned char *)(c + 0x2d) = 3;
            _ZN5Sound12PlayBank2_2DEj(0x153);
            return;
        }
        if (func_ov006_020f9cbc((Node *)c, data_ov006_02142570) != 0) {
            data_ov006_02142574 = (Node *)c;
            data_ov006_02142558 = 0x20;
            *(unsigned char *)(c + 0x2d) = 3;
            _ZN5Sound12PlayBank2_2DEj(0x154);
            return;
        }
        *(unsigned char *)((char *)data_ov006_02142570 + 0x2d) = 2;
        data_ov006_02142570 = 0;
        func_02012790(0xe);
        return;
    case 3:
        if (func_ov006_020f9bec((Node *)c) == 0)
            return;
        *(unsigned char *)(c + 0x2d) = 2;
        if (data_ov006_02142570 != (Node *)c)
            return;
        if (data_ov006_02142574 != 0)
            return;
        _ZN5Sound12PlayBank2_2DEj(0x155);
        data_ov006_02142570 = 0;
        return;
    case 1:
    case 4:
        ApproachLinear(*(int *)(c + 0xc), *(int *)(c + 0x14), *(int *)(c + 0x1c));
        ApproachLinear(*(int *)(c + 0x10), *(int *)(c + 0x18), *(int *)(c + 0x20));
        ApproachLinear(*(int *)(c + 0x24), 0x4000, 0x300);
        if (func_0203d5dc(c + 0xc, c + 0x14) != 0)
            return;
        if (*(int *)(c + 0x24) != 0x4000)
            return;
        *(unsigned char *)(c + 0x2d) = 2;
        ApproachLinear2(data_ov006_0214255c, 0, 1);
        *(int *)(c + 0x1c) = 0;
        *(int *)(c + 0x20) = 0;
        return;
    case 5:
        if (ApproachLinear2(*(s16 *)(c + 0x28), 0, 1) == 0)
            return;
        ApproachLinear(*(int *)(c + 0xc), -0x30000, 0x10000);
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 10 -- func_ov006_020f98dc, 0x020f98dc, size 0xb8 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020f98dc
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020f98dc(char *thiz)
{
    unsigned char k;
    if (*(unsigned char*)(thiz + 0x2e) == 0) return;
    k = *(unsigned char*)(thiz + 0x2d);
    if (k == 0) return;
    if (k == 3) {
        if (((&data_020a0db0)[0] & 8) != 0) return;
    }
    {
        int idx = (*(unsigned char*)(thiz + 0x2c) + 1) * 5
                  + (*(int*)(thiz + 0x24) >> 12);
        unsigned short e = data_ov006_0213d770[idx];
        Hud_RenderSprite(
            (void*)data_ov006_021425a8[e],
            (*(int*)(thiz + 0xc) >> 12) + 0x18,
            (*(int*)(thiz + 0x10) >> 12) + 0x18,
            -1,
            -1);
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 9 -- func_ov006_020f9760, 0x020f9760, size 0x17c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020f9760
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020f9760(Node* base)
{
    do {
        {
            s16 i = 0;
            do {
                data_ov006_02142580[i] = data_ov006_0212e954[i];
                i = i + 1;
            } while (i < 10);
        }
        {
            s16 j = 0;
            Node* p = base;
            do {
                func_ov006_020f9f40((char *)p, j);
                p = p + 1;
                j = j + 1;
            } while (j < 0x28);
        }
        data_ov006_02142568 = 0;
        data_ov006_0214257c = base;
        data_ov006_0214256c = 0;
        data_ov006_02142570 = 0;
        data_ov006_02142574 = 0;
        data_ov006_02142558 = 0;
        data_ov006_0214255c = 0;
        data_ov006_0213d6f8 = 0x28;
        data_ov006_0213d6f4 = 0x28;
        data_ov006_02142560 = 0;
        data_ov006_02142564 = 0;
        data_ov006_0213d700 = 0;
        {
            s16 i = 0;
            Node* h = base;
            do {
                Node* t = &base[i + 1];
                Node* save = h->next;
                h->next = t;
                t->prev = h;
                {
                    Node* u = t->next;
                    if (u != 0) {
                        do { t = u; u = u->next; } while (u != 0);
                    }
                }
                t->next = save;
                if (t->next == 0) data_ov006_02142568 = t;
                h = h + 1;
                i++;
            } while (i < 0x27);
        }
    } while (func_ov006_020f96e0() == 0);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- func_ov006_020f96e0, 0x020f96e0, size 0x80 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020f96e0
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov006_020f96e0(void)
{
    Node *p, *q;
    p = data_ov006_0214257c;
    while (p != 0 && p->f_2a < 0x14) {
        q = p->next;
        while (q != 0 && q->f_2a < 0x14) {
            if (func_ov006_020f9cbc(p, q) != 0) return 1;
            q = q->next;
        }
        p = p->next;
    }
    return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- func_ov006_020f9668, 0x020f9668, size 0x78 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020f9668
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov006_020f9668(void) {
    int ret = 1;
    int a = (data_ov006_0213d700 << 4) >> 0x10;
    int b = data_ov006_0213d6f8;
    if (b > 0x14) b = 0x14;
    if (a == b && data_ov006_0214255c == 0) {
        if (data_ov006_02142570 == 0 || data_ov006_02142574 == 0) {
            ret = 0;
        }
    }
    return ret;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- func_ov006_020f95f0, 0x020f95f0, size 0x78 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020f95f0
extern "C" {  /* .c-derived member: C linkage for the whole block */
/* One expression, one exit. The cartridge keeps the result pinned in r0 for
 * the whole body (mov r0,#0 up front, mov r0,#1 on the one success path);
 * spelling the guards as early `return 0`s instead lets the compiler
 * rematerialise the zero at each exit, which is the extra 8 bytes. */
int func_ov006_020f95f0(void) {
    int a = (data_ov006_0213d700 << 4) >> 16;
    int r = 0;
    int b = data_ov006_0213d6f8;

    if (b > 0x14) {
        b = 0x14;
    }
    if (a == b && data_ov006_0214255c == 0
        && (data_ov006_02142570 == 0 || data_ov006_02142574 == 0)) {
        r = 1;
    }
    return r;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- func_ov006_020f9560, 0x020f9560, size 0x90 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020f9560
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov006_020f9560(void){
    unsigned char pick = 0;
    int total = 0;
    int i;
    int r;
    for(i=0;i<0xa;i++) total += data_ov006_02142580[i];
    r = (int)(((unsigned int)RandomIntInternal(data_0209e650) & 0x7fffffff) >> 0x13);
    total = (total * r) >> 0xc;
    for(i=0;i<0xa;i++){
        total -= data_ov006_02142580[i];
        if(total < 0){
            pick = (unsigned char)i;
            data_ov006_02142580[i]--;
            break;
        }
    }
    return pick;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- func_ov006_020f94f4, 0x020f94f4, size 0x6c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020f94f4
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020f94f4(void){
    Node* node = data_ov006_0214257c;
    short i = 0;
    if((data_ov006_0213d700>>12) <= 0) return;
    do {
        if(node == 0) return;
        func_ov006_020f9d68(node, i);
        i = i+1;
        node = node->next;
    } while(i < (data_ov006_0213d700>>12));
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- func_ov006_020f9000, 0x020f9000, size 0x4f4 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020f9000
extern "C" void func_ov006_020f9000(void)
{
    Node* head = data_ov006_0214257c;

    if (data_ov006_02142570 != 0 && data_ov006_02142574 != 0) {
        data_ov006_02142558 -= 1;
        if (data_ov006_02142558 == 0) {
            Node* a;
            Node* b;
            Node* t;
            Node* u;
            s16 kb = data_ov006_02142574->f_2a;
            s16 ka = data_ov006_02142570->f_2a;
            if (ka > kb)
                data_ov006_0213d700 = kb << 12;
            else
                data_ov006_0213d700 = ka << 12;

            a = data_ov006_02142570;
            if (data_ov006_0214257c == a) data_ov006_0214257c = a->next;
            if (data_ov006_0214256c == a) data_ov006_0214256c = a->next;
            if (data_ov006_02142578 == a) data_ov006_02142578 = a->prev;
            if (data_ov006_02142568 == a) data_ov006_02142568 = a->prev;
            if (a->prev != 0) a->prev->next = a->next;
            if (a->next != 0) a->next->prev = a->prev;
            a->next = 0;
            a->prev = a->next;

            b = data_ov006_02142574;
            if (data_ov006_0214257c == b) data_ov006_0214257c = b->next;
            if (data_ov006_0214256c == b) data_ov006_0214256c = b->next;
            if (data_ov006_02142578 == b) data_ov006_02142578 = b->prev;
            if (data_ov006_02142568 == b) data_ov006_02142568 = b->prev;
            if (b->prev != 0) b->prev->next = b->next;
            if (b->next != 0) b->next->prev = b->prev;
            b->next = 0;
            b->prev = b->next;

            a = data_ov006_02142570;
            t = data_ov006_0214256c;
            if (t == 0) {
                data_ov006_0214256c = a;
            } else {
                u = t->next;
                if (u != 0) {
                    do { t = u; u = u->next; } while (u != 0);
                }
                t->next = a;
                a->prev = t;
                a->next = 0;
            }

            b = data_ov006_02142574;
            t = data_ov006_0214256c;
            if (t == 0) {
                data_ov006_0214256c = b;
            } else {
                u = t->next;
                if (u != 0) {
                    do { t = u; u = u->next; } while (u != 0);
                }
                t->next = b;
                b->prev = t;
                b->next = 0;
            }

            data_ov006_02142574 = 0;
            data_ov006_02142570 = 0;
            ApproachLinear2(data_ov006_02142564, 0x270f, 2);
            ApproachLinear2(data_ov006_0213d6f8, 0, 2);
            func_ov004_020adb1c(data_ov006_02142564);

            if (data_ov006_0213d6f8 <= 0x14) {
                {
                    int i = 0;
                    s16 idx = *(volatile s16*)&data_ov006_02142560;
                    int* row = (int*)((char*)data_ov006_0212e97c + idx * 0x28);
                    do {
                        data_ov006_02142580[i] = data_ov006_02142580[i] + row[i];
                        i += 1;
                    } while (i < 10);
                }
                {
                    Node* p = data_ov006_0214256c;
                    if (p != 0) {
                        do {
                            Node* nxt = p->next;
                            if (data_ov006_0214257c == p) data_ov006_0214257c = nxt;
                            if (data_ov006_0214256c == p) data_ov006_0214256c = p->next;
                            if (data_ov006_02142578 == p) data_ov006_02142578 = p->prev;
                            if (data_ov006_02142568 == p) data_ov006_02142568 = p->prev;
                            if (p->prev != 0) p->prev->next = p->next;
                            if (p->next != 0) p->next->prev = p->prev;
                            p->next = 0;
                            p->prev = p->next;
                            func_ov006_020f9f40((char *)p, (s16)(data_ov006_02142568->f_2a + 1));
                            {
                                Node* h = data_ov006_02142568;
                                Node* save = h->next;
                                h->next = p;
                                p->prev = h;
                                if (p->next != 0) {
                                    do { p = *(Node* volatile*)&p->next; } while (p->next != 0);
                                }
                                p->next = save;
                                if (p->next == 0) data_ov006_02142568 = p;
                            }
                            p = nxt;
                        } while (p != 0);
                    }
                }
                ApproachLinear2(data_ov006_02142560, 9, 1);
                data_ov006_0213d6f4 += 0x14;
                data_ov006_0213d6f8 += 0x14;
            }
        }
        {
            s16 i = 0;
            if ((data_ov006_0213d700 >> 12) <= 0) return;
            do {
                if (head == 0) return;
                if (head->f_2d == 1)
                    head->v1(i);
                i = i + 1;
                head = head->next;
            } while (i < (data_ov006_0213d700 >> 12));
        }
    } else {
        int v = data_ov006_0213d6f8;
        if (v > 0x14) v = 0x14;
        ApproachLinear(data_ov006_0213d700, v << 12, 0x800);
        {
            s16 i = 0;
            if ((data_ov006_0213d700 >> 12) <= 0) return;
            do {
                if (head == 0) return;
                head->v1(i);
                i = i + 1;
                head = head->next;
            } while (i < (data_ov006_0213d700 >> 12));
        }
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- func_ov006_020f8ff0, 0x020f8ff0, size 0x10 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020f8ff0
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020f8ff0(int *p) { p[0] = (int)data_ov006_0213d728; }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN14dScMgMCarlo2_cD0Ev, 0x020f8f68, size 0x88            */
/* --------------------------------------------------------------------------
 * ROM ordinals 0 and 1 -- _ZN14dScMgMCarlo2_cD1Ev at 0x020f8ef4 (0x74)
 *                     and _ZN14dScMgMCarlo2_cD0Ev at 0x020f8f68 (0x88).
 *
 * Neither is written out here. ~dScMgMCarlo2_c() is defined inline in the
 * class body (include/dScMgMCarlo2_c.h), so it cannot be this TU's key
 * function; InitResources, the next virtual declared and non-inline, is, and
 * this TU defines it. That emits _ZTV14dScMgMCarlo2_c right here, whose slots
 * 16 and 17 name D1 and D0, odr-using both, so the compiler emits the pair out
 * of line for us -- in cartridge order. D0's extra 0x14 bytes over D1 are the
 * `operator delete` tail dScMgBase_c already provides.
 *
 * Written out of line instead, mwcc emits the synthesized D0 AHEAD of the
 * written D1, and linkcheck's pre-link audit refuses a TU whose licensed .text
 * is not in ROM address order. Inlining also removes the homeless D2 that an
 * out-of-line definition would emit with no ROM address to claim.
 * -------------------------------------------------------------------------- */
